#include "pitch.h"
#include "clickableshadow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QDataStream>

const QString M_WHITE = QObject::trUtf8("biały");
const QString M_BLACK = QObject::trUtf8("czarny");
const QString M_HUM = QObject::trUtf8("(człowiek)");
const QString M_COM = QObject::trUtf8("(komputer)");
const QString M_WHITE_WIN = QObject::trUtf8("Wygrana białych.");
const QString M_BLACK_WIN = QObject::trUtf8("Wygrana czarnych.");
const QString M_WHITE_UNFAIR = QObject::trUtf8("Biali przegrywają z powodu gry nie fair.");
const QString M_BLACK_UNFAIR = QObject::trUtf8("Czarni przegrywają z powodu gry nie fair.");
const QString M_INCORRECT = QObject::trUtf8("Stan gry niepoprawny.");
const QString M_AUTOMOVES_ON = QObject::trUtf8("Autoruchy włączone.");
const QString M_AUTOMOVES_OFF = QObject::trUtf8("Autoruchy wyłączone.");
const QString M_LOAD_ERR = QObject::trUtf8("Błąd podczas wczytywania.");
const QString M_SAVE_ERR = QObject::trUtf8("Bład podczas zapisywania.");
const QString M_DEF_TITLE = QObject::trUtf8("Uwaga");

Pitch::Pitch(QWidget *parent) :
	QGraphicsView(parent), focus(-1), blocked(false), mode(two_hum),
	wait_for_poke(true), ended(true), saving(false)
{
	setScene(&scene);
	game = new Game();
}

Pitch::~Pitch()
{
	delete game;
}

void Pitch::set_stat(QLabel *stat)
{
	this->stat = stat;
}

void Pitch::set_two_humans()
{
	mode = two_hum;
}

void Pitch::set_hum_white()
{
	mode = white_hum;
}

void Pitch::set_hum_black()
{
	mode = black_hum;
}

void Pitch::set_two_comps()
{
	mode = zer_hum;
}

void Pitch::new_game()
{
	delete game;
	game = new Game();
	blocked = false;
	ended = false;
	draw_pitch();
}

void Pitch::resizeEvent(QResizeEvent *event)
{
	fitInView(scene.sceneRect());
}

bool Pitch::load_game()
{
	QString file_name = QFileDialog::getOpenFileName(this, tr("Wczytaj grę"),
								"", tr("Pliki zapisu gry (*.sav)"));
	QFile file(file_name);
	QMessageBox msgBox;
	Game* tmpgame;
	msgBox.setWindowTitle(M_DEF_TITLE);
	try
	{
		tmpgame = new Game();
		tmpgame->clear_pitch();
		if(!file.open(QIODevice::ReadOnly))
			throw QString(M_LOAD_ERR);
		QDataStream stream(&file);
		bool b;
		quint8 i;
		quint16 j;
		QList<quint8> pos_list;
		QList<QPair<quint8, quint8> > move_list;
		stream >> i;
		if(stream.status() != QDataStream::Ok)
			throw QString(M_LOAD_ERR);
		switch(i)
		{
		case two_hum:
			set_two_humans(); break;
		case white_hum:
			set_hum_white(); break;
		case black_hum:
			set_hum_black(); break;
		case zer_hum:
			set_two_comps(); break;
		default:
			throw QString(M_LOAD_ERR);
		}
		stream >> pos_list;
		if(stream.status() != QDataStream::Ok)
			throw QString(M_LOAD_ERR);
		for(int i = 0; i < 14; i++)
		{
			tmpgame->set_position(i, pos_list.at(i));
		}
		tmpgame->set_ball_owner(Game::WHITE, pos_list.at(14));
		tmpgame->set_ball_owner(Game::BLACK, pos_list.at(15));
		stream >> move_list;
		if(stream.status() != QDataStream::Ok)
			throw QString(M_LOAD_ERR);
		for(int i = 0; i < move_list.size(); i++)
		{
			Game::move m;
			m.first = move_list.at(i).first;
			m.second = move_list.at(i).second;
			tmpgame->add_move(m);
		}
		stream >> i;
		if(stream.status() != QDataStream::Ok)
			throw QString(M_LOAD_ERR);
		tmpgame->set_act_player((Game::color_spec) i);
		stream >> b;
		if(stream.status() != QDataStream::Ok)
			throw QString(M_LOAD_ERR);
		tmpgame->set_white_made_move(b);
		stream >> j;
		if(stream.status() != QDataStream::Ok)
			throw QString(M_LOAD_ERR);
		tmpgame->set_turn((int)j);
		stream >> b;
		if(stream.status() != QDataStream::Ok)
			throw QString(M_LOAD_ERR);
		tmpgame->set_passed(b);
		stream >> i;
		if(stream.status() != QDataStream::Ok)
			throw QString(M_LOAD_ERR);
		tmpgame->set_moves_made(i);
		if(tmpgame->moves_back_stored() > 0)
			emit enable_back(true);
	}
	catch(QString str)
	{
		msgBox.setText(str);
		msgBox.exec();
		delete tmpgame;
		file.close();
		return false;
	}
	file.close();
	delete game;
	game = tmpgame;
	if(game->get_game_state() != Game::CORRECT)
		ended = true;
	else
		ended = false;
	blocked = false;
	draw_pitch();
	return true;
}

void Pitch::save_game()
{
	if(game->get_game_state() == Game::INCORRECT)
	{
		QMessageBox msgBox;
		msgBox.setWindowTitle(M_DEF_TITLE);
		msgBox.setText(tr("Odmawiam zapisania niepoprawnego układu planszy."));
		msgBox.exec();
		return;
	}
	saving = true;
	blocked = true;
	QString file_name = QFileDialog::getSaveFileName(this, tr("Zapisz grę"),
											"", tr("Pliki zapisu gry (*.sav)"));
	QFile file(file_name);
	if(!file.open(QIODevice::WriteOnly))
	{
		QMessageBox msgBox;
		msgBox.setWindowTitle(M_DEF_TITLE);
		msgBox.setText(tr("Nie udało się dokonać zapisu do pliku."));
		msgBox.exec();
		blocked = false;
		saving = false;
		return;
	}
	QList<quint8> pos_list;
	QDataStream stream(&file);
	for(int i = 0; i < 14; i++)
	{
		pos_list.append(game->get_elem_pos(i));
	}
	pos_list.append(game->get_ball_owner(Game::WHITE));
	pos_list.append(game->get_ball_owner(Game::BLACK));
	QList<QPair<quint8, quint8> > move_list;
	for(int i = 0; i < game->moves_back_stored(); i++)
	{
		Game::move m = game->get_move(i);
		QPair<quint8, quint8> p(m.first, m.second);
		move_list.append(p);
	}
	stream << (quint8) mode << pos_list << move_list << (quint8) game->get_act_player();
	stream << (bool) game->get_white_made_move() << (quint16) game->get_turn();
	stream << (bool) game->get_passed() << (quint8) game->get_moves_made();
	file.close();
	blocked = false;
	saving = false;
	update_state();
}

bool Pitch::is_auto_to_move()
{
	Game::color_spec pl = game->get_act_player();
	switch(mode)
	{
	case white_hum:
		if(pl == Game::BLACK) return true; break;
	case black_hum:
		if(pl == Game::WHITE) return true; break;
	case zer_hum:
		return true; break;
	default:
		return false; break;
	}
	return false;
}

void Pitch::draw_pitch()
{
	scene.clear();
	footballers.clear();
	balls.clear();
	scene.update();
	viewport()->update();
	QPen line_pen(Qt::black);
	for(int i = 0; i < 8; i++)
	{
		scene.addLine(0, i * ROW_SIZE, 7 * ROW_SIZE, i * ROW_SIZE, line_pen);
		scene.addLine(i * ROW_SIZE, 0, i * ROW_SIZE, 7 * ROW_SIZE, line_pen);
	}
	fitInView(scene.sceneRect());
	for(int i = 0; i < 14; i++)
	{
		footballers.append(new Footballer(game->id_color(i), i % 7, ROW_SIZE));
		scene.addItem(footballers.last());
		footballers.last()->setPos(pos_to_coord(game->get_elem_pos(i)));
		QObject::connect(footballers.last(), SIGNAL(chosen(int,int)), this,
						 SLOT(show_pos_moves(int,int)));
		QObject::connect(footballers.last(), SIGNAL(anim_start()), this, SLOT(on_anim_start()));
		QObject::connect(footballers.last(), SIGNAL(anim_end()), this, SLOT(on_anim_end()));
	}
	balls.append(new Ball(Game::WHITE, ROW_SIZE));
	scene.addItem(balls.last());
	int wh_ball_pos = game->get_elem_pos(game->get_ball_owner(Game::WHITE));
	int bl_ball_pos = game->get_elem_pos(game->get_ball_owner(Game::BLACK));
	balls.last()->setPos(ball_pos_to_coord(wh_ball_pos));
	QObject::connect(balls.last(), SIGNAL(anim_start()), this, SLOT(on_anim_start()));
	QObject::connect(balls.last(), SIGNAL(anim_end()), this, SLOT(on_anim_end()));
	balls.append(new Ball(Game::BLACK, ROW_SIZE));
	scene.addItem(balls.last());
	balls.last()->setPos(ball_pos_to_coord(bl_ball_pos));
	QObject::connect(balls.last(), SIGNAL(anim_start()), this, SLOT(on_anim_start()));
	QObject::connect(balls.last(), SIGNAL(anim_end()), this, SLOT(on_anim_end()));
	update_state();
}

void Pitch::show_pos_moves(int team_int, int id)
{
	Game::color_spec team = (Game::color_spec) team_int;
	if(!game->get_edit_mode() &&
			(blocked || is_auto_to_move()))
		return;
	if(focus != -1)
	{
		focus = -1;
		clear_shadows();
	}
	int z = (game->has_ball(game->get_id(team, id)) ? 2 : 1);
	Game::int_vector moves = game->possible_moves(game->get_id(team, id));
	for(Game::int_vector::iterator it = moves.begin();
		it != moves.end(); ++it)
	{
		shadows.append(new ClickableShadow(*it, ROW_SIZE, z));
		scene.addItem(shadows.last());
		shadows.last()->setPos(pos_to_coord(*it));
		QObject::connect(shadows.last(), SIGNAL(chosen(int)), this,
						 SLOT(on_chosen(int)));
	}
	focus = game->get_elem_pos(game->get_id(team, id));
}

void Pitch::show_hint()
{
	if(blocked)
		return;
	clear_shadows();
	Game::move m = game->random_move();
	if(m.first == -1)
		return;
	int z = (game->has_ball(game->get_pos_id(m.first)) ? 2 : 1);
	shadows.append(new ClickableShadow(m.second, ROW_SIZE, z));
	scene.addItem(shadows.last());
	shadows.last()->setPos(pos_to_coord(m.second));
	QObject::connect(shadows.last(), SIGNAL(chosen(int)), this,
					 SLOT(on_chosen(int)));
	focus = m.first;
}

void Pitch::on_chosen(int pos)
{
	if(blocked)
		return;
	int fid = game->get_pos_id(focus);
	if(game->get_act_player() != game->id_color(fid) && !game->get_edit_mode())
	{
		end_turn();
	}
	clear_shadows();
	if(game->has_ball(fid))
	{
		int i = (game->id_color(fid) == Game::WHITE ? 0 : 1);
		balls.at(i)->move(ball_pos_to_coord(pos));
	}
	else
	{
		footballers.at(fid)->move(pos_to_coord(pos));
	}
	game->make_move(Game::move(focus, pos));
	focus = -1;
}

void Pitch::end_turn()
{
	if(blocked)
		return;
	if(game->get_edit_mode() || !game->can_end_turn())
	{
		QMessageBox msgBox;
		msgBox.setWindowTitle(M_DEF_TITLE);
		msgBox.setText(tr("Nie można zakończyć tury."));
		msgBox.exec();
		return;
	}
	clear_shadows();
	game->end_turn();
	update_state();
}

void Pitch::move_back()
{
	if(!get_auto_moves())
		switch_auto_moves();
	if(blocked)
		return;
	clear_shadows();
	Game::move m = game->move_back();
	if(m != Game::MOVE_END)
	{
		int fid = game->get_pos_id(m.first);
		if(game->has_ball(fid))
		{
			int i = (game->id_color(fid) == Game::WHITE ? 0 : 1);
			balls.at(i)->move(ball_pos_to_coord(m.first));
		} else
		{
			footballers.at(fid)->move(pos_to_coord(m.first));
		}
	}
	focus = -1;
}

void Pitch::move_forward()
{
	if(!get_auto_moves())
		switch_auto_moves();
	if(blocked)
		return;
	clear_shadows();
	Game::move m = game->move_forward();
	if(m != Game::MOVE_END)
	{
		int fid = game->get_pos_id(m.second);
		if(game->has_ball(fid))
		{
			int i = (game->id_color(fid) == Game::WHITE ? 0 : 1);
			balls.at(i)->move(ball_pos_to_coord(m.second));
		} else
		{
			footballers.at(fid)->move(pos_to_coord(m.second));
		}
	}
	focus = -1;
}

void Pitch::move_random()
{
	if(blocked || ended)
		return;
	Game::move m = game->random_move();
	if(m == Game::MOVE_END)
	{
		end_turn();
	}
	else
	{
		focus = m.first;
		on_chosen(m.second);
	}
}

void Pitch::on_anim_end()
{
	if(!saving)
		blocked = false;
	update_state();
}

void Pitch::on_anim_start()
{
	blocked = true;
}

void Pitch::switch_edit_mode()
{
	if(!game->get_edit_mode())
	{
		QMessageBox msgBox;
		msgBox.setWindowTitle(M_DEF_TITLE);
		msgBox.setText(tr("Przejście w tryb edycji oznacza utratę zapisanych ruchów."));
		msgBox.exec();
		game->set_edit_mode(true);

	} else
	{
		if(game->get_game_state() == Game::INCORRECT)
		{
			QMessageBox msgBox;
			msgBox.setWindowTitle(M_DEF_TITLE);
			msgBox.setText(tr("Odmawiam wznowienia gry w niepoprawnym stanie."));
			msgBox.exec();
		} else
		{
			game->set_edit_mode(false);
			ended = false;
			clear_shadows();
		}
	}
	update_state();
}

QPoint Pitch::pos_to_coord(int pos)
{
	QPoint result;
	result.setX((pos % 7) * ROW_SIZE);
	result.setY((pos - (pos % 7)) / 7 * ROW_SIZE);
	return result;
}

QPoint Pitch::ball_pos_to_coord(int pos)
{
	QPoint result = pos_to_coord(pos);
	result.setX(result.x() + 10);
	result.setY(result.y() + 10);
	return result;
}

QString Pitch::player_info()
{
	QString result;
	switch(game->get_act_player())
	{
	case Game::WHITE:
		result += M_WHITE;
		break;
	case Game::BLACK:
		result += M_BLACK;
		break;
	}
	if(is_auto_to_move())
		result += M_COM;
	else
		result += M_HUM;
	return result;
}

void Pitch::clear_shadows()
{
	foreach(ClickableShadow* sh, shadows)
	{
		scene.removeItem(sh);
		delete sh;
	}
	shadows.clear();
}

void Pitch::update_state()
{
	if(game->get_edit_mode())
	{
		emit(enable_back(false));
		emit(enable_forth(false));
		stat->setText(QString("Tryb edycji."));
		return;
	}
	if(ended)
		return;
	QString autoruchy =
			(get_auto_moves() ? M_AUTOMOVES_OFF : M_AUTOMOVES_ON);
	stat->setText(QString("Tura %1. Gracz %2. %3").arg(game->get_turn()).arg(player_info()).arg(autoruchy));
	Game::game_state state = game->get_game_state();
	QMessageBox msgBox;
	msgBox.setWindowTitle(M_DEF_TITLE);
	switch(state)
	{
	case Game::WHITE_WIN:
		stat->setText(M_WHITE_WIN);
		msgBox.setText(M_WHITE_WIN);
		msgBox.exec();
		ended = true;
		break;
	case Game::BLACK_WIN:
		stat->setText(M_BLACK_WIN);
		msgBox.setText(M_BLACK_WIN);
		ended = true;
		msgBox.exec();
		break;
	case Game::WHITE_UNFAIR:
		stat->setText(M_WHITE_UNFAIR);
		msgBox.setText(M_WHITE_UNFAIR);
		ended = true;
		msgBox.exec();
		break;
	case Game::BLACK_UNFAIR:
		stat->setText(M_BLACK_UNFAIR);
		msgBox.setText(M_BLACK_UNFAIR);
		ended = true;
		msgBox.exec();
		break;
	case Game::INCORRECT:
		stat->setText(M_INCORRECT);
		ended = true;
		break;
	default:
		break;
	}
	bool active_opt = false;
	switch(mode)
	{
	case white_hum:
		if(game->get_act_player() == Game::WHITE)
			active_opt = true;
		break;
	case black_hum:
		if(game->get_act_player() == Game::BLACK)
			active_opt = true;
		break;
	case two_hum:
		active_opt = true;
		break;
	}
	emit(enable_turn_end(active_opt));
	emit(enable_hint(active_opt));
	if(game->moves_back_stored() > 0)
		emit(enable_back(true));
	else
		emit(enable_back(false));
	if(game->moves_forward_stored() > 0)
		emit(enable_forth(true));
	else
		emit(enable_forth(false));
	if(is_auto_to_move() && !get_auto_moves())
		move_random();
}
