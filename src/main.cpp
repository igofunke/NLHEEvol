#include <unistd.h>
#include <cstdlib>
#include <fstream>
#include <signal.h>

#include "game.h"
#include "player.h"
#include "evolution.h"

void onevsone () {
	float mean = 0;
	int n = 0;

	for (int i=0; i<10;i++){
		//PlayerValuePlotter plotter;
		//PlayerNash player1;
		PlayerRaiseFoldPercent player1(0.58,0.38);
		//player1.attachObserver(&plotter);
		//PlayerNashFloat player2;
		PlayerRaiseFoldPercent player2(0.52,0.35);
		PokerGame game = PokerGame(player1,player2);
		game.playSeveralHands(100000);

		mean = (mean*n + player1.ev())/(float)(n+1);
		n++;

		//Results
		printf("Player 1 performance (bb/hands): %f\n", player1.ev());
		printf("Player 1 raised: %f , Player 2 raised: %f\n", player1.raised(), player2.raised());
		printf("Player 1 called: %f , Player 2 called: %f\n", player1.called(), player2.called());
		//printf("Player 2 performance (bb/hands): %f\n", player2.ev());
		//printf("Player 1 outcome: %f\n", player1.outcome());
		//printf("Player 2 outcome: %f\n", player2.ev());
		//printf("%f;",player1.ev());
	}

	printf("Media de los valores: %f", mean);
}

void onevsone_mean () {

	for (int h=0; h <10; h++){
		float mean = 0;
		int n = 0;
		for (int i=0; i<20;i++){
			//PlayerValuePlotter plotter;
			//PlayerNash player1;
			PlayerRaiseFoldPercent player1(0.52,0.35);
			//player1.attachObserver(&plotter);
			//PlayerNashFloat player2;
			PlayerRaiseFoldPercent player2(0.58,0.37);
			PokerGame game = PokerGame(player1,player2);
			game.playSeveralHands(50000);

			mean = (mean*n + player1.ev())/(float)(n+1);
			n++;
		}
	printf("Media de los valores: %f\n", mean);
	}

}
