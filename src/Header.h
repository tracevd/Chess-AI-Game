#pragma once

#include <raylib.h>

#include "text.h"

struct Header
{
    Rectangle box;

    void render( const char* msg ) const
    {
        DrawRectangle( box.x, box.y, box.width, box.height, DARKGRAY );

        auto const textPos = text::centerTextInRectangle( box, msg, 30 );
        
        DrawTextEx( GetFontDefault(), msg, textPos, 30, 3, WHITE );
    }
};