#ifndef _POKERGAME_H_
#define _POKERGAME_H_

#include <cstring>
#include <cstdio>
#include <list>
#include <set>
#include <algorithm>
#include <random>
#include <iterator>

#include <pbots_calc/pbots_calc.h>
#define MC_ITER 1000

#include "deck.h"
#include "player.h"
#include "action.h"
#include "card.h"

using namespace std;

class Player;


/**
 *  @brief Returns the string representation of the passed array of
 *  cards. It is used to interact with poker-eval lib routines.
 *
 *  @param cards An array of Cards
 *  @param n The number of cards in the array.
 */
char* str_cards(Card* cards, int n);

/**
 *  @brief Poker game simulator.
 */
class PokerGame {
public:

	/**
	 *  @brief Creates a new game engine with two players.
	 *
	 *  @param player1 An opponent.
	 *  @param player2 Another opponent.
	 */
	PokerGame(Player& player1, Player& player2);

	/**
	 *  @brief Destructor
	 */
	~PokerGame ();

	/**
	 *  @brief Runs the game. Only one single hand is played.
	 *
	 *  Player's statistics are not reseted.
	 */
	void
	playHand ();

	/**
	 *  @brief Resets the players and simulates a certain number of hands.
	 *
	 *  @param n Number of hands to be played.
	 */
	void
	playSeveralHands (int n);

	//const string& state ();
		
	/**
	 *  @brief Returns the current round bet.
	 */
	int
	roundBet () const { return _roundBet; }

	/**
	 *  @brief Returns the current effective stack.
	 */
	int
	effectiveStack () const {return _effectiveStack; }

	/**
	 *  @brief Sets whether the effective stack will be chosen randomly
	 *  or not.
	 *
	 *  @param b True to chose it randomly. False otherwise.
	 */
	void
	randomEffectiveStack (bool b) { _randomEffectiveStack = b; }
		
private:
	typedef list<Player*>::iterator player_t;

	list<Player*>::iterator& nextPlayer (list<Player*>::iterator& p);
	list<Player*>::iterator& removePlayer (list<Player*>::iterator& p);
	void betting_round ();
	double* showdown ();
	void prizes (double* shares);

	void output_results ();

	Deck _deck;

	list<Player*> _players,_playing;
	list<Player*>::iterator _dealer;
	list<Player*>::iterator _current,_first,_button,_sb;
	int _dealer_pos;
		
	set<Player*> _ready;

	Card _community_cards[5], _dead_cards[3];
	int _pot,_roundBet;
	int _minStack, _effectiveStack;

	bool _randomEffectiveStack;
};

#endif
