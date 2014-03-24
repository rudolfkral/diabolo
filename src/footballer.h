#ifndef FOOTBALLER_H
#define FOOTBALLER_H

#include <QGraphicsPixmapItem>

class Footballer :  public QObject, public QGraphicsPixmapItem
{
	Q_OBJECT
	Q_PROPERTY(QPointF pos READ pos WRITE setPos)
public:
	explicit Footballer(bool team, int id, int row, QObject *parent = 0);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void move(QPointF where);
signals:
	void chosen(bool team, int id);
	void anim_start();
	void anim_end();
public slots:
	void on_anim_end();
private:
	bool team;
	int id;
	const static QString WHITE_FILE_NAME;
	const static QString BLACK_FILE_NAME;
};

#endif // FOOTBALLER_H
