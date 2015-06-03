#ifndef _ACTION_H_
#define _ACTION_H_

/**
 *  @brief Each of the different actions that a player can make.
 */
enum ActionType {
	FOLD,
	CALL,
	RAISE
};

/**
 *  @brief Encapsulates the action that the player has taken.
 */
class Action {
public:
	/**
	 *  @brief Creates the action.
	 *
	 *  @param action Type of action.
	 *  @param to the value to raise to.
	 */
	Action(ActionType action,int to) :
		_to(to),
		_type(action) {}

	/**
	 *  @brief Returns the type of the action.
	 */
	ActionType
	type() const { return _type; }

	/**
	 *  @brief Returns the value to which the player is raising.
	 */
	int
	to () const { return _to; }

private:
	int _to;
	ActionType _type;
};

#endif
