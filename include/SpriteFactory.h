#pragma once
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <vector>
#include <regex>
#include "Direction.h"

class SpriteFactory
{
public:
    template <typename SpriteType, typename... Args>
    static std::shared_ptr<SpriteType> create(const std::string& textureKey, Args&&... args)
    {
        SpriteFactory& instance = getInstance();
        const std::string& texturePath = instance.getTexture(textureKey);
        return std::make_shared<SpriteType>(texturePath.c_str(), std::forward<Args>(args)...);
    }

    SpriteFactory(const SpriteFactory&) = delete;
    SpriteFactory& operator=(const SpriteFactory&) = delete;

private:
    SpriteFactory() = default;
    ~SpriteFactory() = default;

    void registerTexture(const std::string& key, const std::string& path)
    {
        m_registry[key] = path;
    }

    const std::string& getTexture(const std::string& key) const
    {
        auto it = m_registry.find(key);
        if (it == m_registry.end()) {
            throw std::out_of_range("Texture not found: " + key);
        }
        return it->second;
    }

    static SpriteFactory& getInstance()
    {
        static SpriteFactory instance;
        instance.init();
        return instance;
    }

    void init()
    {
        if (!m_initialized)
        {
            const std::string spritesPath = "resources/sprites";

            try 
            {
                for (const auto& entry : std::filesystem::directory_iterator(spritesPath)) 
                {
                    if (entry.is_regular_file()) 
                    {
                        const std::string filePath = entry.path().string();
                        const std::string fileName = entry.path().stem().string(); // Remove file extension
                        registerTexture(fileName, filePath);
                    }
                }
            } 
            catch (const std::filesystem::filesystem_error& e) 
            {
                throw std::runtime_error("Failed to initialize sprite textures: " + std::string(e.what()));
            }

            m_initialized = true;
        }
    }

    void parseTileFilename(const std::string& filename)
    {
        // Parse tile filenames
        std::regex pattern(R"(tile_(\w+)_(\w+)_(\w+)(?:_(\w+))?)");
        std::smatch matches;

        if (!std::regex_match(filename, matches, pattern)) 
            throw std::invalid_argument("Invalid tile sprite filename: " + filename);

        std::string material = matches[1];      // Material: grass, water, etc.
        std::string type = matches[2];          // Type: solid, border
        std::string directionStr = matches[3];  // Direction
        std::string variant = matches[4].matched ? matches[4].str() : "";
        Direction::Type direction = Direction::stringToDirection(directionStr);

        // Initialize connection rules
        if (type == "solid" && direction == Direction::NOWHERE) 
            m_connectionRules[filename][Direction::NOWHERE] = { material + "_solid_nowhere" };
    
        else if (type == "border")
        {
            std::array<Direction::Type, 10> directions = Direction::getDirections();            
            for (Direction::Type d : directions) 
            {
                m_connectionRules[filename][d].push_back("water_solid_nowhere"); // Inward
                m_connectionRules[filename][d].push_back(material + "_solid_nowhere"); // Outward
                
                std::vector<Direction::Type> neighbors = getNeighborDirections(d);
                for (Direction::Type neighbor : neighbors)
                    m_connectionRules[filename][d].push_back(material + "_border_" + directionToString(neighbor));
            }
        }
    }

    bool m_initialized = false;

    using SpriteName = std::string;
    using SpritePath = std::string;
    using TileName = std::string;
    using ValidConnections = std::vector<TileName>;
    std::unordered_map<SpriteName, SpritePath> m_registry;
    static std::unordered_map<TileName, std::unordered_map<Direction::Type, ValidConnections>> m_connectionRules;
};
