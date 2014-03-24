#ifndef PITCH_H
#define PITCH_H

#include <QGraphicsView>
#include <QList>
#include <QPoint>
#include <QLabel>
#include "game.h"
#include "footballer.h"
#include "ball.h"
#include "clickableshadow.h"

class Pitch : public QGraphicsView
{
	Q_OBJECT
public:
	explicit Pitch(QWidget *parent = 0);
	~Pitch();
	void set_stat(QLabel* stat);
	void set_two_humans();
	void set_human_comp();
	void set_two_comps();
	void switch_auto_moves() { wait_for_poke = !wait_for_poke; update_state(); }
	void new_game();
	bool get_auto_moves() { return wait_for_poke; }
signals:
	void enable_back(bool st);
	void enable_forth(bool st);
public slots:
	void draw_pitch();
	void show_pos_moves(bool team, int id);
	void on_chosen(int pos);
	void end_turn();
	void move_back();
	void move_forward();
	void move_random();
	void on_anim_end();
	void on_anim_start();
	void switch_edit_mode();
	void load_game();
	void save_game();
private:
	static const int ROW_SIZE = 50;
	bool blocked;
	bool two_humans;
	bool human_comp;
	bool two_comps;
	bool wait_for_poke;
	bool ended;
	Game::pos focus;
	QLabel* stat;
	QGraphicsScene scene;
	Game* game;
	QList<Footballer*> footballers;
	QList<Ball*> balls;
	QList<ClickableShadow*> shadows;
	QPoint pos_to_coord(int pos);
	QPoint ball_pos_to_coord(int pos);
	void clear_shadows();
	void update_state();
};

#endif // PITCH_H
