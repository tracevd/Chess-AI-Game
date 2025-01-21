#pragma once

#include <bit>
#include <array>
#include <mutex>
#include <thread>

#include "window.h"
#include "board.h"

#include "Move.h"
#include "AI.h"
#include "Highlight.h"
#include "DangerLevel.h"

enum class State : uint8_t
{
    Quit,
    AiChooseMove,
    AiMakeMove,
    UserMakeMove,
    MainMenu
};

struct AiData
{
    std::mutex mutex;
    std::thread thread;
    ai::Result result;
    uint64_t whenToMakeMove;
    Highlight originalPosition = { Highlight::NoPieceSelected, color::Blue };
    Highlight newPosition = { Highlight::NoPieceSelected, color::Green };
    ai::Difficulty difficulty = ai::Difficulty::Hard;
};

struct Game
{
    std::array< Piece, 64 > board = board::init::DefaultBoard;
    AiData ai;
    Highlight selectedPiece = { Highlight::NoPieceSelected, color::Blue };
    State state = State::MainMenu;
    danger::Level kingDangerLevel = danger::Level::None;

    bool hasSelectedPiece() const
    {
        return selectedPiece.index != Highlight::NoPieceSelected;
    }

    bool trySelectPiece( Vec2 coords )
    {
        auto const index = board::coordsToIndex( coords );

        auto const pieceAtIndex = board[ index ];

        if ( pieceAtIndex.isNull() || pieceAtIndex.isAi() )
        {
            return false;
        }

        selectedPiece.index = index;

        return true;
    }

    bool tryMovePiece( Vec2 coords )
    {
        auto const index = board::coordsToIndex( coords );

        // de-select if they clicked the same piece
        if ( index == selectedPiece.index )
        {
            selectedPiece.index = Highlight::NoPieceSelected;
            return false;
        }

        if ( !isValidMove( board::indexToCoords( selectedPiece.index ), coords ) )
        {
            return false;
        }

        board::movePiece( board.data(), selectedPiece.index, index );
        selectedPiece.index = Highlight::NoPieceSelected;

        return true;
    }

    bool aiHasLost()
    {
        for ( int i = 0; i < 64; ++i )
        {
            auto const piece = board[ i ];

            if ( piece.isAi() && piece.type == piece::Type::King )
                return false;
        }

        return true;
    }

    bool userHasLost()
    {
        for ( int i = 0; i < 64; ++i )
        {
            auto const piece = board[ i ];

            if ( piece.isUser() && piece.type == piece::Type::King )
                return false;
        }

        return true;
    }

    void startAiMove()
    {
        state = State::AiChooseMove;
        ai.result.ready = false;
        ai.thread = std::thread([b = board.data(), m = &ai.mutex, r = &ai.result, d = ai.difficulty](){
            ai::makeMove( b, *m, *r, d );
        });
    }

    void update( uint64_t sdlTicks )
    {
        if ( state == State::AiChooseMove )
        {
            auto const lock = std::scoped_lock( ai.mutex );

            if ( ai.result.ready )
            {
                state = State::AiMakeMove;

                ai.whenToMakeMove = sdlTicks + 1500;

                ai.originalPosition.index = board::coordsToIndex( ai.result.move.from );
                ai.newPosition.index = board::coordsToIndex( ai.result.move.dst );

                if ( ai.thread.joinable() )
                    ai.thread.join();
            }
        }
        else if ( state == State::AiMakeMove )
        {
            if ( ai.whenToMakeMove <= sdlTicks )
            {
                board::movePiece( board.data(), ai.result.move.from, ai.result.move.dst );
                ai.result.ready = false;
                ai.originalPosition.index = Highlight::NoPieceSelected;
                ai.newPosition.index = Highlight::NoPieceSelected;
                state = State::UserMakeMove;
            }
        }

        kingDangerLevel = getKingDangerLevel();
    }

    void render( SDL_Renderer* r, SDL_Texture* t ) const
    {
        for ( Coord j = 0; j < 8; ++j )
        {
            for ( Coord i = 0; i < 8; ++i )
            {
                auto const coords = Vec2{ i, j };
                auto const index = board::coordsToIndex( coords );
                auto const piece = board[ index ];

                renderCheckerBoardAt( r, coords, index, piece );
                renderGamePieceAt( r, t, coords, piece );
            }
        }
    }
private:
    static void setRenderDrawColor( SDL_Renderer* r, RGB color )
    {
        SDL_SetRenderDrawColor( r, color.r, color.g, color.b, 255 );
    }

    void renderCheckerBoardAt( SDL_Renderer* r, Vec2 coords, int16_t index, Piece piece ) const
    {
        auto const isBlack = (coords.j & 1) ? !(coords.i & 1) : !!(coords.i & 1);

        RGB color;

        // make king red if in danger
        if ( piece.isUser() && piece.type == piece::Type::King && kingDangerLevel != danger::Level::None )
        {
            if ( kingDangerLevel == danger::Level::MustMove )
                color = color::Red;
            else if ( kingDangerLevel == danger::Level::MustMoveAndCant )
                color = color::DarkRed;
        }
        else if ( isBlack )
        {
            color = color::Black;
        }
        else
        {
            color = color::White;
        }
        
        // highlight selected piece
        if ( index == selectedPiece.index )
        {
            color = selectedPiece.color;
        }
        else if ( index == ai.originalPosition.index )
        {
            color = ai.originalPosition.color;
        }
        else if ( index == ai.newPosition.index )
        {
            color = ai.newPosition.color;
        }

        setRenderDrawColor( r, color );

        auto const rect = window::getBoxPosition( coords );

        SDL_RenderFillRect( r, &rect );
    }

    void renderGamePieceAt( SDL_Renderer* r, SDL_Texture* t, Vec2 coords, Piece piece ) const
    {
        auto const imgCrop = piece::getImageCrop( piece );

        auto const imgPos = window::getBoxPosition( coords );

        SDL_RenderTexture( r, t, &imgCrop, &imgPos );
    }

    bool isValidMove( Vec2 from, Vec2 dst )
    {
        auto const fromIdx = board::coordsToIndex( from );
        auto const dstIdx = board::coordsToIndex( dst );
        auto const pieceToBeMoved = board[ fromIdx ];
        auto const pieceToBeCaptured = board[ dstIdx ];

        // can't take your own piece
        if ( pieceToBeCaptured.isUser() )
        {
            return false;
        }

        // handle pawn separately
        if ( pieceToBeMoved.type == piece::Type::Pawn )
        {
            auto const isMovingUpTwo = from + move::Up * 2 == dst;

            if ( board::isPawnStartingPosition( fromIdx ) && isMovingUpTwo )
            {
                return true;
            }

            auto const dstIsEmpty = pieceToBeCaptured.type == piece::Type::Null;
            auto const isMovingUpOne = from + move::Up == dst;

            if ( dstIsEmpty && isMovingUpOne )
                return true;

            auto const isAttackingDiagonally =
                pieceToBeCaptured.isAi()
                && ( from + move::UpAndLeft == dst || from + move::UpAndRight == dst );
            
            if ( isAttackingDiagonally )
                return true;

            return false;
        }

        bool foundDst = false;
        bool isValidDst = false;

        auto it = move::Iterator( pieceToBeMoved.type );

        while ( it.hasNext() && !foundDst )
        {
            auto const possibleDst = from + it.next();

            if ( board::isOutOfBounds( possibleDst ) )
            {
                it.skipToNextDirection();
                continue;
            }

            auto const possiblePiece = board[ board::coordsToIndex( possibleDst ) ];

            if ( possibleDst == dst )
            {
                foundDst = true;
                isValidDst = !possiblePiece.isUser();
                break;
            }

            // stop searching direction if we run into a piece
            if ( !possiblePiece.isNull() )
            {
                it.skipToNextDirection();
                continue;
            }
        }

        if ( !foundDst )
            return false;

        if ( pieceToBeMoved.type == piece::Type::King )
        {
            // make move
            auto const [b4from, b4dst, _] = board::movePiece( board.data(), fromIdx, dstIdx );

            auto const dangerLevel = getKingDangerLevel();

            // undo move
            board[ fromIdx ] = b4from;
            board[ dstIdx ]  = b4dst;

            if ( dangerLevel != danger::Level::None )
                return false;            
        }

        return isValidDst;
    }

    danger::Level getKingDangerLevel()
    {
        for ( int16_t i = 0; i < 64; ++i )
        {
            auto const piece = board[ i ];
            if ( piece.isNull() || piece.isAi() || piece.type != piece::Type::King )
                continue;

            return danger::getDangerLevel( board.data(), piece, board::indexToCoords( i ) );
        }

        return danger::Level::None;
    }
};
