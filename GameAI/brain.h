#ifndef BRAIN_H
#define BRAIN_H

#include <string>
#include "../Game/game.h"
#include <cstdlib>
#include <ctime>

class Brain
{
private:
    bool flag_picked;       // Track if flag is picked in Stage 4
    int move_counter;       // Track the number of moves
    int current_stage;      // Track the current stage to reset move_counter
    int prev_move;          // Track previous move (1=up, 2=left, 3=down, 4=right)
    int prev_prev_move;     // Track move before previous move
    bool right_blocked;
    bool down_blocked;

    // Helper function to update movement history
    int updateMoveHistory(int move);

public:
    Brain();                               // Constructor
    int getNextMove(GameState &gamestate); // Returns the next move for the AI
};

#endif // BRAIN_H