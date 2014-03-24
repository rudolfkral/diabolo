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
	void set_hum_white();
	void set_hum_black();
	void set_two_comps();
	void switch_auto_moves() { wait_for_poke = !wait_for_poke; update_state(); }
	void set_auto_moves(bool am) { wait_for_poke = am; update_state(); }
	void new_game();
	bool get_auto_moves() { return wait_for_poke; }
	bool is_auto_to_move();
	bool is_edit_mode() { return game->get_edit_mode(); }
	enum game_modes {
		two_hum = 0,
		white_hum = 1,
		black_hum = 2,
		zer_hum = 3
	};
	void resizeEvent(QResizeEvent *event);
signals:
	void enable_back(bool st);
	void enable_forth(bool st);
	void enable_turn_end(bool st);
	void enable_hint(bool st);
	void enable_game(bool st);
public slots:
	void draw_pitch();
	void show_pos_moves(int team_int, int id);
	void show_hint();
	void on_chosen(int pos);
	void end_turn();
	void move_back();
	void move_forward();
	void move_random();
	void on_anim_end();
	void on_anim_start();
	void switch_edit_mode();
	bool load_game();
	void save_game();
private:
	static const int ROW_SIZE = 50;
	bool blocked;
	game_modes mode;
	bool wait_for_poke;
	bool ended;
	bool saving;
	int focus;
	QLabel* stat;
	QGraphicsScene scene;
	Game* game;
	QList<Footballer*> footballers;
	QList<Ball*> balls;
	QList<ClickableShadow*> shadows;
	QPoint pos_to_coord(int pos);
	QPoint ball_pos_to_coord(int pos);
	QString player_info();
	void clear_shadows();
	void update_state();
};

#endif // PITCH_H
