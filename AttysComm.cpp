#include "AttysComm.h"


AttysComm::AttysComm(SOCKET _btsocket)
{
	btsocket = _btsocket;

	adcMuxRegister = new int[2];
	adcMuxRegister[0] = 0;
	adcMuxRegister[1] = 0;
	adcGainRegister = new int[2];
	adcGainRegister[0] = 0;
	adcGainRegister[1] = 0;
	adcCurrNegOn = new int[2];
	adcCurrNegOn[0] = 0;
	adcCurrNegOn[1] = 0;
	adcCurrPosOn = new int[2];
	adcCurrPosOn[0] = 0;
	adcCurrNegOn[1] = 0;

	ringBuffer = new float*[nMem];
	for (int i = 0; i < nMem; i++) {
		ringBuffer[i] = new float[NCHANNELS];
	}

	data = new long[NCHANNELS];
	raw = new char[256];
	sample = new float[NCHANNELS];
	inbuffer = new char[100000];
	strcpy(inbuffer, "");
	doRun = 0;
	inPtr = 0;
	outPtr = 0;
	adc_rate_index = ADC_DEFAULT_RATE;
	timestamp = 0;
	adc0_gain_index = ADC_GAIN_1;
	adc1_gain_index = ADC_GAIN_1;
	adc0_mux_index = ADC_MUX_NORMAL;
	adc1_mux_index = ADC_MUX_NORMAL;
	accel_full_scale_index = ACCEL_16G;
}


AttysComm::~AttysComm() {
#ifdef __linux__ 
	close(btsocket);
#elif _WIN32
	shutdown(btsocket, SD_BOTH);
	closesocket(btsocket);
#else
#endif
	doRun = 0;
}


/////////////////////////////////////////////////
// ringbuffer keeping data for chunk-wise plotting
float* AttysComm::getSampleFromBuffer() {
	if (inPtr != outPtr) {
		float* sample = ringBuffer[outPtr];
		outPtr++;
		if (outPtr == nMem) {
			outPtr = 0;
		}
		return sample;
	} else {
		// cause segfault, should never happen
		return nullptr;
	}
}



void AttysComm::sendSyncCommand(const char *message, int waitForOK) {
	// Put the socket in non-blocking mode:
	char cr[] = "\n";
	_RPT1(0,"Sending: %s",message);
	send(btsocket, message, (int)strlen(message), 0);
	if (!waitForOK) {
		return;
	}
	for (int k = 0; k < 10; k++) {
		send(btsocket, message, (int)strlen(message), 0);
		send(btsocket, cr, (int)strlen(cr), 0);
		for (int i = 0; i < 100; i++) {
			Sleep(1);
			char linebuffer[8192];
			int ret = recv(btsocket, linebuffer, 8191, 0);
			if (ret > 1) {
				linebuffer[ret] = 0;
				if (strstr(linebuffer, "OK")) {
					_RPT1(0, " -- OK received: %s \n",linebuffer);
					return;
				}
			}
		}
		_RPT0(0, "-- no OK received!\n");
	}
}



void AttysComm::sendSamplingRate() {
	char tmp[256];
	sprintf(tmp, "r=%d", adc_rate_index);
	sendSyncCommand(tmp, 1);
}

void AttysComm::sendFullscaleAccelRange() {
	char tmp[256];
	sprintf(tmp, "t=%d", accel_full_scale_index);
	sendSyncCommand(tmp, 1);
}

void AttysComm::sendCurrentMask() {
	char tmp[256];
	sprintf(tmp, "c=%d", current_mask);
	sendSyncCommand(tmp, 1);
}

void AttysComm::sendBiasCurrent() {
	char tmp[256];
	sprintf(tmp, "i=%d", current_index);
	sendSyncCommand(tmp, 1);
}

void AttysComm::sendGainMux(int channel, int gain, int mux) {
	char tmp[256];
	int v = (mux & 0x0f) | ((gain & 0x0f) << 4);
	switch (channel) {
	case 0:
		sprintf(tmp, "a=%d", v);
		sendSyncCommand(tmp, 1);
		break;
	case 1:
		sprintf(tmp, "b=%d", v);
		sendSyncCommand(tmp, 1);
		break;
	}
	adcGainRegister[channel] = gain;
	adcMuxRegister[channel] = mux;
}

void AttysComm::sendInit() {
	char rststr[] = "\n\n\n\n\r";
	_RPT0(0,"Sending Init\n");
#ifdef _WIN32
	u_long iMode = 1;
	ioctlsocket(btsocket, FIONBIO, &iMode);
#else
	fcntl(btsocket, F_SETFL, fcntl(btsocket, F_GETFL, 0) | O_NONBLOCK);
#endif
	send(btsocket, rststr, (int)strlen(rststr), 0);
	sendSyncCommand("x=0", 1);
	// switching to base64 encoding
	sendSyncCommand("d=1", 1);
	sendSamplingRate();
	sendFullscaleAccelRange();
	sendGainMux(0, adc0_gain_index, adc0_mux_index);
	sendGainMux(1, adc1_gain_index, adc1_mux_index);
	sendCurrentMask();
	sendBiasCurrent();
	sendSyncCommand("x=1\n", 0);
#ifdef _WIN32
	iMode = 0;
	ioctlsocket(btsocket, FIONBIO, &iMode);
#else
	fcntl(btsocket, F_SETFL, fcntl(btsocket, F_GETFL, 0) & ~O_NONBLOCK);
#endif
	_RPT0(0,"Init finished. Waiting for data.\n");
}


void AttysComm::run() {
	char recvbuffer[8192];

	int nTrans = 1;

	sendInit();

	strcpy(inbuffer, "");

	doRun = 1;

	char* lf = nullptr;

	// Keep listening to the InputStream until an exception occurs
	while (doRun) {

		int ret = recv(btsocket, recvbuffer, 8191, 0);
		if (ret > 0) {
			recvbuffer[ret] = 0;
			strcat(inbuffer, recvbuffer);
			// search for LF (CR is first)
			while ((lf = strchr(inbuffer, 0x0A))) {

				*lf = 0;

				if (strlen(inbuffer) == 29) {

					Base64decode(raw, inbuffer);

					for (int i = 0; i < 2; i++) {
						long v = (raw[i * 3] & 0xff)
							| ((raw[i * 3 + 1] & 0xff) << 8)
							| ((raw[i * 3 + 2] & 0xff) << 16);
						data[INDEX_Analogue_channel_1 + i] = v;
					}

					for (int i = 0; i < 6; i++) {
						long v = (raw[8 + i * 2] & 0xff)
							| ((raw[8 + i * 2 + 1] & 0xff) << 8);
						data[i] = v;
					}

					// check that the timestamp is the expected one
					int ts = 0;
					nTrans = 1;
					ts = raw[7];
					if ((ts - expectedTimestamp) > 0) {
						if (correctTimestampDifference) {
							nTrans = 1 + (ts - expectedTimestamp);
						}
						else {
							correctTimestampDifference = true;
						}
					}
					// update timestamp
					expectedTimestamp = ++ts;

					// acceleration
					for (int i = INDEX_Acceleration_X;
						i <= INDEX_Acceleration_Z; i++) {
						float norm = 0x8000;
						sample[i] = ((float)data[i] - norm) / norm *
							getAccelFullScaleRange();
					}

					// magnetometer
					for (int i = INDEX_Magnetic_field_X;
						i <= INDEX_Magnetic_field_Z; i++) {
						float norm = 0x8000;
						sample[i] = ((float)data[i] - norm) / norm *
							MAG_FULL_SCALE;
						//Log.d(TAG,"i="+i+","+sample[i]);
					}

					for (int i = INDEX_Analogue_channel_1;
						i <= INDEX_Analogue_channel_2; i++) {
						float norm = 0x800000;
						sample[i] = ((float)data[i] - norm) / norm *
							ADC_REF / ADC_GAIN_FACTOR[adcGainRegister[i
							- INDEX_Analogue_channel_1]];
					}

				}
				else {
					_RPT1(0, "Reception error, length=%d, ", (int)strlen(inbuffer));
					_RPT1(0, "recbuffer=>>>%s<<<\n\n,", recvbuffer);
				}

				// in case a sample has been lost
				for (int j = 0; j < nTrans; j++) {
					for (int k = 0; k < NCHANNELS; k++) {
						ringBuffer[inPtr][k] = sample[k];
					}
					timestamp = timestamp + 1.0 / getSamplingRateInHz();
					sampleNumber++;
					inPtr++;
					if (inPtr == nMem) {
						inPtr = 0;
					}
				}

				lf++;
				int rem = (int)strlen(lf) + 1;
				memmove(inbuffer, lf, rem);

			}
		}
	}
};
