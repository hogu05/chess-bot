#ifndef MOVE_H
#define MOVE_H



class Move {
public:
    static const int START_SQUARE_MASK =    0b000000000111111;
    static const int TARGET_SQUARE_MASK =   0b000111111000000;
    static const int FLAG_MASK =            0b111000000000000;
    static const int PAWN_PROMOTION_MASK =  0b100000000000000;

    static const int NO_FLAG = 0b000;
    static const int EN_PASSANT_FLAG = 0b001;
    static const int CASTLE_FLAG = 0b010;
    static const int TWO_SPACE_PAWN_MOVE_FLAG = 0b011;
    static const int PROMOTE_TO_KNIGHT_FLAG = 0b100;
    static const int PROMOTE_TO_BISHOP_FLAG = 0b101;
    static const int PROMOTE_TO_ROOK_FLAG = 0b110;
    static const int PROMOTE_TO_QUEEN_FLAG = 0b111;

    static int create_move(int from_square, int to_square, int flag);
    static int create_move(int move, int flag);
    static int get_start_square(int move);
    static int get_target_square(int move);
    static int get_flag(int move);

    static bool is_pawn_promotion(int move);
    static int get_pawn_promotion_piece_type(int move);
};



#endif
