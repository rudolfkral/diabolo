#include "gamemodedialog.h"
#include "ui_gamemodedialog.h"
#include <QCloseEvent>

GameModeDialog::GameModeDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GameModeDialog)
{
	ui->setupUi(this);
}

GameModeDialog::~GameModeDialog()
{
	delete ui;
}

void GameModeDialog::closeEvent(QCloseEvent *event)
{
	emit chosen(-1);
	event->accept();
}

void GameModeDialog::on_hum_vs_hum_but_clicked()
{
	emit chosen(0);
}

void GameModeDialog::on_hum_white_but_clicked()
{
	emit chosen(1);
}

void GameModeDialog::on_hum_zer_but_clicked()
{
	emit chosen(3);
}

void GameModeDialog::on_hum_black_but_clicked()
{
	emit chosen(2);
}
