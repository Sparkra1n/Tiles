#include "Direction.h"
#include <string>
#include <stdexcept>
#include <unordered_map>

Direction::Type Direction::stringToDirection(const std::string& direction) 
{
    using namespace Direction;
    static const std::unordered_map<std::string, Type> directionMap = 
    {
        {"north", NORTH},
        {"east", EAST},
        {"south", SOUTH},
        {"west", WEST},
        {"northeast", NORTHEAST},
        {"southeast", SOUTHEAST},
        {"southwest", SOUTHWEST},
        {"northwest", NORTHWEST},
        {"none", NONE}
    };

    auto it = directionMap.find(direction);
    if (it != directionMap.end()) {
        return it->second;
    }
    throw std::runtime_error("Invalid direction passed to stringToDirection");
}

std::string Direction::directionToString(Direction::Type direction) 
{
    switch (direction) 
    {
    case NORTH: 
        return "north";
    case EAST:  
        return "east";
    case SOUTH: 
        return "south";
    case WEST:  
        return "west";
    case NORTHEAST: 
        return "northeast";
    case SOUTHEAST:
        return "southeast";
    case SOUTHWEST:
        return "southwest";
    case NORTHWEST:
        return "northwest";
    case NONE:
        return "none";
    }
}

Direction::Type Direction::getComplimentaryDirection(Direction::Type direction)
{
    using namespace Direction;
    switch (direction) 
    { 
    case NORTH:
        return SOUTH;
    case NORTHEAST:
        return SOUTHWEST;
    case EAST:
        return WEST;
    case SOUTHEAST:
        return NORTHWEST;
    case SOUTH:
        return NORTH;
    case SOUTHWEST:
        return NORTHEAST;
    case WEST:
        return EAST;
    case NORTHWEST:
        return SOUTHEAST;
    default:
        return NONE;
    }
}

std::array<Direction::Type, 2> Direction::getNeighborDirections(Direction::Type direction) 
{
    using namespace Direction;
    switch (direction) 
    { 
    case NORTH:
        return { NORTHWEST, NORTHEAST };
    case NORTHEAST:
        return { NORTH, EAST };
    case EAST:
        return { NORTHEAST, SOUTHEAST };
    case SOUTHEAST:
        return { EAST, SOUTH, };
    case SOUTH:
        return { SOUTHEAST, SOUTHWEST };
    case SOUTHWEST:
        return { SOUTH, WEST };
    case WEST:
        return { SOUTHWEST, NORTHWEST };
    case NORTHWEST:
        return { WEST, NORTH };
    default:
        throw std::runtime_error("getNeighborDirections: invalid direction");
    }
}

bool Direction::isPrimaryDirection(Direction::Type direction)
{
    using namespace Direction;
    if (direction == NORTH || direction == SOUTH || direction == WEST || direction == EAST)
        return true;
    return false;
}

bool Direction::isSecondaryDirection(Direction::Type direction)
{
    using namespace Direction;
    if (direction == NORTHWEST || direction == NORTHEAST || direction == SOUTHWEST || direction == SOUTHEAST)
        return true;
    return false;
}

std::array<Direction::Type, 2> Direction::decomposeSecondaryDirection(Direction::Type direction)
{
    using namespace Direction;
    switch (direction)
    {
    case NORTHEAST:
        return { NORTH, EAST };
    case NORTHWEST:
        return { NORTH, WEST };
    case SOUTHEAST:
        return { SOUTH, EAST };
    case SOUTHWEST:
        return { SOUTH, WEST };
    default:
        throw std::runtime_error("Decomposition can only be called on secondary direction");
    }
}
