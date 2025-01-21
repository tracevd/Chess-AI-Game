#pragma once

#include "Piece.h"
#include "Move.h"
#include "MoveIterator.h"

namespace danger
{
    namespace details
    {
        inline bool mustMove( const Piece* board, Piece piece, Vec2 pieceCoords )
        {
            // see if any pieces are one move away from the piece
            auto const isOneMoveAway = []( const Piece* board, move::Iterator& move_it, Vec2 pieceCoords, bool pieceIsAi )
            {
                auto const newCoords = pieceCoords + move_it.next();

                if ( board::isOutOfBounds( newCoords ) )
                {
                    move_it.skipToNextDirection();
                    return false;
                }

                auto const moveIndex = board::coordsToIndex( newCoords );
                auto const pieceAtMove = board[ moveIndex ];

                if ( pieceAtMove.isNull() )
                {
                    return false;
                }

                // stop searching direction after first hit
                move_it.skipToNextDirection();

                const bool isOpponent = pieceAtMove.isAi() ^ pieceIsAi;

                if ( isOpponent && pieceAtMove.type == move_it.type() )
                {
                    return true;
                }

                return false;
            };

            // handle pawn attacks
            {
                auto move_it = move::Iterator::pawnAttacks( !piece.isBlack );

                while ( move_it.hasNext() )
                {
                    if ( isOneMoveAway( board, move_it, pieceCoords, piece.isAi() ) )
                        return true;
                }
            }

            // handle other types
            for ( piece::Type t = piece::Type::King; t != piece::Type::Pawn; ++t )
            {
                auto move_it = move::Iterator( t );

                while ( move_it.hasNext() )
                {
                    if ( isOneMoveAway( board, move_it, pieceCoords, piece.isAi() ) )
                        return true;
                }
            }

            return false;
        }
    }

    enum class Level : uint8_t
    {
        None             = 0,
        MustMove         = 1,
        MustMoveAndCant  = 2,
    };

    inline Level getDangerLevel( Piece* board, Piece piece, Vec2 pieceCoords )
    {
        auto const mustMove = details::mustMove( board, piece, pieceCoords );

        if ( !mustMove )
            return Level::None;

        auto const pieceIndex = board::coordsToIndex( pieceCoords );

        auto const checkMove = []( Piece* board, bool& allMovesAreDangerous, Piece piece, Vec2 pieceCoords, int16_t pieceIndex, Vec2 newCoords )
        {
            if ( board::isOutOfBounds( newCoords ) )
            {
                return false;
            }

            auto const newIndex = board::coordsToIndex( newCoords );
            auto const newPiece = board[ newIndex ];
            
            auto const foundOwnPiece = newPiece.isBlack == piece.isBlack && !newPiece.isNull();
            if ( foundOwnPiece )
            {
                return false;
            }

            // make move
            auto const [b4from, b4dst, _] = board::movePiece( board, pieceCoords, newCoords );

            allMovesAreDangerous = allMovesAreDangerous && details::mustMove( board, piece, newCoords );

            // undo move
            board[ pieceIndex ] = b4from;
            board[ newIndex ]  = b4dst;

            return true;
        };

        bool allMovesAreDangerous = true;

        if ( piece.type == piece::Type::Pawn )
        {
            // loop through regular moves
            auto it = board::isPawnStartingPosition( pieceIndex )
                ? move::Iterator::pawnStartingMoves( piece.isBlack )
                : move::Iterator::pawnMoves( piece.isBlack );

            while ( it.hasNext() && allMovesAreDangerous )
            {
                if ( !checkMove( board, allMovesAreDangerous, piece, pieceCoords, pieceIndex, pieceCoords + it.next() ) )
                {
                    it.skipToNextDirection();
                }
            }

            it = move::Iterator::pawnAttacks( piece.isBlack );

            while ( it.hasNext() && allMovesAreDangerous )
            {
                auto newCoords = pieceCoords + it.next();

                if ( board::isOutOfBounds( newCoords ) )
                {
                    it.skipToNextDirection();
                    continue;
                }

                auto const newIndex = board::coordsToIndex( newCoords );
                auto const newPiece = board[ newIndex ];

                auto const newPieceIsSelf = newPiece.isBlack == piece.isBlack && !newPiece.isNull();

                if ( newPieceIsSelf )
                {
                    it.skipToNextDirection();
                    continue;
                }

                checkMove( board, allMovesAreDangerous, piece, pieceCoords, pieceIndex, newCoords );
            }

            if ( !allMovesAreDangerous )
                return Level::MustMove;

            return Level::MustMoveAndCant;
        }

        auto it = move::Iterator( piece.type );

        while ( it.hasNext() && allMovesAreDangerous )
        {
            if ( !checkMove( board, allMovesAreDangerous, piece, pieceCoords, pieceIndex, pieceCoords + it.next() ) )
            {
                it.skipToNextDirection();
            }
        }

        if ( !allMovesAreDangerous )
            return Level::MustMove;

        return Level::MustMoveAndCant;
    }
}

