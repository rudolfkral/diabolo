#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow), mode_set(false)
{
	ui->setupUi(this);
	enable_all(false);
	status_label = new QLabel();
	ui->statusBar->addPermanentWidget(status_label);
	ui->graphicsView->set_stat(status_label);
	QCoreApplication::setOrganizationName("Adrian Kral");
	QCoreApplication::setApplicationName("Diabolo");
	QSettings settings;
	move(settings.value("window_pos", QPoint(0, 0)).value<QPoint>());
	resize(settings.value("window_size", QSize(400, 450)).value<QSize>());
	bool auto_moves = settings.value("auto_moves", false).value<bool>();
	ui->graphicsView->set_auto_moves(auto_moves);
}

MainWindow::~MainWindow()
{
	QSettings settings;
	settings.setValue("window_pos", pos());
	settings.setValue("window_size", size());
	settings.setValue("auto_moves", ui->graphicsView->get_auto_moves());
	delete status_label;
	delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
	int ac_height = size().height() - ui->menuBar->size().height() - ui->statusBar->size().height();
	int new_size = (ac_height > size().width() ? size().width() : ac_height);
	ui->graphicsView->resize(new_size, new_size);
	int new_x = (size().width() - new_size) / 2;
	ui->graphicsView->move(new_x, ui->graphicsView->pos().y());
}

void MainWindow::on_actionZakoncz_gre_triggered()
{
	QCoreApplication::exit();
}

void MainWindow::on_actionNowa_gra_triggered()
{
	setEnabled(false);
	enable_all(false);
	mode_set = false;
	gmd = new GameModeDialog(this);
	gmd->show();

	QObject::connect(gmd, SIGNAL(chosen(int)), this, SLOT(on_mode_choose(int)));
}

void MainWindow::on_actionZakoncz_ture_triggered()
{
	ui->graphicsView->end_turn();
}

void MainWindow::on_actionCofnij_ruch_triggered()
{
	ui->graphicsView->move_back();
}

void MainWindow::on_actionPowtorz_ruch_triggered()
{
	ui->graphicsView->move_forward();
}

void MainWindow::on_actionRuch_komputera_triggered()
{
	ui->graphicsView->move_random();
}

void MainWindow::on_actionWlacz_tryb_edycji_triggered()
{
	ui->graphicsView->switch_edit_mode();
	enable_game(!ui->graphicsView->is_edit_mode());
}

void MainWindow::on_mode_choose(int x)
{
	switch(x)
	{
	case 0:
		ui->graphicsView->set_two_humans();
		break;
	case 1:
		ui->graphicsView->set_hum_white();
		break;
	case 2:
		ui->graphicsView->set_hum_black();
		break;
	case 3:
		ui->graphicsView->set_two_comps();
		break;
	default:
		delete gmd;
		setEnabled(true);
		return;
	}
	enable_all(true);
	setEnabled(true);
	ui->graphicsView->new_game();
	mode_set = true;
	delete gmd;
}

void MainWindow::enable_all(bool st)
{
	ui->actionRuch_komputera->setEnabled(st);
	ui->actionPodpowiedz->setEnabled(st);
	ui->actionWlacz_tryb_edycji->setEnabled(st);
	ui->actionZapisz_gre->setEnabled(st);
	ui->actionUstawienia->setEnabled(st);
	ui->actionZakoncz_ture->setEnabled(st);
}

void MainWindow::enable_game(bool st)
{
	ui->actionRuch_komputera->setEnabled(st);
	ui->actionPodpowiedz->setEnabled(st);
	ui->actionZapisz_gre->setEnabled(st);
	ui->actionUstawienia->setEnabled(st);
	ui->actionZakoncz_ture->setEnabled(st);
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

void MainWindow::on_actionPodpowiedz_triggered()
{
	ui->graphicsView->show_hint();
}

void MainWindow::on_actionWczytaj_gre_triggered()
{
	if(ui->graphicsView->load_game())
		enable_all(true);
}
