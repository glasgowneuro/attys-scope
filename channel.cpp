#include "channel.h"

#include<qfont.h>
#include<stdio.h>


Channel::Channel(const int nChannels, const std::string* _channelLabels) : QComboBox() {

	numberOfChannels = nChannels;
	channelLabels = _channelLabels;

	setSizeAdjustPolicy(QComboBox::AdjustToContents);

	for(int i=0;i<nChannels;i++) {
		QString str = QString::fromUtf8(channelLabels[i].c_str());
		addItem(str,i);
	}
	addItem(tr("off"),nChannels);
		
	connect(this,
		SIGNAL( activated(int) ),
		this,
		SLOT( channelChanged(int) ) );
	
	int width = minimumSizeHint().width();
	setMinimumWidth(width * 1.25);
	
}

void Channel::channelChanged ( int index ) {
 	channel = index;
}

void Channel::setChannel( int c ) {
	if ((c >= 0)&&(c<numberOfChannels)) {
		// proper channel number
		setCurrentIndex( c );
		channel = c;
	} else {
		// channel off
		setCurrentIndex( numberOfChannels );
		channel = numberOfChannels;
	}
}
