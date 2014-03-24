#include "gamerules.h"
#include "ui_gamerules.h"
#include <QFile>

const QString file_name = QString(":/game_rules.txt");

GameRules::GameRules(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GameRules)
{
	ui->setupUi(this);
	QFile file(file_name);
	file.open(QIODevice::ReadOnly);
	ui->textEdit->setHtml(file.readAll());
	file.close();
}

GameRules::~GameRules()
{
	delete ui;
}
