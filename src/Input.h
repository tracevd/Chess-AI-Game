#pragma once

#include "Game.h"

void processInput( Game& game )
{
    if ( game.state == State::AiChooseMove || game.state == State::AiMakeMove )
        return;
    
    auto const clicked = IsMouseButtonPressed( MOUSE_BUTTON_LEFT );

    if ( !clicked )
        return;

    auto const mousePos = GetMousePosition();

    auto const coords = board::windowCoordsToBoardCoords( mousePos.x, mousePos.y );

    if ( game.state == State::UserMakeMove )
    {
        if ( game.hasSelectedPiece() )
        {
            game.tryMovePiece( coords );
        }
        else if ( !game.trySelectPiece( coords ) )
        {
            std::cout << "Piece at " << coords << " can't be selected\n";
        }
    }
    else if ( game.state == State::MainMenu )
    {
        if ( CheckCollisionPointRec( mousePos, game.startGameButton.box ) )
        {
            game.state = State::UserMakeMove;
        }
        else if ( CheckCollisionPointRec( mousePos, game.quitButton.box ) )
        {
            game.state = State::Quit;
        }
    }
    else if ( game.state == State::AiWins || game.state == State::UserWins )
    {
        if ( CheckCollisionPointRec( mousePos, game.playAgainButton.box ) )
        {
            game.reset();
            game.state = State::UserMakeMove;
        }
        else if ( CheckCollisionPointRec( mousePos, game.quitButton.box ) )
        {
            game.state = State::Quit;
        }
    }
}