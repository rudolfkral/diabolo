#include "clickableshadow.h"
#include <QPen>
#include <QBrush>
#include <QPainter>

ClickableShadow::ClickableShadow(int pos, int row, int z, QObject *parent) :
	QObject(parent), QGraphicsItem()
{
	this->pos = pos;
	this->row = row;
	setOpacity(0.25);
	setZValue(z);
}

void ClickableShadow::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	emit chosen(pos);
}

void ClickableShadow::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QRect r(0, 0, row, row);
	QPen pen(Qt::transparent);
	QBrush brush(Qt::black, Qt::SolidPattern);
	painter->setPen(pen);
	painter->setBrush(brush);
	painter->drawRect(r);
}

QRectF ClickableShadow::boundingRect() const
{
	QRect r(0, 0, row, row);
	return QRectF(r);
}
