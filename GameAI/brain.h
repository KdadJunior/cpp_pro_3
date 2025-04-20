#ifndef BRAIN_H
#define BRAIN_H

#include <string>
#include "../Game/game.h"
#include <cstdlib>
#include <ctime>

class Brain
{
private:
    bool flag_picked; // Track if flag is picked in Stage 4
    int move_counter; // Track the number of moves
    int current_stage; // Track the current stage to reset move_counter

public:
    Brain();                               // Constructor
    int getNextMove(GameState &gamestate); // Returns the next move for the AI
};

#endif // BRAIN_H