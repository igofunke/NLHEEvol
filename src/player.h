
#ifndef _PLAYER_H
#define _PLAYER_H

#include "card.h"
#include "action.h"
#include "game.h"
#include "handutils.h"

#include <ga/ga.h>

#include "gnuplot-iostream.h"

#include <iostream>

class PokerGame;

enum PlayerRole {SB,BB};

#define MAX_PLAYER_DESC 1024
#define TABLE_INFO_WEIGHT_CONSTANT 14112

#define PLOTTING

/**
 *  @brief Base class for an observer expecting player events.
 */
class PlayerObserver {
public:
	virtual
	~PlayerObserver () {}

	/**
	 * @brief Event raised when a player wins or loses chips.
	 */
	virtual void
	valueChanged (double value) = 0;
};

#ifdef PLOTTING

/**
 * @brief Plots the performance of a player during a game.
 */
class PlayerValuePlotter : public PlayerObserver{
public:
	/**
	 * @brief Constructs the plotter.
	 */
	PlayerValuePlotter () :
		gp("gnuplot -persistent")
		{}

	/**
	 *  Event. Refresh the graph.
	 */
	void valueChanged (double value){
		points.push_back(value*100);
		gp << "plot '-' binary" << gp.binFmt1d(points,"array") << "with lines notitle" << std::endl;
		gp.sendBinary1d(points);
		gp.flush();
	}
private:
	Gnuplot gp;

	std::vector<double> points;
};

#endif

/**
 *  @brief Base class for a player.
 */
class Player {
public:

	/**
	 *  @brief Constructs the player.
	 */
	Player();

	/**
	 *  @brief Destructor
	 */
	virtual
	~Player () {}

	/**
	 *  @brief Sets the first card of the player.
	 *
	 *  @param c The card to be inserted in the first place of the player's hand.
	 */
	void
	firstCard (const Card& c) {_hand[0] = c;}

	/**
	 *  @brief Returns the first card in the player's hand.
	 */
	Card
	firstCard () const { return _hand[0]; }

	/**
	 *  @brief Sets the second card of the player.
	 *
	 *  @param c The card to be inserted in the second place of the player's hand.
	 */
	void
	secondCard (const Card& c) {_hand[1] = c;}

	/**
	 *  @brief Returns the second card in the player's hand.
	 */
	Card
	secondCard () const { return _hand[1]; }

	/**
	 *  @brief Returns the entire hand.
	 *
	 *  @return A two card array.
	 */
	Card*
	hand () { return _hand; }

	/**
	 *  @brief Sets the player's stack.
	 *
	 *  @param stack Amount of chips.
	 */
	void
	stack (int stack) { _stack = stack; }

	/**
	 *  @brief Returns the current stack.
	 */
	int
	stack() const { return _stack; }

	/**
	 *  @brief Sets the player's current bet.
	 */
	void
	bet (int bet) { _bet = bet; }

	/**
	 *  @brief Returns the player's current bet.
	 */
	int
	bet() const { return _bet; }

	/**
	 *  @brief Sets the player's current role.
	 */
	void
	setRole (PlayerRole role) {_role = role;}

	/**
	 *  @brief Returns the player's current role.
	 */
	PlayerRole
	role () const { return _role; }

	/**
	 *  @brief Returns the action that the player should take when the state
	 *  of the game is %game.
	 *
	 *  @param game State of the game.
	 *
	 *  A pointer to an Action is returned. The caller of the method must take
	 *  the responsibility of destroying the pointer.
	 */
	Action*
	action(PokerGame* game);

	/**
	 *  @brief Player parametric description. All the parameters that define the
	 *  player's strategy take part in the description.
	 *
	 *  @param buffer A buffer in which the description will be stored.
	 *  @return argument %buffer is returned.
	 *
	 *  The description must be less than MAX_PLAYER_DESC long.
	 *  In same cases could be used as a direct input for gnuplot.
	 */
	virtual char*
	desc (char* buffer) const {return NULL;}

	/**
	 *  @brief Comparator.
	 */
	virtual bool
	equal (const Player& other) = 0;

protected:

	/**
	 *  @brief Computes the action that should be taken at the given state of
	 *  the game.
	 *
	 *  @param state Game state.
	 */
	virtual
	Action* caction (PokerGame* game) = 0;

	Card _hand[2];
	long unsigned int _stack;
	long int _acc;
	int _outcome;

	int _bet;
	PlayerRole _role;

public:
	/**
	 *  @brief Updates the player with a profit or loss.
	 *
	 *  @param quantity The amount of chips won or lost.
	 */
	void
	update_ev (int quantity);

	double
	outcome () { return (_outcome/(double)_nhands); }

	long int
	accumulated() { return _acc; }

	/**
	 * @brief Returns the player's performance.
	 *
	 * It is used as an scoring system in a player based genetic algorithm.
	 */
	double
	ev () { return (_acc/(double)_nhands)/2; }

	/**
	 *  @brief Returns the number of hands played so far.
	 */
	unsigned long int
	hands_played () { return _nhands; }

	/**
	 *  @brief Resets player's statistics. It is usually invoked before a tournament.
	 */
	virtual void
	prepareForCompetition () {
		_ev = 0;
		_nhands = 0;
		_stack = 2000;
		_acc = 0;
		_outcome = 0;
	}

protected:

	long double _ev;
	unsigned long int _nhands;

public:
	/**
	 *  @brief Writes the player's performance information to an output stream.
	 *
	 *  @param out A stream in which the data will be placed.
	 */
	virtual void
	writeResults (std::ostream& out) const {
		out.write((char*) &_acc,sizeof(long int));
		out.write((char*) &_nhands,sizeof(unsigned long int));
	}

	/**
	 * @brief Reads a player's performance information from an input stream.
	 *
	 * @param in A stream from which the data will be read.
	 */
	virtual void
	readResults (std::istream& in){
		in.read((char*) &_acc, sizeof(long int));
		in.read((char*) &_nhands, sizeof(unsigned long int));
	}

	/**
	 * @brief Clones the player.
	 */
	virtual Player*
	clonePlayer () const = 0;

public:
	/**
	 *  @brief Attaches an observer. The observer will be notified of events raised
	 *  by the player.
	 *
	 *  @param observer The observer to attach.
	 */
	void attachObserver (PlayerObserver* observer) {
		observers.push_back(observer); }

	/**
	 *  @brief Detaches an observer.
	 *
	 *  @param observer The observer to detach.
	 */
	void dettachObserver (PlayerObserver* observer){
		observers.erase(std::find(observers.begin(),observers.end(),observer));}

private:

	void
	notifyValueChanged (double value);

	std::vector<PlayerObserver*> observers;

public:

	/**
	 *  @brief Returns the number of times(%) that the player shoved.
	 */
	float
	raised () { return (_nsb) ? (_nraised/(float)_nsb) : -1;}

	/**
	 *  @brief Returns the number of times (%) that the player called a shove.
	 */
	float
	called () { return (_nbb) ? (_ncalled/(float)_nbb) : -1;}

private:
	int _nbb,_nsb,_nraised,_ncalled;
};

/**
 * @brief A player whose strategy is to always go all-in.
 */
class PlayerAlwaysIn : public Player {
public:
	/**
	 *  @brief Clones the player.
	 */
	Player*
	clonePlayer () const {
		return new PlayerAlwaysIn(*this);
	}

	/**
	 *  @brief Comparator.
	 */
	bool
	equal (const Player& other);

protected:
	/**
	 *  @brief The player goes all-in no matter the state of the game.
	 *
	 *  @param game Game state.
	 */
	Action*
	caction (PokerGame* game);

};

/**
 *  @brief A player whose strategy is to always fold.
 */
class PlayerAlwaysOut : public Player {
public:
	/**
	 *  @brief Clones the player.
	 */
	Player*
	clonePlayer () const {
		return new PlayerAlwaysOut(*this);
	}

	/**
	 *  @brief Comparator.
	 */
	bool
	equal (const Player& other);

protected:
	/**
	 * @brief The player folds no matter the state of the game.
	 *
	 * @param game Game state.
	 */
	Action*
	caction (PokerGame* game);

};

/**
 *  @brief The chart that defines Nash player's strategy when it is in
 *  the small blind position.
 */
const unsigned char sb_max_stack[NRANKS][NRANKS] = {
	{20,20,20,20,20,20,20,20,20,20,20,20,20},
	{20,20,20,20,20,20,20,20,20,20,20,19,19},
	{20,20,20,20,20,20,20,20,20,20,15,13,12},
	{20,20,20,20,20,20,20,20,17,15,12,10, 9},
	{20,20,20,20,20,20,20,20,17,11,10, 9, 6},
	{20,20,20,20,20,20,20,20,17,15, 7, 5, 0},
	{20,16,13,12,17,20,20,20,20,20,11, 0, 0},
	{20,15,10, 8, 9,11,18,20,20,20,14, 0, 0},
	{20,14, 9, 6, 5, 5, 8,14,20,20,20,10, 0},
	{20,13, 8, 6, 0, 0, 0, 0, 0,20,20,14, 0},
	{20,12, 7, 5, 0, 0, 0, 0, 0, 0,20,11, 0},
	{20,11, 7, 5, 0, 0, 0, 0, 0, 0, 0,20, 0},
	{20,11, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0,14}
};

/**
 *  @brief The chart that defines Nash player's strategy when it is in
 *  the big blind position.
 */
const unsigned char bb_max_stack[NRANKS][NRANKS] = {
	{20,20,20,20,20,20,20,20,20,20,20,20,20},
	{20,20,20,20,20,20,18,15,14,13,12,11,11},
	{20,20,20,20,20,16,13,11, 9, 9, 8, 7, 7},
	{20,20,19,20,18,13,11, 9, 7, 6, 6, 6, 5},
	{20,20,15,13,20,12, 9, 7, 6, 5, 5, 5, 0},
	{20,17,12, 9, 8,20, 8, 7, 6, 5, 0, 0, 0},
	{20,14, 9, 7, 6, 6,20, 6, 5, 5, 0, 0, 0},
	{20,13, 8, 6, 5, 5, 5,20, 5, 5, 0, 0, 0},
	{20,11, 7, 5, 0, 0, 0, 0,20, 5, 0, 0, 0},
	{20,10, 6, 5, 0, 0, 0, 0, 0,20, 0, 0, 0},
	{18, 9, 6, 0, 0, 0, 0, 0, 0, 0,20, 0, 0},
	{16, 8, 6, 0, 0, 0, 0, 0, 0, 0, 0,20, 0},
	{15, 8, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0,14}
};

/**
 *  @brief A player that uses the nash equilibrium strategy.
 */
class PlayerNash : public Player {
public:
	/**
	 *  @brief Clones the player.
	 */
	Player*
	clonePlayer () const {
		return new PlayerNash(*this);
	}

	/**
	 *  @brief Comparator.
	 */
	bool
	equal (const Player& other);

protected:
	/**
	 * @brief The player uses the nash equilibrium charts to make
	 * a decision
	 *
	 * @param game Game state.
	 */
	Action*
	caction (PokerGame* game);

};

/**
 *  @brief A player whose is strategy is based on a probability of going all-in and a probability
 *  of calling the raise.
 */
class PlayerRaiseFoldPercent : public Player {
public:

	/**
	 *  @brief Constructs the player.
	 *
	 *  @param r Probability of going all-in
	 *  @param c Probability of calling the shove.
	 */
	PlayerRaiseFoldPercent (double r, double c) :
		_r(r),
		_c(c) {}
	/**
	 *  @brief Player's parametric description. Raise and call probabilities are
	 *  returned in the description.
	 *
	 *  @param buffer A buffer in which the description will be stored.
	 *
	 *  The format is compatible with gnuplot.
	 */
	char*
	desc (char* buffer) const {
		char* desc = buffer;
		sprintf(desc, "%1.4f %1.4f", _r,_c);

		return desc;
	}

	/**
	 *  @brief Clones the player.
	 */
	Player*
	clonePlayer () const {
		return new PlayerRaiseFoldPercent(*this);
	}

	/**
	 *  @brief Comparator.
	 */
	bool
	equal (const Player& other);

protected:
	/**
	 *  @brief The player goes all-in if its hand is in the top r% of best hands.
	 *  The player calls a raise if its hand is in the top c% of best hands.
	 *
	 *  To determine the hand strength, the hands are sorted by their probability of
	 *  winning against a random hand.
	 */
	Action*
	caction (PokerGame* game);

private:
	double _r,_c;
};


/**
 *  @brief A player whose is strategy is based on a probability of going all-in and a probability
 *  of calling the raise. It has evolution capabilities.
 */
class RCPlayer : public Player, public GA1DArrayGenome<float>{
public:
	// Required by GAlib
	GADefineIdentity("PlayerRaiseFoldPercent",205);

	/**
	 *  @brief Basic initializer that assigns random values to the parameters
	 *  raise and call.
	 */
	static void
	init (GAGenome& g);

	/**
	 *  @brief Constructs the player and sets the initializer.
	 */
	RCPlayer () :
		Player (),
		GA1DArrayGenome<float>(2)
	{
		initializer(init);
	}

	/**
	 *  @brief Initializes the player with certain raise and call
	 *  ranges.
	 */
	RCPlayer (float raise, float call) :
		Player (),
		GA1DArrayGenome<float>(2)
	{
		initializer(init);
		gene(0,raise);
		gene(1,call);
	}

	/**
	 *  @brief Copy constructor.
	 */
	RCPlayer (const RCPlayer& rhs):
		Player (),
		GA1DArrayGenome<float>(2)
	{
		copy(rhs);
	}

	/**
	 *  @brief Destructor.
	 */
	~RCPlayer () {}

	/**
	 *  @brief Assignment operator.
	 */
	RCPlayer&
	operator= (const GAGenome& rhs){
		if (&rhs != this) copy(rhs);
		return *this;
	}

	/**
	 *  @brief Clones the player and returns it as a GAGenome.
	 */
	GAGenome*
	clone (GAGenome::CloneMethod flags=CONTENTS) const {
		return new RCPlayer(*this);
	}

	/**
	 *  @brief Clones the player and returns it as a Player.
	 */
	Player*
	clonePlayer () const {
		return new RCPlayer(*this);
	}

	/**
	 *  @brief Copies the contents of another GAGenome.
	 *
	 *  @param orig The GAGnome to be copied.
	 *
	 *  This method assumes that the argument is a RCPlayer. Trying to copy
	 *  a different type of object will throw a bad_cast exception.
	 */
	void
	copy (const GAGenome& orig){
		GAGenome::copy(orig);
		GA1DArrayGenome<float>::copy(orig);
		const RCPlayer& p = dynamic_cast<const RCPlayer&>(orig);
		_bet = p.bet();
		firstCard(p.firstCard());
		secondCard(p.secondCard());
		//strcpy(_name,"Genetics copy");
		_role = p.role();
	}

	/**
	 * @brief Player's parametric description. Raise/Call probabilities are included
	 * in the description.
	 *
	 * @param buffer A buffer that will store the description. It has to be less than
	 * MAX_PLAYER_DESC long.
	 */
	char*
	desc (char* buffer) const {
		char* desc = buffer;
		sprintf(desc, "%1.4f %1.4f", r(),c());

		return desc;
	}

	/**
	 *  @brief Writes the player's parameters along with its performance to an output
	 *  stream.
	 *
	 *  @param out The stream where the data will be written.
	 */
	void
	writeResults (std::ostream& out) const{
		for (int i=0; i<length(); i++)
    		out.write((const char*) &gene(i), sizeof(float));

		Player::writeResults(out);
	}

	/**
	 *  @brief Reads the player's parameters along with its performance from an input
	 *  stream.
	 *
	 *  @param in The stream from which the data will be read.
	 */
	void
	readResults (std::istream& in){
		for (int i=0; i<length(); i++)
			in.read((char*) &gene(i), sizeof(float));

		Player::readResults(in);
	}

	/**
	 *  @brief Comparator.
	 */
	bool
	equal (const Player& other);

protected:
	/**
	 *  @brief The player goes all-in according to a fixed probability given by
	 *  its internal raise/call parameters.
	 */
	Action*
	caction (PokerGame* game);

private:

	/**
	 *  @brief Raising probability.
	 */
	float r () const { return gene(0); }
	/**
	 *  @brief Calling probability.
	 */
	float c () const { return gene(1); }

};

/**
 *  @brief Player which base the decision to go all-in or call on a raise/call
 *  parameter for each possible hand.
 *
 *  When making a decision, the player compares the parameter associated with its current
 *  hand and if it is greater than the current effective stack of the game, then the player
 *  goes all-in.
 */
class RCTPlayer : public GA3DArrayGenome<int>, public Player {
public:
	GADefineIdentity("PlayerRaiseFoldTableGenome",202);

	/**
	 *  @brief Initializes the parameters with a random value between 3 and 20
	 *  chips.
	 */
	static void
	init (GAGenome& g);

	/**
	 *  @brief Initializes the parameters to 0 or 20 randomly.
	 */
	static void
	AllOrNothingInitializer (GAGenome& g);

	/**
	 *  @brief Initializes the parameters of the first <random number> strongest cards
	 *  to 20 and leaves the rest initialized to 0.
	 */
	static void
	raiseCallInitializer (GAGenome& g);

	/**
	 *  @brief Performs a crossover between two players. The crossover loops over the
	 *  parameters of both players and swap their contents with a certain probability.
	 */
	static int
	mergeCrossover(const GAGenome& p1, const GAGenome& p2, GAGenome* c1, GAGenome* c2);

	/**
	 *  @brief Loops over the parameters of the player and reduces or increases by a
	 *  small amount with a given probability pmut.
	 */
	static int
	SubtleMutator (GAGenome& c, float pmut);

	/**
	 *  @brief Constructs the player.
	 */
	RCTPlayer ();

	/**
	 *  @brief Constructs the player. Initializes to 20 the raise parameters associated
	 *  with the first raise% strongest hands. The same for the call parameters.
	 */
	RCTPlayer (float raise, float call);

	/**
	 *  @brief Copy constructor.
	 */
	RCTPlayer (const RCTPlayer& orig) :
		GA3DArrayGenome(NRANKS,NRANKS,2),
		Player (){ copy(orig); }

	/**
	 *  @brief Destructor.
	 */
	virtual
	~RCTPlayer () {}

	/**
	 *  @brief Assignment operator.
	 */
	RCTPlayer&
	operator= (const GAGenome& orig) {
	if (&orig != this) copy(orig);
		return *this;
	}

	/**
	 *  @brief Clones the player. Returns a pointer to a GAGenome
	 */
	virtual GAGenome*
	clone (GAGenome::CloneMethod flags=CONTENTS) const {
		return new RCTPlayer(*this);
	}

	/**
	 *  @brief Clones the player. Returns a pointer to a Player.
	 */
	Player* clonePlayer () const {
		return new RCTPlayer(*this);
	}

	/**
	 *  @brief Copies the contents of another GAGenome.
	 *
	 *  @param orig The GAGenome to be copied.
	 *
	 *  This method assumes that the GAGenome copied is of type RCTPlayer.
	 */
	virtual void
	copy (const GAGenome& orig){
		GAGenome::copy(orig);
		GA3DArrayGenome<int>::copy(orig);
		const RCTPlayer& p = DYN_CAST(const RCTPlayer&,orig);
		_bet = p.bet();
		firstCard(p.firstCard());
		secondCard(p.secondCard());
		//strcpy(_name,"Genetics copy");
		_role = p.role();
	}

	/**
	 *  @brief Player's parametric description.
	 */
	char*
	desc (char* buffer) const;
	/**
	 *  @brief Writes the player's performance along with its parameters to an
	 *  output stream.
	 *
	 *  @param out The stream to write the contents to.
	 */
	void
	writeResults (std::ostream& out) const{
		//GA3DArrayGenome<int>::write(out);
		for (int i=0; i<width(); i++)
			for (int j=0; j<height(); j++)
				for (int k=0; k<depth(); k++)
					out.write((const char*) &gene(i,j,k), sizeof(int));

		Player::writeResults(out);
	}

	/**
	 *  @brief Reads a player's performance along with its parameters from an
	 *  input stream.
	 *
	 *  @param in The stream to read the contents from.
	 */
	void
	readResults (std::istream& in){
		for (int i=0; i<width(); i++)
			for (int j=0; j<height(); j++)
				for (int k=0; k<depth(); k++)
					in.read((char*) &gene(i,j,k), sizeof(int));

		Player::readResults(in);
	}

	/**
	 *  @brief Comparator.
	 */
	bool
	equal (const Player& other);

	/**
	 *  @brief Returns the raise parameter associated with the encoded
	 *  hand (i,j).
	 */
	int
	raiseTable(int i, int j) const
		{ return gene(i,j,0); }

	/**
	 *  @brief Returns the call parameter associated with the encoded
	 *  hand (i,j).
	 */
	int
	callTable(int i, int j) const
		{ return gene(i,j,1); }

protected:
	/**
	 *  @brief The player goes all-in if the parameter associated with its current
	 *  hand is less than the current effective stack.
	 *
	 *  @param game Game state.
	 */
	Action*
	caction (PokerGame* game);
};

#endif
