#ifndef _DECK_H_
#define _DECK_H_

#include <vector>
#include <random>
#include <algorithm>

#define CARDS_PER_DECK 52

/**
 *  @brief Simulates the behavior of a deck of 52 cards.
 */
class Deck {
public:
	/**
	 *  @briefs Resets the deck to its original state.
	 */
	void
	shuffle ();

	/**
	 *  @brief Extracts a card randomly from the deck. The identifier of
	 *  the card extracted is returned.
	 */
	int
	popCard ();

private:
	std::vector<int> _deck;
};

#endif
