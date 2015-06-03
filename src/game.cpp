#include "game.h"
#include <iostream>

using namespace std;

char*
str_cards(Card* cards, int n){
	char* str = new char[n*2+1];
	for (int i=0; i<n;i++)
		strncpy(&str[2*i],cards[i].cstr(),2);
	str[2*n] = '\0';

	return str;
}

PokerGame::PokerGame(Player& player1, Player& player2){
	_players.push_back(&player1);
	_players.push_back(&player2);
	_dealer = _players.begin();
	_pot = 0;
	_roundBet = 0;
	_effectiveStack = 20;
	_randomEffectiveStack = false;
	_dealer_pos = 0;
}

PokerGame::~PokerGame(){
}

void
PokerGame::playSeveralHands (int n){
	std::default_random_engine generator;
	std::uniform_int_distribution<int> uniform(3,20);

	for (player_t it = _players.begin(); it != _players.end(); it++){
		(*it)->stack(1000);
	}

	int num_players = _players.size();
	int i = 0;

	while (num_players > 1 && i++ < n){
		if (_randomEffectiveStack) _effectiveStack = uniform(generator);
		else _effectiveStack = 20;

		//Reset players
		for (player_t it = _players.begin(); it != _players.end(); it++){
			(*it)->bet(0);
			_effectiveStack = min(_effectiveStack,(*it)->stack());
		}

		playHand();

		for (player_t it = _players.begin(); it != _players.end(); it++){
			if ((*it)->stack() < 3){
				removePlayer(it);
				num_players--;
			}
		}

		/**
		 * Important: for more than 2 players this may not work when one of the
		 * players was removed.
		 */
		_dealer_pos = (_dealer_pos + 1) % _players.size();
	}
}

void
PokerGame::playHand () {
	//Create a new round of players.
	_playing = list<Player*>(_players.begin(),_players.end());
	_ready.clear();
	_pot = 0;
	_roundBet = 0;

	//Establish the first player.
	_current = _playing.begin();
	advance(_current, _dealer_pos);
	_first = _current;

	//Shuffle the deck.
	_deck.shuffle();

	//Establish each players' role
	//Button
	_button = _current;

	//Small blind
	_sb = nextPlayer(_current);
	(*_current)->setRole(PlayerRole::SB);
	(*_current)->bet((*_current)->bet() + 1);
	_pot += 1;

	//Big blind
	nextPlayer(_current);
	(*_current)->setRole(PlayerRole::BB);
	(*_current)->bet((*_current)->bet() + 2);
	_pot += 2;

	_roundBet = 3; //blinds
	
	// Give the initial cards
	_current = _sb;
	do {
		(*_current)->firstCard(_deck.popCard());
		(*_current)->secondCard(_deck.popCard());
	}
	while (nextPlayer(_current) != _sb);

	// Preflop betting rounds --
	betting_round();

	// Flop
	_dead_cards[0] = _deck.popCard(); // Burn a card
	_community_cards[0] = _deck.popCard();
	_community_cards[1] = _deck.popCard();
	_community_cards[2] = _deck.popCard();
	if (_roundBet < _effectiveStack)
		betting_round();

	// Turn
	_dead_cards[1] = _deck.popCard(); // Burn a card
	_community_cards[3] = _deck.popCard();
	if (_roundBet < _effectiveStack)
		betting_round();

	// River
	_dead_cards[2] = _deck.popCard(); // Burn a card
	_community_cards[4] = _deck.popCard();
	if (_roundBet < _effectiveStack)
		betting_round();

	// Showdown
	double* shares;
	if (_playing.size() > 1)
		shares = showdown();
	else {
		shares = new double[1];
		*shares = 1;
	}

	prizes(shares);
}

void
PokerGame::betting_round () {
	int num_players = _playing.size();
	while (num_players > 1 && _ready.size() != num_players){
		Action* a = (*_current)->action(this);
		if (a->type() == ActionType::FOLD){
			num_players--;
			// pot stays the same
			(*_current)->update_ev(-(*_current)->bet());
			removePlayer(_current); //_playing and _current
			// _ready remains the same
			
		} else if (a->type() == ActionType::CALL) {
			int increase = a->to() - (*_current)->bet();
			_pot += increase; //_pot
			//_playing remains the same
			_ready.insert(*_current); // _ready
			(*_current)->bet((*_current)->bet() + increase);
			nextPlayer(_current); //_current

		} else if (a->type() == ActionType::RAISE){
			int increase = a->to() - (*_current)->bet();
			_pot += increase; // _pot
			_roundBet = a->to();
			//_playing remains the same
			_ready.clear(); // _ready
			_ready.insert(*_current);
			(*_current)->bet((*_current)->bet() + increase);
			nextPlayer(_current); //_current
		}

		delete a;
	}
}

double*
PokerGame::showdown (){
	player_t tmp = _first;
	char* hands = new char[_playing.size()*5], *ptr = hands;
	char* str_com = str_cards(_community_cards, 5);
	char* str_dead = str_cards(_dead_cards,3);
	Results* res = alloc_results();

	char* tmp_hand = str_cards((*tmp)->hand(),2);
	strncpy(ptr,tmp_hand,4);
	delete [] tmp_hand;
	ptr += 4;
	nextPlayer(tmp);

	do {
		strncpy(ptr++,":",1);
		tmp_hand = str_cards((*tmp)->hand(),2);
		strncpy(ptr,tmp_hand,4);
		delete [] tmp_hand;
		ptr += 4;
	}
	while (nextPlayer(tmp) != _first);
	*ptr = '\0';

	calc(hands, str_com, str_dead, MC_ITER, res);

	double* shares = new double[res->size];
	memcpy(shares,res->ev,res->size*sizeof(double));

	free_results(res);
	delete [] hands;
	delete [] str_com;
	delete [] str_dead;

	return shares;
}

void
PokerGame::prizes (double* shares){
	player_t tmp = _first;

	double* rptr = shares;
	do {
		(*tmp)->update_ev(_pot*(*rptr) - (*tmp)->bet());
		rptr++;
	} while (nextPlayer(tmp) != _first);

	delete [] shares;
}

void
PokerGame::output_results (){
	FILE* out = fopen("data.csv","a");
	if (out != NULL){
		fprintf(out,"%f\n",_players.front()->ev());
		fclose(out);
	}
}

list<Player*>::iterator&
PokerGame::nextPlayer (list<Player*>::iterator& p){
	if (++p == _playing.end())
		p = _playing.begin();
	return p;
}

list<Player*>::iterator&
PokerGame::removePlayer (list<Player*>::iterator& p){
	if (_first == p) nextPlayer(_first);
	p = _playing.erase(p);
	if (p == _playing.end()) p = _playing.begin();
	return p;
}
