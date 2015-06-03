#include "player.h"


Player::Player() :
	_stack(2000),
	_acc(0),
	_outcome(0),
	_bet(0),
	_ev(0),
	_nhands(0),
	observers(0),
	_nsb(0),
	_nbb(0),
	_nraised(0),
	_ncalled(0) {}

void
Player::update_ev (int quantity) {
	_stack += quantity;
	_acc += quantity;
	_ev = (_ev*_nhands + quantity)/_nhands;
	if (quantity > 0) _outcome++;
	else if (quantity < 0) _outcome--;
	_nhands++;
	notifyValueChanged(ev());
}

Action*
Player::action(PokerGame* game) {
	Action* a = caction(game);
	if (_role == PlayerRole::SB){
		_nsb++;
		if (a->type() == ActionType::RAISE) _nraised++;
	}
	else if (_role == PlayerRole::BB){
		_nbb++;
		if (a->type() == ActionType::CALL) _ncalled++;
	}

	return a;
}

void
Player::notifyValueChanged (double value){
	for (std::vector<PlayerObserver*>::iterator it = observers.begin(); it != observers.end(); it++)
		(*it)->valueChanged(value);
}

Action*
PlayerAlwaysIn::caction(PokerGame* game){
	if (_role == PlayerRole::SB)
		return new Action(ActionType::RAISE,game->effectiveStack());
	else
		return new Action(ActionType::CALL,game->roundBet());
}

bool
PlayerAlwaysIn::equal (const Player& other){
	if (this == &other) return true;

	try { dynamic_cast<const PlayerAlwaysIn&>(other); }
	catch (std::bad_cast& e) {return false;}

	return true;
}

Action*
PlayerAlwaysOut::caction(PokerGame* game){
	return new Action(ActionType::FOLD,0);
}

bool
PlayerAlwaysOut::equal (const Player& other){
	if (this == &other) return true;

	try { dynamic_cast<const PlayerAlwaysOut&>(other); }
	catch (std::bad_cast& e) {return false;}

	return true;
}

Action*
PlayerNash::caction(PokerGame* game){
	if (_role == PlayerRole::SB){
		if (game->effectiveStack() <= sb_max_stack[_hand[0].rank()][_hand[1].rank()])
			return new Action(ActionType::RAISE,game->effectiveStack());
		else
			return new Action(ActionType::FOLD,0);
	}
	else {
		if (game->effectiveStack() <= bb_max_stack[_hand[0].rank()][_hand[1].rank()])
			return new Action(ActionType::CALL,game->roundBet());
		else
			return new Action(ActionType::FOLD,0);
	}
}

bool
PlayerNash::equal (const Player& other){
	if (this == &other) return true;

	try { dynamic_cast<const PlayerNash&>(other); }
	catch (std::bad_cast& e) {return false;}

	return true;
}

Action*
PlayerRaiseFoldPercent::caction (PokerGame* game){
	if (_role == PlayerRole::SB){
		if (handPercent(_hand[0],_hand[1]) < _r){
			return new Action(ActionType::RAISE,game->effectiveStack());
		}
		else
			return new Action(ActionType::FOLD,0);
	}
	else {
		if (handPercent(_hand[0],_hand[1]) < _c){
			return new Action(ActionType::CALL,game->roundBet());
		}
		else
			return new Action(ActionType::FOLD,0);	
	}
}

bool
PlayerRaiseFoldPercent::equal (const Player& other){
	if (this == &other) return true;

	try {
		const PlayerRaiseFoldPercent& o = dynamic_cast<const PlayerRaiseFoldPercent&>(other);
		if (o._r == _r && o._c == _c)
			return true;
		else return false;
	}
	catch (std::bad_cast& e) {return false;}
}

Action*
RCPlayer::caction (PokerGame* game){
	if (_role == PlayerRole::SB){
		if (handPercent(_hand[0],_hand[1]) < r())
			return new Action(ActionType::RAISE,game->effectiveStack());
		else
			return new Action(ActionType::FOLD,0);
	}
	else {
		if (handPercent(_hand[0],_hand[1]) < c())
			return new Action(ActionType::CALL,game->roundBet());
		else
			return new Action(ActionType::FOLD,0);
	}
}

bool
RCPlayer::equal (const Player& other){
	if (this == &other) return true;

	try {
		const RCPlayer& o = dynamic_cast<const RCPlayer&>(other);
		float norm = 0;
		for (int i=0; i<length(); i++)
			norm += fabs(gene(i) - o.gene(i));

		if (norm < 0.02) return true;
		else return false;
	}
	catch (std::bad_cast& e){ return false; }
}

void
RCPlayer::init (GAGenome& g){
	GA1DArrayGenome<float>& genome = dynamic_cast<GA1DArrayGenome<float>&>(g);
	GARandomSeed();
	genome.gene(0,GARandomFloat(0,1));
	genome.gene(1,GARandomFloat(0,1));
}

RCTPlayer::RCTPlayer() :
	GA3DArrayGenome(NRANKS,NRANKS,2),
	Player ()
{
	initializer(init);
	crossover(mergeCrossover);
}


RCTPlayer::RCTPlayer (float raise, float call) :
	GA3DArrayGenome(NRANKS,NRANKS,2),
	Player ()
{
	initializer(init);
	crossover(mergeCrossover);

	for (int i=0;i<this->width(); i++)
		for (int j=0;j<this->height(); j++){
			if (handPercentTableCoords(i,j) < raise) gene(i,j,0,20);
			else gene(i,j,0,0);

			if (handPercentTableCoords(i,j) < call) gene(i,j,1,20);
			else gene(i,j,1,0);
		}
}

char*
RCTPlayer::desc (char* buffer) const {
	char* desc = buffer, *ptr = desc;
	for (int i = 0; i<NRANKS; i++){
		for (int j=0; j<NRANKS; j++){
			ptr += sprintf(ptr,"%d ",raiseTable(i,j));
		}
		*ptr = '\n';ptr++;
	}
	*ptr = '\0';

	return desc;
}

bool
RCTPlayer::equal (const Player& other){
	if (this == &other) return true;
	else return false;

	/* Norm comparator
	int norm = 0;
	for (int i=0; i<width(); i++)
		for (int j=0; j<height(); j++)
			for (int k=0; k<depth(); k++)
				norm += abs(gene(i,j,k) - o.gene(i,j,k));

	if (norm <= 1) return true;
	else return false;*/
}

void
RCTPlayer::init (GAGenome& g){
	GARandomSeed();
	GA3DArrayGenome<int>& genome = dynamic_cast<GA3DArrayGenome<int>&>(g);
	for (int i=0;i<genome.width(); i++)
		for (int j=0;j<genome.height(); j++)
			for (int k=0;k<genome.depth(); k++){
				genome.gene(i,j,k,GARandomInt(3,20));
			}
}

//Constant!!
void
RCTPlayer::AllOrNothingInitializer (GAGenome& g){
	GA3DArrayGenome<int>& genome = dynamic_cast<GA3DArrayGenome<int>&>(g);
	for (int i=0;i<genome.width(); i++)
		for (int j=0;j<genome.height(); j++)
			for (int k=0;k<genome.depth(); k++){
				genome.gene(i,j,k,20*GARandomInt(0,1));
			}
}

void
RCTPlayer::raiseCallInitializer (GAGenome& g){
	GA3DArrayGenome<int>& genome = dynamic_cast<GA3DArrayGenome<int>&>(g);
	float raise = GARandomFloat(0,1);
	float call = GARandomFloat(0,1);

	for (int i=0;i<genome.width(); i++)
		for (int j=0;j<genome.height(); j++){
			if (handPercentTableCoords(i,j) < raise) genome.gene(i,j,0,20);
			else genome.gene(i,j,0,0);

			if (handPercentTableCoords(i,j) < call) genome.gene(i,j,1,20);
			else genome.gene(i,j,1,0);
		}
}

int
RCTPlayer::mergeCrossover(const GAGenome& p1, const GAGenome& p2, GAGenome* c1, GAGenome* c2){
	float pSwap = 0.03;
	const GA3DArrayGenome<int>& mom = dynamic_cast<const GA3DArrayGenome<int>&>(p1);
	const GA3DArrayGenome<int>& dad = dynamic_cast<const GA3DArrayGenome<int>&>(p2);
	GA3DArrayGenome<int>* sis = dynamic_cast<GA3DArrayGenome<int>*>(c1);
	GA3DArrayGenome<int>* bro = dynamic_cast<GA3DArrayGenome<int>*>(c2);

	int n = 0;
	if (sis) n++;
	if (bro) n++;

	for (int k=0; k<mom.depth();k++){
		for (int i=0; i<mom.width();i++){
			for (int j=0; j<mom.height();j++){
				if (GAFlipCoin(pSwap)){
					if(sis) sis->gene(i,j,k,dad.gene(i,j,k));
					if (bro) bro->gene(i,j,k,mom.gene(i,j,k));
				}
				else {
					if(sis) sis->gene(i,j,k,mom.gene(i,j,k));
					if (bro) bro->gene(i,j,k, dad.gene(i,j,k));
				}
			}
		}
	}

	return n;
}

int
RCTPlayer::SubtleMutator (GAGenome& c, float pmut){
	GA3DArrayGenome<int>& genome = dynamic_cast<GA3DArrayGenome<int>&>(c);
	if (pmut <= 0) return 0;

	int n = 0;
	for (int i=0; i<genome.width(); i++)
		for (int j=0; j<genome.height(); j++){
			for (int k=0; k<genome.depth(); k++){
				if (GAFlipCoin(pmut)){
					int m = genome.gene(i,j,k);
					if (m==20) m -= GARandomInt(0,2);
					else if (m==0) m += GARandomInt(0,2);
					else {
						m += (GARandomInt()) ? GARandomInt(0,2) : -GARandomInt(0,2);
						if (m<0) m=0;
						else if (m>20) m=20;
					}

					genome.gene(i,j,k,m);
					n++;
				}
			}
		}

	return n;
}

Action*
RCTPlayer::caction (PokerGame* game){
	int h = handToNumeric(_hand[0], _hand[1]);
	int r = raiseTable(h/13,h%13);
	int c = callTable(h/13,h%13);
	if (_role == PlayerRole::SB){
		if (game->effectiveStack() <= raiseTable(h/13,h%13)){
			return new Action(ActionType::RAISE,game->effectiveStack());}
		else
			return new Action(ActionType::FOLD,0);
	}
	else {
		if(game->effectiveStack() <= callTable(h/13,h%13)){
			return new Action(ActionType::CALL,game->roundBet());}
		else
			return new Action(ActionType::FOLD,0);

	}
}
