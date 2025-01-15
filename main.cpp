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
        NONE,
        WHITE,
        BLACK,
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

class Chessboard {
public:
    const int files = 8;
    const int ranks = 8;
    const int total_squares = files * ranks;
    std::array<Piece::Type, 64> pieces_types = {};
    std::array<Piece::Color, 64> pieces_colors = {};

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
        for (int take_offset: {offset - 1, offset + 1}) {
            int target_square = square + take_offset;
            int file_difference = abs(get_file(square) - get_file(target_square));
            if (target_square >= 0 && target_square < total_squares) {
                if (file_difference == 1) {
                    if ((is_occupied(target_square) && are_different_colors(square, target_square)) || target_square == en_passant) {
                        i++;
                    }
                }
            }
        }

        // TODO: Promotion
        return i;
    }

    std::array<int, 8> knight_offsets= {-17, -15, -10, -6, 6, 10, 15, 17};
    int generate_knight_moves(int square) {
        int i = 0;

        for (int offset: knight_offsets) {
            int target_square = square + offset;
            int file_difference = abs(get_file(square) - get_file(target_square));

            if (target_square >= 0 && target_square < total_squares) {
                if (file_difference == 2 || file_difference == 1) {
                    if (is_empty(target_square) || are_different_colors(square, target_square)) {
                        i++;
                    }
                }
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
        for (std::array<int, 4> offset_array: {bishop_directions_offsets, rook_directions_offsets}) {
            for (int offset: offset_array) {
                int target_square = square + offset;
                int file_difference = abs(get_file(square) - get_file(target_square));
                if (target_square >= 0 && target_square < total_squares && file_difference < 2) {
                    if (is_empty(target_square) || are_different_colors(square, target_square)) {
                        i++;
                    }
                }
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
    const std::string starting_position_fen = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1";
    board.load_position(starting_position_fen);
    board.generate_all_moves();
    //board.print();
    return 0;
}
