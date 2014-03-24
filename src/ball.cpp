#include "ball.h"
#include <QPropertyAnimation>

const QString Ball::FILE_NAME =
		QString(":/img/ball.png");

Ball::Ball(int team, int row, QObject *parent) :
	QObject(parent), QGraphicsPixmapItem()
{
	this->team = team;
	QPixmap f_pixmap(FILE_NAME);
	f_pixmap.scaledToHeight(row);
	setPixmap(f_pixmap);
	setTransformationMode(Qt::SmoothTransformation);
	setZValue(3);
}

void Ball::move(QPointF where)
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

void Ball::on_anim_end()
{
	emit anim_end();
}
