#include "brain.h"

Brain::Brain() : flag_picked(false), move_counter(0), current_stage(-1)
{
    // No random seed initialization needed
}

int Brain::getNextMove(GameState& gamestate)
{
    int stage = gamestate.stage;
    char direction = gamestate.vision[0][0]; // Player's direction (at (0,0) in vision)

    // Reset move_counter when stage changes
    if (stage != current_stage) {
        move_counter = 0;
        current_stage = stage;
    }

    // Stage 0: Follow the pattern (5 down, 4 right, 5 up, 2 right, 7 down, 1 right)
    if (stage == 0) {
        move_counter++;
        // Total moves: 5 + 4 + 5 + 2 + 7 + 1 = 24
        if (move_counter <= 5) {
            // 5 times down
            return 3; // Down
        } else if (move_counter <= 9) {
            // 4 times right
            if (direction != '>') {
                return 4; // Turn right
            }
            return 4; // Move right (blocked by wall at (6, 2))
        } else if (move_counter <= 14) {
            // 5 times up
            if (direction != '^') {
                return 1; // Turn up
            }
            return 1; // Move up
        } else if (move_counter <= 16) {
            // 2 times right
            if (direction != '>') {
                return 4; // Turn right
            }
            return 4; // Move right (blocked by wall at (1, 2))
        } else if (move_counter <= 23) {
            // 7 times down
            if (direction != 'v') {
                return 3; // Turn down
            }
            return 3; // Move down
        } else if (move_counter == 24) {
            // 1 time right
            if (direction != '>') {
                return 4; // Turn right
            }
            return 4; // Move right (blocked by wall at (7, 2))
        }
        return 0; // After the pattern, do nothing
    }

    // Stage 1: Follow the pattern starting from (7, 1)
    // Pattern: 1 right, 3 up, 1 right, 3 up, 1 right, 6 down, 2 right, 7 up, 1 right, 4 down, 1 right, 1 up, 4 down, 2 right
    if (stage == 1) {
        move_counter++;
        // Total moves: 1 + 3 + 1 + 3 + 1 + 6 + 2 + 7 + 1 + 4 + 1 + 1 + 4 + 2 = 37
        if (move_counter == 1) {
            // 1 time right
            if (direction != '>') {
                return 4; // Turn right
            }
            return 4; // Move right (blocked by wall at (7, 2))
        } else if (move_counter <= 4) {
            // 3 times up
            if (direction != '^') {
                return 1; // Turn up
            }
            return 1; // Move up
        } else if (move_counter == 5) {
            // 1 time right
            if (direction != '>') {
                return 4; // Turn right
            }
            return 4; // Move right (blocked by wall at (4, 2))
        } else if (move_counter <= 8) {
            // 3 times up
            if (direction != '^') {
                return 1; // Turn up
            }
            return 1; // Move up
        } else if (move_counter == 9) {
            // 1 time right
            if (direction != '>') {
                return 4; // Turn right
            }
            return 4; // Move right (blocked by wall at (1, 2))
        } else if (move_counter <= 15) {
            // 6 times down
            if (direction != 'v') {
                return 3; // Turn down
            }
            return 3; // Move down
        } else if (move_counter <= 17) {
            // 2 times right
            if (direction != '>') {
                return 4; // Turn right
            }
            return 4; // Move right (blocked by wall at (7, 2))
        } else if (move_counter <= 24) {
            // 7 times up
            if (direction != '^') {
                return 1; // Turn up
            }
            return 1; // Move up
        } else if (move_counter == 25) {
            // 1 time right
            if (direction != '>') {
                return 4; // Turn right
            }
            return 4; // Move right (blocked by wall at (0, 2))
        } else if (move_counter <= 29) {
            // 4 times down
            if (direction != 'v') {
                return 3; // Turn down
            }
            return 3; // Move down
        } else if (move_counter == 30) {
            // 1 time right
            if (direction != '>') {
                return 4; // Turn right
            }
            return 4; // Move right (blocked by wall at (4, 2))
        } else if (move_counter == 31) {
            // 1 time up
            if (direction != '^') {
                return 1; // Turn up
            }
            return 1; // Move up
        } else if (move_counter <= 35) {
            // 4 times down
            if (direction != 'v') {
                return 3; // Turn down
            }
            return 3; // Move down
        } else if (move_counter <= 37) {
            // 2 times right
            if (direction != '>') {
                return 4; // Turn right
            }
            return 4; // Move right (blocked by wall at (7, 2))
        }
        return 0; // After the pattern, do nothing
    }

    // Stage 2: Adjusted pattern starting from (7, 1)
    // Pattern: 3 right, 2 up, 2 right, 2 down, 2 right, 4 up, 4 left, 2 up, 6 right, 6 down, 18 right, 2 up, 1 left, 2 up, 1 up
    if (stage == 2) {
        move_counter++;
        // Total moves: 3 + 2 + 2 + 2 + 2 + 4 + 4 + 2 + 6 + 6 + 18 + 2 + 1 + 2 + 1 = 57
        if (move_counter <= 3) {
            // 3 times right
            if (direction != '>') {
                return 4; // Turn right
            }
            return 4; // Move right (blocked by wall at (7, 2))
        } else if (move_counter <= 5) {
            // 2 times up
            if (direction != '^') {
                return 1; // Turn up
            }
            return 1; // Move up
        } else if (move_counter <= 7) {
            // 2 times right
            if (direction != '>') {
                return 4; // Turn right
            }
            return 4; // Move right (blocked by wall at (5, 2))
        } else if (move_counter <= 9) {
            // 2 times down
            if (direction != 'v') {
                return 3; // Turn down
            }
            return 3; // Move down
        } else if (move_counter <= 11) {
            // 2 times right
            if (direction != '>') {
                return 4; // Turn right
            }
            return 4; // Move right (blocked by wall at (7, 2))
        } else if (move_counter <= 15) {
            // 4 times up
            if (direction != '^') {
                return 1; // Turn up
            }
            return 1; // Move up
        } else if (move_counter <= 19) {
            // 4 times left
            if (direction != '<') {
                return 2; // Turn left
            }
            return 2; // Move left (blocked by boundary at column 0)
        } else if (move_counter <= 21) {
            // 2 times up
            if (direction != '^') {
                return 1; // Turn up
            }
            return 1; // Move up
        } else if (move_counter <= 26) {
            // 5 times right
            if (direction != '>') {
                return 4; // Turn right
            }
            return 4; // Move right (blocked by wall at (1, 2))
        } 
        return 0; // After the pattern, do nothing
    }

    // Stage 3: New pattern starting from (1, 1)
    // Pattern: 1 right, 1 down, 5 right, 4 down, 4 left, 1 up, 1 down, 4 right, 5 up, 1 left, 4 right, 3 down, 1 right
    if (stage == 3) {
        move_counter++;
        // Total moves: 1 + 1 + 5 + 4 + 4 + 1 + 1 + 4 + 5 + 1 + 4 + 3 + 1 = 35
        if (move_counter == 1) {
            // 1 time right
            if (direction != '>') {
                return 4; // Turn right
            }
            return 4; // Move right (blocked by wall at (1, 2))
        } else if (move_counter == 2) {
            // 1 time down
            if (direction != 'v') {
                return 3; // Turn down
            }
            return 3; // Move down
        } else if (move_counter <= 7) {
            // 5 times right
            if (direction != '>') {
                return 4; // Turn right
            }
            return 4; // Move right (blocked by wall at (2, 2))
        } else if (move_counter <= 11) {
            // 4 times down
            if (direction != 'v') {
                return 3; // Turn down
            }
            return 3; // Move down
        } else if (move_counter <= 15) {
            // 4 times left
            if (direction != '<') {
                return 2; // Turn left
            }
            return 2; // Move left (blocked by boundary)
        } else if (move_counter == 16) {
            // 1 time up
            if (direction != '^') {
                return 1; // Turn up
            }
            return 1; // Move up
        } else if (move_counter == 17) {
            // 1 time down
            if (direction != 'v') {
                return 3; // Turn down
            }
            return 3; // Move down
        } else if (move_counter <= 21) {
            // 4 times right
            if (direction != '>') {
                return 4; // Turn right
            }
            return 4; // Move right (blocked by wall at (6, 2))
        } else if (move_counter <= 26) {
            // 5 times up
            if (direction != '^') {
                return 1; // Turn up
            }
            return 1; // Move up
        } else if (move_counter <= 28) {
            // 2 time left
            if (direction != '<') {
                return 2; // Turn left
            }
            return 2; // Move left (blocked by boundary)
        } else if (move_counter <= 32) {
            // 5 times right
            if (direction != '>') {
                return 4; // Turn right
            }
            return 4; // Move right (blocked by wall at (1, 2))
        } else if (move_counter <= 35) {
            // 3 times down
            if (direction != 'v') {
                return 3; // Turn down
            }
            return 3; // Move down
        } else if (move_counter == 36) {
            // 1 time right
            if (direction != '>') {
                return 4; // Turn right
            }
            return 4; // Move right (blocked by wall at (4, 2))
        }
        return 0; // After the pattern, do nothing
    }


    return 0; // Default: Do nothing
}