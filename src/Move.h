#pragma once

#include <vector>

#include "Piece.h"
#include "Vec2.h"

struct Move
{
    Vec2 direction;
    int16_t maxDistance = 1;
};

namespace move
{
    constexpr Vec2 Up    = {  0, -1 };
    constexpr Vec2 Down  = {  0,  1 };
    constexpr Vec2 Left  = { -1,  0 };
    constexpr Vec2 Right = {  1,  0 };
    
    constexpr Vec2 UpAndLeft    = Up + Left;
    constexpr Vec2 UpAndRight   = Up + Right;
    constexpr Vec2 DownAndLeft  = Down + Left;
    constexpr Vec2 DownAndRight = Down + Right;


    constexpr std::array KingMoves = {
        Move{ move::Up, 1 },
        Move{ move::Down, 1 },
        Move{ move::Left, 1 },
        Move{ move::Right, 1 },
        Move{ move::UpAndLeft, 1 },
        Move{ move::UpAndRight, 1 },
        Move{ move::DownAndLeft, 1 },
        Move{ move::DownAndRight, 1 }
    };

    constexpr std::array QueenMoves = {
        Move{ move::Up, 7 },
        Move{ move::Down, 7 },
        Move{ move::Left, 7 },
        Move{ move::Right, 7 },
        Move{ move::UpAndLeft, 7 },
        Move{ move::UpAndRight, 7 },
        Move{ move::DownAndLeft, 7 },
        Move{ move::DownAndRight, 7 }
    };

    constexpr std::array BishopMoves = {
        Move{ move::UpAndLeft, 7 },
        Move{ move::UpAndRight, 7 },
        Move{ move::DownAndLeft, 7 },
        Move{ move::DownAndRight, 7 }
    };

    constexpr std::array KnightMoves = {
        Move{ move::Up * 2    + move::Left, 1 },
        Move{ move::Up * 2    + move::Right, 1 },
        Move{ move::Down * 2  + move::Left, 1 },
        Move{ move::Down * 2  + move::Right, 1 },
        Move{ move::Left * 2  + move::Up, 1 },
        Move{ move::Left * 2  + move::Down, 1 },
        Move{ move::Right * 2 + move::Up, 1 },
        Move{ move::Right * 2 + move::Down, 1 },
    };

    constexpr std::array RookMoves = {
        Move{ move::Up, 7 },
        Move{ move::Down, 7 },
        Move{ move::Left, 7 },
        Move{ move::Right, 7 },
    };

    constexpr Vec2 WhitePawnDirection = move::Down;

    constexpr Vec2 BlackPawnDirection = move::Up;

    constexpr std::array WhitePawnAttacks = {
        Move{ WhitePawnDirection + move::Left, 1 },
        Move{ WhitePawnDirection + move::Right, 1 }
    };

    constexpr std::array BlackPawnAttacks = {
        Move{ BlackPawnDirection + move::Left, 1 },
        Move{ BlackPawnDirection + move::Right, 1 }
    };

    constexpr std::array WhitePawnMoves = {
        Move{ WhitePawnDirection, 1 }
    };

    constexpr std::array BlackPawnMoves = {
        Move{ BlackPawnDirection, 1 }
    };

    constexpr std::array WhitePawnStartingMoves = {
        Move{ WhitePawnDirection, 2 }
    };

    constexpr std::array BlackPawnStartingMoves = {
        Move{ BlackPawnDirection, 2 }
    };

    constexpr std::pair< const Move*, const Move* > getMovesForPieceType( piece::Type t )
    {
        using enum piece::Type;
        
        switch ( t )
        {
        case King:
            return { KingMoves.begin(), KingMoves.end() };
        case Queen:
            return { QueenMoves.begin(), QueenMoves.end() };
        case Bishop:
            return { BishopMoves.begin(), BishopMoves.end() };
        case Knight:
            return { KnightMoves.begin(), KnightMoves.end() };
        case Rook:
            return { RookMoves.begin(), RookMoves.end() };
        case Pawn:
        case Null:
            // handle pawns as separate cases
            return { nullptr, nullptr };
        }

        return { nullptr, nullptr };
    }

    constexpr bool canHopPieces( piece::Type t )
    {
        return t == piece::Type::Knight;
    }

    constexpr bool isOneSpaceMove( Vec2 from, Vec2 dst )
    {
        return from + Up == dst
            || from + Down == dst
            || from + Left == dst
            || from + Right == dst
            || from + UpAndLeft == dst
            || from + UpAndRight == dst
            || from + DownAndLeft == dst
            || from + DownAndRight == dst;
    }

    constexpr bool isMultiSpaceMove( Vec2 from, Vec2 dst )
    {
        return !isOneSpaceMove( from, dst );
    }
}


