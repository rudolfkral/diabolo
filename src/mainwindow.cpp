#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QDialogButtonBox>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow), mode_set(false)
{
	ui->setupUi(this);
	statusBar()->setSizeGripEnabled(false);
	status_label = new QLabel();
	ui->statusBar->addPermanentWidget(status_label);
	ui->graphicsView->set_stat(status_label);
	QCoreApplication::setOrganizationName("Adrian Kral");
	QCoreApplication::setApplicationName("Diabolo");
	QSettings settings;
	move(settings.value("window_pos", QPoint(0, 0)).value<QPoint>());
	bool auto_moves = settings.value("auto_moves", false).value<bool>();
	if(auto_moves)
		ui->graphicsView->switch_auto_moves();
}

MainWindow::~MainWindow()
{
	QSettings settings;
	settings.setValue("window_pos", pos());
	settings.setValue("auto_moves", ui->graphicsView->get_auto_moves());
	delete status_label;
	delete ui;
}

void MainWindow::on_actionZako_cz_gr_triggered()
{
	QCoreApplication::exit();
}

void MainWindow::on_actionNowa_gra_triggered()
{
	setEnabled(false);
	mode_set = false;
	gmd = new GameModeDialog(this);
	gmd->show();

	QObject::connect(gmd, SIGNAL(chosen(int)), this, SLOT(on_mode_choose(int)));
}

void MainWindow::on_actionZako_cz_tur_triggered()
{
	ui->graphicsView->end_turn();
}

void MainWindow::on_actionCofnij_ruch_triggered()
{
	ui->graphicsView->move_back();
}

void MainWindow::on_actionPowt_rz_ruch_triggered()
{
	ui->graphicsView->move_forward();
}

void MainWindow::on_actionRuch_komputera_triggered()
{
	ui->graphicsView->move_random();
}

void MainWindow::on_actionW_cz_tryb_edycji_triggered()
{
	ui->graphicsView->switch_edit_mode();
}

void MainWindow::on_mode_choose(int x)
{
	switch(x)
	{
	case -1:
		return;
	case 0:
		ui->graphicsView->set_two_humans();
		break;
	case 1:
		ui->graphicsView->set_human_comp();
		break;
	case 2:
		ui->graphicsView->set_two_comps();
		break;
	default:
		return;
	}
	ui->graphicsView->new_game();
	setEnabled(true);
	mode_set = true;
	delete gmd;
}

void MainWindow::on_actionUstawienia_triggered()
{
	ui->graphicsView->switch_auto_moves();
}

void MainWindow::on_actionO_programie_triggered()
{
	abpr = new AboutProg(this);
	abpr->exec();
	delete abpr;
}

void MainWindow::on_actionZasady_gry_triggered()
{
	grul = new GameRules(this);
	grul->exec();
	delete grul;
}
