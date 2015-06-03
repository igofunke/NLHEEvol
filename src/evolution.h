#ifndef _EVOLUTION_H_
#define _EVOLUTION_H_

//#define DEBUG

#include <time.h>

#include <cstdio>
#include <thread>
#include <random>
#include <cmath>
#include <vector>

#include <ga/ga.h>

#include "gnuplot-iostream.h"

#include "player.h"

void match(Player* p1, Player* p2);
void normalMatch(Player* p1, Player* p2);

/*
void concurrent_tournament(const std::vector<Player*>& players);
void concurrent_tournament_with_coin(const std::vector<Player*>& players,
		float p);
		*/
void oneVsAllTournament(std::vector<Player*>& players, Player* player);

/*
void evolveRCPlayers();
void evolveTablePlayers();
void experimentEvolveRCTPlayersOverlappingPopulations();
void experimentAES();*/
//void evolve(GAGeneticAlgorithm& ga);


/**
 *  @brief Abstract class for a tournament of players.
 */
class Tournament {
public:

	/**
	 *  @brief Creates the tournament.
	 */
	Tournament () :
		_match(simpleMatch) {}

	typedef void (*Match) (Player*,Player*);

	/**
	 *  @brief Matches two players one single time.
	 */
	static void
	simpleMatch (Player* p1, Player* p2);

	/**
	 *  @brief Matches two players several times.
	 */
	static void
	repeatedMatch (Player* p1, Player* p2);

	/**
	 *  @brief Matches two players one single time. Effective
	 *  stack is chosen randomly in each hand.
	 */
	static void
	randomEffectiveStackMatch (Player* p1, Player* p2);

	/**
	 *  @brief Destructor.
	 */
	virtual
	~Tournament() {}
	/**
	 *  @brief Starts the tournament.
	 *
	 *  @param players Participants of the tournament.
	 */
	virtual void
	playTournament (const std::vector<Player*>& players) = 0;

	/**
	 *  @brief Clones the tournament.
	 */
	virtual Tournament*
	clone () const = 0;

	/**
	 *  @brief Sets the match used in the tournament.
	 */
	void
	match (Match match) {_match = match;}

	/**
	 *  @brief Returns the current match.
	 */
	Match
	match () const { return _match; }


protected:
	Match _match;

};

/**
 *  @brief Tournament of players conducted concurrently.
 */
class ConcurrentTournament : public Tournament {
public:
	/**
	 *  @brief Round robin of players disputed concurrently.
	 *
	 *  @param players Participants of the tournament.
	 */
	void
	playTournament (const std::vector<Player*>& players);

	/**
	 *  @brief Clones the tournament.
	 */
	Tournament*
	clone () const { return new ConcurrentTournament(*this); }
};

/**
 *  @brief Tournament of players conducted concurrently. Not all matches
 *  are played. A match is played depending on a coin toss.
 */
class ConcurrentTournamentWithCoin : public Tournament {
public:
	/**
	 *  @brief Constructs the tournament.
	 *
	 *  @param p Probability of a match to be played.
	 */
	ConcurrentTournamentWithCoin(float p) :
		_p (p) {};
	/**
	 *  @brief Round robin of players disputed concurrently. Not all matches
	 *  are played. To determine if a match is played or not, a coin with a
	 *  certain probability is thrown and if it turns out to be heads then the
	 *  match is played.
	 *
	 *  @params players Participants of the tournament.
	 */
	void
	playTournament (const std::vector<Player*>& players);

	/**
	 *  @brief Clones the tournament.
	 */
	Tournament*
	clone () const { return new ConcurrentTournamentWithCoin(*this); }

private:
	float _p;
};

/**
 *  @brief Tournament of players in which every player stands against one single
 *  foe.
 */
class OneVsAllTournament : public Tournament {
public:
	/**
	 *  @brief Constructs the tournament.
	 *
	 *  @param opponent The player which will face all the other players.
	 */
	OneVsAllTournament (const Player& opponent) {
		_opponent = opponent.clonePlayer();
	}

	/**
	 *  @brief Copy constructor.
	 */
	OneVsAllTournament (const OneVsAllTournament& other){
		_opponent = other._opponent->clonePlayer();
	}

	/**
	 *  @brief Assignement operator.
	 */
	OneVsAllTournament&
	operator= (const OneVsAllTournament& other){
		if (this == &other) return *this;

		_opponent = other._opponent->clonePlayer();
		return *this;
	}

	/**
	 *  @brief Destructor.
	 */
	~OneVsAllTournament () { delete _opponent; }

	/**
	 *  @brief Matches every player against the opponent passed in the
	 *  constructor
	 *
	 *  @param players Contestants.
	 */
	void
	playTournament (const std::vector<Player*>& players);

	/**
	 *  @brief Clones the tournament.
	 */
	Tournament*
	clone () const { return new OneVsAllTournament(*this); }

private:
	Player* _opponent;
};


//extern bool term;
//void evolve (GAGeneticAlgorithm& ga);

/**
 *  @brief Abstract class base of an observer. A view that uses information
 *  from the model must inherit this class in order to receive events.
 */
class EvolutionObserver {
public:
	virtual ~EvolutionObserver() {
	}

	/**
	 *  @brief Informs that a new generation of players has been achieved.
	 */
	virtual void
	generationCreated() = 0;

	/**
	 *  @brief Informs that a new player has been inserted in the population
	 *  recently created.
	 *
	 *  @param p The player added.
	 */
	virtual void
	playerGenerated(Player* p) = 0;

	/**
	 *  @brief Informs that the new generation is ready to be evolved.
	 */
	virtual void
	generationFinished() = 0;
};

/**
 *  @brief Reads an algorithm output from the disc and passes it to its
 *  attached observers.
 *
 *  Different kind of plotters may be attached to it.
 */
class ResultsReader {
public:

	/**
	 *  @brief Creates the reader.
	 *  @param in The input to read from.
	 *  @param base Base type player that the algorithm which generated the file used
	 */
	ResultsReader(std::istream& in, const Player& base) :
			_i(in), _base(base) {
	}

	/**
	 *  @brief Starts reading the contents of the file.
	 *
	 *  Simulates the behavior of a genetic algorithm, raising the appropiate events
	 *  and notifying the attached observers.
	 */
	void
	read();

	/**
	 *  @brief Attaches an observer. The observer attached will begin to be informed
	 *  when an event occurs.
	 *
	 *  @param observer Observer to be attached.
	 *
	 *  It may be used to plot the output of an algorithm by attaching a plotter.
	 */
	void attachObserver(EvolutionObserver* observer) {
		observers.push_back(observer);
	}

	/**
	 *  @brief Detaches an observer.
	 *
	 *  @param observer Observer to be detached.
	 *
	 *  If the observer was not attached, nothing happens.
	 */
	void detachObserver(EvolutionObserver* observer) {
		observers.erase(
				std::find(observers.begin(), observers.end(), observer));
	}

private:
	std::vector<EvolutionObserver*> observers;

	void notifyGenerationCreated();
	void notifyPlayerGenerated(Player* player);
	void notifyGenerationFinished();

	std::istream& _i;
	const Player& _base;
};

/**
 *  @brief Allows writing an algorithm output to disc.
 */
class ResultsWriter: public EvolutionObserver {
public:
	/**
	 *  @brief Constructs the writer.
	 *
	 *  @param out Output stream where the output will be placed.
	 *  @param nPlayers Number of players in each generation in the algorithm.
	 */
	ResultsWriter(ostream& out, int nPlayers) :
			_o(out) {
		writeHeader(nPlayers);
	}

	~ResultsWriter() {}

	/**
	 *  Event. Inherited from EvolutionObserver. Nothing to do in this case.
	 */
	void generationCreated() {}

	/**
	 *  Event. Inherited from EvolutionObserver. Write the contents of the newly created
	 *  player to disc.
	 */
	void playerGenerated(Player* p) {
		p->writeResults(_o);
	}

	/**
	 *  Event. Inherited from EvolutionObserver.
	 */
	void generationFinished() {
		_o.flush();
	}

private:

	/**
	 *  @brief Writes a header with the number of players in each generation of
	 *  the genetic algorithm associated.
	 */
	void writeHeader(int nPlayers) {
		_o.write((const char*) &nPlayers, sizeof(int));
	}

	ostream& _o;
};


/**
 *  @brief Prints the output of a genetic algorithm.
 */
class Console: public EvolutionObserver {
public:
	/**
	 * Event. Inform the user that a new generation has been created.
	 */
	void generationCreated() {
		printf("Generation achieved\n");
	}
	/**
	 * Event. Prints information about the new user added to the population.
	 */
	void playerGenerated(Player* p) {
		char* desc = new char[MAX_PLAYER_DESC];
		printf("(%s) - %f \n", p->desc(desc), p->ev());
		delete desc;
	}
	/**
	 * Event. Formats output properly.
	 */
	void generationFinished() {
		printf("\n");
	}
};

/*
 * Plotting capabilities. Gnuplot must be installed for this classes
 * to work. Also, boost libraries are required to compile.
 */
#ifdef PLOTTING

/**
 *  @brief Plots the output of a genetic algorithm working with RC players.
 *  Shows a scatter plot composed of points representing the different players.
 *  X coordinate is used for the raise parameter of the player whereas
 *  Y coordinate is used for the call parameter.
 *
 *  The genetic algorithm must work with RC players for this class to work.
 */
class RCPlotter: public EvolutionObserver {
public:

	/**
	 *  @brief Constructs the plotter.
	 */
	RCPlotter();

	/**
	 *  Event. Tells gnuplot to wait for data input.
	 */
	void generationCreated() {
		gp << "plot '-' using 1:2 with points ls 1 notitle" << std::endl;
	}

	/**
	 *  Event. Sends the data of the recently created player to gnuplot.
	 */
	void playerGenerated(Player* p) {
		char* desc = new char[MAX_PLAYER_DESC];
		gp << p->desc(desc) << std::endl;
		delete desc;
	}

	/**
	 * Event. Tells gnuplot to finally plot the received data.
	 */
	void generationFinished() {
		gp << "e" << std::endl;
		gp.flush();
	}

private:
	Gnuplot gp;
};

/**
 *  @brief Plots the output of a genetic algorithm working with RCT players.
 *  A heatmap is shown for both the raise table and call table. Each of the cells
 *  of the heat map is associated with a cell of the raise and call table of the player
 *  respectively.
 *
 *  This plotter will slow down a lot a genetic algorithm.
 *
 */
class RCTPlotter: public EvolutionObserver {
public:

	/**
	 * @brief Constructs the plotter.
	 */
	RCTPlotter() :
			gpR("gnuplot -persist"),
			gpC("gnuplot -persist"),
			_best(0) {}

	/**
	 *  Event. Nothing to do in this case.
	 */
	void generationCreated() { _best = 0;}

	/**
	 *  Event. Keeps track of the best player to plot it later.
	 */
	void playerGenerated(Player* p) {
		if (_best ==  0 || p->ev() > _best->ev())
			_best = dynamic_cast<RCTPlayer*>(dynamic_cast<RCTPlayer*>(p)->clonePlayer());
	}

	/**
	 *  Event. Plot the best player of the current generation.
	 */
	void generationFinished();

private:
	Gnuplot gpR,gpC;

	RCTPlayer* _best;
};

/**
 *  @brief Plots the output of a genetic algorithm working with RCT players.
 *  It shows a heat map depicting all the raise tables of all the players in the
 *  current generation. Another heat map is shown for the call tables as well. It
 *  is used to give a general overview of the current population and how it is evolving.
 *
 *  This plotter will slow down a lot a genetic algorithm.
 */
class RCTOverallPlotter: public EvolutionObserver {
public:

	/**
	 *  @brief Constructs the plotter.
	 */
	RCTOverallPlotter(int nPlayers);

	/**
	 *  Event. Resets the heat maps.
	 */
	void generationCreated() {
		_current = 0;
	}

	/**
	 *  Event. Formats the heat map to depict the player in the place
	 *  reserved to it.
	 */
	void playerGenerated(Player* p);

	/**
	 *  Event. Sends the heat map to gnuplot.
	 */
	void generationFinished();

private:
	Gnuplot gpR, gpC;

	int _nPlayers, _playersPerLine, _maxPlottedPlayers;
	int _current;

	std::vector<std::vector<unsigned char>> _imageR;
	std::vector<std::vector<unsigned char>> _imageC;
};

#endif


/**
 *  @brief Population evaluator for a player based genetic algorithm.
 */
void PlayersTournamentEvaluator(GAPopulation& pop);
void PlayersEvaluator(GAPopulation& pop);

/**
 *  @brief Scaling scheme used for a player based genetic algorithm.
 */
class PlayersScalingScheme: public GAScalingScheme {
public:
	GADefineIdentity("PlayersScoresScalingScheme",210)
	;

	const float eps = 0.001;

	/**
	 *  @brief Clones the scaling scheme.
	 */
	GAScalingScheme*
	clone() const {
		return new PlayersScalingScheme(*this);
	}

	/**
	 *  @brief Computes fitness scores for all the members in the population
	 *  by linearly transforming their objective scores.
	 *
	 *  @param p The population whose fitness scores will be computed.
	 */
	void
	evaluate(const GAPopulation& p);
};

/**
 *  @brief Selector used for a player based genetic algorithm. Based on
 *  a roulette wheel selection scheme.
 */
class PlayersSelector: public GARouletteWheelSelector {
public:
	GADefineIdentity("PlayersTableTournamentSelector", 203);

	/**
	 *  @brief Constructs the selector.
	 *
	 *  @param w GASelectionScheme::SCALED to use fitness scores or
	 *  GASelectionScheme::RAW to use objective scores.
	 */
	PlayersSelector(int w = GASelectionScheme::SCALED) :
			GARouletteWheelSelector(w) {
	}

	/**
	 *  @brief Copy constructor.
	 */
	PlayersSelector(const PlayersSelector& orig) {
		copy(orig);
	}

	/**
	 *  @brief Assignement operator.
	 */
	PlayersSelector&
	operator=(const GASelectionScheme& orig) {
		if (&orig != this)
			copy(orig);
		return *this;
	}

	/**
	 *  @brief Destructor.
	 */
	virtual
	~PlayersSelector() {}

	/**
	 *  @brief Prepares the population before selecting players. It is
	 *  used to notify events to the attached observers.
	 */
	void
	update();


	GAGenome &
	select() const;
	/**
	 *  @brief Clones the selector.
	 */
	virtual
	GASelectionScheme* clone() const {
		return new PlayersSelector(*this);
	}

	/**
	 *  @brief Copies data from another scheme.
	 */
	void
	copy(const GASelectionScheme& orig) {
		GARouletteWheelSelector::copy(orig);
		const PlayersSelector& o =
				dynamic_cast<const PlayersSelector&>(orig);
		this->observers = o.observers;
	}

	/**
	 *  @brief Attaches an observer. The attached observer will receive evolution
	 *  events.
	 *
	 *  @param observer The observer to attach.
	 */
	void
	attachObserver(EvolutionObserver* observer) {
		observers.push_back(observer);
	}

	/**
	 *  @brief Detaches a previously attached observer.
	 *  @param observer The observer to detach.
	 *
	 *  If the observer was not attached, nothing happens.
	 */
	void
	detachObserver(EvolutionObserver* observer) {
		observers.erase(
				std::find(observers.begin(), observers.end(), observer));
	}

private:

	std::vector<EvolutionObserver*> observers;

	void notifyGenerationCreated();
	void notifyPlayerGenerated(Player* player);
	void notifyGenerationFinished();
};

/**
 *  @brief Helper class to make player evolution easier. It acts as an
 *  interface between this player evolution framework and GAlib framework.
 */
class PlayerEvolver {
public:
	/**
	 *  @brief Creates the evolver and sets the type of the player.
	 *
	 *  @param player Type of the player to be evolved.
	 */
	PlayerEvolver(const GAGenome& player);

	/**
	 *  @brief Creates the evolver.
	 *
	 *  @param player Type of the player to be evolved.
	 *  @param nPlayers Number of players in each population.
	 */
	PlayerEvolver(const GAGenome& player, int nPlayers);

	/**
	 *  @brief Destructor.
	 */
	~PlayerEvolver () {
		delete _tournament;
		delete _ga;
	}

	/**
	 *  @brief Sets the tournament type.
	 */
	void
	tournament (const Tournament& tournament){
		delete _tournament;
		_tournament = tournament.clone();
	}

	/**
	 *  @brief Gets the current tournament type.
	 */
	const Tournament&
	tournament () const{
		return *_tournament;
	}

	/**
	 *  @brief Sets the number of players in the population.
	 */
	void
	nPlayers(int nPlayers) { _ga->populationSize(nPlayers); }

	/**
	 *  @brief Gets the number of players in the population.
	 */
	int
	nPlayers() { return _ga->populationSize(); }

	/**
	 *  @brief Sets the number of generations.
	 */
	void
	nGenerations (int nGenerations) {_ga->nGenerations(nGenerations); }

	/**
	 *  @brief Gets the number of generations.
	 */
	int
	nGenerations() { return _ga->nGenerations();}

	/**
	 *  @brief Starts the evolving process.
	 */
	void
	evolve () { _ga->evolve(); }

	/**
	 *  @brief Returns the best genome in the population.
	 */
	GAGenome&
	best () {return _ga->population().best();}

	/**
	 *  @brief Returns the current genetic algorithm used to evolve the players.
	 *
	 *  It is used to further configuration.
	 */
	GASimpleGA&
	geneticAlgorithm(){
		return *_ga;
	}

	/**
	 *  @brief Attaches an observer willing to receive events during the evolution
	 *  process.
	 */
	void
	attachObserver (EvolutionObserver* observer){
		_selector.attachObserver(observer);
		_ga->selector(_selector);
	}

	/**
	 *  @brief Detaches a previously attached observer.
	 */
	void
	detachObserver (EvolutionObserver* observer) {
		_selector.detachObserver(observer);
		_ga->selector(_selector);
	}


private:
	GASimpleGA* _ga;
	GAPopulation _pop;

	PlayersSelector _selector;

	Tournament* _tournament;
};

#endif
