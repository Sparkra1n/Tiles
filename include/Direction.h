#pragma once
#include <string>
#include <vector>
#include <array>

namespace Direction
{  
    enum Type 
    {
        NONE = 0,
        NORTH,
        NORTHEAST,
        EAST,
        SOUTHEAST,
        SOUTH,
        SOUTHWEST,
        WEST,
        NORTHWEST
    };

    Type stringToDirection(const std::string& direction);
    Type getComplimentaryDirection(Type direction);
    std::string directionToString(Type direction);
    std::array<Type, 2> getNeighborDirections(Type Direction);
    std::array<Type, 2> decomposeSecondaryDirection(Type Direction);
    bool isPrimaryDirection(Type Direction);
    bool isSecondaryDirection(Type direction);
}


