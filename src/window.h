#pragma once

#include <SDL3/SDL.h>

#include "Vec2.h"

namespace window
{
    constexpr const char* Title = "Chess";
    constexpr int Width = 600;
    constexpr int Height = 600;

    namespace box
    {
        constexpr SDL_FRect PositionRect = {
            .x = 0,
            .y = 0,
            .w = window::Width / 8,
            .h = window::Height / 8
        };
    }

    constexpr SDL_FRect getBoxPosition( Vec2 coords )
    {
        auto boxPos = box::PositionRect;

        boxPos.x = coords.i * window::Width / 8;
        boxPos.y = coords.j * window::Height / 8;

        return boxPos;
    }
}