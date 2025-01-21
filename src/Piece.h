#pragma once

#include <cstdint>

#include <SDL3/SDL.h>

#include "image.h"

namespace piece
{
    enum Type : uint8_t
    {
        King,
        Queen,
        Bishop,
        Knight,
        Rook,
        Pawn,
        Null
    };
}

struct Piece
{
    bool isBlack:     1 = false;
    piece::Type type: 7 = piece::Type::Null;

    // not the inverse of isAi(), as pieces can be null
    constexpr bool isUser() const { return type != piece::Type::Null && isBlack; }
    // not the inverse of isUser(), as pieces can be null
    constexpr bool isAi() const { return type != piece::Type::Null && !isBlack; }
    
    constexpr bool isNull() const { return type == piece::Type::Null; }
};

namespace piece
{
    constexpr SDL_FRect BlackImageRect = {
        .x = 0,
        .y = image::crop::Height,
        .w = image::crop::Width,
        .h = image::crop::Height
    };

    constexpr SDL_FRect WhiteImageRect = {
        .x = 0,
        .y = 0,
        .w = image::crop::Width,
        .h = image::crop::Height
    };

    constexpr int getImageCropX( piece::Type t )
    {
        return static_cast< int >( t ) * image::crop::Width;
    }

    constexpr SDL_FRect getImageCrop( Piece p )
    {
        auto imgCrop = p.isBlack ? BlackImageRect : WhiteImageRect;
        imgCrop.x = getImageCropX( p.type );
        return imgCrop;
    }
}

constexpr piece::Type& operator++( piece::Type& t )
{
    t = static_cast< piece::Type >( static_cast< uint8_t >( t ) + 1 );
    return t;
}
