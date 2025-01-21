#pragma once

#include <cstdint>
#include <iostream>

using Coord = int16_t;

struct Vec2
{
    Coord i = 0;
    Coord j = 0;
};

constexpr bool operator==( Vec2 v1, Vec2 v2 )
{
    return v1.i == v2.i && v1.j == v2.j;
}

constexpr Vec2 operator+( Vec2 v1, Vec2 v2 )
{
    return { Coord( v1.i + v2.i ), Coord( v1.j + v2.j ) };
}

constexpr Vec2 operator-( Vec2 v1, Vec2 v2 )
{
    return { Coord( v1.i - v2.i ), Coord( v1.j - v2.j ) };
}

constexpr Vec2 operator*( Vec2 v, int16_t n )
{
    return { Coord( v.i * n ), Coord( v.j * n ) };
}

constexpr Vec2 operator/( Vec2 v, int16_t n )
{
    return { Coord( v.i / n ), Coord( v.j / n ) };
}

inline std::ostream& operator<<( std::ostream& os, Vec2 v )
{
    os << '{' << v.i << ',' << v.j << '}';
    return os;
}