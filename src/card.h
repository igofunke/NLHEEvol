#ifndef _CARD_H_
#define _CARD_H_

#define CARDS_PER_SUIT 13
#define NRANKS 13

#include <cstdio>

/**
 *  @brief Enumeration containing the different ranks of a card.
 */
enum Rank {
	A=0,
	KING,
	QUEEN,
	JACK,
	TEN,
	NINE,
	EIGHT,
	SEVEN,
	SIX,
	FIVE,
	FOUR,
	THREE,
	TWO
};

/**
 *  @brief Char representation of the different cards.
 */
const char ranks[] = {'A','K','Q','J','T','9','8','7','6','5','4','3','2'};

/**
 *  @brief Enumeration containing the different suits of a card.
 */
enum Suit {
	SPADES=0,
	HEARTS,
	DIAMONDS,
	CLUBS
};

/**
 *  @brief Char representation of the different suits.
 */

const char suits[] = {'s','h','d','c'};

/**
 *  @brief Represents a card from the deck, encapsulating its rank and suit.
 */
class Card {
public:

	/**
	 *  @brief Creates a new card with no rank and suit assigned
	 */
	Card() :
		_id(0),
		_str(0) {}

	/**
	 *  @brief Creates a new card based on its identifier.
	 *  @param id Card identifier.
	 */
	Card(int id) :
		_id(id),
		_str(0) {}

	/**
	 *  @brief Creates a new card given its rank and its suit.
	 *  @param rank The rank of the card.
	 *  @param suit The suit of the card.
	 */
	Card(Rank rank, Suit suit) :
		_id(suit*CARDS_PER_SUIT + rank),
		_str(0) {}

	/**
	 *  @brief Copy constructor.
	 */
	Card (const Card& c) :
		_id(c._id),
		_str(0) {}

	/**
	 *  @brief assignment operator
	 */
	Card&
	operator= (const Card& rhs)
		{ _id = rhs._id; return *this;}


	~Card () { delete [] _str; }

	/**
	 *  @brief Returns the rank of the card.
	 */
	Rank
	rank () const { return Rank(_id % CARDS_PER_SUIT); }

	/**
	 *  @brief Returns the suit of the card.
	 */
	Suit
	suit () const { return Suit(_id / CARDS_PER_SUIT); }

	/**
	 *  @brief Returns the identifier of the card.
	 */
	int
	id() const { return _id; }

	/**
	 *  @brief Returns the string representation of the card.
	 */
	char*
	cstr () {
		if (_str == 0) _str = new char[2];
		_str[0] = ranks[_id % CARDS_PER_SUIT];
		_str[1] = suits[_id / CARDS_PER_SUIT];

		return _str;
	}

private:
	int _id;
	char* _str;
};

#endif
