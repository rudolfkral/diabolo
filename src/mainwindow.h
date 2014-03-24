#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "gamemodedialog.h"
#include "aboutprog.h"
#include "gamerules.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
	
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

public slots:
	void on_mode_choose(int);
	
private slots:
	void on_actionZako_cz_gr_triggered();

	void on_actionNowa_gra_triggered();

	void on_actionZako_cz_tur_triggered();

	void on_actionCofnij_ruch_triggered();

	void on_actionPowt_rz_ruch_triggered();

	void on_actionRuch_komputera_triggered();

	void on_actionW_cz_tryb_edycji_triggered();

	void on_actionUstawienia_triggered();

	void on_actionO_programie_triggered();

	void on_actionZasady_gry_triggered();

private:
	GameModeDialog* gmd;
	GameRules* grul;
	AboutProg* abpr;
	bool mode_set;
	Ui::MainWindow *ui;
	QLabel* status_label;
};

#endif // MAINWINDOW_H
