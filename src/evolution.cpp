#include "evolution.h"

#define MAX_HANDS_PLAYED 100000

void
Tournament::simpleMatch (Player* p1, Player* p2){
	if (!p1->equal(*p2)){
		PokerGame g(*p1,*p2);
		g.playSeveralHands(MAX_HANDS_PLAYED);
	} else {
#ifdef DEBUGV
		char desc[MAX_PLAYER_DESC];
		printf("Stepped match: %s vs %s\n", p1->desc(desc),p2->desc(desc));
#endif
	}
}

void
Tournament::repeatedMatch (Player* p1, Player* p2){
	for (int i=0; i<10;i++){
		PokerGame g(*p1,*p2);
		g.playSeveralHands(MAX_HANDS_PLAYED);
	}
}

void
Tournament::randomEffectiveStackMatch (Player* p1, Player* p2){
	if (!p1->equal(*p2)){
		PokerGame g(*p1,*p2);
		g.randomEffectiveStack(true);
		g.playSeveralHands(MAX_HANDS_PLAYED);
	} else {
#ifdef DEBUGVV
		char desc[MAX_PLAYER_DESC];
		printf("Stepped match: %s vs %s\n", p1->desc(desc),p2->desc(desc));
#endif
	}
}

void
ConcurrentTournament::playTournament (const std::vector<Player*>& players){
	std::vector<std::thread> threads;

	int m = players.size(), n, start;
	(m % 2 == 0) ? (n=m, start=0) : (n = m+1, start=1);
	for (int round = 1; round <= n-1; round++){
		for (int i = start; i < n/2; i++){
			int a,b;
			a = (i + (round-1)*n/2) % (n-1);
			(i==0) ? b=n-1-i : b = (n-1-i + (round-1)*n/2) % (n-1);
			//match(players.at(a), players.at(b));
			threads.push_back(std::thread(match(),players.at(a),players.at(b)));
		}
		for (std::vector<std::thread>::iterator it = threads.begin(); it != threads.end(); it++) {
			(*it).join();
		}
		threads.clear();
	}

#ifdef DEBUG
	long int acc=0;
	bool nhandseq = true;
	for (std::vector<Player*>::const_iterator it = players.begin(); it!=players.end();it++){
		acc += (*it)->accumulated();
		nhandseq &= ((*it)->hands_played() == (m-1)*MAX_HANDS_PLAYED);
	}
	printf("Tournament test: Total value: %ld, Same number of hands: %d\n", acc,nhandseq);
#endif
}


void
ConcurrentTournamentWithCoin::playTournament (const std::vector<Player*>& players){
	std::vector<std::thread> threads;

	int m = players.size(), n, start;
	(m % 2 == 0) ? (n=m, start=0) : (n = m+1, start=1);
	for (int round = 1; round <= n-1; round++){
		for (int i = start; i < n/2; i++){
			int a,b;
			a = (i + (round-1)*n/2) % (n-1);
			(i==0) ? b=n-1-i : b = (n-1-i + (round-1)*n/2) % (n-1);
			//match(players.at(a), players.at(b));
			if (GAFlipCoin(_p)){
				threads.push_back(std::thread(match(),players.at(a),players.at(b)));
			}
		}
		for (std::vector<std::thread>::iterator it = threads.begin(); it != threads.end(); it++) {
			(*it).join();
		}
		threads.clear();
	}
#ifdef DEBUG
	long int acc=0;
	bool nhandseq = true;
	for (std::vector<Player*>::const_iterator it = players.begin(); it!=players.end();it++){
		acc += (*it)->accumulated();
		nhandseq &= ((*it)->hands_played() == (m-1)*MAX_HANDS_PLAYED);
	}
	printf("Tournament test: Total value: %ld, Same number of hands: %d\n", acc,nhandseq);
#endif
}

void
OneVsAllTournament::playTournament (const std::vector<Player*>& players){
	std::vector<std::thread> threads;
	std::vector<Player*> copies;
	for (std::vector<Player*>::const_iterator it = players.begin(); it!=players.end();it++){
		copies.push_back(_opponent->clonePlayer());
		threads.push_back(std::thread(match(),(*it),copies.back()));
	}
	for (std::vector<std::thread>::iterator it = threads.begin(); it != threads.end(); it++)
		(*it).join();
	for (std::vector<Player*>::iterator it = copies.begin(); it != copies.end(); it++)
		delete *it;
}

void
ResultsReader::read (){
	int nPlayers;
	_i.read((char*) &nPlayers, sizeof(int));

	while (!(_i.peek() == EOF)){
		notifyGenerationCreated ();
		for (int j=0; j<nPlayers; j++){
			Player *p = _base.clonePlayer ();
			p->readResults (_i);
			notifyPlayerGenerated (p);

				delete p;
		}
		notifyGenerationFinished ();
		sleep(1);
	}
}

void ResultsReader::notifyGenerationCreated () {
	for (std::vector<EvolutionObserver*>::iterator it = observers.begin(); it != observers.end(); it++)
		(*it)->generationCreated ();

}
void ResultsReader::notifyPlayerGenerated (Player* player){
	for (std::vector<EvolutionObserver*>::iterator it = observers.begin(); it != observers.end(); it++)
		(*it)->playerGenerated (player);
}
void ResultsReader::notifyGenerationFinished () {
	for (std::vector<EvolutionObserver*>::iterator it = observers.begin(); it != observers.end(); it++)
		(*it)->generationFinished ();
}

#ifdef PLOTTING


RCPlotter::RCPlotter () :
	gp("gnuplot -persist")
{
	gp << "set xrange [0:1]" << std::endl;
	gp << "set yrange [0:1]" << std::endl;
	gp << "set style line 1 lc rgb 'black' pt 5" << std::endl;
}

void
RCTPlotter::generationFinished (){
	//Create matrices to plot.
	std::vector<std::vector<unsigned char>> raisem(NRANKS);
	std::vector<std::vector<unsigned char>> callm(NRANKS);

	//Allocate space.
	raisem.resize(NRANKS);
	for (std::vector<std::vector<unsigned char>>::iterator it =
			raisem.begin(); it != raisem.end(); it++)
		(*it).resize(NRANKS);

	callm.resize(NRANKS);
	for (std::vector<std::vector<unsigned char>>::iterator it =
			callm.begin(); it != callm.end(); it++)
		(*it).resize(NRANKS);

	//Fill the matrices
	for (int i = 0; i < NRANKS; i++) {
		for (int j = 0; j < NRANKS; j++) {
			raisem[NRANKS-1-i][j] = _best->raiseTable(i, j);
			callm[NRANKS-1-i][j] = _best->callTable(i, j);
		}
	}

	//Plot the raise table
    gpR << "set palette gray" << std::endl;
    gpR << "set xtics format \"\"" << std::endl;
    gpR << "set x2tics (\"A\" 0,\"K\" 1,\"Q\" 2,\"J\" 3,\"T\" 4,\"9\" 5,\"8\" 6,\"7\" 7,\"6\" 8,\"5\" 9,\"4\" 10,\"3\" 11,\"2\" 12)" << std::endl;
    gpR << "set ytics (\"A\" 12,\"K\" 11,\"Q\" 10,\"J\" 9,\"T\" 8,\"9\" 7,\"8\" 6,\"7\" 5,\"6\" 4,\"5\" 3,\"4\" 2,\"3\" 1,\"2\" 0)" << std::endl;
	gpR << "plot '-' binary" << gpR.binFmt2d(raisem, "array")
			<< "with image title 'raise table' axes x2y1" << std::endl;
	gpR.sendBinary2d(raisem);
	gpR.flush();

	//Plot the call table
	gpC << "set palette gray" << std::endl;
    gpC << "set xtics format \"\"" << std::endl;
    gpC << "set x2tics (\"A\" 0,\"K\" 1,\"Q\" 2,\"J\" 3,\"T\" 4,\"9\" 5,\"8\" 6,\"7\" 7,\"6\" 8,\"5\" 9,\"4\" 10,\"3\" 11,\"2\" 12)" << std::endl;
    gpC << "set ytics (\"A\" 12,\"K\" 11,\"Q\" 10,\"J\" 9,\"T\" 8,\"9\" 7,\"8\" 6,\"7\" 5,\"6\" 4,\"5\" 3,\"4\" 2,\"3\" 1,\"2\" 0)" << std::endl;
	gpC << "plot '-' binary" << gpC.binFmt2d(callm, "array")
			<< "with image title 'call table' axes x2y1" << std::endl;
	gpC.sendBinary2d(callm);
	gpC.flush();

	delete _best;
    //gp << "set palette gray" << std::endl;
	//gp << "plot '-' matrix with image" << std::endl;
	//const Player& best = dynamic_cast<const Player&>(ga.population().best());
	//char* desc = new char[MAX_PLAYER_DESC];
	//gp << best.desc(desc);
	//gp << "e" << std::endl << "e" << std::endl;
	//gp.flush();

	//delete desc;
}

RCTOverallPlotter::RCTOverallPlotter(int nPlayers) :
	gpR(),
	gpC(),
	_nPlayers(nPlayers),
	_current(0)
{
	_playersPerLine = static_cast<int>(sqrt(nPlayers));
	_maxPlottedPlayers = _playersPerLine * _playersPerLine;
		_imageR.resize(NRANKS * _playersPerLine);

	for (std::vector<std::vector<unsigned char>>::iterator it =
			_imageR.begin(); it != _imageR.end(); it++)
		(*it).resize(NRANKS * _playersPerLine);

	_imageC.resize(NRANKS * _playersPerLine);
	for (std::vector<std::vector<unsigned char>>::iterator it =
			_imageC.begin(); it != _imageC.end(); it++)
		(*it).resize(NRANKS * _playersPerLine);
}

void
RCTOverallPlotter::playerGenerated(Player* p) {
	if (_current >= _maxPlottedPlayers)
		return;

	const RCTPlayer& t =
			dynamic_cast<const RCTPlayer&>(*p);

	int r = (_current / _playersPerLine) * NRANKS;
	int s = (_current % _playersPerLine) * NRANKS;

	for (int i = 0; i < NRANKS; i++) {
		for (int j = 0; j < NRANKS; j++) {
			_imageR[r + NRANKS-1-i][s + j] = t.raiseTable(i, j);
			_imageC[r + NRANKS-1-i][s + j] = t.callTable(i, j);
		}
	}

	_current++;

}

void
RCTOverallPlotter::generationFinished() {
	gpR << "set palette gray" << std::endl;
	gpR << "set xtics format \"\"" << std::endl;
	gpR << "set ytics format \"\"" << std::endl;
	gpR << "plot '-' binary" << gpR.binFmt2d(_imageR, "array")
			<< "with image title 'raise table'" << std::endl;
	gpR.sendBinary2d(_imageR);
	gpR.flush();

	gpC << "set palette gray" << std::endl;
	gpC << "set xtics format \"\"" << std::endl;
	gpC << "set ytics format \"\"" << std::endl;
	gpC << "plot '-' binary" << gpC.binFmt2d(_imageC, "array")
			<< "with image title 'call table'" << std::endl;
	gpC.sendBinary2d(_imageC);
	gpC.flush();
}

#endif

void PlayersSelector::update () {
	GARouletteWheelSelector::update();
#ifdef DEBUG
	printf("Selector update method called\n");
#endif

	notifyGenerationCreated();

	for (int i=0; i<pop->size(); i++){
		notifyPlayerGenerated(dynamic_cast<Player*>(&pop->individual(i)));
	}

//	PlayersEvaluator(*pop);
//	pop->scale(GABoolean::gaTrue);

	notifyGenerationFinished();
}

GAGenome &
PlayersSelector::select() const {
	GAGenome& g = GARouletteWheelSelector::select();

#ifdef DEBUG
	printf("Selected player with score %f and fitness %f\n", g.score(),
			g.fitness());
#endif

	return g;
}

void PlayersSelector::notifyGenerationCreated () {
	for (std::vector<EvolutionObserver*>::iterator it = observers.begin(); it != observers.end(); it++)
		(*it)->generationCreated ();

}
void PlayersSelector::notifyPlayerGenerated (Player* player){
	for (std::vector<EvolutionObserver*>::iterator it = observers.begin(); it != observers.end(); it++)
		(*it)->playerGenerated (player);
}
void PlayersSelector::notifyGenerationFinished () {
	for (std::vector<EvolutionObserver*>::iterator it = observers.begin(); it != observers.end(); it++)
		(*it)->generationFinished ();
}

void PlayersEvaluator(GAPopulation& pop){
#ifdef DEBUG
	printf("Players pop evaluator called\n");
#endif

	std::vector<Player*> players;
	for (int i=0; i<pop.size(); i++){
		Player* p = dynamic_cast<Player*>(&pop.individual(i));
		p->prepareForCompetition();
		players.push_back(p);
	}

	Tournament& tournament = (Tournament&) *reinterpret_cast<Tournament*>(pop.userData());
	tournament.playTournament(players);

	char desc[MAX_PLAYER_DESC];
	for (int i=0; i<pop.size(); i++){
		pop.individual(i).score(players.at(i)->ev());
#ifdef DEBUG
#ifdef DEBUGV
		printf("(%s) - %f \n", players.at(i)->desc(desc), pop.individual(i).score());
#endif
#endif
	}

#ifdef DEBUG
	printf("-- Notify generation finished --\n");
#endif
}


void
PlayersScalingScheme::evaluate(const GAPopulation& p) {
	float b = (p.min() < 0) ? -p.min() : 0;
	for (int i = 0; i < p.size(); i++) {
		float f = p.individual(i).score() + b + eps;
		p.individual(i).fitness(f);
#ifdef DEBUG
		printf("Fitness score: %f from score: %f and b: %f\n", f,
				p.individual(i).score(), b);
#endif
	}
}

PlayerEvolver::PlayerEvolver(const GAGenome& player) :
	_pop (player)
{
	//Population evaluator
	_pop.evaluator(PlayersEvaluator);
	//Schaling scheme
	_pop.scaling(PlayersScalingScheme());
	//Default tournament
	_tournament = new ConcurrentTournament();
	_pop.userData(_tournament);
	//Genetic algorithm
	_ga = new GASimpleGA(_pop);
	//Selector
	_ga->selector(_selector);
	//Default configuration
	_ga->minimaxi(GAGeneticAlgorithm::MAXIMIZE);
	_ga->terminator(GAGeneticAlgorithm::TerminateUponConvergence);
}

PlayerEvolver::PlayerEvolver(const GAGenome& player, int nPlayers) :
		_pop(player)
{
	//Population evaluator
	_pop.evaluator(PlayersEvaluator);
	//Schaling scheme
	_pop.scaling(PlayersScalingScheme());
	//Default tournament
	_tournament = new ConcurrentTournament();
	_pop.userData(_tournament);
	//Genetic algorithm
	_ga = new GASimpleGA(_pop);
	//Selector
	_ga->selector(_selector);
	//Default configuration
	_ga->minimaxi(GAGeneticAlgorithm::MAXIMIZE);
	_ga->terminator(GAGeneticAlgorithm::TerminateUponConvergence);
	_ga->populationSize(nPlayers);
}
