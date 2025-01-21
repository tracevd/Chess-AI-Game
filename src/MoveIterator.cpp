#include "MoveIterator.h"

#include "Move.h"

namespace move
{
    Iterator::Iterator( piece::Type type )
    {
        auto const [ b, e ] = getMovesForPieceType( type );

        m_cur = b;
        m_end = e;

        if ( m_cur )
        {
            m_type = type;
        }
        else
        {
            m_type = piece::Type::Null;
        }
    }

    Iterator Iterator::pawnMoves( bool black )
    {
        if ( black )
        {
            return Iterator{ move::BlackPawnMoves.begin(), move::BlackPawnMoves.end(), piece::Type::Pawn };
        }
        else
        {
            return Iterator{ move::WhitePawnMoves.begin(), move::WhitePawnMoves.end(), piece::Type::Pawn };
        }
    }

    Iterator Iterator::pawnAttacks( bool black )
    {
        if ( black )
        {
            return Iterator{ move::BlackPawnAttacks.begin(), move::BlackPawnAttacks.end(), piece::Type::Pawn };
        }
        else
        {
            return Iterator{ move::WhitePawnAttacks.begin(), move::WhitePawnAttacks.end(), piece::Type::Pawn };
        }
    }

    Iterator Iterator::pawnStartingMoves( bool black )
    {
        if ( black )
        {
            return Iterator{ move::BlackPawnStartingMoves.begin(), move::BlackPawnStartingMoves.end(), piece::Type::Pawn };
        }
        else
        {
            return Iterator{ move::WhitePawnStartingMoves.begin(), move::WhitePawnStartingMoves.end(), piece::Type::Pawn };
        }
    }

    bool Iterator::hasNext() const
    {
        if ( m_cur == m_end )
            return false;
        return m_distance < m_cur->maxDistance;
    }

    Vec2 Iterator::next()
    {
        ++m_distance;

        auto const vec = m_cur->direction * m_distance;

        if (m_distance >= m_cur->maxDistance )
        {
            ++m_cur;
            m_distance = 0;
        }

        return vec;
    }

    void Iterator::skipToNextDirection()
    {
        if ( m_cur != m_end && m_distance > 0 )
        {
            ++m_cur;
            m_distance = 0;
        }
    }
}