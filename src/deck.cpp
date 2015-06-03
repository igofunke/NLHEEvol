#include "deck.h"

void Deck::shuffle(){
	_deck.clear();
	for (int i=0; i<CARDS_PER_DECK; i++)
		_deck.push_back(i);

	std::random_device device;
	std::mt19937 generator(device());

	std::shuffle(_deck.begin(),_deck.end(), generator);
}

int Deck::popCard(){
	int card = _deck.back();
	_deck.pop_back();

	return card;
}
