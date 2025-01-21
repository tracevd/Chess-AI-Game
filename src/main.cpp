#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "Game.h"
#include "Input.h"

#include "bitboard/bitboard.h"


int main( int, char** )
{
    if ( !SDL_Init( SDL_INIT_VIDEO ) )
    {
        std::cout << "Failed to initialize SDL\n";
        return -1;
    }

    SDL_Window* window {};
    SDL_Renderer* renderer {};

    if ( !SDL_CreateWindowAndRenderer( window::Title, window::Width, window::Height, 0, &window, &renderer ) )
    {
        std::cout << "Couldn't create window and renderer\n";
        return -1;
    }

    SDL_SetWindowResizable( window, false );
    
    SDL_Texture* pieces = nullptr;
    
    {
        SDL_Surface* img = IMG_Load("../img/pieces.png");

        if ( !img )
        {
            std::cout << "Could not load pieces png\n";
            return 37;
        }

        pieces = SDL_CreateTextureFromSurface(renderer, img);
        SDL_DestroySurface( img );

        if ( !pieces )
        {
            std::cout << "Could not create texture from surface\n";
            return 43;
        }
    }

    Game game;

    game.state = State::UserMakeMove;

    uint32_t lastTicks = 0;

    while ( true )
    {
        auto const ticks = SDL_GetTicks();
        auto const timeDelta = ticks - lastTicks;

        if ( timeDelta < 1000 / 60 )
            continue;

        lastTicks = ticks;

        processInput( game );

        auto inputTicks = SDL_GetTicks();

        if ( game.state == State::Quit )
	        break;

        game.update( inputTicks );

        auto updateTicks = SDL_GetTicks();

        SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
        SDL_RenderClear( renderer );

        game.render( renderer, pieces );

        SDL_RenderPresent( renderer );

        auto renderTicks = SDL_GetTicks();

        renderTicks -= updateTicks;
        updateTicks -= inputTicks;
        inputTicks -= ticks;

        // std::cout << "Input processing took: " << inputTicks << "ms\n";
        // std::cout << "Updating game took:    " << updateTicks << "ms\n";
        // std::cout << "Rendering game took:   " << renderTicks << "ms\n";
    }

    SDL_DestroyTexture( pieces );
    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( window );
    SDL_Quit();

    return 0;
}