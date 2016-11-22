#ifndef _CHANNEL
#define _CHANNEL

#include <QComboBox>

class Channel : public QComboBox {

    Q_OBJECT

public:
    /**
     * constructor
     **/
	Channel(int nChannels, std::string* _channelLabels);

private slots:
	void channelChanged(int i);
	
private:
	int channel;
	int numberOfChannels;
	std::string* channelLabels;
	
public:
	inline int getChannel() {return channel;};

	void setChannel(int Channel);

	inline int isActive() {return channel != numberOfChannels;};

};

#endif
