#pragma once

#include <raylib.h>

#include "Vec2.h"

namespace window
{
    constexpr const char* Title = "Chess";
    constexpr int Width = 600;
    constexpr int Height = 600;

    namespace box
    {
        constexpr Rectangle PositionRect = {
            .x = 0,
            .y = 0,
            .width = window::Width / 8,
            .height = window::Height / 8
        };
    }

    constexpr Rectangle getBoxPosition( Vec2 coords )
    {
        auto boxPos = box::PositionRect;

        boxPos.x = coords.i * window::Width / 8;
        boxPos.y = coords.j * window::Height / 8;

        return boxPos;
    }
}