#ifndef _CURRENT
#define _CURRENT

#include <QComboBox>

class Current : public QComboBox {

    Q_OBJECT

public:
    /**
     * constructor
     **/
	Current();

private slots:
	void currentChanged(int i);

signals:
	void signalRestart();
	
private:
	int current = 0;
	
public:
	inline int getCurrent() {return current;};

	void setCurrent(int _current);
};

#endif
