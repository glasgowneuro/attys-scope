#include "AttysComm.h"

#include <assert.h>


/**
 * These global variables are filled by attysScan() below
 **/


/**
 * Actual number of Attys Devices
 **/
int nAttysDevices = 0;

/**
 * file descriptor for bt devices
 **/
SOCKET *dev = NULL;

/**
 * name of the Attys
 **/
char** attysName = NULL;

/**
 * Pointer to AttysComm
 **/
AttysComm** attysComm = NULL;



/**
 * Scans for Attys Devices
 * Fills up the global arrays above
 * Parameter splash is an optional splash screen
 * Returns zero on success
 **/
int attysScan(QSplashScreen* splash, int maxAttysDevs) {
	dev = new SOCKET[maxAttysDevs];
	attysName = new char*[maxAttysDevs];
	attysComm = new AttysComm*[maxAttysDevs];
	assert(attysComm != nullptr);
	for (int devNo = 0; devNo < maxAttysDevs; devNo++) {
		dev[devNo] = 0;
		attysComm[devNo] = nullptr;
		attysName[devNo] = new char[256];
		attysName[devNo][0] = 0;
	}

	nAttysDevices = 0;

#ifdef __linux__

	inquiry_info *ii = NULL;
	int max_rsp, num_rsp;
	int dev_id, sock, len, flags;
	int i;
	char addr[19] = { 0 };
	char name[248] = { 0 };
	struct sockaddr_rc saddr;
	memset(&saddr,0,sizeof(struct sockaddr_rc));

	fprintf(stderr,"Searching for Attys devices.\n");
	if (splash) {
		splash->showMessage("Searching for\nAttys devices");
	}
	
	dev_id = hci_get_route(NULL);
	if (dev_id < 0) {
		fprintf(stderr,"No bluetooth device available.\n");
		if (splash) {
			splash->showMessage("No bluetooth\ndevices available");
		}
		return dev_id;
	}
	sock = hci_open_dev( dev_id );
	if (sock < 0) {
		perror("opening socket");
		if (splash) {
			splash->showMessage("Error opening socket");
		}
		return sock;
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
		return num_rsp;
	}
		
	// let's probe how many we have
	nAttysDevices = 0;
	for (i = 0; (i < num_rsp) && (i < maxAttysDevs); i++) {
		for(int j = 0;j<3;j++) {
			ba2str(&(ii+i)->bdaddr, addr);
			memset(name, 0, sizeof(name));
			if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), 
						 name, 0) < 0)
				strcpy(name, "[unknown]");
			fprintf(stderr,"%s  %s", addr, name);
			if (strstr(name,"GN-ATTYS")!=0) {
				fprintf(stderr,"! Found one. Connecting. ");
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
					fprintf(stderr,"Success.\n");
					nAttysDevices++;
					break;
				} else {
					fprintf(stderr,"Connect failed. Error code = %d. ",status);
					if (status == -1) {
						fprintf(stderr,"Permission denied. Please pair the Attys with your bluetooth adapter.\n");
					}
					::close(s);
					fprintf(stderr,"\n");
				}
			} else {
				fprintf(stderr,"\n");
			}
		}
	}

	delete[] ii;
	
	
#elif _WIN32

	WSADATA wsd;
	int r = WSAStartup(MAKEWORD(2, 2), &wsd);
	if (r != 0) {
		_RPT1(0, " WASStartup failed: %d\n",r);
		return r;
	}

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
			if (splash) {
				splash->showMessage("Internal windows\nbluetooth driver problem:\nWSALookupServiceBegin failed\n");
			}
			return iRet;
		}
		else {
			_RPT0(0,"No bluetooth devices found\n");
			if (splash) {
				splash->showMessage("No bluetooth devices found.\n\nHave you paired the Attys?");
			}
			return -1;
		}
	}

	CHAR buf[4096];
	LPWSAQUERYSET pwsaResults = (LPWSAQUERYSET)buf;
	ZeroMemory(pwsaResults, sizeof(WSAQUERYSET));
	pwsaResults->dwSize = sizeof(WSAQUERYSET);
	pwsaResults->dwNameSpace = NS_BTH;
	pwsaResults->lpBlob = NULL;

	DWORD dwSize = sizeof(buf);
	while ((WSALookupServiceNext(hLookup, LUP_RETURN_NAME | LUP_RETURN_ADDR, &dwSize, pwsaResults) == 0) &&
	       (nAttysDevices < maxAttysDevs ) ){
		LPWSTR name = pwsaResults->lpszServiceInstanceName;
		if (wcsstr(name,L"GN-ATTYS1") != 0) {
			_RPT0(0,"Found an Attys!\n");

			char tmp[256];
			sprintf(tmp, "Connecting to Attys #%d: %S", nAttysDevices,name);
			if (splash) {
				splash->showMessage(tmp);
			}

			for (int i = 0; i < 5; i++) {

				// allocate a socket
				SOCKET s = ::socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);

				if (INVALID_SOCKET == s) {
					_RPT0(0,"=CRITICAL= | socket() call failed.\n");
					return -1;
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
					assert(attysComm[nAttysDevices] != nullptr);
					sprintf(attysName[nAttysDevices], "#%d: %S", nAttysDevices, name);
					nAttysDevices++;
					break;
				}
				else {
					_RPT0(0,"Connect failed\n");
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

	return 0;
}





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
	unregisterCallback();
	adc_rate_index = ADC_DEFAULT_RATE;
	timestamp = 0;
	adc0_gain_index = ADC_GAIN_1;
	adc0_mux_index = ADC_MUX_NORMAL;
	adc1_gain_index = ADC_GAIN_1;
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
					_RPT0(0, " -- OK received\n");
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
					// _RPT1(0, "%d\n", data[INDEX_Analogue_channel_1]);


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
					if (callbackInterface) {
						callbackInterface->hasSample(timestamp,sample);
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
