#pragma once

#include <mutex>
#include <algorithm>
#include <string>

#include <raylib.h>

#include "Vec2.h"

namespace ai
{
    uint64_t nodesGenerated = 0;

    enum Difficulty
    {
        Easy     = 1,
        Medium   = 2,
        Hard     = 3,
        VeryHard = 4,
        Crazy    = 5
    };

    struct Move
    {
        Vec2 from;
        Vec2 dst;
    };

    struct Result
    {
        Move move;
        bool ready = false;
    };

    namespace details
    {
        constexpr int MaxDepth = 5;

        constexpr std::array takePieceScores = {
            50000, // king
            45, // queen
            15, // bishop
            15, // knight
            25, // rook
            5, // pawn
            -2, // empty space
        };

        constexpr int Aggressiveness = 4;

        constexpr int PromotedToQueen = 40;

        constexpr getPieceScore( piece::Type t )
        {
            return takePieceScores[ static_cast< uint8_t >( t ) ];
        }

        struct MoveAndScore
        {
            Move move;
            int score;

            MoveAndScore( bool isMaximizing ):
                score( isMaximizing ? std::numeric_limits< int >::min() : std::numeric_limits< int >::max() ) {}

            MoveAndScore( Move move, int score ):
                move( move ),
                score( score ) {}
        };

        template< class Fn >
        void forAllLegalMoves( Piece* board, Piece pieceToMove, Vec2 coords, int depth, bool isMaximizing, Fn fn )
        {
            auto const pieceToMoveIndex = board::coordsToIndex( coords );

            if ( pieceToMove.type == piece::Type::Pawn )
            {
                auto const direction = isMaximizing ? move::Down : move::Up;

                // regular move
                {
                    auto const potentialMoveCoords = coords + direction;
                    auto const potentialMoveIdx = board::coordsToIndex( potentialMoveCoords );
                    auto const pieceToMoveTo = board[ potentialMoveIdx ];

                    if ( pieceToMoveTo.isNull() )
                    {
                        fn( board, pieceToMoveIndex, potentialMoveIdx, depth, isMaximizing );

                        if ( board::isPawnStartingPosition( pieceToMoveIndex ) )
                        {
                            auto const potentialDblMoveCoords = coords + direction + direction;
                            auto const potentialDblMoveIdx = board::coordsToIndex( potentialDblMoveCoords );
                            auto const pieceToDblMoveTo = board[ potentialDblMoveIdx ];

                            if ( pieceToDblMoveTo.isNull() )
                            {
                                fn( board, pieceToMoveIndex, potentialDblMoveIdx, depth, isMaximizing );
                            }
                        }
                    }
                }

                // diagonal attacks
                constexpr std::array secondaryDirections = {
                    move::Left,
                    move::Right
                };

                for ( uint32_t i = 0; i < secondaryDirections.size(); ++i )
                {
                    auto const potentialMoveCoords = coords + direction + secondaryDirections[ i ];

                    if ( board::isOutOfBounds( potentialMoveCoords ) )
                        continue;

                    auto const potentialMoveIdx = board::coordsToIndex( potentialMoveCoords );
                    auto const pieceToMoveTo = board[ potentialMoveIdx ];

                    auto const pieceToMoveToIsEnemy = !pieceToMoveTo.isNull() && isMaximizing ?
                                                            pieceToMoveTo.isUser()
                                                          : pieceToMoveTo.isAi();
                    if ( pieceToMoveToIsEnemy )
                    {
                        fn( board, pieceToMoveIndex, potentialMoveIdx, depth, isMaximizing );
                    }
                }

                return;
            }

            auto [begin, end] = move::getMovesForPieceType( pieceToMove.type );

            for ( ; begin != end; ++begin )
            {
                int16_t i = 0;
                while ( i < begin->maxDistance )
                {
                    ++i;
                    auto const possibleDst = coords + ( begin->direction * i );

                    if ( board::isOutOfBounds( possibleDst ) )
                        break;

                    auto const possibleIndex = board::coordsToIndex( possibleDst );
                    auto const possiblePiece = board[ possibleIndex ];

                    if ( possiblePiece.isNull() )
                    {
                        fn( board, pieceToMoveIndex, possibleIndex, depth, isMaximizing );
                        continue;
                    }

                    auto const foundOwnPiece = isMaximizing ? possiblePiece.isAi() : possiblePiece.isUser();

                    if ( foundOwnPiece )
                        break;

                    fn( board, pieceToMoveIndex, possibleIndex, depth, isMaximizing );
                    break;
                }
            }
        }

        template< class RetTy = int >
        RetTy miniMax( Piece* board, int depth, bool isMaximizing )
        {            
            MoveAndScore bestMove( isMaximizing );

            for ( int16_t j = 0; j < 8; ++j )
            {
                for ( int16_t i = 0; i < 8; ++i )
                {
                    auto const coords = Vec2{ i, j };
                    auto const index = board::coordsToIndex( coords );
                    auto const piece = board[ index ];

                    if ( piece.isNull() )
                        continue;

                    auto const pieceIsSelf = isMaximizing == piece.isAi();

                    if ( !pieceIsSelf )
                        continue;

                    forAllLegalMoves( board, piece, { i, j }, depth, isMaximizing,
                        [&bestMove]( Piece* board, int16_t from, int16_t dst, int depth, bool isMaximizing )
                        {
                            nodesGenerated += 1;

                            // make move
                            auto [fromB4, dstB4, promotedToQueen] = board::movePiece( board, from, dst );

                            auto score = getPieceScore( dstB4.type ) * ( isMaximizing ? 1 : -1 );

                            if ( promotedToQueen )
                            {
                                score += isMaximizing ? PromotedToQueen : -PromotedToQueen;
                            }

                            if ( score > 0 && isMaximizing )
                            {
                                score += Aggressiveness;
                            }

                            if ( dstB4.type != piece::Type::King && depth > 0 )
                            {
                                score += miniMax( board, depth - 1, !isMaximizing );
                            }

                            auto const isBetterScore = isMaximizing ? score > bestMove.score : score < bestMove.score;

                            if ( isBetterScore )
                            {
                                bestMove = { { board::indexToCoords( from ), board::indexToCoords( dst ) }, score };
                            }

                            // undo move
                            board[ from ] = fromB4;
                            board[ dst ]  = dstB4;
                        }
                    );
                }
            }

            if constexpr ( std::is_same_v< RetTy, Move > )
            {
                return bestMove.move;
            }
            else if constexpr ( std::is_same_v< RetTy, int > )
            {
                return bestMove.score;
            }
            else
            {
                static_assert( std::is_same_v< RetTy, int >, "Return type must be int or ai::Move!" );
            }
        }
    }

    bool printNumberWithCommas( uint64_t n )
    {
        if ( n == 0 )
            return false;

        auto const comma = printNumberWithCommas( n / 1000 );

        if ( comma )
        {
            printf(",%03lld", n % 1000 );
        }
        else
        {
            printf("%lld", n % 1000 );
        }

        return true;
    };

    void makeMove( Piece const* b, std::mutex& m, Result& res, Difficulty difficulty )
    {
        nodesGenerated = 0;
        auto const timeBefore = GetTime();

        std::array< Piece, 64 > b_arr;

        std::copy( b, b + 64, b_arr.begin() );

        auto const bestMove = details::miniMax< Move >( b_arr.data(), static_cast< int >( difficulty ), true );

        auto const timeAfter = GetTime();

        auto lock = std::scoped_lock< std::mutex >( m );

        res = Result{ bestMove, true };

        std::cout << "Took " << timeAfter - timeBefore << "s to generate ";
        printNumberWithCommas( nodesGenerated );
        std::cout << " nodes\n";
    }
}
