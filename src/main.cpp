#include <iostream>

#include <raylib.h>

#include "Game.h"
#include "Input.h"

#include "bitboard/bitboard.h"


int main( int, char** )
{
    InitWindow( window::Width, window::Height, window::Title );

    SetWindowMaxSize( window::Width, window::Height );
    SetWindowMinSize( window::Width, window::Height );

    SetTargetFPS( 60 );
    
    auto pieces = LoadTexture( "../img/pieces.png" );

    Game game;

    game.state = State::UserMakeMove;

    while ( !WindowShouldClose() )
    {
        processInput( game );

        auto frameTime = GetFrameTime();

        if ( game.state == State::Quit )
	        break;

        game.update( frameTime );

        BeginDrawing();

        ClearBackground( SKYBLUE );

        game.render( pieces );

        EndDrawing();
    }

    UnloadTexture( pieces );
    CloseWindow();

    return 0;
}