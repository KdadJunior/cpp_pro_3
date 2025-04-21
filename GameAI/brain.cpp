#include "brain.h"
#include <vector>
#include <utility>

Brain::Brain() : flag_picked(false), move_counter(0), current_stage(-1), 
                prev_move(0), prev_prev_move(0), right_blocked(false), down_blocked(false) {}

int Brain::updateMoveHistory(int move) {
    prev_prev_move = prev_move;
    prev_move = move;
    return move;
}

int Brain::getNextMove(GameState& gamestate) {
    int stage = gamestate.stage;
    
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

    // Compute wall positions using the 3x3 grid
    bool wall_up = local_grid[0][1] == '+';
    bool wall_down = local_grid[2][1] == '+';
    bool wall_left = local_grid[1][0] == '+';
    bool wall_right = local_grid[1][2] == '+';
    bool wall_up_right = local_grid[0][2] == '+';

    // Stage 0: Navigation using loops and conditions
    if (stage == 0) {
        const int START = 0;
        const int RIGHT_MOVE = 1;
        const int ZIGZAG = 2;
        static int current_state = START;

        if (current_state == START) {
            // Move up as long as there's no wall above
            while (!wall_up) {
                if (direction != '^') return updateMoveHistory(1);
                return updateMoveHistory(1);
            }
            current_state = RIGHT_MOVE;
        }

        if (current_state == RIGHT_MOVE) {
            while (!wall_right) {
                if (direction != '>') return updateMoveHistory(4);
                return updateMoveHistory(4);
            }
            right_blocked = true;
            current_state = ZIGZAG;
        }

        if (current_state == ZIGZAG) {
            // Priority: If last move was down, move right if possible
            if (prev_move == 3 && !wall_right) {
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
            // Try to move right and up (only if last move wasn't down)
            else if (!wall_right && !wall_up_right && prev_move != 3) {
                if (direction != '>') return updateMoveHistory(4);
                current_state = START;
                return updateMoveHistory(1);
            }
            // Try to move right
            else if (!wall_right) {
                if (direction != '>') return updateMoveHistory(4);
                return updateMoveHistory(4);
            }
            // Try to move down
            else if (!wall_down) {
                if (direction != 'v') return updateMoveHistory(3);
                return updateMoveHistory(3);
            }
            // If previously down_blocked but right is now open, move right
            else if (down_blocked && !wall_right) {
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
        if (local_grid[1][1] == '0') return updateMoveHistory(0);
        
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (local_grid[i][j] == '0') {
                    if (i < 1 && !wall_up) {
                        if (direction != '^') return updateMoveHistory(1);
                        return updateMoveHistory(1);
                    }
                    else if (i > 1 && !wall_down) {
                        if (direction != 'v') return updateMoveHistory(3);
                        return updateMoveHistory(3);
                    }
                    else if (j < 1 && !wall_left) {
                        if (direction != '<') return updateMoveHistory(2);
                        return updateMoveHistory(2);
                    }
                    else if (j > 1 && !wall_right) {
                        if (direction != '>') return updateMoveHistory(4);
                        return updateMoveHistory(4);
                    }
                }
            }
        }
        
        if (!wall_right) return updateMoveHistory(4);
        else if (!wall_down) return updateMoveHistory(3);
        else if (!wall_left) return updateMoveHistory(2);
        else return updateMoveHistory(1);
    }

    // Stage 2: Spiral navigation
    if (stage == 2) {
        static int spiral_state = 0;
        static int steps_in_dir = 0;
        static int steps_max = 1;
        static int dir_changes = 0;

        bool should_turn = false;
        if ((spiral_state == 0 && wall_right) ||
            (spiral_state == 1 && wall_down) ||
            (spiral_state == 2 && wall_left) ||
            (spiral_state == 3 && wall_up)) {
            should_turn = true;
        }
        else if (steps_in_dir >= steps_max) {
            should_turn = true;
        }

        if (should_turn) {
            spiral_state = (spiral_state + 1) % 4;
            steps_in_dir = 0;
            dir_changes++;
            if (dir_changes % 2 == 0) steps_max++;
        }

        int move_dir = spiral_state + 1;
        if (direction != ">v<^"[spiral_state]) return updateMoveHistory(move_dir);
        steps_in_dir++;
        return updateMoveHistory(move_dir);
    }

    // Stage 3: Flag capture
    if (stage == 3) {
        if (!flag_picked && local_grid[1][1] == 'A') {
            flag_picked = true;
            return updateMoveHistory(0);
        }
        else if (flag_picked && local_grid[1][1] == 'B') {
            flag_picked = false;
            return updateMoveHistory(0);
        }
        else if (!flag_picked && local_grid[1][1] == 'D') {
            return updateMoveHistory(0);
        }

        char target = flag_picked ? 'B' : 'A';
        if (!flag_picked && local_grid[1][1] == 'D') target = 'A';
        
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (local_grid[i][j] == target) {
                    if (i < 1 && !wall_up) {
                        if (direction != '^') return updateMoveHistory(1);
                        return updateMoveHistory(1);
                    }
                    else if (i > 1 && !wall_down) {
                        if (direction != 'v') return updateMoveHistory(3);
                        return updateMoveHistory(3);
                    }
                    else if (j < 1 && !wall_left) {
                        if (direction != '<') return updateMoveHistory(2);
                        return updateMoveHistory(2);
                    }
                    else if (j > 1 && !wall_right) {
                        if (direction != '>') return updateMoveHistory(4);
                        return updateMoveHistory(4);
                    }
                }
            }
        }
        
        if (!wall_right) return updateMoveHistory(4);
        else if (!wall_down) return updateMoveHistory(3);
        else if (!wall_left) return updateMoveHistory(2);
        else return updateMoveHistory(1);
    }

    return updateMoveHistory(0);
}