#ifndef ABOUTPROG_H
#define ABOUTPROG_H

#include <QDialog>

namespace Ui {
class AboutProg;
}

class AboutProg : public QDialog
{
	Q_OBJECT
	
public:
	explicit AboutProg(QWidget *parent = 0);
	~AboutProg();
	
private:
	Ui::AboutProg *ui;
};

#endif // ABOUTPROG_H
