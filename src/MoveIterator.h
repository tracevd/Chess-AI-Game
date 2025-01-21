#pragma once

#include "Vec2.h"
#include "Piece.h"

struct Move;

namespace move
{
    /*
        A class used to iterate over all possible moves for a given piece type.
        Does not handle pawns
    */
    class Iterator
    {
    public:
        Iterator( piece::Type type );

        static Iterator pawnMoves( bool isBlack );

        static Iterator pawnAttacks( bool isBlack );

        static Iterator pawnStartingMoves( bool isBlack );

        bool hasNext() const;

        Vec2 next();

        // Skips the current direction and advances to the next direction in the move set
        void skipToNextDirection();

        constexpr piece::Type type() const { return m_type; }

    private:
        Iterator( const Move* begin, const Move* end, piece::Type t ):
            m_cur( begin ),
            m_end( end ),
            m_type( t ) {}

    private:
        const Move* m_cur;
        const Move* m_end;
        int16_t m_distance = 0;
        piece::Type m_type = piece::Type::Null;
    };
}