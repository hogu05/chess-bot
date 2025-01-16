#include <iostream>
#include <array>
#include <memory>
#include <string>
#include <unordered_map>

namespace Piece {
    enum Type {
        EMPTY,
        PAWN,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
        KING,
        ANY
    };

    enum Color {
        WHITE,
        BLACK,
        NONE,
        BOTH
    };

    std::unordered_map<char, std::pair<Type, Color>> char_to_piece = {
        {'.', {EMPTY, NONE}},
        {'P', {PAWN, WHITE}},
        {'N', {KNIGHT, WHITE}},
        {'B', {BISHOP, WHITE}},
        {'R', {ROOK, WHITE}},
        {'Q', {QUEEN, WHITE}},
        {'K', {KING, WHITE}},
        {'p', {PAWN, BLACK}},
        {'n', {KNIGHT, BLACK}},
        {'b', {BISHOP, BLACK}},
        {'r', {ROOK, BLACK}},
        {'q', {QUEEN, BLACK}},
        {'k', {KING, BLACK}},
    };

    char get_char_from_piece(Type type, Color color) {
        if (type == EMPTY && color == NONE) return '.';
        if (type == PAWN && color == WHITE) return 'P';
        if (type == KNIGHT && color == WHITE) return 'N';
        if (type == BISHOP && color == WHITE) return 'B';
        if (type == ROOK && color == WHITE) return 'R';
        if (type == QUEEN && color == WHITE) return 'Q';
        if (type == KING && color == WHITE) return 'K';
        if (type == PAWN && color == BLACK) return 'p';
        if (type == KNIGHT && color == BLACK) return 'n';
        if (type == BISHOP && color == BLACK) return 'b';
        if (type == ROOK && color == BLACK) return 'r';
        if (type == QUEEN && color == BLACK) return 'q';
        if (type == KING && color == BLACK) return 'k';
        return '\0';
    }

}

int bitscan_forward(uint64_t x) {
    return __builtin_ffsll(x) - 1;
}

int pop_bit(uint64_t& x) {
    int bit_position = bitscan_forward(x);
    x ^= 1ULL << bit_position;
    return bit_position;
}

enum Square {
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8
};

/*
  * 56 57 58 59 60 61 62 63
  * 48 49 50 51 52 53 54 55
  * 40 41 42 43 44 45 46 47
  * 32 33 34 35 36 37 38 39
  * 24 25 26 27 28 29 30 31
  * 16 17 18 19 20 21 22 23
  * 08 09 10 11 12 13 14 15
  * 00 01 02 03 04 05 06 07
*/

class Chessboard {
public:
    const int files = 8;
    const int ranks = 8;
    const int total_squares = files * ranks;
    std::array<Piece::Type, 64> pieces_types = {};
    std::array<Piece::Color, 64> pieces_colors = {};

    std::array<uint64_t, 64> KNIGHT_MOVES = {};
    void initialize_knight_bitboards() {
        for (int square = 0; square < total_squares; square++) {
            KNIGHT_MOVES[square] = get_knight_moves(square);
        }
    }

    uint64_t get_knight_moves(int square) {
        std::array<int, 8> knight_offsets= {-17, -15, -10, -6, 6, 10, 15, 17};
        uint64_t knight_moves = 0;
        for (int offset: knight_offsets) {
            int target_square = square + offset;
            int file_difference = abs(get_file(square) - get_file(target_square));

            if (target_square >= 0 && target_square < total_squares) {
                if (file_difference == 2 || file_difference == 1) {
                    knight_moves |= (1ULL << target_square);
                }
            }
        }
        return knight_moves;
    }

    std::array<uint64_t, 64> KING_MOVES = {};
    void initialize_king_bitboards() {
        for (int square = 0; square < total_squares; square++) {
            KING_MOVES[square] = get_king_moves(square);
        }
    }

    uint64_t get_king_moves(int square) {
        std::array<int, 8> king_offsets= {-9, -7, 7, 9, -8, -1, 1, 8};
        uint64_t king_moves = 0;
        for (int offset: king_offsets) {
            int target_square = square + offset;
            int file_difference = abs(get_file(square) - get_file(target_square));

            if (target_square >= 0 && target_square < total_squares && file_difference < 2) {
                king_moves |= (1ULL << target_square);
            }
        }
        return king_moves;
    }

    std::array<std::array<uint64_t, 64>, 2> PAWN_ATTACKS = {};
    void initialize_pawn_bitboards() {
        for (Piece::Color color: {Piece::Color::WHITE, Piece::Color::BLACK}) {
            for (int square = 0; square < total_squares; square++) {
                PAWN_ATTACKS[color][square] = get_pawn_attacks(square, color);
            }
        }
    }

    uint64_t get_pawn_attacks(int square, Piece::Color color) {
        int offset = pieces_colors[square] == Piece::Color::WHITE? 8: -8;
        uint64_t pawn_attacks = 0;
        for (int attack_offset: {offset - 1, offset + 1}) {
            int target_square = square + attack_offset;
            int file_difference = abs(get_file(square) - get_file(target_square));
            if (target_square >= 0 && target_square < total_squares && file_difference == 1) {
                pawn_attacks |= (1ULL << target_square);
            }
        }
        return pawn_attacks;
    }


    void load_position(std::string fen) {
        int square = 56;
        for (char current_char: fen) {
            if (current_char == '/') {
                square -= 16;
                continue;
            }
            if (std::isdigit(current_char)) {
                square += current_char - '0';
            } else {
                auto piece_info = Piece::char_to_piece[current_char];
                pieces_types[square] = piece_info.first;
                pieces_colors[square] = piece_info.second;
                square++;
            }
        }
    }

    void print() {
        int square = 56;
        while (square >= 0) {
            if (is_occupied(square)) {
                std::cout << Piece::get_char_from_piece(pieces_types[square], pieces_colors[square]);
            } else {
                std::cout << ".";
            }

            std::cout << " ";
            square++;
            if (square % 8 == 0) {
                std::cout << std::endl;
                square -= 16;
            }
        }
        std::cout << std::endl;
    }

    int get_file(int square) {
        return square % 8;
    }

    int get_rank(int square) {
        return square / 8;
    }

    bool is_occupied(int square) {
        return pieces_types[square] != Piece::Type::EMPTY;
    }

    bool is_empty(int square) {
        return pieces_types[square] == Piece::Type::EMPTY;
    }

    bool are_different_colors(int square_1, int square_2) {
        return pieces_colors[square_1] != pieces_colors[square_2];
    }

    Piece::Color on_move = Piece::Color::WHITE;
    int generate_moves(int square) {
        int moves = 0;
        switch (pieces_types[square]) {
            case Piece::Type::PAWN:
                moves = generate_pawn_moves(square);
                break;
            case Piece::Type::KNIGHT:
                moves = generate_knight_moves(square);
                break;
            case Piece::Type::BISHOP:
            case Piece::Type::ROOK:
            case Piece::Type::QUEEN:
                moves = generate_sliding_piece_moves(square);
                break;
            case Piece::Type::KING:
                moves = generate_king_moves(square);
                break;
            default:
                break;
        }
        return moves;
    }

    void generate_all_moves() {
        int total_moves = 0;
        for (int i = 0; i < total_squares; i++) {
            if (is_occupied(i) && pieces_colors[i] == on_move) {
                int moves = generate_moves(i);
                std::cout << i << ": " << moves << std::endl;
                total_moves += moves;
            }
        }
        std::cout << total_moves << std::endl;
    }

    int en_passant = -1;
    int generate_pawn_moves(int square) {
        int i = 0;
        int rank = get_rank(square);

        // Moving forward
        int offset = pieces_colors[square] == Piece::Color::WHITE? 8: -8;
        int target_square = square + offset;
        if (is_empty(target_square)) {
            i++;
            if ((pieces_colors[square] == Piece::Color::WHITE && rank == 1) || (pieces_colors[square] == Piece::Color::WHITE && rank == 6)) {
                if (is_empty(target_square + offset)) {
                    i++;
                }
            }
        }

        // Taking
        uint64_t pawn_attacks = PAWN_ATTACKS[pieces_colors[square]][square];
        while (pawn_attacks != 0) {
            int target_square = pop_bit(pawn_attacks);
            if ((is_occupied(target_square) && are_different_colors(square, target_square)) || target_square == en_passant) {
                i++;
            }
        }

        // TODO: Promotion
        return i;
    }

    int generate_knight_moves(int square) {
        int i = 0;
        uint64_t knight_moves = KNIGHT_MOVES[square];
        while (knight_moves != 0) {
            int target_square = pop_bit(knight_moves);
            if (is_empty(target_square) || are_different_colors(square, target_square)) {
                i++;
            }
        }
        return i;
    }

    std::array<int, 4> bishop_directions_offsets = {-9, -7, 7, 9};
    std::array<int, 4> rook_directions_offsets = {-8, -1, 1, 8};
    int generate_sliding_piece_moves(int square) {
        int i = 0;

        if (pieces_types[square] == Piece::Type::BISHOP || pieces_types[square] == Piece::Type::QUEEN) {
            for (int offset_direction: bishop_directions_offsets) {
                int target_square = square + offset_direction;
                int file_difference = abs(get_file(square) - get_file(target_square));
                while (target_square >= 0 && target_square < total_squares && file_difference < 2) {
                    if (is_occupied(target_square)) {
                        if (are_different_colors(square, target_square)) {
                            i++;
                        }
                        break;
                    }
                    i++;
                    file_difference = abs(get_file(target_square) - get_file(target_square + offset_direction));
                    target_square += offset_direction;
                }
            }
        }
        if (pieces_types[square] == Piece::Type::ROOK || pieces_types[square] == Piece::Type::QUEEN) {
            for (int offset_direction: rook_directions_offsets) {
                int target_square = square + offset_direction;
                int file_difference = abs(get_file(square) - get_file(target_square));
                while (target_square >= 0 && target_square < total_squares && file_difference < 2) {
                    if (is_occupied(target_square)) {
                        if (are_different_colors(square, target_square)) {
                            i++;
                        }
                        break;
                    }
                    i++;
                    file_difference = abs(get_file(target_square) - get_file(target_square + offset_direction));
                    target_square += offset_direction;
                }
            }
        }

        return i;
    }

    bool short_castle = true;
    bool long_castle = true;
    int generate_king_moves(int square) {
        int i = 0;
        uint64_t king_moves = KING_MOVES[square];
        while (king_moves != 0) {
            int target_square = pop_bit(king_moves);
            if (is_empty(target_square) || are_different_colors(square, target_square)) {
                i++;
            }
        }

        //Castles
        if (short_castle && is_empty(square + 1) && is_empty(square + 2)) {
            i++;
        }
        if (long_castle && is_empty(square - 1) && is_empty(square - 2) && is_empty(square - 3)) {
            i++;
        }
        return i;
    }
};

int main() {
    auto board = Chessboard();
    const std::string starting_position_fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R";
    board.load_position(starting_position_fen);
    board.initialize_knight_bitboards();
    board.initialize_king_bitboards();
    board.initialize_pawn_bitboards();
    board.generate_all_moves();
    //board.print();
    return 0;
}
