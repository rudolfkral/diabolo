#ifndef CLICKABLESHADOW_H
#define CLICKABLESHADOW_H

#include <QGraphicsItem>

class ClickableShadow : public QObject, public QGraphicsItem
{
	Q_OBJECT
	Q_INTERFACES(QGraphicsItem)
public:
	explicit ClickableShadow(int pos, int row, int z, QObject *parent = 0);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	QRectF boundingRect() const;
signals:
	void chosen(int pos);
public slots:
	
private:
	int pos;
	int row;
};

#endif // CLICKABLESHADOW_H
