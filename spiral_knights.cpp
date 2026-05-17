#include <iostream>
#include <array>
#include <vector>
#include <cmath>
#include <utility>
#include <algorithm>
#include <cstdint>
#include <cstring>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;

// A knight moves in an L shape: 2 in one direction and 1 in the other.
const vector<pair<int64_t, int64_t>> knight_moves_deltas = {
    {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
    {1, 2}, {1, -2}, {-1, 2}, {-1, -2}
};

// This is a list of positions that are already occupied by knights.
// We will use this to check if a knight can be placed at a given
// position without being attacked by another knight of a different color.
//
// Position occupied by a knight have that negative knight color minus one.
// Position attacked by a knight the negative of the power of two of that knight color.
// (Because a position can be attacked by knights of multiple colors.)
// Unoccupied positions are outside the size of the list or zero.
vector<int8_t> occupied_positions;

int8_t knight_color_to_attack_bit(int64_t knight_color) {
   return int8_t(1) << knight_color;
}

int64_t find_lowest_unoccupied_position_for_knight(size_t lowest_unoccupied, int8_t knight_color) {
    const int8_t other_attack_bits = ~knight_color_to_attack_bit(knight_color);
    for (int64_t i = lowest_unoccupied; i < static_cast<int64_t>(occupied_positions.size()); i++) {
        if (occupied_positions[i] >= 0 && (occupied_positions[i] & other_attack_bits) == 0) {
            return i;
        }
    }
    return static_cast<int64_t>(occupied_positions.size());
}

int64_t find_lowest_unoccupied_position(size_t lowest_unoccupied, int8_t all_attacks) {
    for (int64_t i = lowest_unoccupied; i < static_cast<int64_t>(occupied_positions.size()); i++) {
        if (occupied_positions[i] >= 0 && occupied_positions[i] != all_attacks) {
            return i;
        }
    }
    return static_cast<int64_t>(occupied_positions.size());
}



pair<int64_t, int64_t> spiral_number_to_position(int64_t n) {
    /*
    Given a number n, return the (x, y) position of that number in a spiral
    starting at (0, 0) with 1 and moving to the right, then up, left, down, and so on.
    */
    if (n == 0) {
        return {0, 0};
    }

    // The spiral is contained in a full square of odd side-length,
    // containing that length squared values. So we find the largest
    // odd square that is <= n
    int64_t sqrt_n = static_cast<int64_t>(sqrt(n));
    if (sqrt_n % 2 == 0) {
        sqrt_n -= 1;
    }
    int64_t inner_square_max_number = sqrt_n * sqrt_n - 1;

    // Find the offset of n from the maximum value in that square,
    // and determine the position based on that offset.
    int64_t base_pos = (sqrt_n + 1) / 2;
    int64_t outer_arm_length = sqrt_n + 1;
    int64_t offset = n - inner_square_max_number;

    if (offset <= outer_arm_length) {
        return {base_pos, base_pos - offset};
    } else if (offset <= 2 * outer_arm_length) {
        return {base_pos - (offset - outer_arm_length), -base_pos};
    } else if (offset <= 3 * outer_arm_length) {
        return {-base_pos, -base_pos + (offset - 2 * outer_arm_length)};
    } else {
        return {-base_pos + (offset - 3 * outer_arm_length), base_pos};
    }
}


int64_t position_to_spiral_number(pair<int64_t, int64_t> position) {
    /*
    Given a position (x, y), return the number at that position in the spiral
    defined in the previous function.
    */
    int64_t x = position.first;
    int64_t y = position.second;
    int64_t layer = max(abs(x), abs(y));
    int64_t side_length = 2 * layer;
    int64_t max_num_inner_layer = (side_length - 1) * (side_length - 1) - 1;

    // Find the offset of the position from the maximum value in that layer,
    // and determine the spiral number based on that offset.
    if (y == layer) {
        return max_num_inner_layer + (3 * side_length + x + layer);
    } else if (x == layer) {
        return max_num_inner_layer + (-y + layer);
    } else if (y == -layer) {
        return max_num_inner_layer + (side_length - x + layer);
    } else if (x == -layer) {
        return max_num_inner_layer + (2 * side_length + y + layer);
    }
    return 0;
}


vector<pair<int64_t, int64_t>> knight_attacks_for_position(pair<int64_t, int64_t> start) {
    vector<pair<int64_t, int64_t>> attacks;
    for (auto& delta : knight_moves_deltas) {
        attacks.push_back({start.first + delta.first, start.second + delta.second});
    }
    return attacks;
}


void fill_attacked_positions(int64_t knight_position_number, int64_t knight_color) {
    pair<int64_t, int64_t> knight_position = spiral_number_to_position(knight_position_number);
    const int8_t attack_bit = knight_color_to_attack_bit(knight_color);
    
    for (auto& delta : knight_moves_deltas) {
        pair<int64_t, int64_t> attacked_position = {knight_position.first + delta.first, knight_position.second + delta.second};
        int64_t attacked_position_number = position_to_spiral_number(attacked_position);
        if (attacked_position_number >= static_cast<int64_t>(occupied_positions.size())) {
            occupied_positions.resize(attacked_position_number + 1, 0);
        }
        if (occupied_positions[attacked_position_number] >= 0) {
            occupied_positions[attacked_position_number] |= attack_bit;
        }
    }
}


bool is_position_valid_for_color(int64_t position_number, int64_t knight_color) {
    if (position_number >= static_cast<int64_t>(occupied_positions.size())) {
        return true;
    }
    int8_t value = occupied_positions[position_number];
    if (value < 0) {
        return false;
    }
    return (value & ~knight_color_to_attack_bit(knight_color)) == 0;
}



int main(int argc, const char** argv) {

    int8_t number_of_knight_colors = int8_t(std::max(1, min(8, argc > 1 ? atoi(argv[1]) : 2)));
    int8_t current_color = 0;

    int8_t all_attacks = 0;
    for (int64_t i = 0; i < number_of_knight_colors; i++) {
        all_attacks |= knight_color_to_attack_bit(i);
    }

    int64_t square_size = int64_t(std::max(3, argc > 2 ? atoi(argv[2]) : 1000));
    int64_t square_area = square_size * square_size;
    // occupied_positions.resize(size_t(square_area * 1.1), 0);

    std::vector<int64_t> lowest_unoccupied_for_colors(number_of_knight_colors, 0);
    // int64_t lowest_unoccupied = 0;

    while (true) {
        int64_t possible_number = find_lowest_unoccupied_position_for_knight(lowest_unoccupied_for_colors[current_color], current_color);
        // lowest_unoccupied = find_lowest_unoccupied_position(lowest_unoccupied, all_attacks);
        // int64_t possible_number = lowest_unoccupied;
        if (possible_number > square_area)
           break;
        while (true) {
            if (possible_number >= static_cast<int64_t>(occupied_positions.size())) {
                occupied_positions.resize(possible_number + 1, 0);
                occupied_positions[possible_number] = -current_color - 1;
                fill_attacked_positions(possible_number, current_color);
                lowest_unoccupied_for_colors[current_color] = possible_number + 1;
                break;
            } else if (is_position_valid_for_color(possible_number, current_color)) {
                occupied_positions[possible_number] = -current_color - 1;
                fill_attacked_positions(possible_number, current_color);
                lowest_unoccupied_for_colors[current_color] = possible_number + 1;
                break;
            }
            possible_number += 1;
        }
        current_color = (current_color + 1) % number_of_knight_colors;
    }

    // Note: we add one because if the square size is even, the center of the square is between four positions,
    // and we want to include the position at the top right of the center.
    // vector<vector<char>> board(square_size + 1, vector<char>(square_size + 1, ' '));
    
    // for (int64_t i = 0; i < square_area; i++) {
    //     pair<int64_t, int64_t> pos = spiral_number_to_position(i);
    //     int64_t x = pos.first + square_size / 2;
    //     int64_t y = pos.second + square_size / 2;
    //     if (occupied_positions[i] < 0) {
    //         board[y][x] = '0' + static_cast<char>(-occupied_positions[i]);
    //     } else if (occupied_positions[i] > 0) {
    //         board[y][x] = '.';
    //     } else {
    //         board[y][x] = ' ';
    //     }
    // }

    // for (auto& row : board) {
    //     for (auto& cell : row) {
    //         cout << cell;
    //     }
    //     cout << '\n';
    // }

    string png_filename = argc > 3 ? argv[3] : "knights.png";
    int img_width = static_cast<int>(square_size + 3);
    int img_height = static_cast<int>(square_size + 3);
    int x_offset = 1;
    int y_offset = 1;

    vector<uint8_t> image(static_cast<size_t>(img_width) * static_cast<size_t>(img_height) * 4, 255);
    const vector<array<uint8_t, 3>> palette = {
        array<uint8_t, 3>{220,   0,   0},
        array<uint8_t, 3>{  0, 110, 220},
        array<uint8_t, 3>{200, 200,   0},
        array<uint8_t, 3>{  0, 200,  80},
        array<uint8_t, 3>{210, 140,  80},
        array<uint8_t, 3>{  0, 220, 220},
        array<uint8_t, 3>{128, 160, 130},
        array<uint8_t, 3>{128,   0, 220}
    };

    for (int64_t i = 0; i < square_area; i++) {
        pair<int64_t, int64_t> pos = spiral_number_to_position(i);
        int64_t x = pos.first + square_size / 2 + x_offset;
        int64_t y = pos.second + square_size / 2 + y_offset;
        if (x < 0 || x >= img_width || y < 0 || y >= img_height) {
            continue;
        }

        size_t pixel_index = (static_cast<size_t>(y) * static_cast<size_t>(img_width) + static_cast<size_t>(x)) * 4;
        if (occupied_positions[i] < 0) {
            int color_index = static_cast<int>(-occupied_positions[i] - 1);
            const auto& color = palette[color_index % static_cast<int>(palette.size())];
            image[pixel_index + 0] = color[0];
            image[pixel_index + 1] = color[1];
            image[pixel_index + 2] = color[2];
            image[pixel_index + 3] = 255;
        } else {
            image[pixel_index + 0] = 0;
            image[pixel_index + 1] = 0;
            image[pixel_index + 2] = 0;
            image[pixel_index + 3] = 255;
        }
    }

    if (!stbi_write_png(png_filename.c_str(), img_width, img_height, 4, image.data(), img_width * 4)) {
        cerr << "Failed to write PNG file: " << png_filename << '\n';
        return 1;
    }

    cout << "Wrote PNG: " << png_filename << '\n';
    return 0;
}
