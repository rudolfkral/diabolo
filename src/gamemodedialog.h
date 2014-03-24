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
	void closeEvent(QCloseEvent *event);
	
signals:
	void chosen(int);

private slots:

	void on_hum_vs_hum_but_clicked();

	void on_hum_white_but_clicked();

	void on_hum_black_but_clicked();

	void on_hum_zer_but_clicked();

private:
	Ui::GameModeDialog *ui;
};

#endif // GAMEMODEDIALOG_H
