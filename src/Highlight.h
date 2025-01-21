#pragma once

#include <cstdint>

struct RGB
{
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
};

namespace color
{
    constexpr RGB Red   = { 180,  20,  60 };
    constexpr RGB Green = {  20, 180,  60 };
    constexpr RGB Blue  = {  20,  60, 180 };
    constexpr RGB Black = {  20,  20,  20 };
    constexpr RGB White = { 235, 235, 235 };

    constexpr RGB DarkRed = { 90, 10, 30 };
}

struct Highlight
{
    static constexpr int16_t NoPieceSelected = -1;

    int16_t index = NoPieceSelected;
    RGB color;
};