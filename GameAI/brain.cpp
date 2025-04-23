#include "brain.h"
#include <vector>
#include <utility>

Brain::Brain() : flag_picked(false), move_counter(0), current_stage(-1), 
                highest_stage(-1), prev_move(0), prev_prev_move(0), 
                right_blocked(false), down_blocked(false) {}

int Brain::updateMoveHistory(int move) {
    prev_prev_move = prev_move;
    prev_move = move;
    return move;
}

int Brain::getNextMove(GameState& gamestate) {
    // Use highest_stage to prevent stage regression
    int stage = gamestate.stage;
    if (stage > highest_stage) {
        highest_stage = stage;
    }
    stage = highest_stage; // Use the highest stage reached

    // Reset state only when moving to a new higher stage
    if (stage != current_stage) {
        move_counter = 0;
        current_stage = stage;
        flag_picked = false;
        prev_move = 0;
        prev_prev_move = 0;
        right_blocked = false;
        down_blocked = false;
    }
    move_counter++;

    // Find the player's direction by searching the vision grid
    char direction = ' ';
    int player_row = -1, player_col = -1;
    for (int i = 0; i < gamestate.vision.size(); i++) {
        for (int j = 0; j < gamestate.vision[i].size(); j++) {
            if (gamestate.vision[i][j] == 'v' || gamestate.vision[i][j] == '^' || 
                gamestate.vision[i][j] == '>' || gamestate.vision[i][j] == '<') {
                direction = gamestate.vision[i][j];
                player_row = i;
                player_col = j;
                break;
            }
        }
        if (player_row != -1) break;
    }

    if (direction == ' ') {
        return updateMoveHistory(0);
    }

    // Create a 3x3 grid centered on the player
    std::vector<std::vector<char>> local_grid(3, std::vector<char>(3, '+')); // Default to wall
    local_grid[1][1] = direction; // Player position at center

    // Map vision grid to 3x3 grid relative to player
    for (int di = -1; di <= 1; di++) {
        for (int dj = -1; dj <= 1; dj++) {
            if (di == 0 && dj == 0) continue; // Skip player position
            int vision_row = player_row + di;
            int vision_col = player_col + dj;
            int grid_row = 1 + di; // Map to 3x3 grid (0 to 2)
            int grid_col = 1 + dj;

            // Check if the vision coordinates are within bounds
            if (vision_row >= 0 && vision_row < gamestate.vision.size() &&
                vision_col >= 0 && vision_col < gamestate.vision[0].size()) {
                local_grid[grid_row][grid_col] = gamestate.vision[vision_row][vision_col];
            }
            // Out-of-bounds positions remain as '+' (wall)
        }
    }

    // Compute wall positions using the 3x3 grid (only '+' is a wall)
    bool wall_up = (local_grid[0][1] == '+');
    bool wall_down = (local_grid[2][1] == '+');
    bool wall_left = (local_grid[1][0] == '+');
    bool wall_right = (local_grid[1][2] == '+');
    bool wall_up_right = (local_grid[0][2] == '+');

    // Check destination tiles for movement and handle 'A' and 'B'
    static bool A_is_encountered = false;
    bool can_move_up = true;
    bool can_move_down = true;
    bool can_move_left = true;
    bool can_move_right = true;

    // Check the tile in each direction from gamestate.vision
    if (player_row - 1 >= 0) {
        char up_tile = gamestate.vision[player_row - 1][player_col];
        if (up_tile == '+') {
            can_move_up = false;
        } else if (up_tile == 'A') {
            A_is_encountered = true;
            can_move_up = true;
        } else if (up_tile == 'B') {
            can_move_up = A_is_encountered;
        }
    } else {
        can_move_up = false;
    }

    if (player_row + 1 < gamestate.vision.size()) {
        char down_tile = gamestate.vision[player_row + 1][player_col];
        if (down_tile == '+') {
            can_move_down = false;
        } else if (down_tile == 'A') {
            A_is_encountered = true;
            can_move_down = true;
        } else if (down_tile == 'B') {
            can_move_down = A_is_encountered;
        }
    } else {
        can_move_down = false;
    }

    if (player_col - 1 >= 0) {
        char left_tile = gamestate.vision[player_row][player_col - 1];
        if (left_tile == '+') {
            can_move_left = false;
        } else if (left_tile == 'A') {
            A_is_encountered = true;
            can_move_left = true;
        } else if (left_tile == 'B') {
            can_move_left = A_is_encountered;
        }
    } else {
        can_move_left = false;
    }

    if (player_col + 1 < gamestate.vision[0].size()) {
        char right_tile = gamestate.vision[player_row][player_col + 1];
        if (right_tile == '+') {
            can_move_right = false;
        } else if (right_tile == 'A') {
            A_is_encountered = true;
            can_move_right = true;
        } else if (right_tile == 'B') {
            can_move_right = A_is_encountered;
        }
    } else {
        can_move_right = false;
    }

    // Stage 0: Navigation using loops and conditions
    if (stage == 0) {
        const int START = 0;
        const int RIGHT_MOVE = 1;
        const int ZIGZAG = 2;
        static int current_state = START;

        if (current_state == START) {
            // Move up as long as there's no wall above
            while (!wall_up && can_move_up) {
                if (direction != '^') return updateMoveHistory(1);
                return updateMoveHistory(1);
            }
            current_state = RIGHT_MOVE;
        }
        // Move right till AI hits a wall
        if (current_state == RIGHT_MOVE) {
            while (!wall_right && can_move_right) {
                if (direction != '>') return updateMoveHistory(4);
                return updateMoveHistory(4);
            }
            right_blocked = true;
            current_state = ZIGZAG;
        }
        // Logic for zigzag movement in stage 0
        if (current_state == ZIGZAG) {
            // Priority: If last move was down, move right if possible
            if (prev_move == 3 && !wall_right && can_move_right) {
                if (direction != '>') return updateMoveHistory(4);
                return updateMoveHistory(4);
            }

            // Check if blocked in both right and down directions
            if (right_blocked && wall_down) {
                down_blocked = true;
            }

            // If blocked in both directions, reset state only if last move wasn't down
            if (right_blocked && down_blocked) {
                if (prev_move != 3) {
                    current_state = START;
                    right_blocked = false;
                    down_blocked = false;
                }
                return updateMoveHistory(0);
            }
            // Try to move right and up (only if last move was not down)
            else if (!wall_right && !wall_up_right && prev_move != 3 && can_move_right) {
                if (direction != '>') return updateMoveHistory(4);
                current_state = START;
                return updateMoveHistory(1);
            }
            // Try to move right
            else if (!wall_right && can_move_right) {
                if (direction != '>') return updateMoveHistory(4);
                return updateMoveHistory(4);
            }
            // Try to move down
            else if (!wall_down && can_move_down) {
                if (direction != 'v') return updateMoveHistory(3);
                return updateMoveHistory(3);
            }
            // If previously down_blocked but right is now open, move right
            else if (down_blocked && !wall_right && can_move_right) {
                if (direction != '>') return updateMoveHistory(4);
                return updateMoveHistory(4);
            }
            // If all directions are blocked and last move wasn't down, reset to START
            else {
                if (prev_move != 3) {
                    current_state = START;
                }
                return updateMoveHistory(0);
            }
        }
    }

    // Stage 1: Food collection
    if (stage == 1) {
        const int MOVE_UP = 0;
        const int MOVE_DOWN = 1;
        const int MOVE_RIGHT = 2;
        const int CHECK_RIGHT = 3;
        static int current_state = MOVE_UP;

        if (current_state == MOVE_UP) {
            if (!wall_up && can_move_up) {
                if (direction != '^') return updateMoveHistory(1);
                return updateMoveHistory(1);
            }
            current_state = MOVE_DOWN;
        }

        if (current_state == MOVE_DOWN) {
            if (!wall_down && can_move_down) {
                if (direction != 'v') return updateMoveHistory(3);
                return updateMoveHistory(3);
            }
            current_state = MOVE_RIGHT;
        }

        if (current_state == MOVE_RIGHT) {
            if (!wall_right && can_move_right) {
                if (direction != '>'){
                    current_state = MOVE_UP;
                    right_blocked = false;
                    return updateMoveHistory(4);
                }
                right_blocked = false; // Reset right_blocked since right move succeeded
                current_state = MOVE_UP;
                return updateMoveHistory(4);
            }
            else {
                right_blocked = true;
                current_state = CHECK_RIGHT;
                // Since right is blocked, start moving up as per the pattern
                if (!wall_up && can_move_up) {
                    if (direction != '^') return updateMoveHistory(1);
                    return updateMoveHistory(1);
                }
            }
        }

        if (current_state == CHECK_RIGHT) {
            // After moving up, check if right is open
            if (prev_move == 1 && !wall_right && can_move_right) {
                if (direction != '>') {
                    right_blocked = false;
                    current_state = MOVE_UP;
                    return updateMoveHistory(4);
                }
                right_blocked = false;
                current_state = MOVE_UP;
                return updateMoveHistory(4);
            }
            // Otherwise, continue moving up until hitting a wall
            else if (!wall_up && can_move_up) {
                if (direction != '^') return updateMoveHistory(1);
                return updateMoveHistory(1);
            }
            // Once a wall is hit, transition to MOVE_DOWN
            current_state = MOVE_DOWN;
        }
    }

    // Stage 2: New movement pattern with priorities
    if (stage == 2) {
        const int MOVE_RIGHT = 0;
        const int MOVE_UP = 1;
        const int MOVE_DOWN = 2;
        const int MOVE_LEFT = 3;
        static int current_state = MOVE_RIGHT;

        if (current_state == MOVE_RIGHT) {
            // Move right until hitting a wall
            while (!wall_right && can_move_right) {
                if (direction != '>') return updateMoveHistory(4);
                return updateMoveHistory(4);
            }
            // Priorities after hitting a wall
            if (!wall_up && can_move_up) {
                // 1st priority: Go up if up is open
                current_state = MOVE_UP;
                if (direction != '^') return updateMoveHistory(1);
                return updateMoveHistory(1);
            } else if (!wall_down && can_move_down) {
                // 2nd priority: Go down if down is open and up is closed
                current_state = MOVE_DOWN;
                if (direction != 'v') return updateMoveHistory(3);
                return updateMoveHistory(3);
            } else {
                // 3rd priority: Go left if up and down are closed
                current_state = MOVE_LEFT;
                if (!wall_left && can_move_left) {
                    if (direction != '<') return updateMoveHistory(2);
                    return updateMoveHistory(2);
                }
                return updateMoveHistory(0);
            }
        }

        if (current_state == MOVE_UP) {
            // Move up until hitting a wall
            while (!wall_up && can_move_up) {
                if (direction != '^') return updateMoveHistory(1);
                return updateMoveHistory(1);
            }
            // Priorities after hitting a wall
            if (!wall_right && can_move_right) {
                // 1st priority: Go right if right is open
                current_state = MOVE_RIGHT;
                if (direction != '>') return updateMoveHistory(4);
                return updateMoveHistory(4);
            } else if (!wall_left && can_move_left) {
                // 2nd priority: Go left if left is open and right is closed
                current_state = MOVE_LEFT;
                if (direction != '<') return updateMoveHistory(2);
                return updateMoveHistory(2);
            } else {
                // 3rd priority: Go down if left and right are closed
                current_state = MOVE_DOWN;
                if (!wall_down && can_move_down) {
                    if (direction != 'v') return updateMoveHistory(3);
                    return updateMoveHistory(3);
                }
                return updateMoveHistory(0);
            }
        }

        if (current_state == MOVE_DOWN) {
            // Move down until hitting a wall
            while (!wall_down && can_move_down) {
                if (direction != 'v') return updateMoveHistory(3);
                return updateMoveHistory(3);
            }
            // Priorities after hitting a wall
            if (!wall_right && can_move_right) {
                // 1st priority: Go right if right is open
                current_state = MOVE_RIGHT;
                if (direction != '>') return updateMoveHistory(4);
                return updateMoveHistory(4);
            } else {
                // 2nd priority: Go up if right is closed
                current_state = MOVE_UP;
                if (!wall_up && can_move_up) {
                    if (direction != '^') return updateMoveHistory(1);
                    return updateMoveHistory(1);
                }
                return updateMoveHistory(0);
            }
        }

        if (current_state == MOVE_LEFT) {
            // Move left until hitting a wall
            while (!wall_left && can_move_left) {
                if (direction != '<') return updateMoveHistory(2);
                return updateMoveHistory(2);
            }
            // Priorities after hitting a wall
            if (!wall_up && can_move_up) {
                // 1st priority: Go up if up is open
                current_state = MOVE_UP;
                if (direction != '^') return updateMoveHistory(1);
                return updateMoveHistory(1);
            } else if (!wall_right && can_move_right) {
                // 2nd priority: Go right if up is closed and right is open
                current_state = MOVE_RIGHT;
                if (direction != '>') return updateMoveHistory(4);
                return updateMoveHistory(4);
            } else {
                // No 3rd priority: Do nothing if up and right are closed
                return updateMoveHistory(0);
            }
        }
    }

    // Stage 3: Updated serpentine movement pattern with phases
    if (stage == 3) {
        // Define states for each phase
        const int PHASE_1_UP = 0;
        const int PHASE_1_DOWN = 1;
        const int PHASE_2_UP = 2;
        const int PHASE_2_DOWN = 3;
        const int PHASE_3_UP = 4;
        const int PHASE_3_DOWN = 5;
        const int PHASE_4_UP = 6;
        const int PHASE_5_RIGHT = 7;
        const int PHASE_5_LEFT = 8;
        const int PHASE_6_RIGHT = 9;
        const int PHASE_6_LEFT = 10;
        const int PHASE_7_RIGHT = 11;
        const int PHASE_7_LEFT = 12;
        const int PHASE_8_UP_RIGHT = 13;
        const int PHASE_8_RIGHT = 14;

        static int current_state = PHASE_1_UP;
        static int current_phase = 1; // Track which phase we're in (1 to 8)

        // Phase 1: Sweep Right
        if (current_phase == 1) {
            if (current_state == PHASE_1_UP) {
                // Move up until hitting a wall
                if (!wall_up && can_move_up) {
                    if (direction != '^') return updateMoveHistory(1);
                    return updateMoveHistory(1);
                }
                // Hit a wall, transition to moving down
                current_state = PHASE_1_DOWN;
                if (!wall_down && can_move_down) {
                    if (direction != 'v') return updateMoveHistory(3);
                    return updateMoveHistory(3);
                }
                return updateMoveHistory(0);
            }

            if (current_state == PHASE_1_DOWN) {
                // Move down until hitting a wall
                if (!wall_down && can_move_down) {
                    if (direction != 'v') return updateMoveHistory(3);
                    return updateMoveHistory(3);
                }
                // Hit a wall, check if right and down are blocked
                if (wall_right && wall_down) {
                    // Transition to Phase 2
                    current_phase = 2;
                    current_state = PHASE_2_UP;
                    if (!wall_up && can_move_up) {
                        if (direction != '^') return updateMoveHistory(1);
                        return updateMoveHistory(1);
                    }
                    return updateMoveHistory(0);
                }
                // Move right once and return to PHASE_1_UP
                current_state = PHASE_1_UP;
                if (!wall_right && can_move_right) {
                    if (direction != '>') return updateMoveHistory(4);
                    return updateMoveHistory(4);
                }
                return updateMoveHistory(0);
            }
        }

        // Phase 2: Sweep Left
        if (current_phase == 2) {
            if (current_state == PHASE_2_UP) {
                // Move up until hitting a wall
                if (!wall_up && can_move_up) {
                    if (direction != '^') return updateMoveHistory(1);
                    return updateMoveHistory(1);
                }
                // Hit a wall, transition to moving down
                current_state = PHASE_2_DOWN;
                if (!wall_down && can_move_down) {
                    if (direction != 'v') return updateMoveHistory(3);
                    return updateMoveHistory(3);
                }
                return updateMoveHistory(0);
            }

            if (current_state == PHASE_2_DOWN) {
                // Move down until hitting a wall
                if (!wall_down && can_move_down) {
                    if (direction != 'v') return updateMoveHistory(3);
                    return updateMoveHistory(3);
                }
                // Hit a wall, check if left and down are blocked
                if (wall_left && wall_down) {
                    // Transition to Phase 3
                    current_phase = 3;
                    current_state = PHASE_3_UP;
                    if (!wall_up && can_move_up) {
                        if (direction != '^') return updateMoveHistory(1);
                        return updateMoveHistory(1);
                    }
                    return updateMoveHistory(0);
                }
                // Move left once and return to PHASE_2_UP
                current_state = PHASE_2_UP;
                if (!wall_left && can_move_left) {
                    if (direction != '<') return updateMoveHistory(2);
                    return updateMoveHistory(2);
                }
                return updateMoveHistory(0);
            }
        }

        // Phase 3: Sweep Right Again
        if (current_phase == 3) {
            if (current_state == PHASE_3_UP) {
                // Move up until hitting a wall
                if (!wall_up && can_move_up) {
                    if (direction != '^') return updateMoveHistory(1);
                    return updateMoveHistory(1);
                }
                // Hit a wall, transition to moving down
                current_state = PHASE_3_DOWN;
                if (!wall_down && can_move_down) {
                    if (direction != 'v') return updateMoveHistory(3);
                    return updateMoveHistory(3);
                }
                return updateMoveHistory(0);
            }

            if (current_state == PHASE_3_DOWN) {
                // Move down until hitting a wall
                if (!wall_down && can_move_down) {
                    if (direction != 'v') return updateMoveHistory(3);
                    return updateMoveHistory(3);
                }
                // Hit a wall, check if right and down are blocked
                if (wall_right && wall_down) {
                    // Transition to Phase 4
                    current_phase = 4;
                    current_state = PHASE_4_UP;
                    if (!wall_up && can_move_up) {
                        if (direction != '^') return updateMoveHistory(1);
                        return updateMoveHistory(1);
                    }
                    return updateMoveHistory(0);
                }
                // Move right once and return to PHASE_3_UP
                current_state = PHASE_3_UP;
                if (!wall_right && can_move_right) {
                    if (direction != '>') return updateMoveHistory(4);
                    return updateMoveHistory(4);
                }
                return updateMoveHistory(0);
            }
        }

        // Phase 4: Move Up Until Wall
        if (current_phase == 4) {
            if (current_state == PHASE_4_UP) {
                // Move up until hitting a wall
                if (!wall_up && can_move_up) {
                    if (direction != '^') return updateMoveHistory(1);
                    return updateMoveHistory(1);
                }
                // Hit a wall, transition to Phase 5
                current_phase = 5;
                current_state = PHASE_5_RIGHT;
                if (!wall_right && can_move_right) {
                    if (direction != '>') return updateMoveHistory(4);
                    return updateMoveHistory(4);
                }
                return updateMoveHistory(0);
            }
        }

        // Phase 5: Sweep Down
        if (current_phase == 5) {
            if (current_state == PHASE_5_RIGHT) {
                // Move right until hitting a wall
                if (!wall_right && can_move_right) {
                    if (direction != '>') return updateMoveHistory(4);
                    return updateMoveHistory(4);
                }
                // Hit a wall, transition to moving left
                current_state = PHASE_5_LEFT;
                if (!wall_left && can_move_left) {
                    if (direction != '<') return updateMoveHistory(2);
                    return updateMoveHistory(2);
                }
                return updateMoveHistory(0);
            }

            if (current_state == PHASE_5_LEFT) {
                // Move left until hitting a wall
                if (!wall_left && can_move_left) {
                    if (direction != '<') return updateMoveHistory(2);
                    return updateMoveHistory(2);
                }
                // Hit a wall, check if down and left are blocked
                if (wall_down && wall_left) {
                    // Transition to Phase 6
                    current_phase = 6;
                    current_state = PHASE_6_RIGHT;
                    if (!wall_right && can_move_right) {
                        if (direction != '>') return updateMoveHistory(4);
                        return updateMoveHistory(4);
                    }
                    return updateMoveHistory(0);
                }
                // Move down once and return to PHASE_5_RIGHT
                current_state = PHASE_5_RIGHT;
                if (!wall_down && can_move_down) {
                    if (direction != 'v') return updateMoveHistory(3);
                    return updateMoveHistory(3);
                }
                return updateMoveHistory(0);
            }
        }

        // Phase 6: Sweep Up
        if (current_phase == 6) {
            if (current_state == PHASE_6_RIGHT) {
                // Move right until hitting a wall
                if (!wall_right && can_move_right) {
                    if (direction != '>') return updateMoveHistory(4);
                    return updateMoveHistory(4);
                }
                // Hit a wall, transition to moving left
                current_state = PHASE_6_LEFT;
                if (!wall_left && can_move_left) {
                    if (direction != '<') return updateMoveHistory(2);
                    return updateMoveHistory(2);
                }
                return updateMoveHistory(0);
            }

            if (current_state == PHASE_6_LEFT) {
                // Move left until hitting a wall
                if (!wall_left && can_move_left) {
                    if (direction != '<') return updateMoveHistory(2);
                    return updateMoveHistory(2);
                }
                // Hit a wall, check if up and left are blocked
                if (wall_up && wall_left) {
                    // Transition to Phase 7
                    current_phase = 7;
                    current_state = PHASE_7_RIGHT;
                    if (!wall_right && can_move_right) {
                        if (direction != '>') return updateMoveHistory(4);
                        return updateMoveHistory(4);
                    }
                    return updateMoveHistory(0);
                }
                // Move up once and return to PHASE_6_RIGHT
                current_state = PHASE_6_RIGHT;
                if (!wall_up && can_move_up) {
                    if (direction != '^') return updateMoveHistory(1);
                    return updateMoveHistory(1);
                }
                return updateMoveHistory(0);
            }
        }

        // Phase 7: Sweep Down Again
        if (current_phase == 7) {
            if (current_state == PHASE_7_RIGHT) {
                // Move right until hitting a wall
                if (!wall_right && can_move_right) {
                    if (direction != '>') return updateMoveHistory(4);
                    return updateMoveHistory(4);
                }
                // Hit a wall, transition to moving left
                current_state = PHASE_7_LEFT;
                if (!wall_left && can_move_left) {
                    if (direction != '<') return updateMoveHistory(2);
                    return updateMoveHistory(2);
                }
                return updateMoveHistory(0);
            }

            if (current_state == PHASE_7_LEFT) {
                // Move left until hitting a wall
                if (!wall_left && can_move_left) {
                    if (direction != '<') return updateMoveHistory(2);
                    return updateMoveHistory(2);
                }
                // Hit a wall, check if down and left are blocked
                if (wall_down && wall_left) {
                    // Transition to Phase 8
                    current_phase = 8;
                    current_state = PHASE_8_UP_RIGHT;
                    if (!wall_up && can_move_up) {
                        if (direction != '^') return updateMoveHistory(1);
                        return updateMoveHistory(1);
                    }
                    return updateMoveHistory(0);
                }
                // Move down once and return to PHASE_7_RIGHT
                current_state = PHASE_7_RIGHT;
                if (!wall_down && can_move_down) {
                    if (direction != 'v') return updateMoveHistory(3);
                    return updateMoveHistory(3);
                }
                return updateMoveHistory(0);
            }
        }

        // Phase 8: Zigzag Right
        if (current_phase == 8) {
            if (current_state == PHASE_8_UP_RIGHT) {
                // Move up exactly once
                current_state = PHASE_8_RIGHT;
                if (!wall_up && can_move_up) {
                    if (direction != '^') return updateMoveHistory(1);
                    return updateMoveHistory(1);
                }
                // If up is blocked, skip to moving right
                if (!wall_right && can_move_right) {
                    if (direction != '>') return updateMoveHistory(4);
                    return updateMoveHistory(4);
                }
                return updateMoveHistory(0);
            }

            if (current_state == PHASE_8_RIGHT) {
                // Move right until hitting a wall
                if (!wall_right && can_move_right) {
                    if (direction != '>') return updateMoveHistory(4);
                    return updateMoveHistory(4);
                }
                // Hit a wall, return to PHASE_8_UP_RIGHT
                current_state = PHASE_8_UP_RIGHT;
                if (!wall_up && can_move_up) {
                    if (direction != '^') return updateMoveHistory(1);
                    return updateMoveHistory(1);
                }
                return updateMoveHistory(0);
            }
        }
    }

    return updateMoveHistory(0);
}