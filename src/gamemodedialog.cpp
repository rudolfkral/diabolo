#include "gamemodedialog.h"
#include "ui_gamemodedialog.h"

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

void GameModeDialog::on_pushButton_2_clicked()
{
	emit chosen(0);
}

void GameModeDialog::on_pushButton_clicked()
{
	emit chosen(1);
}

void GameModeDialog::on_pushButton_3_clicked()
{
	emit chosen(2);
}

void GameModeDialog::on_GameModeDialog_destroyed()
{
	emit chosen(-1);
}
