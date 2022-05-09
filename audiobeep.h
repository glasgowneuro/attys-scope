#ifndef _AUDIOBEEP
#define _AUDIOBEEP

#include <QtMultimedia/QAudioFormat>
#include <QtMultimedia/QAudioDeviceInfo>
#include <QtMultimedia/QAudioOutput>
#include <QBuffer>
#include <QMessageBox>

/**
 * Beep generator. Creates and plays a sinewave of specified frequency, duration and
 * volume.
 * GPL 3.0
 * (C) 2021 Bernd Porr
 **/
class AudioBeep : public QObject {

	Q_OBJECT

public:
	/**
	 * Constructor of the player. It calculates the sine wave and checks if audio can be
	 * played. If the audio cannot be played an exception is thrown.
	 * \param w The parent widget.
	 * \param beepDuration The duration of the beep
	 * \param beepFreq The beep frequency in Hz
	 * \param volume The volume of the tone from 0..1
	 **/
	AudioBeep(QObject *w,
		  float beepDuration = 1.0,
		  float beepFreq = 1000,
		  float volume = 1.0);

	/**
	 * Destructor: frees all buffers and the player
	 **/
	~AudioBeep();

	/**
	 * Plays the sine wave asynchronously. 
	 * The function returns instantly. 
	 * Playing is in the background.
	 **/
	void play();

	/**
	* Reports if it's operational
	**/
	bool isOK() const {
		return (nullptr != input) && (nullptr != audio);
	}

private:
	// We use a fixed sampling rate here supported by any sound card
	const unsigned int sampleRate = 48000;

	QObject* qparent = nullptr;
	QByteArray byteBuffer;
	QAudioFormat audioFormat;
	QBuffer* input = nullptr;
	QAudioOutput* audio = nullptr;
	QMessageBox* msgBox;
};

#endif
