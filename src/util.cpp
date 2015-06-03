/**
 *  @file This file contains some methods to generate charts, hand rankings...
 */

#include <cstdio>
#include <cstring>
#include <algorithm>
#include <vector>

#include <pbots_calc/pbots_calc.h>

#include "card.h"

typedef std::pair<double,short> heq_t; 

/**
 *  @brief Computes a hand strength rank, sorting them by the probability of
 *  winning against a random hand. It prints the identifier of the ordered hands
 *  as well as a table with each cell representing a hand filled with the position
 *  of the hand in the rank.
 *
 *  It is used to generate the charts for the strategies of the different players.
 */
void top_cards(){
	char hand_ne[] = "xxt:xx";
	char hand_eq[] = "xx:xx";
	char* hand;

	Results* res;
	res = alloc_results();

	std::vector<heq_t> equities;

	//Compute the probabilities
	for (int i=0; i<NRANKS; i++){
		for (int j=0; j<NRANKS; j++){
			if (i == j) hand = hand_eq;
			else hand = hand_ne;

			hand[0] = ranks[i];
			hand[1] = ranks[j];
			if (i>j) hand[2] = 's';
			else if (i < j) hand[2] = 'o';
	
			calc(hand,"","",100000,res);
			equities.push_back(std::make_pair(res->ev[0],i*NRANKS+j));
		}
	}

	//Sort the hands
	std::sort(equities.begin(),equities.end());
	float i = 1;
	int top[NRANKS][NRANKS];

	//Hand order
	printf("Hand order: ");
	for (std::vector<heq_t>::reverse_iterator it = equities.rbegin(); it!= equities.rend(); it++){
		printf(" %3d,", (*it).second);
		top[(*it).second / NRANKS][(*it).second % NRANKS] = i++;
	}
	printf("\n\n");

	//Table of hands
	printf("Position in the rank for each of the hands: \n");
	printf("   ");
	for (int i=0; i<NRANKS; i++)
		printf("  %c ", ranks[i]);

	printf("\n");

	for (int i=0; i<NRANKS; i++){
		printf(" %c ", ranks[i]);
		for (int j=0; j<NRANKS; j++){
			printf("%3d ", top[i][j]);
		}
		printf("\n");
	}
		
	free_results(res);
}
