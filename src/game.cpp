/*
 * game.cpp
 *
 * Implements game logic.
 *
 */

#include "game.h"
#include <cstdlib>
#include <ctime>
#include <cstdio>

const int Game::NONE;
const Game::move Game::MOVE_END(-1, -1);

const std::vector<Game::move_2d> dirs = {Game::move_2d(1, 0),
										  Game::move_2d(0, 1),
										  Game::move_2d(-1, 0),
										  Game::move_2d(0, -1)};
const std::vector<Game::move_2d> dirs_ext = {Game::move_2d(1, 0),
											  Game::move_2d(0, 1),
											  Game::move_2d(-1, 0),
											  Game::move_2d(0, -1),
											  Game::move_2d(-1, -1),
											  Game::move_2d(-1, 1),
											  Game::move_2d(1, 1),
											  Game::move_2d(1, -1)};
const std::vector<Game::move_2d> dirs_line = {Game::move_2d(1, -1),
										  Game::move_2d(1, 0),
										  Game::move_2d(1, 1)};

/*
 * default constructor
 *
 * Creates a default Diaballik game.
 */

Game::Game() : act_player(WHITE), turn(0), passed(false),
	white_made_move(false),moves_made(0), white_ball_owner(3),
	black_ball_owner(10), edit_mode(false)
{
	for(int i = 0; i < 7; i++)
	{
		bm.insert(boost::bimap<int, int>::value_type(i, i));
		bm.insert(boost::bimap<int, int>::value_type(i + 7, i + 42));
	}
}

/*
 * new_game
 *
 * Clears the pitch and move history.
 */

void Game::new_game()
{
	act_player = WHITE;
	turn = 0;
	passed = false;
	white_made_move = false;
	moves_made = 0;
	white_ball_owner = 3;
	black_ball_owner = 10;
	edit_mode = false;
	bm.left.clear();
	for(int i = 0; i < 7; i++)
	{
		bm.insert(boost::bimap<int, int>::value_type(i, i));
		bm.insert(boost::bimap<int, int>::value_type(i + 7, i + 42));
	}
	moves_history.clear();
	moves_forward.clear();
}

/*
 * get_int
 *
 * 0 - 6 - white figures, 7 - 14 - black figures
 */

int
Game::get_id(const color_spec col, const int num) const
{
	if(col == WHITE)
		return num;
	else
		return num + 7;
}

/*
 * get_elem_pos
 */

int
Game::get_elem_pos(const int elem) const
{
	return bm.left.at(elem);
}

/*
 * get_pos_id
 *
 * Uses bm to check if pt is assigned to any element. If it is, returns its
 * id. If not, returns NONE.
 */

int
Game::get_pos_id(const int pt) const
{
	if(bm.right.find(pt) == bm.right.end())
		return NONE;
	else
		return bm.right.at(pt);
}

/*
 * get_pos_moved
 *
 * Translates pt to carthesian coordinates, then adds u.
 *
 * NOTE: 0 is top left corner, not - as many would think - bottom left corner.
 */

int
Game::get_pos_moved(const int pt, const move_2d u) const
{
	int res_x = pt % 7;
	int res_y = (pt - res_x) / 7;
	res_x += u.first;
	if(res_x > 6 || res_x < 0)
		return -1;
	res_y -= u.second;
	return (res_y * 7) + res_x;
}

/*
 * get_pos_neigh
 *
 * Creates four vectors representing one move towards east, north, west and
 * south. Then, calculates neighboring positions.
 */

Game::int_vector
Game::get_pos_neigh(const int pt) const
{
	int_vector result;
	for(std::vector<move_2d>::const_iterator it = dirs.begin();
			it != dirs.end(); ++it)
	{
		int tmp = get_pos_moved(pt, *it);
		if(tmp >= 0 && tmp < 49)
			result.push_back(tmp);
	}
	return result;
}

/*
 * get_pos_free_neigh - gives a vector of free neighbouring fields
 *
 * Checks if each neighbouring field has an inhabitant. Returns a vector of
 * those who don't.
 */

Game::int_vector
Game::get_pos_free_neigh(const int pt) const
{
	int_vector result = get_pos_neigh(pt);
	for(int_vector::iterator it = result.begin();
			it != result.end(); ++it)
	{
		if(get_pos_id(*it) != NONE)
		{
			result.erase(it);
			--it;
		}
	}
	return result;
}

/*
 * get_ids_reachable_from - get ids of footballers reachable from pos
 *
 * Searches continously for footballers.
 */

Game::int_vector
Game::get_ids_reachable_from(const int pt) const
{
	int_vector result;
	for(std::vector<move_2d>::const_iterator it = dirs_ext.begin();
			it != dirs_ext.end(); ++it)
	{
		int cur_pos = pt;
		int cur_id;
		do
		{
			cur_pos = get_pos_moved(cur_pos, *it);
			cur_id = get_pos_id(cur_pos);
			if(cur_id != NONE)
			{
				result.push_back(cur_id);
				break;
			}
		} while (cur_pos >= 0 && cur_pos < 49);
	}
	return result;
}

/*
 * get_ids_team_reachable_from - get ids of pos's footballer reachable team mates
 *
 * Eliminates opponents from get_its_reachable_from result.
 */

Game::int_vector
Game::get_ids_team_reachable_from(const int pt) const
{
	int_vector result;
	color_spec cur_col = id_color(get_pos_id(pt));
	for(std::vector<move_2d>::const_iterator it = dirs_ext.begin();
			it != dirs_ext.end(); ++it)
	{
		int cur_pos = pt;
		int cur_id;
		do
		{
			cur_pos = get_pos_moved(cur_pos, *it);
			cur_id = get_pos_id(cur_pos);
			if(cur_id != NONE)
			{
				color_spec cur_id_col = id_color(cur_id);
				if(cur_id_col != cur_col)
					break;
				result.push_back(cur_id);
			}
		} while (cur_pos >= 0 && cur_pos < 49);
	}
	return result;
}

/*
 * get_ball_owner
 *
 * returns id of ball owner
 */

int
Game::get_ball_owner(const color_spec col) const
{
	if(col == WHITE)
		return white_ball_owner;
	else
		return black_ball_owner;
}

/*
 * has_ball
 * 
 * returns true if id has ball
 */

bool Game::has_ball(const int fid) const
{
	if(id_color(fid) == WHITE)
		return fid == white_ball_owner;
	else
		return fid == black_ball_owner;
}

/*
 * make_move
 *
 * If fid has ball, passes it to target at pt. If not, moves to the targe
 * field. Changes game flags(passed or moves). Returns true if move was 
 * ultimately made.
 */

bool Game::make_move(const move m)
{
	int fid = get_pos_id(m.first);
	int pt = m.second;
	if(fid == NONE && !edit_mode)
	{
		end_turn();
		return true;
	}
	int target_id = get_pos_id(pt);
	if(has_ball(fid))
	{
		color_spec this_col = id_color(fid);
		if(target_id == NONE || (id_color(target_id) != this_col))
			return false;
		if(this_col == WHITE)
			white_ball_owner = target_id;
		else
			black_ball_owner = target_id;
		if(!edit_mode)
			passed = true;
	}
	else
	{
		if(target_id != NONE)
			return false;
		bm.left.erase(fid);
		bm.insert(boost::bimap<int, int>::value_type(fid, pt));
		if(!edit_mode)
			++moves_made;
	}
	if(!edit_mode)
	{
		moves_history.push_back(m);
		if(moves_forward.size() > 0 && m != moves_forward.back())
			moves_forward.clear();
	}
	return true;
}

/*
 * possible_moves
 *
 * Returns all possible moves, with attention to game rules.
 */

Game::int_vector
Game::possible_moves(const int fid) const
{
	int_vector result;
	if(id_color(fid) != act_player && !edit_mode)
		return result;
	int pt = get_elem_pos(fid);
	if(has_ball(fid))
	{
		if(!passed || edit_mode)
		{
			Game::int_vector idv = get_ids_team_reachable_from(pt);
			for(Game::int_vector::iterator it = idv.begin();
				it != idv.end(); ++it)
			{
				result.push_back(get_elem_pos(*it));
			}
		}
	}
	else
	{
		if((moves_made < 2) || edit_mode)
			result = get_pos_free_neigh(pt);
	}
	return result;
}

/*
 * end_turn
 *
 * Ends turn, thus reseting game state.
 */

void Game::end_turn()
{
	if(edit_mode)
		return;
	if(white_made_move)
	{
		white_made_move = false;
		++turn;
	}
	else
		white_made_move = true;
	passed = false;
	moves_made = 0;
	act_player = ((act_player == WHITE) ? BLACK : WHITE);
}

bool Game::can_end_turn() const
{
	if(!passed && moves_made == 0)
		return false;
	return true;
}

/*
 * random_move
 *
 * Selects random move or turn-ending.
 */

Game::move Game::random_move() const
{
	srand(time(NULL));
	int obj;
	int tar = -1;
	bool can_end = can_end_turn();
	if(passed && moves_made >= 2)
		return MOVE_END;
	do
	{
		if((rand() % 25) == 0 && can_end)
			return MOVE_END;
		obj = get_id(act_player, rand() % 7);
		int_vector chosen = possible_moves(obj);
		if(chosen.size() == 0)
			continue;
		tar = chosen.at(rand() % chosen.size());
	} while(tar == -1);
	return move(get_elem_pos(obj), tar);
}

/* set_edit_mode
 * clears memory if edit mode
 */

void Game::set_edit_mode(bool m)
{
	edit_mode = m;
	moves_made = 0;
	passed = false;
	turn = 0;
	act_player = WHITE;
	moves_history.clear();
	moves_forward.clear();
}

Game::move Game::get_move(int i)
{
	return moves_history.at(i);
}

void Game::add_move(const move m)
{
	moves_history.push_back(m);
}

void Game::set_position(const int fid, const int pt)
{
	bm.insert(boost::bimap<int, int>::value_type(fid, pt));
}

void Game::set_ball_owner(const Game::color_spec col, const int fid)
{
	if(col == WHITE)
		white_ball_owner = fid;
	else
		black_ball_owner = fid;
}

bool Game::bad_game() const
{
	int wh_col = get_elem_pos(white_ball_owner);
	int bl_col = get_elem_pos(black_ball_owner);
	if(bl_col <= 6 && wh_col >= 42)
		return true;
	if(check_unfair(WHITE) && check_unfair(BLACK))
		return true;
	return false;
}

/*
 * move_back
 *
 * Gets last move, then checks if there was a player change in process.
 * If yes, counts moves made before to set game state. Switches 
 * active player. Makes reversed move. Returns it.
 */

Game::move Game::move_back()
{
	if(edit_mode)
		return MOVE_END;
	move last_move = moves_history.back();
	int fid = get_pos_id(last_move.second);
	if(act_player != id_color(fid))
	{
		// give control to previus player
		if(!white_made_move)
			--turn;
		passed = passed_past();
		moves_made = moves_made_past();
		/* 
		 * if(has_ball(fid))
		 * {
		 *
		 *	if(id_color(last_move.first) == WHITE)
		 * 		white_ball_owner = 
		 *			get_pos_id(last_move.first);
		 *	else
		 *		black_ball_owner =
		 *			get_pos_id(last_move.first);
		 *	passed = false;
		 * }
		 *  else
		 *  {
		 *	quiet_move(reverse_move(last_move));
		 *	--moves_made;
		 * }
		 */
		if(act_player == WHITE)
		{
			white_made_move = true;
			act_player = BLACK;
		}
		else
		{
			white_made_move = false;
			act_player = WHITE;
		}
		return MOVE_END;
	}
	else
	{
		if(has_ball(fid))
		{
			passed = false;
			if(act_player == WHITE)
				white_ball_owner =
					get_pos_id(last_move.first);
			else
				black_ball_owner =
					get_pos_id(last_move.first);
		}
		else
		{
			--moves_made;
			quiet_move(reverse_move(last_move));
		}
		moves_forward.push_back(last_move);
		moves_history.pop_back();
		return last_move;
	}
}

/*
 * passed_past - counts moves made in previous turn
 *
 * Stores the color of last move - in order to detect when did the turn
 * start. Goes maximum 3 steps backwards, checking if there was a pass.
 * If there was - returns true.
 */

bool Game::passed_past()
{
	if(moves_history.size() == 0)
		return false;
	std::vector<move> last_moves;
	bool result = false;
	color_spec col =
		id_color(get_pos_id(moves_history.back().second));
	for(std::vector<move>::reverse_iterator it =
			moves_history.rbegin();
			(it != moves_history.rend()) &&
			last_moves.size() < 3; ++it)
	{
		// make reverse move, check if there was a pass
		move m = *it;
		if(id_color(get_pos_id(m.second)) != col)
			break;
		if((get_pos_id(m.first) != NONE) &&
			(get_pos_id(m.second) != NONE))
		{
			result = true;
			break;
		}
		last_moves.push_back(m);
		quiet_move(reverse_move(m));
	}
	for(std::vector<move>::reverse_iterator it = last_moves.rbegin();
			it != last_moves.rend(); ++it)
	{
		quiet_move(*it);
	}
	return result;
}

/*
 * moves_made_past - counts moves made in previous turn
 *
 * Stores the color of last move. Then goes backwards and, checking if
 * there was a color(ergo player) change, counts moves. Goes maximum
 * 3 steps backwards.
 */

int Game::moves_made_past()
{
	if(moves_history.size() == 0)
		return 0;
	std::vector<move> last_moves;
	int result = 0;
	color_spec col =
		id_color(get_pos_id(moves_history.back().second));
	int i = 0;
	for(std::vector<move>::reverse_iterator it =
			moves_history.rbegin();
			(it != moves_history.rend()) &&
			i < 3; ++it)
	{
		// make reverse move, check if there was a move
		move m = *it;
		if(id_color(get_pos_id(m.second)) != col)
			break;
		// dont rewind passes, they don't count
		if(!((get_pos_id(m.first) != NONE) &&
			(get_pos_id(m.second) != NONE)))
		{
			++result;
			last_moves.push_back(*it);
			quiet_move(reverse_move(*it));
		}
		++i;
	}
	// return to original state
	for(std::vector<move>::reverse_iterator it = last_moves.rbegin();
			it != last_moves.rend(); ++it)
	{
		quiet_move(*it);
	}
	return result;
}

/*
 * quiet_move
 *
 * Only edits the bimap.
 */

void Game::quiet_move(const move m)
{
	int fid = get_pos_id(m.first);
	bm.left.erase(fid);
	bm.insert(boost::bimap<int, int>::value_type(fid, m.second));
}

/*
 * move_forward
 * 
 * Re-makes last stored future move. Detects if there was a turn end
 * in process.
 */

Game::move
Game::move_forward()
{
	if(edit_mode)
		return MOVE_END;
	move result = moves_forward.back();
	if(id_color(get_pos_id(result.first)) != act_player)
		end_turn();
	make_move(result);
	moves_forward.pop_back();
	return result;
}

/*
 * making_line
 *
 * Searches if there is a footballer in first column. If not, returns
 * false. Then, checks if every next footballer has a neighbour
 * on the right side of it. If yes, returns true.
 */

bool Game::making_line(const color_spec team) const
{
	int tmp_pos;
	bool status = false;
	for(int i = 0; i < 7; i++)
	{
		tmp_pos = get_elem_pos(get_id(team, i));
		if((tmp_pos % 7) == 0)
		{
			status = true;
			break;
		}
	}
	if(!status)
		return false;
	do
	{

		status = false;
		int tar_pos;
		for(std::vector<move_2d>::const_iterator it = dirs_line.begin();
				it != dirs_line.end(); ++it)
		{
			tar_pos = get_pos_moved(tmp_pos, *it);
			int tar_id = get_pos_id(tar_pos);
			if(tar_id != NONE && (id_color(tar_id) == team))
			{
				status = true;
				break;
			}
		}
		if(!status)
			return false;
		tmp_pos = tar_pos;
	} while((tmp_pos % 7) != 6);
	return true;
}

/*
 * get_opp_neighbours
 *
 * Returns vector of neighbours that belong to opposing team.
 */

Game::int_vector
Game::get_opp_neighbours(const int fid) const
{
	color_spec col = id_color(fid);
	int_vector vect = get_pos_neigh(get_elem_pos(fid));
	int_vector result;
	for(int_vector::iterator it = vect.begin();
			it != vect.end(); ++it)
	{
		int tmp_id = get_pos_id(*it);
		if(tmp_id != NONE && id_color(tmp_id) != col)
		{
			result.push_back(tmp_id);
		}
	}
	return result;
}

/*
 * check_unfair
 *
 * Checks if col is unfair.
 */

bool Game::check_unfair(const color_spec col) const
{
	if(making_line(col))
	{
		int_vector opp_neighs;
		for(int i = 0; i < 7; i++)
		{
			int_vector tmp_neighs =
				get_opp_neighbours(get_id(col, i));
			for(int_vector::iterator it = tmp_neighs.begin();
					it != tmp_neighs.end(); ++it)
			{
				if(std::find(opp_neighs.begin(),
							opp_neighs.end(),
							*it) ==
						opp_neighs.end())
				{
					opp_neighs.push_back(*it);
				}
			}
		}
		if(opp_neighs.size() >= 3)
			return true;
	}
	return false;
}
/*
 * game_state
 *
 * Checks if the ball owners are on the "winning" rows. If all whites or
 * blacks make a line, checks if there are at least three opponents
 * near to the line. If that's true, returns UNFAIR.
 */

Game::game_state
Game::get_game_state() const
{
	if(bad_game())
		return INCORRECT;
	if(edit_mode)
		return CORRECT;
	int wh_col = get_elem_pos(white_ball_owner);
	if(wh_col >= 42)
		return WHITE_WIN;
	int bl_col = get_elem_pos(black_ball_owner);
	if(bl_col <= 6)
		return BLACK_WIN;
	if(check_unfair(WHITE))
		return WHITE_UNFAIR;
	if(check_unfair(BLACK))
		return BLACK_UNFAIR;
	return CORRECT;
}
