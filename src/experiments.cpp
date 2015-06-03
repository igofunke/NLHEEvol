/**
 *  @file This file contains several experiments showing the possibilities
 *  of the framework.
 */

/*
 * Plotting capabilities. Gnuplot must be installed for this classes
 * to work. Also, boost libraries are required to compile.
 */
#define PLOTTING

#include "evolution.h"

void ExperimentRCEquilibrium(int n);
void ExperimentRCAdaptative(int n);
void ExperimentRCTEquilibrium(int n);
void ExperimentRCTAdaptative(int);
void ExperimentRCTRandomEffectiveStack();
double ExperimentOneVsOne (const Player&, const Player&);

void readfile (const char*, const Player&);

int
main () {
//	ExperimentRCEquilibrium(20);
//	ExperimentRCTAdaptative(20);
//	ExperimentRCTEquilibrium(100);
//	ExperimentRCTRandomEffectiveStack();
	ExperimentOneVsOne(PlayerRaiseFoldPercent(0.58,0.37),RCTPlayer(0.70,0.37));

	//readfile("rc1.dat", RCPlayer());
//	readfile("rcte.dat", RCTPlayer());
}

/**
 *  @brief Experiment to evolve RC players to an equilibrium solution.
 */
void ExperimentRCEquilibrium (int n){
	RCPlayer genome;

	/*
	 * Number of players and number of generations of the experiment.
	 * If the selected convergence method above is termination upon convergence, then
	 * the number of generations does not matter.
	 */
	int nPlayers = n;
	int nGenerations = 100;


	/*
	 * Set up the algorithm.
	 */

	PlayerEvolver evolver(genome, nPlayers);

	ConcurrentTournament tournament;
//	ConcurrentTournamentWithCoin tournament(1);
	evolver.tournament(tournament);

	/*
	 * Plotting
	 */
//	RCPlotter plotter;
//	evolver.attachObserver(&plotter);

	Console console;
	evolver.attachObserver(&console);

	/*
	 * Write results to disc.
	 */
//	char filename[] = "rc1.dat";
//	std::ofstream out(filename, ios::binary | ios::trunc);
//	ResultsWriter writer(out,nPlayers);
//	evolver.attachObserver(&writer);

	/*
	 * Configure the algorithm
	 */
	// We don't want to preserve the best players in each generation.
	evolver.geneticAlgorithm().elitist(GABoolean::gaFalse);
	evolver.geneticAlgorithm().nGenerations(nGenerations);

	time_t start = time(NULL);
	evolver.evolve();
	time_t end = time(NULL);
	double elapsed = (end-start)/60.0;

	//Print results
	RCPlayer& best = dynamic_cast<RCPlayer&>(evolver.best());
	char desc[MAX_PLAYER_DESC];
	printf("Best player: %s\n", best.desc(desc));

	//Check results
	PlayerRaiseFoldPercent opp(0.58,0.38);
	double mean = ExperimentOneVsOne(best,opp);

	//Summary
	printf("\nExperiment finished. Elapsed time: %f min.\n",elapsed);
	printf("Number of players: %d\n", nPlayers);
	printf("Best player: %s\n", best.desc(desc));
	printf("Performance: %f\n", mean);

	//Write results to disk
	char outf[] = "rese.txt";
	std::ofstream outr(outf, ios::binary | ios::app);
	outr << "Elapsed time: " << elapsed << " min." << std::endl;
	outr << "Number of players: " << nPlayers << std::endl;
	outr << "Best player: " << best.desc(desc) << std::endl;
	outr << "Performance: " << mean << std::endl;
	outr << "------------------------------" << endl;
	outr.close();


	//out.close();
}

/**
 *  @brief Experiment to evolve RC players to find the optimum strategy
 *  against one single RC player.
 */
void ExperimentRCAdaptative (int n){
	RCPlayer genome;

	/*
	 * Number of players and number of generations of the experiment.
	 * If the selected convergence method above is termination upon convergence, then
	 * the number of generations does not matter.
	 */
	int nPlayers = n; //20
	int nGenerations = 100;


	/*
	 * Set up the algorithm.
	 */

	PlayerEvolver evolver(genome, nPlayers);

//	PlayerRaiseFoldPercent opp(0.7,0.6);
	PlayerRaiseFoldPercent opp(0.4,0.3);
	OneVsAllTournament tournament(opp);
	tournament.match(Tournament::repeatedMatch);
	evolver.tournament(tournament);

	/*
	 * Plotting
	 */
//	RCPlotter plotter;
//	evolver.attachObserver(&plotter);

//	Console console;
//	evolver.attachObserver(&console);

	/*
	 * Write results to disc.
	 */
//	char filename[] = "rc1.dat";
//	std::ofstream out(filename, ios::binary | ios::trunc);
//	ResultsWriter writer(out,nPlayers);
//	evolver.attachObserver(&writer);

	/*
	 * Configure the algorithm
	 */
	// We want to preserve the best players in each generation.
	evolver.geneticAlgorithm().elitist(GABoolean::gaTrue);
	evolver.geneticAlgorithm().nGenerations(nGenerations);

	time_t start = time(NULL);
	evolver.evolve();
	time_t end = time(NULL);
	double elapsed = (end-start)/60.0;

	//Print results
	RCPlayer& best = dynamic_cast<RCPlayer&>(evolver.best());
	char desc[MAX_PLAYER_DESC];
	printf("Best player: %s\n", best.desc(desc));

	//Check results
	double mean = ExperimentOneVsOne(best,opp);

	//Summary
	printf("\nExperiment finished. Elapsed time: %f min.\n",elapsed);
	printf("Number of players: %d\n", nPlayers);
	printf("Best player: %s\n", best.desc(desc));
	printf("Performance: %f\n", mean);

	//Write results to disk
	char outf[] = "resRCAdaptative.txt";
	std::ofstream outr(outf, ios::binary | ios::app);
	outr << "Elapsed time: " << elapsed << " min." << std::endl;
	outr << "Number of players: " << nPlayers << std::endl;
	outr << "Best player: " << best.desc(desc) << std::endl;
	outr << "Performance: " << mean << std::endl;
	outr << "------------------------------" << endl;
	outr.close();

//	out.close();
}

/**
 *  @brief evolve RCT players by matching them against a RC player.
 */
void ExperimentRCTAdaptative (int n) {

	/*
	 * Configure the player
	 */
	RCTPlayer genome;
	genome.initializer(RCTPlayer::raiseCallInitializer);
	genome.crossover(RCTPlayer::mergeCrossover);

	/*
	 * Number of players and number of generations of the experiment.
	 * If the selected convergence method above is termination upon convergence, then
	 * the number of generations does not matter.
	 */
	int nPlayers = n;//20
	int nGenerations = 4;

	/*
	 * Set up the algorithm.
	 */

	PlayerEvolver evolver(genome,nPlayers);

	PlayerRaiseFoldPercent opp(0.58,0.38);
	OneVsAllTournament tournament(opp);
	tournament.match(Tournament::repeatedMatch);
	evolver.tournament(tournament);

	/*
	 * Plotting
	 */
//	Console console;
//	evolver.attachObserver(&console);

//	RCTPlotter plotter;
//	evolver.attachObserver(&plotter);

	RCTOverallPlotter overallPlotter(nPlayers);
	evolver.attachObserver(&overallPlotter);

	/*
	 * Write results to disc.
	 */
//	char filename[] = "rct1.dat";
//	std::ofstream out(filename, ios::binary | ios::trunc);
//	ResultsWriter writer(out,nPlayers);
//	evolver.attachObserver(&writer);

	/*
	 * Configure the algorithm
	 */
	// We want to preserve the best players in each generation.
	evolver.geneticAlgorithm().elitist(GABoolean::gaTrue);
	evolver.geneticAlgorithm().nGenerations(nGenerations);

	time_t start = time(NULL);
	evolver.evolve();
	time_t end = time(NULL);
	double elapsed = (end-start)/60.0;

	//Print results
	RCPlayer& best = dynamic_cast<RCPlayer&>(evolver.best());
	char desc[MAX_PLAYER_DESC];
	printf("Best player: %s\n", best.desc(desc));

	//Check results
	double mean = ExperimentOneVsOne(best,opp);

	//Summary
	printf("\nExperiment finished. Elapsed time: %f min.\n",elapsed);
	printf("Number of players: %d\n", nPlayers);
	printf("Best player: %s\n", best.desc(desc));
	printf("Performance: %f\n", mean);

	//Write results to disk
	char outf[] = "resRCTAdaptative.txt";
	std::ofstream outr(outf, ios::binary | ios::app);
	outr << "Elapsed time: " << elapsed << " min." << std::endl;
	outr << "Number of players: " << nPlayers << std::endl;
	outr << "Best player: " << best.desc(desc) << std::endl;
	outr << "Performance: " << mean << std::endl;
	outr << "------------------------------" << endl;
	outr.close();
}


/**
 *  @brief Evolve RCT players to an equilibrium solution.
 */
void ExperimentRCTEquilibrium (int n){
	/*
	 * Configure the player
	 */
	RCTPlayer genome;
	// Initialize them with different raise,call ranges.
	genome.initializer(RCTPlayer::raiseCallInitializer);
	genome.crossover(RCTPlayer::mergeCrossover);

	/*
	 * Number of players and number of generations of the experiment.
	 * If the selected convergence method above is termination upon convergence, then
	 * the number of generations does not matter.
	 */
	int nPlayers = n;
	int nGenerations = 10000;

	/*
	 * Set up the algorithm.
	 */

	PlayerEvolver evolver(genome,nPlayers);

	ConcurrentTournament tournament;
	evolver.tournament(tournament);

	/*
	 * Plotting
	 */
//	Console console;
//	evolver.attachObserver(&console);

//	RCTPlotter plotter;
//	evolver.attachObserver(&plotter);

//	RCTOverallPlotter overallPlotter(nPlayers);
//	evolver.attachObserver(&overallPlotter);

	/*
	 * Write results to disc.
	 */
	char filename[] = "rcte100.dat";
	std::ofstream out(filename, ios::binary | ios::trunc);
	ResultsWriter writer(out,nPlayers);
	evolver.attachObserver(&writer);

	// We don't want to preserve the best players in each generation.
	evolver.geneticAlgorithm().elitist(GABoolean::gaFalse);
	evolver.geneticAlgorithm().nGenerations(nGenerations);

	time_t start = time(NULL);
	evolver.evolve();
	time_t end = time(NULL);
	double elapsed = (end-start)/60.0;

	//Print results
	RCTPlayer& best = dynamic_cast<RCTPlayer&>(evolver.best());
	char desc[MAX_PLAYER_DESC];
	printf("Best player: %s\n", best.desc(desc));

	//Check results
	PlayerRaiseFoldPercent opp(0.58,0.38);
	double mean = ExperimentOneVsOne(best,opp);

	//Summary
	printf("\nExperiment finished. Elapsed time: %f min.\n",elapsed);
	printf("Number of players: %d\n", nPlayers);
	printf("Best player: %s\n", best.desc(desc));
	printf("Performance: %f\n", mean);

	//Write results to disk
	char outf[] = "resRCTEquilibrium.txt";
	std::ofstream outr(outf, ios::binary | ios::app);
	outr << "Elapsed time: " << elapsed << " min." << std::endl;
	outr << "Number of players: " << nPlayers << std::endl;
	outr << "Best player: " << best.desc(desc) << std::endl;
	outr << "Performance: " << mean << std::endl;
	outr << "------------------------------" << endl;
	outr.close();
}

/**
 *  @brief Experiment to evolve RCT players adding the difficulty of
 *  a random effective stack in each hand played.
 */
void ExperimentRCTRandomEffectiveStack (){
	/*
	 * Configure the player
	 */
	RCTPlayer genome;
	genome.initializer(RCTPlayer::raiseCallInitializer);
	genome.crossover(RCTPlayer::mergeCrossover);
	genome.mutator(RCTPlayer::SubtleMutator);

	/*
	 * Number of players and number of generations of the experiment.
	 * If the selected convergence method above is termination upon convergence, then
	 * the number of generations does not matter.
	 */
	int nPlayers = 100;
	int nGenerations = 10000;

	/**
	 *  Set the algorithm
	 */

	PlayerEvolver evolver(genome,nPlayers);

	ConcurrentTournamentWithCoin tournament(0.2);
	tournament.match(Tournament::randomEffectiveStackMatch);
	evolver.tournament(tournament);

	/*
	 * Plotting
	 */
	Console console;
	evolver.attachObserver(&console);

//	RCTPlotter plotter;
//	evolver.attachObserver(&plotter);

	RCTOverallPlotter overallPlotter(nPlayers);
	evolver.attachObserver(&overallPlotter);

	/*
	 * Write results to disc.
	 */
//	char filename[] = "rctr.dat";
//	std::ofstream out(filename, ios::binary | ios::trunc);
//	ResultsWriter writer(out,nPlayers);
//	evolver.attachObserver(&writer);

	//Increase a little bit mutation probability for this experiment.
	evolver.geneticAlgorithm().pMutation(0.1);
	// We don't want to preserve the best players in each generation.
	evolver.geneticAlgorithm().elitist(GABoolean::gaFalse);
	evolver.geneticAlgorithm().nGenerations(nGenerations);

	evolver.evolve();

	//Print results
	RCTPlayer& best = dynamic_cast<RCTPlayer&>(evolver.best());
	char desc[MAX_PLAYER_DESC];
	printf("Best player: %s\n", best.desc(desc));

	//Check results
	PlayerRaiseFoldPercent opp(0.58,0.37);
	ExperimentOneVsOne(best,opp);
}

/**
 *  @brief Experiment to match two players and analyze their performance.
 */
double ExperimentOneVsOne (const Player& p1, const Player& p2) {
	float mean = 0;
	int n = 0;

	for (int i=0; i<10;i++){
		Player* player1 = p1.clonePlayer();
		PlayerValuePlotter plotter;
		player1->attachObserver(&plotter);
		Player* player2 = p2.clonePlayer();
		PokerGame game = PokerGame(*player1,*player2);
		game.playSeveralHands(100000);

		mean = (mean*n + player1->ev())/(float)(n+1);
		n++;

		//Results
		printf("Match %d\n", i);
		printf("-----------------------------------------------------\n");
		printf("Hands played: %d\n", player1->hands_played());
		printf("Player 1 performance: %.2f bb/hands\n", player1->ev()*100);
		printf("Player 1 raised: %.2f%%, Player 2 raised: %.2f%%\n", player1->raised()*100, player2->raised()*100);
		printf("Player 1 called: %.2f%%, Player 2 called: %.2f%%\n", player1->called()*100, player2->called()*100);
		printf("-----------------------------------------------------\n");
		delete player1;
		delete player2;
	}

	printf("Player 1 performance mean (bb/hands): %f", mean*100);
	return mean;
}

void readfile (const char* filename, const Player& base){
	std::ifstream in(filename,ios::binary);
	ResultsReader reader(in,base);

	Console console;
	reader.attachObserver(&console);

	RCTPlotter plotter;
	reader.attachObserver(&plotter);

	reader.read();

	in.close();
}
