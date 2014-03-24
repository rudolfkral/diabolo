#ifndef BALL_H
#define BALL_H

#include <QGraphicsPixmapItem>

class Ball : public QObject, public QGraphicsPixmapItem
{
	Q_OBJECT
	Q_PROPERTY(QPointF pos READ pos WRITE setPos)
public:
	explicit Ball(bool team, int row, QObject *parent = 0);
	void move(QPointF where);
signals:
	void anim_start();
	void anim_end();
public slots:
	void on_anim_end();
private:
	bool team;
	static const QString FILE_NAME;
};

#endif // BALL_H
