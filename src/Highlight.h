#pragma once

#include <cstdint>

#include <raylib.h>

struct RGB
{
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
};

namespace color
{
    constexpr Color Red   = { 180,  20,  60, 255 };
    constexpr Color Green = {  20, 180,  60, 255 };
    constexpr Color Blue  = {  20,  60, 180, 255 };
    constexpr Color Black = {  20,  20,  20, 255 };
    constexpr Color White = { 235, 235, 235, 255 };

    constexpr Color DarkRed = { 90, 10, 30, 255 };
}

struct Highlight
{
    static constexpr int16_t NoPieceSelected = -1;

    int16_t index = NoPieceSelected;
    Color color;
};