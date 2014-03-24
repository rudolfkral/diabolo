#ifndef GAME_H_
#define GAME_H_

/*
 * game.h
 *
 * Implements game logic.
 *
 */

#include <utility>
#include <boost/bimap.hpp>
#include <vector>

class Game
{
	public:
	Game();

	/*
	 * new_game - resets game state
	 */

	void new_game();

	/*
	 * color_spec - for determining color
	 */

	enum color_spec
	{
		WHITE = 0,
		BLACK = 1
	};

	/*
	 * NONE - specifies id of nothing
	 */

	static const int NONE = -1;

	/*
	 * get_id - get id of an item
	 *
	 * Example: get_id(WHITE, 2);
	 * gets second white footballer
	 *
	 */

	int get_id(const color_spec, const int) const;
	
	/*
	 * id_color - returns color of item identified by id
	 */

	color_spec id_color(const int fid) const { return (fid < 7) ? WHITE : BLACK; }

	/*
	 * get_elem_pos - get element's position
	 *
	 * Example: get_elem_pos(get_id(WHITE, 2))
	 * gets second white footballer's position
	 *
	 */

	int get_elem_pos(const int) const;

	/*
	 * get_pos_id - get id of element on pos
	 *
	 * NOTE: returns NONE if there is none
	 */

	int get_pos_id(const int) const;

	typedef std::pair<int, int> move_2d;

	/*
	 * get_pos_moved - get position moved by a specified vector
	 *
	 * Example: get_pos_moved(pos, move_2d(1, 0))
	 * gets (pos.x + 1, pos.y -1) translated to in-game position
	 *
	 */

	int get_pos_moved(const int, const move_2d) const;

	typedef std::vector<int> int_vector;

	/*
	 * get_pos_neigh - get neighbouring fields
	 *
	 * NOTE: diagonals don't count
	 * NOTE: returns only existent neighbouring fields
	 *
	 * Example: get_pos_neigh(0)
	 * should return {1, 7}
	 */

	int_vector get_pos_neigh(const int) const;

	/*
	 * get_pos_free_neigh - get neighboring fields which are
	 * empty at the moment
	 *
	 * NOTE: diagonals don't count
	 * NOTE: returns only existent neighbouring fields
	 *
	 * Example: get_pos_neigh(0)
	 * should return {7} in standard starting position
	 */

	int_vector get_pos_free_neigh(const int) const;

	/*
	 * get_ids_reachable_from - get ids of footballers
	 * reachable from pos
	 *
	 * NOTE: diagonals DO count
	 *
	 */

	int_vector get_ids_reachable_from(const int) const;

	/*
	 * get_ids_team_reachable_from - get ids of pos's
	 * footballer reachable team mates
	 *
	 * NOTE: diagonals DO count
	 */

	int_vector get_ids_team_reachable_from(const int) const;

	int get_ball_owner(const color_spec) const;

	bool has_ball(const int) const;

	int get_turn() const { return turn; }

	color_spec get_act_player() const { return act_player; }

	bool get_passed() const { return passed; }

	int get_moves_made() const { return moves_made; }
	
	/*
	 * move - WHERE has moved to WHERE
	 *
	 */

	typedef std::pair<int, int> move;

	static const move MOVE_END;

	bool make_move(const move);
	
	/*
	 * possible_moves - returns all possible moves of id
	 */

	int_vector possible_moves(const int) const;
	
	/*
	 * end_turn - end this turn
	 */

	void end_turn();
	
	bool is_turn_end(move m) const { return m == MOVE_END; }

	bool can_end_turn() const;

	/*
	 * random_move - generates random move
	 */

	move random_move() const;

	move get_last_move() const { return moves_history.back(); }

	/*
	 * move_back - goes back, returns the move and stores it in memory
	 */

	move move_back();

	/*
	 * move_forward - goes forward, returns the move
	 */

	move move_forward();

	/*
	 * moves_back_stored - returns quantity of stored past moves
	 */

	int moves_back_stored() const { return moves_history.size(); }

	/*
	 * moves_forward_stored - returns quantity of stored future moves
	 */

	int moves_forward_stored() const { return moves_forward.size(); }

	/*
	 * making_line - checks if team is making an impassable line
	 */

	bool making_line(const color_spec) const;
	
	/*
	 * get_opp_neighbours - returns opposing neighbours
	 */

	int_vector get_opp_neighbours(const int) const;
	
	/*
	 * game_state - describing game state
	 */

	enum game_state
	{
		WHITE_WIN,
		BLACK_WIN,
		WHITE_UNFAIR,
		BLACK_UNFAIR,
		CORRECT,
		INCORRECT
	};

	/*
	 * check_unfair
	 */

	bool check_unfair(const color_spec) const;

	/*
	 * get_game_state - returns current game state
	 */

	game_state get_game_state() const;

	/*
	 * set_edit_mode - sets edit mode
	 */

	void set_edit_mode(bool m);

	/*
	 * get_edit_mode - gets edit mode
	 */

	bool get_edit_mode() { return edit_mode; }

	/*
	 * get_move - returns move
	 */

	move get_move(int i);

	/*
	 * add_move - adds move to moves_history without making it
	 */

	void add_move(const move m);

	/*
	 * set_position - sets one's position, identyfying by id
	 */

	void set_position(const int fid, const int pt);

	/*
	 * clear_pitch - clears the pitch, needs to be set afterwards
	 */

	void clear_pitch() { bm.clear(); moves_history.clear(); moves_forward.clear(); }

	void set_act_player(const color_spec col) { act_player = col; }

	void set_turn(int t) { turn = t; }

	void set_passed(bool p) { passed = p; }

	bool get_white_made_move() { return white_made_move; }

	void set_white_made_move(bool m) { white_made_move = m; }

	void set_moves_made(int m) { moves_made = m; }

	void set_ball_owner(const color_spec col, const int fid);

	/*
	 * bad_game - returns true if game state is incorrect
	 */

	bool bad_game() const;
	
	private:
	
	/*
	 * bm - bidirectional map between ids and positions
	 */

	boost::bimap<int, int> bm;

	/*
	 * act_player - active player
	 */

	color_spec act_player;

	/*
	 * turn - current turn's number
	 */

	int turn;

	/*
	 * passed - true if ball was passed in this turn
	 */

	bool passed;

	/*
	 * white_made_move - true if white has already made his move
	 */

	bool white_made_move;

	/*
	 * moves_made - number of moves made in this turn
	 */

	int moves_made;

	/*
	 * ball_owner - balls owner's id
	 */

	int white_ball_owner, black_ball_owner;
	
	/*
	 * move_history - all moves from the beginning
	 */

	std::vector<move> moves_history;

	/*
	 * moves_forward - moves stored in memory due to going back
	 */

	std::vector<move> moves_forward;

	/*
	 * edit_mode - if on, no rules matter
	 */

	bool edit_mode;

	/*
	 * passed_past - goes back and checks if passed was on during
	 * last move
	 */

	bool passed_past();

	/*
	 * moves_made_past - goes back and counts moves already made
	 * during last move
	 */

	int moves_made_past();

	/*
	 * quiet_move - changes positions only
	 */

	void quiet_move(const move);

	/*
	 * reverse_move
	 */

	move reverse_move(const move m) const { return move(m.second, m.first); }

};

#endif
