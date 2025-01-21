#pragma once

namespace image
{
    constexpr int Height = 300;
    constexpr int Width = 900;

    namespace crop
    {
        constexpr int Height = image::Height / 2;
        constexpr int Width  = image::Width / 6;
    }
}