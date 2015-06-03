#ifndef _HANDUTILS_H_
#define _HANDUTILS_H_

#include <algorithm>

#include "card.h"

#define NRANKS 13
#define NHANDS 169

/**
 *  @brief Table containing the position in the hand ranking for each hand.
 *  Each hand is associated with a cell by its table coordinates and that cell
 *  contains the position.
 */
const unsigned char top[NRANKS][NRANKS] = {
	{ 1,  12,  13,  15,  19,  24,  31,  36,  44,  45,  49,  53,  60}, 
	{ 8,   2,  23,  27,  29,  40,  51,  58,  61,  68,  74,  79,  87},
	{ 9,  16,   3,  37,  46,  56,  69,  76,  81,  91,  95,  99, 107}, 
	{11,  18,  28,   4,  57,  70,  78,  90, 102, 105, 112, 116, 121}, 
	{14,  21,  34,  43,   5,  80,  92,  98, 110, 122, 124, 129, 134}, 
	{20,  30,  41,  54,  62,   6, 100, 111, 119, 130, 141, 145, 149}, 
	{22,  38,  52,  65,  75,  83,   7, 117, 127, 137, 148, 156, 160}, 
	{25,  47,  66,  73,  86,  93, 101,  10, 133, 140, 151, 158, 165}, 
	{33,  50,  67,  85,  96, 106, 109, 115,  17, 144, 154, 163, 167}, 
	{32,  55,  72,  88, 104, 113, 120, 125, 126,  26, 153, 162, 166}, 
	{35,  59,  77,  94, 108, 123, 131, 135, 138, 136,  42, 164, 168}, 
	{39,  63,  82,  97, 114, 128, 139, 143, 147, 146, 150,  64, 169}, 
	{48,  71,  89, 103, 118, 132, 142, 152, 157, 155, 159, 161,  84}
};

/**
 * @brief Array that associates the position in the hand ranking of a
 * certain hand with the numeric identifier of that hand.
 */
const unsigned char hand_order[NHANDS] = {
		  0,  14,  28,  42,  56,  70,  84,  13,  98,  26,   1,  39,   2,
		 52, 112,  27,   3,  65,  40,   4,  53,  15,  78, 126,  91,  16,
		  5,  41,   6,  17,  66, 117,  54, 104, 130,   7,  29, 143,  79,
		 67,  18,  55, 140,   9,  92,  30,   8, 156,  10, 105,  80,  19,
		 68, 118,  11,  20,  31,  43,  12,  21,  93, 154, 131, 144,  81,
		 69, 106,  32,  44, 157,  22, 119,  94,  82,  23, 132,  33,  57,
		 45,  24, 145, 168,  34, 107,  83,  95,  25,  35, 158,  58, 120,
		 46, 133, 108,  96,  36, 146,  37,  71,  97,  47,  59, 109,  38,
		159, 121,  48, 134,  72, 110,  49,  60, 122, 147, 111,  50, 160,
		 85,  73, 123,  61,  51, 135,  62, 124, 148,  86, 125, 136,  99,
		161,  74,  63, 137,  64, 139,  87, 138, 149,  75, 100, 162, 113,
		150, 151,  76, 152,  88,  77, 101, 153, 127, 163, 165, 114,  89,
		164,  90, 166, 102, 128, 115, 167, 141, 103, 129, 116, 142, 155
};

/**
 *  @brief Returns the strength of a hand.
 *
 *  @param a First card in the hand.
 *  @param b Second card in the hand.
 */
double
handPercent (Card a, Card b);

/**
 *  @brief Converts a hand to its unique numeric representation.
 *
 *  @param a First card in the hand.
 *  @param b Second card in the hand.
 */
int
handToNumeric (Card a, Card b);

/**
 *  @brief Returns the position in the hand strength ranking
 *  of a certain hand given by its table coordinates.
 */
double
handPercentTableCoords (int i, int j);


#endif
