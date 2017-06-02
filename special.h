#ifndef _SPECIAL
#define _SPECIAL

#include <QComboBox>

#define SPECIAL_NORMAL 0
#define SPECIAL_ECG 1
#define SPECIAL_I 2

class Special : public QComboBox {

    Q_OBJECT

public:
    /**
     * constructor
     **/
	Special();

private slots:
	void specialChanged(int i);
	
private:
	int special;
	std::string* specialLabels;
	
public:
	inline int getSpecial() {return special;};

	void setSpecial(int _special);
};

#endif
