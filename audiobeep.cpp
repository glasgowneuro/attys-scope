#include "audiobeep.h"

#define _USE_MATH_DEFINES
#include <math.h>

// constructor
AudioBeep::AudioBeep(QObject *w, float duration, float frequency, float volume) {
	qparent = w;

	// 16 bit audio PCM
	audioFormat.setSampleRate(sampleRate);
	audioFormat.setChannelCount(1);
	audioFormat.setSampleSize(16);
	audioFormat.setCodec("audio/pcm");
	audioFormat.setByteOrder((QAudioFormat::Endian)QSysInfo::ByteOrder);
	audioFormat.setSampleType(QAudioFormat::SignedInt);

	// check if we can play it
	QAudioDeviceInfo deviceInfo(QAudioDeviceInfo::defaultOutputDevice());
	if(!deviceInfo.isFormatSupported(audioFormat)) {
		char tmp[] = "QT backend to play audio is not installed. Cannot play audio.";
		msgBox = new QMessageBox;
		msgBox->setText(tmp);
		msgBox->setModal(true);
		msgBox->show();
		return;
	}

	// number of data samples
	const unsigned int n = (unsigned int)round(duration * sampleRate);

	// resize byteBuffer to the total number of bytes that will be needed to accommodate
	// all the n data samples that are of type float
	byteBuffer.resize(sizeof(qint16) * n);

	// create the sinewave
	for (unsigned int i = 0; i < n; i++) {
		// create sine wave data samples, one at a time
		qint16 sinVal = (qint16)(sin(2.0 * M_PI * frequency * i / sampleRate)*32767*volume);
		( (qint16*)byteBuffer.constData() )[i] = sinVal;
	}

	// create the fake streaming buffer
	input  = new QBuffer(&byteBuffer, qparent);
	// open the fake stream
	input->open(QIODevice::ReadOnly);
	// open the audio output
	audio = new QAudioOutput(audioFormat, qparent);
}


// destructor
AudioBeep::~AudioBeep() {
	if (nullptr != input) {
		input->close();
		delete input;
	}
	if (nullptr != audio) {
		delete audio;
	}
}


// play the audio
void AudioBeep::play() {
	if ((nullptr != input) && (nullptr != audio)) {
		input->seek(0);
		audio->reset();
		audio->start(input);
	}
}
