#include <QTimer>
#include <QPainter>
#include <QApplication>
#include <QTimerEvent>
#include <QPaintEvent>

extern "C" {
#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<fcntl.h>
}

#include "scopewindow.h"

#include "special.h"

#ifdef _WIN32
#include <initguid.h>
DEFINE_GUID(g_guidServiceClass, 0xb62c4e8d, 0x62cc, 0x404b, 0xbb, 0xbf, 0xbf, 0x3e, 0x3b, 0xbb, 0x13, 0x74);
#endif


ScopeWindow::ScopeWindow(Attys_scope *attys_scope_tmp,
	float f
)
	: QWidget(attys_scope_tmp) {

	int maxComediDevs = 3;

	tb_init = 1;
	tb_counter = tb_init;
	attys_scope = attys_scope_tmp;
	// erase plot
	eraseFlag = 1;

	// for ASCII
	rec_file = NULL;

	// filename
	rec_filename = new QString();

	// flag if data has been recorded and we need a new filename
	recorded = 0;

	//////////////////////////////////////////////////////////////
	
	setAttribute(Qt::WA_OpaquePaintEvent);

	dev = new SOCKET[maxComediDevs];
	attysName = new char*[maxComediDevs];
	attysComm = new AttysComm*[maxComediDevs];
	for (int devNo = 0; devNo < maxComediDevs; devNo++) {
		dev[devNo] = 0;
		attysComm[devNo] = nullptr;
		attysName[devNo] = new char[256];
		attysName[devNo][0] = 0;
	}

	nAttysDevices = 0;
	channels_in_use = 0;

#ifdef __linux__

	inquiry_info *ii = NULL;
	int max_rsp, num_rsp;
	int dev_id, sock, len, flags;
	int i;
	char addr[19] = { 0 };
	char name[248] = { 0 };
	struct sockaddr_rc saddr;
	memset(&saddr,0,sizeof(struct sockaddr_rc));

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
	for(int i = 0;i<max_rsp;i++) {
		memset(ii+i,0,sizeof(inquiry_info));
	}
    
	num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
	if( num_rsp < 0 ) {
		perror("hci_inquiry");
		exit(1);
	}
		
	// let's probe how many we have
	nAttysDevices = 0;
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

			memset(&saddr,0,sizeof(struct sockaddr_rc));
			// set the connection parameters (who to connect to)
			saddr.rc_family = AF_BLUETOOTH;
			saddr.rc_channel = (uint8_t) 1;
			str2ba( addr, &saddr.rc_bdaddr );

			// connect to server
			int status = ::connect(s,
					       (struct sockaddr *)&saddr,
					       sizeof(saddr));
			
			if (status == 0) {
				attysComm[nAttysDevices] = new AttysComm(s);
				sprintf(attysName[nAttysDevices], "%d:%s", nAttysDevices, name);
				channels_in_use = attysComm[nAttysDevices]->NCHANNELS;
				nAttysDevices++;
				break;
			} else {
				printf("Connect failed: %d\n",status);
				printf("Has the device been paired?\n");
			}
		} else {
			printf("\n");
		}
	}

	delete[] ii;
	
	
#elif _WIN32

	WSADATA wsd;
	WSAStartup(MAKEWORD(2, 2), &wsd);

	WSAQUERYSET wsaq;
	ZeroMemory(&wsaq, sizeof(wsaq));
	wsaq.dwSize = sizeof(wsaq);
	wsaq.dwNameSpace = NS_BTH;
	wsaq.lpcsaBuffer = NULL;
	HANDLE hLookup = nullptr;
	int iRet = WSALookupServiceBegin(&wsaq, LUP_CONTAINERS, &hLookup);
	if (0 != iRet) {
		if (WSAGetLastError() != WSASERVICE_NOT_FOUND) {
			_RPT0(0,"WSALookupServiceBegin failed\n");
			exit(1);
		}
		else {
			_RPT0(0,"No bluetooth devices found\n");
			exit(1);
		}
	}

	CHAR buf[4096];
	LPWSAQUERYSET pwsaResults = (LPWSAQUERYSET)buf;
	ZeroMemory(pwsaResults, sizeof(WSAQUERYSET));
	pwsaResults->dwSize = sizeof(WSAQUERYSET);
	pwsaResults->dwNameSpace = NS_BTH;
	pwsaResults->lpBlob = NULL;

	DWORD dwSize = sizeof(buf);
	while (WSALookupServiceNext(hLookup, LUP_RETURN_NAME | LUP_RETURN_ADDR, &dwSize, pwsaResults) == 0) {
		LPWSTR name = pwsaResults->lpszServiceInstanceName;
		if (wcsstr(name,L"GN-ATTYS1") != 0) {
			_RPT0(0,"Found an Attys!\n");

			char tmp[256];
			sprintf(tmp, "Connecting to Attys #%d: %S", nAttysDevices,name);
			attys_scope->splash->showMessage(tmp);

			for (int i = 0; i < 5; i++) {

				// allocate a socket
				SOCKET s = ::socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);

				if (INVALID_SOCKET == s) {
					_RPT0(0,"=CRITICAL= | socket() call failed.\n");
					exit(1);
				}

				PSOCKADDR_BTH btAddr = (SOCKADDR_BTH *)(pwsaResults->lpcsaBuffer->RemoteAddr.lpSockaddr);
				btAddr->addressFamily = AF_BTH;
				btAddr->serviceClassId = RFCOMM_PROTOCOL_UUID;
				btAddr->port = BT_PORT_ANY;

				int btAddrLen = pwsaResults->lpcsaBuffer->RemoteAddr.iSockaddrLength;

				// connect to server
				int status = ::connect(s, (struct sockaddr *)btAddr, btAddrLen);

				if (status == 0) {

					attysComm[nAttysDevices] = new AttysComm(s);
					sprintf(attysName[nAttysDevices], "#%d: %S", nAttysDevices, name);
					channels_in_use = attysComm[nAttysDevices]->NCHANNELS;
					nAttysDevices++;
					break;
				}
				else {
					_RPT0(0,"Connect failed\n");
					_RPT0(0,"Has the device been paired?\n");
					shutdown(s, SD_BOTH);
					closesocket(s);
				}
			}
		} else {
			_RPT0(0,"\n");
		}
	}

	WSALookupServiceEnd(hLookup);


#else

#endif

	printf("%d rfcomm devices detected\n",nAttysDevices);
	printf("%d channels in use\n",channels_in_use);
	
	// none detected
	if (nAttysDevices<1) {
		_RPT0(0,"No rfcomm devices detected!\n");
		attys_scope->splash->showMessage("Cound not connect and/or no devices paired.");
		Sleep(1000);
		exit(1);
	}

	assert(channels_in_use > 0);

	// initialise the graphics stuff
	ypos = new int**[nAttysDevices];
	assert(ypos != NULL);
	minV = new float*[nAttysDevices];
	maxV = new float*[nAttysDevices];
	for(int devNo=0;devNo<nAttysDevices;devNo++) {
		ypos[devNo]=new int*[channels_in_use];
		minV[devNo] = new float[channels_in_use];
		maxV[devNo] = new float[channels_in_use];
		assert(ypos[devNo] != NULL);
		for(int i=0;i<channels_in_use;i++) {
			minV[devNo][i] = -10;
			maxV[devNo][i] = 10;
			ypos[devNo][i] = new int[MAX_DISP_X];
			assert( ypos[devNo][i] != NULL);
			for(int j=0;j<MAX_DISP_X;j++) {
				ypos[devNo][i][j]=0;
			}
		}
	}

	xpos=0;
	nsamples=0;

	// 50Hz or 60Hz mains notch filter
	iirnotch = new Iir::Butterworth::BandStop<IIRORDER>**[nAttysDevices];
	assert( iirnotch != NULL );
	adAvgBuffer = new float*[nAttysDevices];
	assert( adAvgBuffer != NULL );
	daqData = new float*[nAttysDevices];
	assert( daqData != NULL );
	for(int devNo=0;devNo<nAttysDevices;devNo++) {
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
		daqData[devNo] = new float[channels_in_use];
		assert( daqData[devNo] != NULL );
		for(int i=0;i<channels_in_use;i++) {
			daqData[devNo][i]=0;
		}
	}
	setNotchFrequency(f);
}


void ScopeWindow::startDAQ() {

	for (int i = 0; i < nAttysDevices; i++) {
		if (attysComm[i]) {
			attysComm[i]->setBiasCurrent(attys_scope->current[i]->getCurrent());
		}
		int curr_ch1 = 0;
		int curr_ch2 = 0;
		switch (attys_scope->special[i][0]->getSpecial()) {
		case SPECIAL_NORMAL:
			attysComm[i]->setAdc0_mux_index(attysComm[i]->ADC_MUX_NORMAL);
			break;
		case SPECIAL_ECG:
			attysComm[i]->setAdc0_mux_index(attysComm[i]->ADC_MUX_ECG_EINTHOVEN);
			break;
		case SPECIAL_I:
			attysComm[i]->setAdc0_mux_index(attysComm[i]->ADC_MUX_NORMAL);
			curr_ch1 = 1;
			break;
		}
		switch (attys_scope->special[i][1]->getSpecial()) {
		case SPECIAL_NORMAL:
			attysComm[i]->setAdc1_mux_index(attysComm[i]->ADC_MUX_NORMAL);
			break;
		case SPECIAL_ECG:
			attysComm[i]->setAdc1_mux_index(attysComm[i]->ADC_MUX_ECG_EINTHOVEN);
			break;
		case SPECIAL_I:
			attysComm[i]->setAdc1_mux_index(attysComm[i]->ADC_MUX_NORMAL);
			curr_ch2 = 1;
			break;
		}
		attysComm[i]->enableCurrents(curr_ch1,0,curr_ch2);
	}

	// ready steady go!
	counter = new QTimer(this);
	assert(counter != NULL);
	connect(counter,
		SIGNAL(timeout()),
		this,
		SLOT(updateTime()));

	startTimer(50);		// run continuous timer
	counter->start(500);
	for (int i = 0; i < nAttysDevices; i++) {
		if (attysComm[i])
			attysComm[i]->start();
	}

	for (int n = 0; n < nAttysDevices; n++) {

		for (int i = attysComm[n]->INDEX_Acceleration_X; i <= attysComm[n]->INDEX_Acceleration_Z; i++) {
			minV[n][i] = -attysComm[n]->getAccelFullScaleRange();
			maxV[n][i] = attysComm[n]->getAccelFullScaleRange();
		}
		for (int i = attysComm[n]->INDEX_Magnetic_field_X; i <= attysComm[n]->INDEX_Magnetic_field_Z; i++) {
			minV[n][i] = -attysComm[n]->getMagFullScaleRange();
			maxV[n][i] = attysComm[n]->getMagFullScaleRange();
		}
		minV[n][attysComm[n]->INDEX_Analogue_channel_1] = - attysComm[n]->getADCFullScaleRange(0);
		maxV[n][attysComm[n]->INDEX_Analogue_channel_1] = attysComm[n]->getADCFullScaleRange(0);
		_RPT1(0, "ADC1 max = %f\n", attysComm[n]->getADCFullScaleRange(0));
		minV[n][attysComm[n]->INDEX_Analogue_channel_2] = - attysComm[n]->getADCFullScaleRange(1);
		maxV[n][attysComm[n]->INDEX_Analogue_channel_2] = attysComm[n]->getADCFullScaleRange(1);
		_RPT1(0, "ADC2 max = %f\n", attysComm[n]->getADCFullScaleRange(1));
	}

}


ScopeWindow::~ScopeWindow() {
	if (rec_file) {
		fclose(rec_file);
	}
	for(int i=0; i<nAttysDevices;i++) {
		if (attysComm[i]) {
			attysComm[i]->quit();
		}
	}
}

void ScopeWindow::setNotchFrequency(float f) {
	if (f>(attysComm[0]->getSamplingRateInHz()/2)) {
		fprintf(stderr,
			"Error: The notch frequency %f "
			"is higher than the nyquist freq of %dHz.\n",
			f, attysComm[0]->getSamplingRateInHz()/2);
		return;
	}
	if (f <= 0) return;
	for(int j=0;j<nAttysDevices;j++) {
		for(int i=0;i<channels_in_use;i++) {
			float frequency_width = f/10;
			iirnotch[j][i]->setup (IIRORDER, 
				attysComm[0]->getSamplingRateInHz(),
					       f, 
					       frequency_width);
		}
		notchFrequency = f;
	}
	_RPT1(0, "Notch set to %f Hz.\n", f);
}



void ScopeWindow::updateTime() {
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
			QString().sprintf("--- rec: %ldsec", nsamples/ attysComm[0]->getSamplingRateInHz());
	}
	attys_scope->setWindowTitle( s );

	char tmp[256];
	for(int n=0;n<nAttysDevices;n++) {
		for(int i=0;i<channels_in_use;i++) {
			if (attys_scope->channel[n][i]->isActive()) {
				float phys = daqData[n][attys_scope->channel[n][i]->getChannel()];
				sprintf(tmp, VOLT_FORMAT_STRING, phys);
			}
			else {
				sprintf(tmp, "---");
			}
			attys_scope->voltageTextEdit[n][i]->setText(tmp);
		}
	}
}


void ScopeWindow::setFilename(QString name,int tsv) {
	(*rec_filename)=name;
	recorded=0;
	if (tsv) {
		separator='\t';
	} else {
		separator=',';
	}
}


void ScopeWindow::writeFile() {
	if (!rec_file) return;
	fprintf(rec_file, "%f", ((float)nsamples) / ((float)attysComm[0]->getSamplingRateInHz()));
	for (int n = 0; n < nAttysDevices; n++) {
		for (int i = 0; i < channels_in_use; i++) {
			if (attys_scope->
				channel[n][i]->isActive()
				) {
				float phy = daqData[n][attys_scope->channel[n][i]->getChannel()];
				fprintf(rec_file, "%c%f", separator, phy);
			}
		}
	}
	fprintf(rec_file, "\n");
}

void ScopeWindow::startRec() {
	if (recorded) return;
	if (rec_filename->isEmpty()) return;
	attys_scope->disableControls();
	// counter for samples
	nsamples=0;
	// get possible comments
	QString comment = attys_scope->notchTextEdit->toPlainText();
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
			"Writing failed.\n");
	}
	// print comment
	if ((rec_file)&&(!comment.isEmpty())) {
		fprintf(rec_file,
			"# %s\n",
			comment.toLocal8Bit().constData());
	}
}


void ScopeWindow::stopRec() {
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



void ScopeWindow::paintData(float** buffer) {
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
	
	for(int n=0;n<nAttysDevices;n++) {
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
	for (int n = 0; n < nAttysDevices; n++) {
		for (int i = 0; i < channels_in_use; i++) {
			if (attys_scope->
				channel[n][i]->
				isActive()) {
				float dy = (float)base / (float)(maxV[n][attys_scope->channel[n][i]->getChannel()] 
					- minV[n][attys_scope->channel[n][i]->getChannel()]);
				paint.setPen(penData[act % 3]);
				float gain = attys_scope->gain[n][i]->getGain();
				float value = buffer[n][i] * gain;
				//if (i == 6 ) 
				//	_RPT1(0, "%f\n", value);
				int yZero = base*act - (int)((0 - minV[n][attys_scope->channel[n][i]->getChannel()])*dy);
				int yTmp = base*act - (int)((value - minV[n][attys_scope->channel[n][i]->getChannel()])*dy);
				ypos[n][i][xpos + 1] = yTmp;
				paint.drawLine(xpos, ypos[n][i][xpos],
					xpos + 1, ypos[n][i][xpos + 1]);
				if (xpos % 2) {
					paint.drawPoint(xpos, yZero);
				}
				if ((xpos + 2) == w) {
					ypos[n][i][0] = yTmp;
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

void ScopeWindow::paintEvent(QPaintEvent *) {

	for (;;) {

		for (int n = 0; n < nAttysDevices; n++) {
			int hasSample = attysComm[n]->hasSampleAvilabale();
			if (!hasSample) return;
		}

		for (int n = 0; n < nAttysDevices; n++) {
			float* values = attysComm[n]->getSampleFromBuffer();
			for (int i = 0; i < channels_in_use; i++) {
				daqData[n][i] = values[i];
				if (attys_scope->channel[n][i]->isActive()) {
					// filtering
					float value = values[attys_scope->channel[n][i]->getChannel()];
//					if (i == 6)
//					_RPT1(0, "%f\n",value);
					value = attys_scope->dcSub[n][i]->filter(value);
					value = attys_scope->hp[n][i]->filter(value);
					value = attys_scope->lp[n][i]->filter(value);
					// remove 50Hz
					if (attys_scope->filterCheckbox->checkState() == Qt::Checked) {
						value = iirnotch[n][i]->filter(value);
					}
					// average response if TB is slower than sampling rate
					adAvgBuffer[n][i] = adAvgBuffer[n][i] + value;
				}
			}
		}

		// save data
		if (attys_scope->recPushButton->checkState() == Qt::Checked) {
			writeFile();
		}

		nsamples++;
		tb_counter--;

		// enough averaged?
		if (tb_counter <= 0) {
			for (int n = 0; n < nAttysDevices; n++) {
				for (int i = 0; i < channels_in_use; i++) {
					adAvgBuffer[n][i] = adAvgBuffer[n][i] / tb_init;
				}
			}

			// plot the stuff
			paintData(adAvgBuffer);

			// clear buffer
			tb_counter = tb_init;
			for (int n = 0; n < nAttysDevices; n++) {
				for (int i = 0; i < channels_in_use; i++) {
					adAvgBuffer[n][i] = 0;
				}
			}
		}
	}
}


void ScopeWindow::setTB(int us) {
	tb_init=us/(1000000/ attysComm[0]->getSamplingRateInHz());
	tb_counter=tb_init;
	for(int n=0;n<nAttysDevices;n++) {
		for(int i=0;i<channels_in_use;i++) {
			adAvgBuffer[n][i]=0;
		}
	}
}

//
// Handles timer events for the ComediScope widget.
//

void ScopeWindow::timerEvent( QTimerEvent * )
{
	repaint();
}

void ScopeWindow::clearScreen()
{
	eraseFlag = 1;
	repaint();
}
