#pragma once

#include <raylib.h>

#include "text.h"

struct Button
{
    Rectangle box;
    const char* msg;

    void render() const
    {
        DrawRectangle( box.x, box.y, box.width, box.height, DARKGRAY );

        auto const textPos = text::centerTextInRectangle( box, msg, 20 );
        
        DrawTextEx( GetFontDefault(), msg, textPos, 20, 2, WHITE );
    }
};