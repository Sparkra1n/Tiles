#pragma once
#include <string>
#include <vector>
#include <regex>
#include "Direction.h"
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <tuple>

class TileRules
{
public:
    TileRules(const TileRules&) = delete;
    TileRules& operator=(const TileRules) = delete;

    static TileRules& getInstance()
    {
        static TileRules instance = TileRules();
        return instance;
    }

    std::tuple<std::string, std::string, Direction::Type, std::string>
    parseTileFilename(const std::string& tilePath)
    {
        using namespace Direction;
        // Parse tile filenames
        std::regex pattern(R"((\w+)_(solid)(?:_(\w+))?|(\w+)_((?!solid)\w+)_(\w+)(?:_(\w+))?)");
        std::smatch matches;

        if (!std::regex_match(tilePath, matches, pattern)) 
            throw std::invalid_argument("Invalid tile sprite filename: " + tilePath);

        std::string material = matches[0];
        std::string type = matches[1];
        if (type != "solid" && type != "border")
            throw std::invalid_argument("Invalid tile type: " + type);
                                             
        std::string directionStr = matches[2].matched ? matches[2].str() : "nowhere";
        std::string variant = matches[3].matched ? matches[3].str() : "";
        Type direction = stringToDirection(directionStr);
        return { material, type, direction, variant };
    }

    //TODO: Solid tiles can connect to any other solid tile
    void buildTileRule(const std::string& tilePath)
    {
        using namespace Direction;
        const auto [material, type, direction, variant] = parseTileFilename(tilePath);

        // Initialize connection rules for non-solid tiles
    
        //TODO: Clean up
        if (type == "border")
        {
            if (isPrimaryDirection(direction))
            {
                // Borders connect solid tiles with water
                // Primary directions: border's dir -> solid variant, complimentary dir -> water
                m_connectionRules[tilePath][direction].push_back(material + "_solid");
                m_connectionRules[tilePath][getComplimentaryDirection(direction)].push_back(material + "_solid");
            }
            else if (isSecondaryDirection(direction))
            {
                std::array<Type, 2> neighborTileDirections = getNeighborDirections(direction);
                std::array<Type, 2> neighborJoinPoints = { 
                    getComplimentaryDirection(neighborTileDirections[0]),
                    getComplimentaryDirection(neighborTileDirections[1])
                };
                
                for (size_t i = 0; i < neighborJoinPoints.size(); ++i)
                {
                    m_connectionRules[tilePath][neighborJoinPoints[i]].push_back(
                        material + "_border_" + directionToString(neighborTileDirections[i])
                    );
                }
            }
        }
    }

private:
    TileRules() = default;
    ~TileRules() = default;
    using ValidNeighbors = std::vector<std::string>;
    std::unordered_map<std::string, std::unordered_map<Direction::Type, ValidNeighbors>> m_connectionRules;
};
