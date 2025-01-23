#pragma once

#include <cstdint>

#include "Piece.h"
#include "Vec2.h"
#include "window.h"

namespace board
{
    constexpr Vec2 indexToCoords( int16_t index )
    {
        return { Coord( index % 8 ), Coord( index / 8 ) };
    }

    constexpr int16_t coordsToIndex( Vec2 coords )
    {
        return coords.j * 8 + coords.i;
    }

    constexpr Vec2 windowCoordsToBoardCoords( int x, int y )
    {
        return { Coord( x / ( window::Width / 8 ) ), Coord( y / ( window::Height / 8 ) ) };
    }

    // Moves the piece at "from" to "dst". Returns the piece originally at "from", the piece that originally at "dst", and whether there was a promotion to queen
    constexpr std::tuple< Piece, Piece, bool > movePiece( Piece* board, int16_t fromIdx, int16_t dstIdx )
    {
        auto const pieceAtDst = board[ dstIdx ];
        auto const pieceAtFrom = board[ fromIdx ];

        board[ dstIdx ] = board[ fromIdx ];
        board[ fromIdx ] = Piece{};

        auto const atTopOrBottom = ( 0 <= dstIdx && dstIdx < 8 ) || ( 56 <= dstIdx && dstIdx < 64 );

        auto const promoteToQueen = atTopOrBottom && board[ dstIdx ].type == piece::Type::Pawn;

        if ( promoteToQueen )
        {
            board[ dstIdx ].type = piece::Type::Queen;
        }

        return { pieceAtFrom, pieceAtDst, promoteToQueen };
    }

    constexpr bool hasPiece( Piece* board, Piece piece )
    {
        for ( int i = 0; i < 64; ++i )
        {
            if ( board[ i ] == piece )
            {
                return true;
            }
        }

        return false;
    }

    // Moves the piece at "from" to "dst". Returns the piece originally at "from", the piece that originally at "dst", and whether there was a promotion to queen
    constexpr std::tuple< Piece, Piece, bool > movePiece( Piece* board, Vec2 from, Vec2 dst )
    {
        auto const fromIndex = coordsToIndex( from );
        auto const dstIndex  = coordsToIndex( dst );

        return movePiece( board, fromIndex, dstIndex );
    }

    constexpr bool isPawnStartingPosition( int16_t index )
    {
        return ( 8 <= index && index < 16 ) || ( 48 <= index && index < 56 );
    }

    constexpr bool isPawnStartingPosition( Vec2 coords )
    {
        return isPawnStartingPosition( coordsToIndex( coords ) );
    }

    constexpr bool isOutOfBounds( Vec2 coords )
    {
        return coords.i < 0 || coords.j < 0 || coords.i > 7 || coords.j > 7;
    }

    namespace init
    {
        namespace details
        {
            consteval std::array< Piece, 64 > makeDefaultBoard()
            {
                std::array< Piece, 64 > board;

                auto Black = []( piece::Type t ) -> Piece
                {
                    return { true, t };
                };

                auto White = []( piece::Type t ) -> Piece
                {
                    return { false, t };
                };

                using enum piece::Type;

                board[ 0 ] = White( Rook );
                board[ 1 ] = White( Knight );
                board[ 2 ] = White( Bishop );
                board[ 3 ] = White( King );
                board[ 4 ] = White( Queen );
                board[ 5 ] = White( Bishop );
                board[ 6 ] = White( Knight );
                board[ 7 ] = White( Rook );

                for ( int i = 8; i < 16; ++i )
                {
                    board[ i ] = White( Pawn );
                }

                board[ 56 ] = Black( Rook );
                board[ 57 ] = Black( Knight );
                board[ 58 ] = Black( Bishop );
                board[ 59 ] = Black( Queen );
                board[ 60 ] = Black( King );
                board[ 61 ] = Black( Bishop );
                board[ 62 ] = Black( Knight );
                board[ 63 ] = Black( Rook );

                for ( int i = 48; i < 56; ++i )
                {
                    board[ i ] = Black( Pawn );
                }

                return board;
            }
        }

        constexpr std::array< Piece, 64 > DefaultBoard = details::makeDefaultBoard();
    }
}