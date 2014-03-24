#ifndef GAMEMODEDIALOG_H
#define GAMEMODEDIALOG_H

#include <QDialog>

namespace Ui {
class GameModeDialog;
}

class GameModeDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit GameModeDialog(QWidget *parent = 0);
	~GameModeDialog();
	
signals:
	void chosen(int);

private slots:
	void on_pushButton_2_clicked();

	void on_pushButton_clicked();

	void on_pushButton_3_clicked();

	void on_GameModeDialog_destroyed();

private:
	Ui::GameModeDialog *ui;
};

#endif // GAMEMODEDIALOG_H
