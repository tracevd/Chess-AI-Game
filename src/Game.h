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
#include "Button.h"
#include "Header.h"

enum class State : uint8_t
{
    Quit,
    AiChooseMove,
    AiMakeMove,
    UserMakeMove,
    MainMenu,
    UserWins,
    AiWins
};

struct AiData
{
    std::mutex mutex;
    std::thread thread;
    ai::Result result;
    float whenToMakeMove;
    Highlight originalPosition = { Highlight::NoPieceSelected, color::Blue };
    Highlight newPosition = { Highlight::NoPieceSelected, color::Green };
    ai::Difficulty difficulty = ai::Difficulty::Hard;
};

struct Game
{
    std::array< Piece, 64 > board = board::init::DefaultBoard;
    AiData ai;
    Button startGameButton;
    Button quitButton;
    Button playAgainButton;
    Header endOfGameHeader;
    Highlight selectedPiece = { Highlight::NoPieceSelected, color::Blue };
    State state = State::MainMenu;
    danger::Level kingDangerLevel = danger::Level::None;

    Game()
    {
        constexpr int ButtonWidth = 200;
        constexpr int ButtonHeight = 60;

        constexpr Rectangle centerRect = {
            .x = window::Width / 2 - ButtonWidth / 2,
            .y = window::Height / 2,
            .width = ButtonWidth,
            .height = ButtonHeight
        };

        startGameButton = { centerRect, "Start Game" };

        startGameButton.box.y -= 100;

        playAgainButton = { startGameButton.box, "Play Again" };

        playAgainButton.box.y += 50;

        quitButton = { centerRect, "Quit" };

        quitButton.box.y += 50;

        endOfGameHeader = { centerRect };

        endOfGameHeader.box.y -= 150;
    }

    void reset()
    {
        state = State::MainMenu;
        board = board::init::DefaultBoard;
        kingDangerLevel = danger::Level::None;
        selectedPiece = { Highlight::NoPieceSelected, color::Blue };
    }

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

        auto const [_, pieceCaptured, __] = board::movePiece( board.data(), selectedPiece.index, index );
        selectedPiece.index = Highlight::NoPieceSelected;

        auto const killedKing = pieceCaptured.isAi() && pieceCaptured.type == piece::Type::King;

        if ( killedKing )
        {
            state = State::UserWins;
        }
        else
        {
            startAiMove();
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

    void update( float frameTime )
    {
        if ( state == State::AiChooseMove )
        {
            auto const lock = std::scoped_lock( ai.mutex );

            if ( ai.result.ready )
            {
                state = State::AiMakeMove;

                ai.whenToMakeMove = 1.5;

                ai.originalPosition.index = board::coordsToIndex( ai.result.move.from );
                ai.newPosition.index = board::coordsToIndex( ai.result.move.dst );

                if ( ai.thread.joinable() )
                    ai.thread.join();
            }
        }
        else if ( state == State::AiMakeMove )
        {
            ai.whenToMakeMove -= frameTime;
            if ( ai.whenToMakeMove <= 0 )
            {
                auto const [_, pieceCaptured, __] = board::movePiece( board.data(), ai.result.move.from, ai.result.move.dst );
                ai.result.ready = false;
                ai.originalPosition.index = Highlight::NoPieceSelected;
                ai.newPosition.index = Highlight::NoPieceSelected;
                state = State::UserMakeMove;

                auto const killedKing = pieceCaptured.isUser() && pieceCaptured.type == piece::Type::King;

                if ( killedKing )
                {
                    state = State::AiWins;
                }
            }
        }

        kingDangerLevel = getKingDangerLevel();
    }

    void render( Texture t ) const
    {
        if ( state == State::MainMenu )
        {
            startGameButton.render();
            quitButton.render();
            return;
        }

        for ( Coord j = 0; j < 8; ++j )
        {
            for ( Coord i = 0; i < 8; ++i )
            {
                auto const coords = Vec2{ i, j };
                auto const index = board::coordsToIndex( coords );
                auto const piece = board[ index ];

                renderCheckerBoardAt( coords, index, piece );

                if ( !piece.isNull() )
                    renderGamePieceAt( t, coords, piece );
            }
        }

        if ( state == State::UserWins || state == State::AiWins )
        {
            auto const userWins = state == State::UserWins;

            auto color = WHITE;
            color.a = 156;
            DrawRectangle( 0, 0, window::Width, window::Height, color );

            playAgainButton.render();
            quitButton.render();
            endOfGameHeader.render( userWins ? "You win!" : "Ai wins!" );
        }
    }
private:
    void renderCheckerBoardAt( Vec2 coords, int16_t index, Piece piece ) const
    {
        auto const isBlack = (coords.j & 1) ? !(coords.i & 1) : !!(coords.i & 1);

        Color color;

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

        auto const rect = window::getBoxPosition( coords );

        DrawRectangle( rect.x, rect.y, rect.width, rect.height, color );
    }

    void renderGamePieceAt( Texture t, Vec2 coords, Piece piece ) const
    {
        auto const imgCrop = piece::getImageCrop( piece );

        auto const imgPos = window::getBoxPosition( coords );

        DrawTexturePro( t, imgCrop, imgPos, {0, 0}, 0, WHITE );
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
            auto const dstIsEmpty = pieceToBeCaptured.type == piece::Type::Null;

            auto const isMovingUpTwo = from + move::Up * 2 == dst;

            if ( dstIsEmpty && board::isPawnStartingPosition( fromIdx ) && isMovingUpTwo )
            {
                return true;
            }

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
