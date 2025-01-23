#pragma once

#include <raylib.h>

namespace text
{
    Vector2 centerTextInRectangle( Rectangle rect, const char* text, int textSize )
    {
        auto const defaultFont = GetFontDefault();
        auto const [ textWidth, textHeight ] = MeasureTextEx( defaultFont, text, textSize, static_cast< float >( textSize ) / 10.f );

        int textX = rect.x;
        textX += rect.width / 2;
        textX -= textWidth / 2;

        int textY = rect.y;
        textY += rect.height / 2;
        textY -= textHeight / 2;

        return { static_cast< float >( textX ), static_cast< float >( textY ) };
    }
}