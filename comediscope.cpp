#include <QTimer>
#include <QPainter>
#include <QApplication>
#include <QTimerEvent>
#include <QPaintEvent>

#include<sys/ioctl.h>
#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include <termios.h>

#include <fcntl.h>

#include "comediscope.h"
#include "base64.h"

void ComediScope::btprintf(int s,const char *message) {
	usleep(100000);
	write(s,message,strlen(message));
	tcflush(s,TCIFLUSH);
	usleep(100000);
	tcflush(s,TCIFLUSH);
}

ComediScope::ComediScope( Attys_scope *attys_scope_tmp, 
			  int, 
			  float f, 
			  int port_for_ext_data, 
			  int maxComediDevs,
			  int, // first_dev_no,
			  int req_sampling_rate,
			  const char* defaultTextStringForMissingExtData,
			  int fftdevnumber, int fftchannel, int fftmaxf
	)
    : QWidget( attys_scope_tmp ) {

	inquiry_info *ii = NULL;
	int max_rsp, num_rsp;
	int dev_id, sock, len, flags;
	int i;
	char addr[19] = { 0 };
	char name[248] = { 0 };
	struct sockaddr_rc saddr;
	
	sampling_rate = req_sampling_rate;

	channels_in_use = 11;

	tb_init=1;
	tb_counter=tb_init;
	attys_scope=attys_scope_tmp;
	// erase plot
	eraseFlag = 1;

	fftdevno = fftdevnumber;
	fftch = fftchannel;
	fftmaxfrequency = fftmaxf;

	// for ASCII
	rec_file=NULL;

	// filename
	rec_filename=new QString();

	// flag if data has been recorded and we need a new filename
	recorded=0;

	if (port_for_ext_data>0) {
		fprintf(stderr,
			"Expecting a connection on TCP port %d. \n"
			"Start your client now, for example: \n"
			"telnet localhost %d\n"
			"Press Ctrl-C to abort.\n",
			port_for_ext_data,
			port_for_ext_data);
		ext_data_receive = new Ext_data_receive(
			port_for_ext_data,
			defaultTextStringForMissingExtData
			);
	} else {
		ext_data_receive = NULL;
	}	


	//////////////////////////////////////////////////////////////

	setAttribute(Qt::WA_OpaquePaintEvent);

	dev_id = hci_get_route(NULL);
	sock = hci_open_dev( dev_id );
	if (dev_id < 0 || sock < 0) {
		perror("opening socket");
		exit(1);
	}
	
	len  = 8;
	max_rsp = 255;
	flags = IREQ_CACHE_FLUSH;
	ii = new inquiry_info[max_rsp];
    
	num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
	if( num_rsp < 0 ) perror("hci_inquiry");
	
	dev = new int[maxComediDevs];
	stream = new FILE*[maxComediDevs];
	for(int devNo=0;devNo<maxComediDevs;devNo++) {
		dev[devNo] = 0;
		stream[devNo] = NULL;
	}

	// let's probe how many we have
	nComediDevices = 0;
	for (i = 0; i < num_rsp; i++) {
		ba2str(&(ii+i)->bdaddr, addr);
		memset(name, 0, sizeof(name));
		if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), 
					 name, 0) < 0)
			strcpy(name, "[unknown]");
		printf("%s  %s", addr, name);
		if (strstr(name,"GN-ATTYS")!=0) {
			printf("!\n");
			// allocate a socket
			int s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
			
			// set the connection parameters (who to connect to)
			saddr.rc_family = AF_BLUETOOTH;
			saddr.rc_channel = (uint8_t) 1;
			str2ba( addr, &saddr.rc_bdaddr );

			// connect to server
			int status = ::connect(s,
					       (struct sockaddr *)&saddr,
					       sizeof(saddr));
			
			if (status == 0) {
				dev[nComediDevices] = s;
				stream[nComediDevices] = fdopen(s,"rt");
				btprintf(s,"\n\r\n\r\n\r\n\r\n\r\n\r");
				btprintf(s,"\n\nx=0\r\n");
				btprintf(s,"\n\nv=0\r\n");
				btprintf(s,"\n\nd=1\r\n");
				btprintf(s,"\n\nr=1\r\n");
				btprintf(s,"\n\na=0\r\n"); // a=6 for ECG
				btprintf(s,"\n\nb=0\r\n"); // a=6 for ECG
				btprintf(s,"\n\nt=3\r\n");
				btprintf(s,"\n\ng=3\r\n");
				btprintf(s,"\n\nx=1\r\n");
				nComediDevices++;
			} else {
				printf("Connect failed: %d\n",status);
				printf("Has the device been paired?\n");
			}
		} else {
			printf("\n");
		}
	}

	delete ii;
	
	// none detected
	if (nComediDevices<1) {
		fprintf(stderr,"No rfcomm devices detected!\n");
		exit(1);
	}

	// initialise the graphics stuff
	ypos = new int**[nComediDevices];
	assert(ypos != NULL);
	for(int devNo=0;devNo<nComediDevices;devNo++) {
		ypos[devNo]=new int*[channels_in_use];
		assert(ypos[devNo] != NULL);
		for(int i=0;i<channels_in_use;i++) {
			ypos[devNo][i] = new int[MAX_DISP_X];
			assert( ypos[devNo][i] != NULL);
			for(int j=0;j<MAX_DISP_X;j++) {
				ypos[devNo][i][j]=0;
			}
		}
	}

	xpos=0;
	nsamples=0;

	maxdata = new int*[nComediDevices];
	assert( maxdata != NULL );
	for(int devNo=0;devNo<nComediDevices;devNo++) {
		// we just go for the default ranges
		maxdata[devNo]=new int[channels_in_use];
		// accelerometer
		int idx=0;
		for(int ch=0;ch<3;ch++) {
			maxdata[devNo][idx++]=0xffff;
		}
		// gyro
		for(int ch=0;ch<3;ch++) {
			maxdata[devNo][idx++]=0xffff;
		}
		// compass
		for(int ch=0;ch<3;ch++) {
			maxdata[devNo][idx++]=0xffff;
		}		
		// bio amp
		for(int ch=0;ch<2;ch++) {
			maxdata[devNo][idx++]=0xffffff;
		}		
	}

	// 50Hz or 60Hz mains notch filter
	iirnotch = new Iir::Butterworth::BandStop<IIRORDER>**[nComediDevices];
	assert( iirnotch != NULL );
	adAvgBuffer = new float*[nComediDevices];
	assert( adAvgBuffer != NULL );
	daqData = new int*[nComediDevices];
	assert( daqData != NULL );
	for(int devNo=0;devNo<nComediDevices;devNo++) {
		iirnotch[devNo] = new Iir::Butterworth::BandStop<IIRORDER>*[channels_in_use];
		assert( iirnotch[devNo] != NULL );
		// floating point buffer for plotting
		adAvgBuffer[devNo]=new float[channels_in_use];
		assert( adAvgBuffer[devNo] != NULL );
		for(int i=0;i<channels_in_use;i++) {
			adAvgBuffer[devNo][i]=0;
			iirnotch[devNo][i] = new Iir::Butterworth::BandStop<IIRORDER>;
			assert( iirnotch[devNo][i] != NULL );
		}
		// raw data buffer for saving the data
		daqData[devNo] = new int[channels_in_use];
		assert( daqData[devNo] != NULL );
		for(int i=0;i<channels_in_use;i++) {
			daqData[devNo][i]=0;
		}
	}

	setNotchFrequency(f);

	// let's check that we get data from all devices
	// and clear the buffers
	for (int i=0;i<10;i++) {
		for(int devNo=0;devNo<nComediDevices;devNo++) {
			char linebuffer[65536];
			int ret = fscanf(stream[devNo],
					 "%s\n",
					 linebuffer);
			if (ret<1) {
				fprintf(stderr,
					"No data from rfcomm %d\n",devNo);
				exit(-1);
			}
		}
	}

	// ready steady go!
	counter = new QTimer( this );
	assert( counter != NULL );
	connect( counter, 
		 SIGNAL(timeout()),
		 this, 
		 SLOT(updateTime()) );
}


void ComediScope::startDAQ() {
	startTimer( 50 );		// run continuous timer
	counter->start( 500 );
}


ComediScope::~ComediScope() {
	if (rec_file) {
		fclose(rec_file);
	}
	for(int i=0; i<nComediDevices;i++) {
		fclose(stream[i]);
		shutdown(dev[i],2);
	}
}

void ComediScope::setNotchFrequency(float f) {
	if (f>sampling_rate) {
		fprintf(stderr,
			"Error: The notch frequency %f "
			"is higher than the sampling rate of %dHz.\n",
			f,sampling_rate);
		return;
	}
	for(int j=0;j<nComediDevices;j++) {
		for(int i=0;i<channels_in_use;i++) {
			float frequency_width = f/10;
			iirnotch[j][i]->setup (IIRORDER, 
					       sampling_rate, 
					       f, 
					       frequency_width);
		}
		notchFrequency = f;
	}
}



void ComediScope::updateTime() {
	QString s;
	if (!rec_file) {
		if (rec_filename->isEmpty()) {
			s.sprintf("attys_scope");
		} else {
			if (recorded) {
				s=(*rec_filename)+" --- file saved";
			} else {
				s=(*rec_filename)+" --- press REC to record ";
			}
		}
	} else {
		s = (*rec_filename) + 
			QString().sprintf("--- rec: %ldsec", nsamples/sampling_rate );
	}
	attys_scope->setWindowTitle( s );

	char tmp[256];
	for(int n=0;n<nComediDevices;n++) {
		for(int i=0;i<channels_in_use;i++) {
			float phys=normaliseData(daqData[n][i],maxdata[n][i]);
			sprintf(tmp,VOLT_FORMAT_STRING,phys);
			attys_scope->voltageTextEdit[n][i]->setText(tmp);
		}
	}
}


void ComediScope::setFilename(QString name,int csv) {
	(*rec_filename)=name;
	recorded=0;
	if (csv) {
		separator=',';
	} else {
		separator=' ';
	}
}


void ComediScope::writeFile() {
	if (!rec_file) return;
	if (attys_scope->
	    rawCheckbox->isChecked()) {
		fprintf(rec_file,"%ld",nsamples);
	} else {
		fprintf(rec_file,"%f",((float)nsamples)/((float)sampling_rate));
	}
	for(int n=0;n<nComediDevices;n++) {
		for(int i=0;i<channels_in_use;i++) {
			if (attys_scope->
			    channel[n][i]->isActive()
				) {
				if (attys_scope->
				    rawCheckbox->isChecked()) {
					fprintf(rec_file,
						"%c%d",separator,(int)(daqData[n][i]));
				} else {
					float phy=normaliseData(daqData[n][i],maxdata[n][i]);
					fprintf(rec_file,"%c%f",separator,phy);
				}
			}
		}
	}
	if (ext_data_receive) {
		fprintf(rec_file,"%c%s",separator,ext_data_receive->getData());
	}
	fprintf(rec_file,"\n");
}

void ComediScope::startRec() {
	if (recorded) return;
	if (rec_filename->isEmpty()) return;
	attys_scope->disableControls();
	// counter for samples
	nsamples=0;
	// get possible comments
	QString comment = attys_scope->commentTextEdit->toPlainText();
	// ASCII
	rec_file=NULL;
	// do we have a valid filename?
	if (rec_filename)
		rec_file=fopen(rec_filename->toLocal8Bit().constData(),
			       "wt");
	// could we open it?
	if (!rec_file) {
		// could not open
		delete rec_filename;
		// print error msg
		fprintf(stderr,
			"Writing to %s failed.\n",
			rec_filename->toLocal8Bit().constData());
	}
	// print comment
	if ((rec_file)&&(!comment.isEmpty())) {
		fprintf(rec_file,
			"# %s\n",
			comment.toLocal8Bit().constData());
	}
}


void ComediScope::stopRec() {
	if (rec_file) {
		fclose(rec_file);
		rec_file = NULL;
		recorded = 1;
	}
	// re-enabel channel switches
	attys_scope->enableControls();
	// we should have a filename, get rid of it and create an empty one
	if (rec_filename) delete rec_filename;
	rec_filename = new QString();
}



void ComediScope::paintData(float** buffer) {
	QPainter paint( this );
	QPen penData[3]={QPen(Qt::blue,1),
			 QPen(Qt::green,1),
			 QPen(Qt::red,1)};
	QPen penWhite(Qt::white,2);
	int w = width();
	int h = height();
	if (eraseFlag) {
		paint.fillRect(0,0,w,h,QColor(255,255,255));
		eraseFlag = 0;
		xpos = 0;
	}
	num_channels=0;

	// fprintf(stderr,".");
	
	for(int n=0;n<nComediDevices;n++) {
		for(int i=0;i<channels_in_use;i++) {
			if (attys_scope->channel[n][i]->isActive()) {
				num_channels++;	
			}
		}
	}
	if (!num_channels) {
		return;
	}
	int base=h/num_channels;
	if(w <= 0 || h <= 0) 
		return;
	paint.setPen(penWhite);
	int xer=xpos+5;
	if (xer>=w) {
		xer=xer-w;
	}
	paint.drawLine(xer,0,
		       xer,h);
	int act=1;
	for(int n=0;n<nComediDevices;n++) {
		float minV = -1.0;
		float maxV = 1.0;
		float dy=(float)base/(float)(maxV-minV);
		for(int i=0;i<channels_in_use;i++) {
			if (attys_scope->
			    channel[n][i]->
			    isActive()) {
				paint.setPen(penData[act%3]);
				float gain=attys_scope->gain[n][i]->getGain();
				float value = buffer[n][i] * gain;
				int yZero=base*act-(int)((0-minV)*dy);
				int yTmp=base*act-(int)((value-minV)*dy);
				ypos[n][i][xpos+1]=yTmp;
				paint.drawLine(xpos,ypos[n][i][xpos],
					       xpos+1,ypos[n][i][xpos+1]);
				if (xpos%2) {
					paint.drawPoint(xpos,yZero);
				}
				if ((xpos+2)==w) {
					ypos[n][i][0]=yTmp;
				}
				act++;
			}
		}
	}
	xpos++;
	if ((xpos+1)>=w) {
		xpos=0;
	}
}



//
// Handles paint events for the ComediScope widget.
// When the paint-event is triggered the averaging is done, the data is
// displayed and saved to disk.
//

void ComediScope::paintEvent( QPaintEvent * ) {


struct __attribute__((__packed__)) bin_data_t {
	uint32_t adc_ch1 : 24;
	uint32_t adc_ch2 : 24;
	
	uint8_t adc_gpio;
	uint8_t timestamp;

	uint16_t accel_x;
	uint16_t accel_y;
	uint16_t accel_z;

	uint16_t gyr_x;
	uint16_t gyr_y;
	uint16_t gyr_z;

	uint16_t mag_x;
	uint16_t mag_y;
	uint16_t mag_z;

} alldata;


	while (1) {
		// we need data in all of the comedi devices
		for(int n=0;n<nComediDevices;n++) {
			int fd=dev[n];
			fd_set fds;
			FD_ZERO(&fds);
			FD_SET(fd, &fds);
			struct timeval tv;
			tv.tv_sec = 0;
			tv.tv_usec = 0;
			if (select(fd+1, &fds, 0, 0, &tv)<1) return;
		}

		for(int n=0;n<nComediDevices;n++) {
			int buffer[20];
			
			char linebuffer[256];

			int ret = fscanf(stream[n],
					 "%s\n",
					 linebuffer);
			if (ret<0) {
				fclose(stream[n]);
				shutdown(dev[n],2);
				printf("Error during read. %d\n",ret);
				exit(1);
			}

			Base64decode((char*)(&alldata),linebuffer);

			buffer[0] = alldata.accel_x;
			buffer[1] = alldata.accel_y;
			buffer[2] = alldata.accel_z;

			buffer[3] = alldata.gyr_x;
			buffer[4] = alldata.gyr_y;
			buffer[5] = alldata.gyr_z;

			buffer[6] = alldata.mag_x;
			buffer[7] = alldata.mag_y;
			buffer[8] = alldata.mag_z;

			buffer[9] = alldata.adc_ch1;
			buffer[10] = alldata.adc_ch2;
			
			for(int i=0;i<channels_in_use;i++) {
				int sample;
				if (attys_scope->channel[n][i]->isActive()) {
					int ch = attys_scope->channel[n][i]->getChannel();
					sample = buffer[ch];
					// store raw data
					daqData[n][i] = sample;
					// convert data to physical units for plotting
					float value;
					value = normaliseData(sample,maxdata[n][i]);
					// filtering
					value = attys_scope->dcSub[n][i]->filter(value);
					value = attys_scope->hp[n][i]->filter(value);
					value = attys_scope->lp[n][i]->filter(value);
					// remove 50Hz
					if (attys_scope->filterCheckbox->checkState()==Qt::Checked) {
						value=iirnotch[n][i]->filter(value);
					}
					if ((n==fftdevno) && (ch==fftch) &&
					    (attys_scope->fftscope))
						attys_scope->fftscope->append(value);
					// average response if TB is slower than sampling rate
					adAvgBuffer[n][i] = adAvgBuffer[n][i] + value;
				}
			}
		}

		// save data
		if (attys_scope->recPushButton->checkState()==Qt::Checked) {
			writeFile();
		}

		nsamples++;
		tb_counter--;

		// enough averaged?
		if (tb_counter<=0) {
			for(int n=0;n<nComediDevices;n++) {
				for(int i=0;i<channels_in_use;i++) {
					adAvgBuffer[n][i]=adAvgBuffer[n][i]/tb_init;
				}
			}
		
			// plot the stuff
			paintData(adAvgBuffer);

			// clear buffer
			tb_counter=tb_init;
			for(int n=0;n<nComediDevices;n++) {
				for(int i=0;i<channels_in_use;i++) {
					adAvgBuffer[n][i]=0;
				}
			}
		}
	}
}


void ComediScope::setTB(int us) {
	tb_init=us/(1000000/sampling_rate);
	tb_counter=tb_init;
	for(int n=0;n<nComediDevices;n++) {
		for(int i=0;i<channels_in_use;i++) {
			adAvgBuffer[n][i]=0;
		}
	}
}

//
// Handles timer events for the ComediScope widget.
//

void ComediScope::timerEvent( QTimerEvent * )
{
	if (ext_data_receive) {
		ext_data_receive->readSocket();
	}
	repaint();
}

void ComediScope::clearScreen()
{
	eraseFlag = 1;
	repaint();
}
