#pragma once

#include "Game.h"

void processInput( Game& game )
{
    if ( game.state != State::UserMakeMove )
    {
        return;
    }

    auto const clicked = IsMouseButtonPressed( MOUSE_BUTTON_LEFT );

    if ( !clicked )
        return;

    auto const mousePos = GetMousePosition();

    auto const coords = board::windowCoordsToBoardCoords( mousePos.x, mousePos.y );

    if ( game.hasSelectedPiece() )
    {
        if ( !game.tryMovePiece( coords ) )
        {
            return;
        }

        if ( game.aiHasLost() )
        {
            game.state = State::Quit;
        }
        else
        {
            game.startAiMove();
        }
    }
    else if ( !game.trySelectPiece( coords ) )
    {
        std::cout << "Piece at " << coords << " can't be selected\n";
    }
}