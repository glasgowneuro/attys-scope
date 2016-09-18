#include "channel.h"

#include<qfont.h>
#include<stdio.h>


Channel::Channel(int nChannels) : QComboBox() {

	numberOfChannels = nChannels;

	setMinimumWidth ( fontMetrics().width("x50XX") );

	for(int i=0;i<nChannels;i++) {
		char tmp[10];
		sprintf(tmp,"%02d",i);
		addItem(tr(tmp),i);
	}
	addItem(tr("off"),nChannels);
		
	connect(this,
		SIGNAL( activated(int) ),
		this,
		SLOT( channelChanged(int) ) );

}

void Channel::channelChanged ( int index ) {
 	channel = itemData(index).toInt();
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
