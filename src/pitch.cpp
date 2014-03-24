#include "pitch.h"
#include "clickableshadow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QDataStream>

enum game_modes {
	two_hum,
	one_hum,
	zer_hum
};

Pitch::Pitch(QWidget *parent) :
	QGraphicsView(parent), focus(-1), blocked(false), two_humans(false),
	human_comp(false), two_comps(false), wait_for_poke(true), ended(false)
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
	two_humans = true;
	two_comps = false;
	human_comp = false;
}

void Pitch::set_human_comp()
{
	two_humans = false;
	two_comps = false;
	human_comp = true;
}

void Pitch::set_two_comps()
{
	two_humans = false;
	two_comps = true;
	human_comp = false;
}

void Pitch::new_game()
{
	delete game;
	game = new Game();
	draw_pitch();
}

void Pitch::load_game()
{
	QString file_name = QFileDialog::getOpenFileName(this, tr("Wczytaj grę"),
								"", tr("Pliki zapisu gry (*.sav)"));
	QFile file(file_name);
	QMessageBox msgBox;
	Game* tmpgame;
	msgBox.setWindowTitle(tr("Uwaga"));
	try
	{
		tmpgame = new Game();
		tmpgame->clear_pitch();
		if(!file.open(QIODevice::ReadOnly))
			throw QString(tr("Nie udało się wczytać pliku."));
		QDataStream stream(&file);
		bool b;
		quint8 i;
		quint16 j;
		QList<quint8> pos_list;
		QList<QPair<quint8, quint8> > move_list;
		stream >> i;
		if(stream.status() != QDataStream::Ok)
			throw QString(tr("Błąd podczas wczytywania trybu gry."));
		switch(i)
		{
		case two_hum:
			set_two_humans(); break;
		case one_hum:
			set_human_comp(); break;
		case zer_hum:
			set_two_comps(); break;
		default:
			throw QString(tr("Błędny tryb gry."));
		}
		stream >> pos_list;
		if(stream.status() != QDataStream::Ok)
			throw QString(tr("Błąd podczas wczytywania pozycji."));
		for(int i = 0; i < 14; i++)
		{
			tmpgame->set_position(i, pos_list.at(i));
		}
		tmpgame->set_ball_owner(Game::WHITE, pos_list.at(14));
		tmpgame->set_ball_owner(Game::BLACK, pos_list.at(15));
		stream >> move_list;
		if(stream.status() != QDataStream::Ok)
			throw QString(tr("Błąd podczas wczytywania historii."));
		for(int i = 0; i < move_list.size(); i++)
		{
			Game::move m;
			m.first = move_list.at(i).first;
			m.second = move_list.at(i).second;
			tmpgame->add_move(m);
		}
		stream >> b;
		if(stream.status() != QDataStream::Ok)
			throw QString(tr("Błąd podczas wczytywania gracza."));
		tmpgame->set_act_player(b);
		stream >> b;
		if(stream.status() != QDataStream::Ok)
			throw QString(tr("Błąd podczas wczytywania gracza."));
		tmpgame->set_white_made_move(b);
		stream >> j;
		if(stream.status() != QDataStream::Ok)
			throw QString(tr("Błąd podczas wczytywania ruchu."));
		tmpgame->set_turn((int)j);
		stream >> b;
		if(stream.status() != QDataStream::Ok)
			throw QString(tr("Błąd podczas wczytywania historii."));
		tmpgame->set_passed(b);
		stream >> i;
		if(stream.status() != QDataStream::Ok)
			throw QString(tr("Błąd podczas wczytywania historii."));
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
		return;
	}
	file.close();
	delete game;
	game = tmpgame;
	draw_pitch();
	update_state();
}

void Pitch::save_game()
{
	if(game->get_game_state() == Game::INCORRECT)
	{
		QMessageBox msgBox;
		msgBox.setWindowTitle(tr("Uwaga"));
		msgBox.setText(tr("Odmawiam zapisania niepoprawnego układu planszy."));
		msgBox.exec();
		return;
	}
	QString file_name = QFileDialog::getSaveFileName(this, tr("Zapisz grę"),
											"", tr("Pliki zapisu gry (*.sav)"));
	QFile file(file_name);
	if(!file.open(QIODevice::WriteOnly))
	{
		QMessageBox msgBox;
		msgBox.setWindowTitle(tr("Uwaga"));
		msgBox.setText(tr("Nie udało się dokonać zapisu do pliku."));
		msgBox.exec();
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
	game_modes gmode;
	if(two_humans)
		gmode = two_hum;
	if(human_comp)
		gmode = one_hum;
	if(two_comps)
		gmode = zer_hum;
	stream << (quint8) gmode << pos_list << move_list << (bool) game->get_act_player();
	stream << (bool) game->get_white_made_move() << (quint16) game->get_turn();
	stream << (bool) game->get_passed() << (quint8) game->get_moves_made();
	file.close();
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
		footballers.append(new Footballer(i < 7, i % 7, ROW_SIZE));
		scene.addItem(footballers.last());
		footballers.last()->setPos(pos_to_coord(game->get_elem_pos(i)));
		QObject::connect(footballers.last(), SIGNAL(chosen(bool,int)), this,
						 SLOT(show_pos_moves(bool,int)));
		QObject::connect(footballers.last(), SIGNAL(anim_start()), this, SLOT(on_anim_start()));
		QObject::connect(footballers.last(), SIGNAL(anim_end()), this, SLOT(on_anim_end()));
	}
	balls.append(new Ball(Game::WHITE, ROW_SIZE));
	scene.addItem(balls.last());
	balls.last()->setPos(ball_pos_to_coord(game->get_elem_pos(game->get_ball_owner(Game::WHITE))));
	QObject::connect(balls.last(), SIGNAL(anim_start()), this, SLOT(on_anim_start()));
	QObject::connect(balls.last(), SIGNAL(anim_end()), this, SLOT(on_anim_end()));
	balls.append(new Ball(Game::BLACK, ROW_SIZE));
	scene.addItem(balls.last());
	balls.last()->setPos(ball_pos_to_coord(game->get_elem_pos(game->get_ball_owner(Game::BLACK))));
	QObject::connect(balls.last(), SIGNAL(anim_start()), this, SLOT(on_anim_start()));
	QObject::connect(balls.last(), SIGNAL(anim_end()), this, SLOT(on_anim_end()));
	update_state();
}

void Pitch::show_pos_moves(bool team, int id)
{
	if(!game->get_edit_mode() &&
			(blocked ||
			(human_comp && (game->get_act_player() != Game::WHITE)) ||
			two_comps))
		return;
	if(focus != -1)
	{
		focus = -1;
		clear_shadows();
	}
	int z = (game->has_ball(game->get_id(team, id)) ? 2 : 1);
	Game::pos_vector moves = game->possible_moves(game->get_id(team, id));
	for(Game::pos_vector::iterator it = moves.begin();
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

void Pitch::on_chosen(int pos)
{
	if(blocked)
		return;
	Game::id fid = game->get_pos_id(focus);
	if(game->get_act_player() != game->id_color(fid))
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
	update_state();
	emit enable_back(true);
	if(game->moves_forward_stored() == 0)
		emit enable_forth(false);
}

void Pitch::end_turn()
{
	if(game->get_edit_mode())
		return;
	clear_shadows();
	game->end_turn();
	update_state();
	if((human_comp && (game->get_act_player() != Game::WHITE) && !wait_for_poke)
			|| (two_comps && !wait_for_poke))
		move_random();
	else
		emit enable_back(true);
}

void Pitch::move_back()
{
	if(blocked)
		return;
	clear_shadows();
	Game::move m = game->move_back();
	if(m != Game::move(-1, -1))
	{
		Game::id fid = game->get_pos_id(m.first);
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
	update_state();
	emit enable_forth(true);
	if(game->moves_back_stored() == 0)
		emit enable_back(false);
}

void Pitch::move_forward()
{
	if(blocked)
		return;
	clear_shadows();
	Game::move m = game->move_forward();
	if(m != Game::move(-1, -1))
	{
		Game::id fid = game->get_pos_id(m.second);
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
	update_state();
	if(game->moves_forward_stored() == 0)
		emit enable_forth(false);
	emit enable_back(true);
}

void Pitch::move_random()
{
	if(blocked || ended)
		return;
	Game::move m = game->random_move();
	if(m == Game::move(-1, -1))
	{
		end_turn();
		if((human_comp && (game->get_act_player() != Game::WHITE) && !wait_for_poke)
				|| (two_comps && !wait_for_poke))
			move_random();
	}
	else
	{
		focus = m.first;
		on_chosen(m.second);
	}
}

void Pitch::on_anim_end()
{
	blocked = false;
	if(((human_comp && (game->get_act_player() != Game::WHITE) && !wait_for_poke)
			|| (two_comps && !wait_for_poke))
			&& !game->get_edit_mode())
		move_random();
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
		msgBox.setWindowTitle(tr("Uwaga"));
		msgBox.setText(tr("Przejście w tryb edycji oznacza utratę zapisanych ruchów."));
		msgBox.exec();
		game->set_edit_mode(true);
		emit enable_back(false);
		emit enable_forth(false);
	} else
	{
		if(game->get_game_state() == Game::INCORRECT)
		{
			QMessageBox msgBox;
			msgBox.setWindowTitle(tr("Uwaga"));
			msgBox.setText(tr("Odmawiam wznowienia gry w niepoprawnym stanie."));
			msgBox.exec();
		} else
		game->set_edit_mode(false);
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
	/* if(game->get_passed() == true && game->get_moves_made() == 2)
	 * {
	 * 	end_turn();
	 * }
	 */
	if(game->get_edit_mode())
	{
		stat->setText(QString("Tryb edycji."));
		return;
	}
	QString gracz =
			(game->get_act_player() == Game::WHITE ? tr("biały") : tr("czarny"));
	stat->setText(QString("Tura %1. Gracz %2.").arg(game->get_turn()).arg(gracz));
	Game::game_state state = game->get_game_state();
	QMessageBox msgBox;
	msgBox.setWindowTitle(tr("Uwaga"));
	switch(state)
	{
	case Game::WHITE_WIN:
		stat->setText(tr("Wygrana białych."));
		msgBox.setText(tr("Wygrana białych."));
		msgBox.exec();
		ended = true;
		break;
	case Game::BLACK_WIN:
		stat->setText("Wygrana czarnych.");
		msgBox.setText(tr("Wygrana czarnych."));
		ended = true;
		msgBox.exec();
		break;
	case Game::WHITE_UNFAIR:
		stat->setText(tr("Biali przegrywają z powodu gry nie fair."));
		msgBox.setText(tr("Biali przegrywają z powodu gry nie fair."));
		ended = true;
		msgBox.exec();
		break;
	case Game::BLACK_UNFAIR:
		stat->setText(tr("Czarni przegrywają z powodu gry nie fair."));
		msgBox.setText(tr("Czarni przegrywają z powodu gry nie fair."));
		ended = true;
		msgBox.exec();
		break;
	case Game::INCORRECT:
		stat->setText(tr("Stan gry niepoprawny."));
		ended = true;
		break;
	default:
		ended = false;
		break;
	}
}
