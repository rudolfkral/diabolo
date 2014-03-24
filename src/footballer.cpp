#include "footballer.h"
#include <QPixmap>
#include <QPropertyAnimation>
#include <QDebug>

const QString Footballer::WHITE_FILE_NAME =
		QString(":/img/white_wo_ball.png");
const QString Footballer::BLACK_FILE_NAME =
		QString(":/img/black_wo_ball.png");

Footballer::Footballer(bool team, int id, int row, QObject *parent) :
	QObject(parent), QGraphicsPixmapItem()
{
	this->team = team;
	this->id = id;
	QPixmap f_pixmap(team ? WHITE_FILE_NAME : BLACK_FILE_NAME);
	f_pixmap.scaledToHeight(row);
	setPixmap(f_pixmap);
	setTransformationMode(Qt::SmoothTransformation);
	setZValue(2);
}

void Footballer::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	emit chosen(team, id);
}

void Footballer::move(QPointF where)
{
	QPropertyAnimation* anim = new QPropertyAnimation(this, "pos");
	anim->setDuration(1000);
	anim->setStartValue(pos());
	anim->setEndValue(where);
	anim->setEasingCurve(QEasingCurve::OutCubic);
	anim->start(QAbstractAnimation::DeleteWhenStopped);
	QObject::connect(anim, SIGNAL(finished()), this, SLOT(on_anim_end()));
	emit anim_start();
}

void Footballer::on_anim_end()
{
	emit anim_end();
}
