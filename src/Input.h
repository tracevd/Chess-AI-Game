#pragma once

#include "Game.h"

void processInput( Game& game )
{
    SDL_Event e;
    while ( SDL_PollEvent( &e ) )
    {
        if ( e.type == SDL_EVENT_QUIT )
        {
            game.state = State::Quit;
            return;
        }

        if ( game.state != State::UserMakeMove )
            return;

        if ( e.type == SDL_EVENT_MOUSE_BUTTON_DOWN )
        {
            auto const coords = board::windowCoordsToBoardCoords( e.button.x, e.button.y );

            if ( game.hasSelectedPiece() )
            {
                if ( !game.tryMovePiece( coords ) )
                {
                    continue;
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
    }
}