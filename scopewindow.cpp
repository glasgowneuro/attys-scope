#include <QTimer>
#include <QPainter>
#include <QApplication>
#include <QTimerEvent>
#include <QPaintEvent>
#include <QMessageBox>

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


ScopeWindow::ScopeWindow(Attys_scope *attys_scope_tmp)
	: QWidget(attys_scope_tmp) {

	display_data = 1;

	tb_init = 1;
	tb_counter = tb_init;
	attys_scope = attys_scope_tmp;
	// erase plot
	eraseFlag = 1;

	// for ASCII
	rec_file = NULL;

	// filename
	rec_filename = NULL;

	// flag if data has been recorded and we need a new filename
	recorded = 0;

	//////////////////////////////////////////////////////////////
	
	setAttribute(Qt::WA_OpaquePaintEvent);

	// points to itself for the message listener
	attysScopeCommMessage.scopeWindow = this;

	// initialise the graphics stuff
	ypos = new int**[attysScan.nAttysDevices];
	assert(ypos != NULL);
	minV = new float*[attysScan.nAttysDevices];
	maxV = new float*[attysScan.nAttysDevices];
	for(int devNo=0;devNo<attysScan.nAttysDevices;devNo++) {
		attysScan.attysComm[devNo]->registerMessageCallback(&attysScopeCommMessage);
		ypos[devNo]=new int*[AttysComm::NCHANNELS];
		minV[devNo] = new float[AttysComm::NCHANNELS];
		maxV[devNo] = new float[AttysComm::NCHANNELS];
		assert(ypos[devNo] != NULL);
		for(int i=0;i<AttysComm::NCHANNELS;i++) {
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

	adAvgBuffer = new float*[attysScan.nAttysDevices];
	assert( adAvgBuffer != NULL );
	unfiltDAQData = new float*[attysScan.nAttysDevices];
	filtDAQData = new float*[attysScan.nAttysDevices];
	assert( unfiltDAQData != NULL );
	assert(filtDAQData != NULL);
	for(int devNo=0;devNo<attysScan.nAttysDevices;devNo++) {
		// floating point buffer for plotting
		adAvgBuffer[devNo]=new float[AttysComm::NCHANNELS];
		assert( adAvgBuffer[devNo] != NULL );
		for(int i=0;i<AttysComm::NCHANNELS;i++) {
			adAvgBuffer[devNo][i]=0;
		}
		// raw data buffer for saving the data
		unfiltDAQData[devNo] = new float[AttysComm::NCHANNELS];
		filtDAQData[devNo] = new float[AttysComm::NCHANNELS];
		assert( unfiltDAQData[devNo] != NULL );
		assert(filtDAQData[devNo] != NULL);
		for(int i=0;i<AttysComm::NCHANNELS;i++) {
			unfiltDAQData[devNo][i]=0;
			filtDAQData[devNo][i] = 0;
		}
	}
}


void ScopeWindow::startDAQ() {

	for (int i = 0; i < attysScan.nAttysDevices; i++) {
		if (attysScan.attysComm[i]) {
			attysScan.attysComm[i]->setBiasCurrent(attys_scope->current[i]->getCurrent());
			int curr_ch1 = 0;
			int curr_ch2 = 0;

			switch (attys_scope->special[i][0]->getSpecial()) {
			case SPECIAL_NORMAL:
				attysScan.attysComm[i]->setAdc0_mux_index(attysScan.attysComm[i]->ADC_MUX_NORMAL);
				break;
			case SPECIAL_ECG:
				attysScan.attysComm[i]->setAdc0_mux_index(attysScan.attysComm[i]->ADC_MUX_ECG_EINTHOVEN);
				break;
			case SPECIAL_TEMPERATURE:
				attysScan.attysComm[i]->setAdc0_mux_index(attysScan.attysComm[i]->ADC_MUX_TEMPERATURE);
				break;
			case SPECIAL_I:
				attysScan.attysComm[i]->setAdc0_mux_index(attysScan.attysComm[i]->ADC_MUX_NORMAL);
				curr_ch1 = 1;
				break;
			default:
				attysScan.attysComm[i]->setAdc0_mux_index(attysScan.attysComm[i]->ADC_MUX_NORMAL);
			}
			attysScan.attysComm[i]->setAdc0_gain_index(attys_scope->special[i][0]->getGainIndex());

			switch (attys_scope->special[i][1]->getSpecial()) {
			case SPECIAL_NORMAL:
				attysScan.attysComm[i]->setAdc1_mux_index(attysScan.attysComm[i]->ADC_MUX_NORMAL);
				break;
			case SPECIAL_ECG:
				attysScan.attysComm[i]->setAdc1_mux_index(attysScan.attysComm[i]->ADC_MUX_ECG_EINTHOVEN);
				break;
			case SPECIAL_TEMPERATURE:
				attysScan.attysComm[i]->setAdc1_mux_index(attysScan.attysComm[i]->ADC_MUX_TEMPERATURE);
				break;
			case SPECIAL_I:
				attysScan.attysComm[i]->setAdc1_mux_index(attysScan.attysComm[i]->ADC_MUX_NORMAL);
				curr_ch2 = 1;
				break;
			default:
				attysScan.attysComm[i]->setAdc1_mux_index(attysScan.attysComm[i]->ADC_MUX_NORMAL);
			}
			attysScan.attysComm[i]->setAdc1_gain_index(attys_scope->special[i][1]->getGainIndex());

			attysScan.attysComm[i]->enableCurrents(curr_ch1, 0, curr_ch2);
		}
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
	for (int i = 0; i < attysScan.nAttysDevices; i++) {
		if (attysScan.attysComm[i])
			attysScan.attysComm[i]->start();
	}

	for (int n = 0; n < attysScan.nAttysDevices; n++) {

		for (int i = attysScan.attysComm[n]->INDEX_Acceleration_X; i <= attysScan.attysComm[n]->INDEX_Acceleration_Z; i++) {
			minV[n][i] = -attysScan.attysComm[n]->getAccelFullScaleRange();
			maxV[n][i] = attysScan.attysComm[n]->getAccelFullScaleRange();
		}
		for (int i = attysScan.attysComm[n]->INDEX_Magnetic_field_X; i <= attysScan.attysComm[n]->INDEX_Magnetic_field_Z; i++) {
			minV[n][i] = -attysScan.attysComm[n]->getMagFullScaleRange();
			maxV[n][i] = attysScan.attysComm[n]->getMagFullScaleRange();
		}
		if (attys_scope->special[n][0]->getSpecial() == SPECIAL_TEMPERATURE) {
			minV[n][attysScan.attysComm[n]->INDEX_Analogue_channel_1] = -20;
			maxV[n][attysScan.attysComm[n]->INDEX_Analogue_channel_1] = 80;
		}
		else {
			minV[n][attysScan.attysComm[n]->INDEX_Analogue_channel_1] = -attysScan.attysComm[n]->getADCFullScaleRange(0);
			maxV[n][attysScan.attysComm[n]->INDEX_Analogue_channel_1] = attysScan.attysComm[n]->getADCFullScaleRange(0);
		}
		// _RPT1(0, "ADC1 max = %f\n", attysScan.attysComm[n]->getADCFullScaleRange(0));
		if (attys_scope->special[n][1]->getSpecial() == SPECIAL_TEMPERATURE) {
			minV[n][attysScan.attysComm[n]->INDEX_Analogue_channel_2] = -20;
			maxV[n][attysScan.attysComm[n]->INDEX_Analogue_channel_2] = 80;
		}
		else {
			minV[n][attysScan.attysComm[n]->INDEX_Analogue_channel_2] = -attysScan.attysComm[n]->getADCFullScaleRange(1);
			maxV[n][attysScan.attysComm[n]->INDEX_Analogue_channel_2] = attysScan.attysComm[n]->getADCFullScaleRange(1);
		}
		// GPIO channel 1 min/max range
		minV[n][attysScan.attysComm[n]->INDEX_GPIO0] = -1;
		maxV[n][attysScan.attysComm[n]->INDEX_GPIO0] = 1;
		// GPIO channel 2 min/max range
		minV[n][attysScan.attysComm[n]->INDEX_GPIO1] = -1;
		maxV[n][attysScan.attysComm[n]->INDEX_GPIO1] = 1;
		// _RPT1(0, "ADC2 max = %f\n", attysScan.attysComm[n]->getADCFullScaleRange(1));
	}

}

ScopeWindow::~ScopeWindow() {
	if (rec_file) {
		fclose(rec_file);
	}
	for(int i=0; i<attysScan.nAttysDevices;i++) {
		if (attysScan.attysComm[i]) {
			attysScan.attysComm[i]->quit();
		}
	}
}

// called after an Attys has re-connected
void ScopeWindow::attysHasReconnected() {
	_RPT0(0, "Attys has reconnected. Clearing ringbuffers.\n");
	for (int n = 0; n < attysScan.nAttysDevices; n++) {
		attysScan.attysComm[n]->resetRingbuffer();
	}
}

void ScopeWindow::updateTime() {
	QString s;
	if (!rec_file) {
		if (rec_filename == NULL) {
			s.sprintf(EXECUTABLE_NAME " " VERSION);
		} else {
			if (recorded) {
				s=(*rec_filename)+" --- file saved";
			} else {
				s=(*rec_filename)+" --- press REC to record ";
			}
		}
	} else {
		if (rec_filename) {
			s = (*rec_filename) +
				QString().sprintf("--- rec: %ldsec", nsamples / attysScan.attysComm[0]->getSamplingRateInHz());
		}
	}
	attys_scope->setWindowTitle( s );

	char tmp[256];
	for(int n=0;n<attysScan.nAttysDevices;n++) {
		for(int i=0;i<AttysComm::NCHANNELS;i++) {
			if (attys_scope->channel[n][i]->isActive()) {
				float phys = unfiltDAQData[n][attys_scope->channel[n][i]->getChannel()];
				sprintf(tmp, VOLT_FORMAT_STRING, phys);
			}
			else {
				sprintf(tmp, "---");
			}
			attys_scope->voltageTextEdit[n][i]->setText(tmp);
		}
	}

	if (udpSocket) {
		if (udpStatus < 0) {
			attys_scope->setInfo(" UDP broadcast error");
		}
	} else {
		attys_scope->setInfo("");
	}

}


void ScopeWindow::setFilename(QString name,int tsv) {
	if (rec_filename) delete rec_filename;
	rec_filename = new QString(name);
	recorded=0;
	if (tsv) {
		separator='\t';
	} else {
		separator=',';
	}
}


void ScopeWindow::writeFile() {
	if (!rec_file) return;
	fprintf(rec_file, "%f", ((float)nsamples) / ((float)attysScan.attysComm[0]->getSamplingRateInHz()));
	for (int n = 0; n < attysScan.nAttysDevices; n++) {
		int nFiltered = 0;
		for (int i = 0; i < AttysComm::NCHANNELS; i++) {
			if (attys_scope->channel[n][i]->isActive()) {
				nFiltered++;
			}
			float phy = unfiltDAQData[n][i];
			fprintf(rec_file, "%c%f", separator, phy);
		}
		for (int i = 0; i < nFiltered; i++) {
			float phy = filtDAQData[n][i];
			fprintf(rec_file, "%c%f", separator, phy);
		}
	}
	fprintf(rec_file, "\n");
}


void ScopeWindow::startUDP(int port)
{
	stopUDP();
	udpSocket = new QUdpSocket(this);
	udpPort = port;
	_RPT1(0, "UDP transmit on port %d\n",port);
}

void ScopeWindow::stopUDP()
{
	if (udpSocket != NULL) {
		delete udpSocket;
	}
	udpSocket = NULL;
}


void ScopeWindow::writeUDP() {
	if (!udpSocket) return;
	char tmp[1024];
	sprintf(tmp, "%f", ((float)nsamples) / ((float)attysScan.attysComm[0]->getSamplingRateInHz()));
	for (int n = 0; n < attysScan.nAttysDevices; n++) {
		int nFiltered = 0;
		for (int i = 0; i < AttysComm::NCHANNELS; i++) {
			if (attys_scope->channel[n][i]->isActive()) {
				nFiltered++;
			}
			float phy = unfiltDAQData[n][i];
			sprintf(tmp+strlen(tmp), ",%f", phy);
		}
		for (int i = 0; i < nFiltered; i++) {
			float phy = filtDAQData[n][i];
			sprintf(tmp+strlen(tmp), ",%f", phy);
		}
		for (int i = 0; i < (AttysComm::NCHANNELS - nFiltered); i++) {
			float phy = 0;
			sprintf(tmp + strlen(tmp), ",%f", phy);
		}
	}
	sprintf(tmp+strlen(tmp), "\n");
	if (udpSocket) {
		if (udpStatus > -1) {
#ifdef __linux__
			// thre seems to be a bug either in QT or deeper down that broadcast packets
			// won't work under Linux so we send them just to the localhost. That works!
			udpStatus = udpSocket->writeDatagram(tmp, strlen(tmp), QHostAddress("127.0.0.1"), udpPort);
#else
			udpStatus = udpSocket->writeDatagram(tmp, strlen(tmp), QHostAddress::Broadcast, udpPort);
#endif
			//_RPT1(0, "%s", tmp);
		}
	}
}

void ScopeWindow::startRec() {
	if (recorded) return;
	if (rec_filename == NULL) return;
	attys_scope->disableControls();
	// counter for samples
	nsamples = 0;
	rec_file = fopen(rec_filename->toLocal8Bit().constData(),
		"wt");
	// could we open it?
	if (!rec_file) {
		// could not open
		delete rec_filename;
		rec_filename = NULL;
		// print error msg
		fprintf(stderr,
			"Writing failed.\n");
		QMessageBox msgBox;
		msgBox.setText("File could not be saved: " + QString(strerror(errno)));
		msgBox.exec();
		attys_scope->recCheckBox->setChecked(0);
		attys_scope->recCheckBox->setEnabled(0);
		recorded = 0;
		attys_scope->enableControls();
		if (rec_filename) delete rec_filename;
		rec_filename = NULL;
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
	rec_filename = NULL;
}



void ScopeWindow::paintData(float** buffer) {
	QPainter paint( this );
	QPen penData[3]={QPen(QColor(0,0,128),1),
			 QPen(QColor(0,128,0),1),
			 QPen(QColor(128,0,0),1)};
	QPen penWhite(Qt::white,2);
	int w = width();
	int h = height();
	if (eraseFlag) {
		paint.fillRect(0,0,w,h,QColor(255,255,255));
		eraseFlag = 0;
		xpos = 0;
	}
	num_channels=0;
	
	for(int n=0;n<attysScan.nAttysDevices;n++) {
		for(int i=0;i<AttysComm::NCHANNELS;i++) {
			if (attys_scope->channel[n][i]->isActive()) {
				num_channels++;	
			}
		}
	}
	if (!num_channels) {
		return;
	}

	if (attys_scope->displayCheckbox->isChecked()) {
		display_data = 1;
	}

	if (!display_data) {
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
	for (int n = 0; n < attysScan.nAttysDevices; n++) {
		for (int i = 0; i < AttysComm::NCHANNELS; i++) {
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
		if (!(attys_scope->displayCheckbox->isChecked())) {
			display_data = 0;
		}
	}
}



//
// Handles paint events for the ComediScope widget.
// When the paint-event is triggered the averaging is done, the data is
// displayed and saved to disk.
//

void ScopeWindow::paintEvent(QPaintEvent *) {

	for (;;) {

		for (int n = 0; n < attysScan.nAttysDevices; n++) {
			int hasSample = attysScan.attysComm[n]->hasSampleAvilabale();
			if (!hasSample) return;
		}

		for (int n = 0; n < attysScan.nAttysDevices; n++) {
			float* values = attysScan.attysComm[n]->getSampleFromBuffer();
			if (attys_scope->special[n][0]->getSpecial() == SPECIAL_TEMPERATURE) {
				values[attysScan.attysComm[n]->INDEX_Analogue_channel_1] = AttysComm::phys2temperature(values[attysScan.attysComm[n]->INDEX_Analogue_channel_1]);
			}
			if (attys_scope->special[n][1]->getSpecial() == SPECIAL_TEMPERATURE) {
				values[attysScan.attysComm[n]->INDEX_Analogue_channel_2] = AttysComm::phys2temperature(values[attysScan.attysComm[n]->INDEX_Analogue_channel_2]);
			}
			int nFiltered = 0;
			for (int i = 0; i < AttysComm::NCHANNELS; i++) {
				unfiltDAQData[n][i] = values[i];
				if (attys_scope->channel[n][i]->isActive()) {
					// filtering
					float value = values[attys_scope->channel[n][i]->getChannel()];
//					if (i == 6)
//					_RPT1(0, "%f\n",value);
					value = attys_scope->highpass[n][i]->filter(value);
					value = attys_scope->lowpass[n][i]->filter(value);
					value = attys_scope->bandstop[n][i]->filter(value);
					filtDAQData[n][nFiltered++] = value;
					// average response if TB is slower than sampling rate
					adAvgBuffer[n][i] = adAvgBuffer[n][i] + value;
				}
			}
		}

		// save data
		if (attys_scope->recCheckBox->checkState() == Qt::Checked) {
			writeFile();
		}

		writeUDP();

		nsamples++;
		tb_counter--;

		// enough averaged?
		if (tb_counter <= 0) {
			for (int n = 0; n < attysScan.nAttysDevices; n++) {
				for (int i = 0; i < AttysComm::NCHANNELS; i++) {
					adAvgBuffer[n][i] = adAvgBuffer[n][i] / tb_init;
				}
			}

			// plot the stuff
			paintData(adAvgBuffer);

			// clear buffer
			tb_counter = tb_init;
			for (int n = 0; n < attysScan.nAttysDevices; n++) {
				for (int i = 0; i < AttysComm::NCHANNELS; i++) {
					adAvgBuffer[n][i] = 0;
				}
			}
		}
	}
}


void ScopeWindow::setTB(int us) {
	tb_init=us/(1000000/ attysScan.attysComm[0]->getSamplingRateInHz());
	tb_counter=tb_init;
	for(int n=0;n<attysScan.nAttysDevices;n++) {
		for(int i=0;i<AttysComm::NCHANNELS;i++) {
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
