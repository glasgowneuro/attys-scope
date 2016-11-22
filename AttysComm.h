#ifdef __linux__ 
#include<sys/ioctl.h>
#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<termios.h>
#define SOCKET int
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#define Sleep(u) usleep((u*1000))
#define _RPT0(u,v) fprintf(stderr,v)
#define _RPT1(u,v,w) fprintf(stderr,v,w)
#define OutputDebugStringW(s)
#elif _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <winsock2.h>
#include <ws2bth.h>
#include <BluetoothAPIs.h>
#else
#endif



#include <stdio.h>
#include <QThread>

#include "base64.h"

#pragma once
class AttysComm : public QThread
{
public:
	/////////////////////////////////////////////////
	// Constructor: takes the bluetooth device as an argument
	// it then tries to connect to the Attys
	AttysComm(SOCKET _btsocket);

	~AttysComm();

public:

	static const int NCHANNELS = 8;

	// index numbers of the channels returned in the data array
	const int INDEX_Acceleration_X = 0;
	const int INDEX_Acceleration_Y = 1;
	const int INDEX_Acceleration_Z = 2;
	const int INDEX_Magnetic_field_X = 3;
	const int INDEX_Magnetic_field_Y = 4;
	const int INDEX_Magnetic_field_Z = 5;
	const int INDEX_Analogue_channel_1 = 6;
	const int INDEX_Analogue_channel_2 = 7;

	// descriptions the channels in text form
	const char* const CHANNEL_DESCRIPTION[NCHANNELS] = {
		"Acceleration X",
		"Acceleration Y",
		"Acceleration Z",
		"Magnetic field X",
		"Magnetic field Y",
		"Magnetic field Z",
		"Analogue channel 1",
		"Analogue channel 2"
	};

	// descriptions the channels in text form
	const char* const CHANNEL_SHORT_DESCRIPTION[NCHANNELS] = {
		"Acc X",
		"Acc Y",
		"Acc Z",
		"Mag X",
		"Mag Y",
		"Mag Z",
		"ADC 1",
		"ADC 2"
	};

	// units of the channels
	const char* const CHANNEL_UNITS[NCHANNELS] = {
		"m/s^2",
		"m/s^2",
		"m/s^2",
		"T",
		"T",
		"T",
		"V",
		"V"
	};

	///////////////////////////////////////////////////////////////////
	// ADC sampling rate and for the whole system
	const int ADC_RATE_125HZ = 0;
	const int ADC_RATE_250HZ = 1;
	const int ADC_RATE_500Hz = 2;
	const int ADC_DEFAULT_RATE = ADC_RATE_250HZ;
	// array of the sampling rates converting the index
	// to the actual sampling rate
	const int ADC_SAMPLINGRATE[4] = { 125, 250, 500, 1000 };
	// the actual sampling rate in terms of the sampling rate index
	int adc_rate_index;

	void setAdc_samplingrate_index(int idx) {
		adc_rate_index = idx;
	}

	// get the sampling rate in Hz (not index number)
	int getSamplingRateInHz() {
		return ADC_SAMPLINGRATE[adc_rate_index];
	}

	int getAdc_samplingrate_index() {
		return adc_rate_index;
	}


	////////////////////////////////////////////////////////////////////////////
	// ADC gain
	// the strange numbering scheme comes from the ADC's numbering
	// scheme. Index=0 is really a gain factor of 6
	// On the ATttys we refer to channel 1 and 2 which are 0 and 1 here for
	// indexing.
	const int ADC_GAIN_6 = 0;
	const int ADC_GAIN_1 = 1;
	const int ADC_GAIN_2 = 2;
	const int ADC_GAIN_3 = 3;
	const int ADC_GAIN_4 = 4;
	const int ADC_GAIN_8 = 5;
	const int ADC_GAIN_12 = 6;
	// mapping between index and actual gain
	const int ADC_GAIN_FACTOR[7] = { 6, 1, 2, 3, 4, 8, 12 };
	// the voltage reference of the ADC in volts
	const float ADC_REF = 2.42F;

	float getADCFullScaleRange(int channel) {
		switch (channel) {
		case 0:
			return ADC_REF / ADC_GAIN_FACTOR[adc0_gain_index];
		case 1:
			return ADC_REF / ADC_GAIN_FACTOR[adc1_gain_index];
		default:
			fprintf(stderr, "getADCFullScaleRange wrong index\n");
			exit(1);
		}
	}

	void setAdc1_gain_index(int idx) {
		adc0_gain_index = idx;
	}

	void setAdc2_gain_index(int idx) {
		adc1_gain_index = idx;
	}

	// initial gain factor is 1 for both channels
	int adc0_gain_index = ADC_GAIN_1;
	int adc1_gain_index = ADC_GAIN_1;


	/////////////////////////////////////////////////////////////////////
	// Bias currents for resistance measurement
	// selectable bias current index numbers for the ADC inputs
	// used to measure resistance
	const int ADC_CURRENT_6NA = 0;
	const int ADC_CURRENT_22NA = 1;
	const int ADC_CURRENT_6UA = 2;
	const int ADC_CURRENT_22UA = 3;
	int current_index = 0;
	int current_mask = 0;

	// sets the bias current which can be switched on
	void setBiasCurrent(int currIndex) {
		current_index = currIndex;
	}

	// gets the bias current as in index
	int getBiasCurrent() {
		return current_index;
	}

	// switches the currents on
	void enableCurrents(int pos_ch1, int neg_ch1, int pos_ch2) {
		current_mask = 0;
		if (pos_ch1) {
			current_mask = (int)(current_mask | (int)0b00000001);
		}
		if (neg_ch1) {
			current_mask = (int)(current_mask | (int)0b00000010);
		}
		if (pos_ch2) {
			current_mask = (int)(current_mask | (int)0b00000100);
		}
	}


	//////////////////////////////////////////////////////////////////////////////
	// selectable different input mux settings
	// for the ADC channels
	const int ADC_MUX_NORMAL = 0;
	const int ADC_MUX_SHORT = 1;
	const int ADC_MUX_SUPPLY = 3;
	const int ADC_MUX_TEMPERATURE = 4;
	const int ADC_MUX_TEST_SIGNAL = 5;
	const int ADC_MUX_ECG_EINTHOVEN = 6;
	int adc0_mux_index;
	int adc1_mux_index;

	void setAdc0_mux_index(int idx) {
		adc0_mux_index = idx;
	}

	void setAdc1_mux_index(int idx) {
		adc1_mux_index = idx;
	}


	///////////////////////////////////////////////////////////////////////////////
	// accelerometer
	const int ACCEL_2G = 0;
	const int ACCEL_4G = 1;
	const int ACCEL_8G = 2;
	const int ACCEL_16G = 3;
	const float oneG = 9.80665F; // m/s^2
	const float ACCEL_FULL_SCALE[4] = { 2 * oneG, 4 * oneG, 8 * oneG, 16 * oneG }; // m/s^2
	int accel_full_scale_index;

	float getAccelFullScaleRange() {
		return ACCEL_FULL_SCALE[accel_full_scale_index];
	}

	void setAccel_full_scale_index(int idx) {
		accel_full_scale_index = idx;
	}


	///////////////////////////////////////////////////
	// magnetometer
	//
	const float MAG_FULL_SCALE = 4800.0E-6F; // TESLA

	float getMagFullScaleRange() {
		return MAG_FULL_SCALE;
	}


	////////////////////////////////////////////////
	// timestamp stuff as double
	// note this might drift in the long run
	void setTimestamp(double ts) {
		timestamp = ts;
	}

	double getTimestamp() {
		return timestamp;
	}


	////////////////////////////////////////////////
	// sample counter
	long sampleNumber = 0;

	long getSampleNumber() {
		return sampleNumber;
	}

	void setSampleNumber(long sn) {
		sampleNumber = sn;
	}


	///////////////////////////////////////////////////////////////////////
	// message listener
	// sends error/success messages back
	// for MessageListener
	// here are the messages:
	const int MESSAGE_CONNECTED = 0;
	const int MESSAGE_ERROR = 1;
	const int MESSAGE_RETRY = 2;
	const int MESSAGE_CONFIGURE = 3;
	const int MESSAGE_STARTED_RECORDING = 4;
	const int MESSAGE_STOPPED_RECORDING = 5;
	const int MESSAGE_CONNECTING = 6;

	////////////////////////////////////////////
	// connection info
	int hasActiveConnection() {
		return isConnected;
	}

	int hasFatalError() {
		return fatalError;
	}


	/////////////////////////////////////////////////
	// ringbuffer keeping data for chunk-wise plotting
	float* getSampleFromBuffer();

	int hasSampleAvilabale() {
		return (inPtr != outPtr);
	}

	private:
	///////////////////////////////////////////////////////
	// from here it's private
	SOCKET btsocket;
	int doRun = 0;
	// ringbuffer
	float** ringBuffer;
	// number of entries in the ringbuffer
	const int nMem = 1000;
	int inPtr = 0;
	int outPtr = 0;
	int isConnected = 0;
	int fatalError = 0;
	int* adcMuxRegister;
	int* adcGainRegister;
	int* adcCurrNegOn;
	int* adcCurrPosOn;
	int expectedTimestamp = 0;
	int correctTimestampDifference = 0;
	double timestamp = 0.0; // in secs
	int connectionEstablished;
	long* data;
	char* raw;
	float* sample;
	char* inbuffer;

	void sendSyncCommand(const char *message, int checkOK);

	void sendSamplingRate();

	void sendFullscaleAccelRange();

	void sendCurrentMask();

	void sendBiasCurrent();

	void sendGainMux(int channel, int gain, int mux);

	void setADCGain(int channel, int gain) {
		sendGainMux(channel, gain, adcMuxRegister[channel]);
	}

	void setADCMux(int channel, int mux) {
		sendGainMux(channel, adcGainRegister[channel], mux);
	}

	void sendInit();

	void run();

	public:
	/* Call this from the main activity to shutdown the connection */
	void quit() {
		doRun = false;
	}
};
