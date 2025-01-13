#pragma once
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <filesystem>
#include "TileRules.h"

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
        if (m_initialized)
            return;

        const std::string gameObjectsPath = ("resources/sprites/gameobjects");
        const std::string tilesPath = ("resources/sprites/tiles");

        try 
        {
            for (const auto& entry : std::filesystem::directory_iterator(gameObjectsPath)) 
            {
                if (entry.is_regular_file()) 
                {
                    const std::string filePath = entry.path().string();
                    const std::string fileName = entry.path().stem().string(); // Remove file extension
                    registerTexture(fileName, filePath);
                }
            }

            for (const auto& entry : std::filesystem::directory_iterator(tilesPath))
            {
                if (entry.is_regular_file()) 
                {
                    const std::string filePath = entry.path().string();
                    const std::string fileName = entry.path().stem().string(); // Remove file extension

                    // Extract file name
                    std::regex pattern(R"((\w+)_(solid)(?:_(\w+))?|(\w+)_((?!solid)\w+)_(\w+)(?:_(\w+))?)");
                    std::smatch matches;
                    if (!std::regex_match(fileName, matches, pattern))
                        throw std::invalid_argument("Invalid tile sprite filename: " + filePath);

                    std::string material = matches[1];
                    std::string variant = matches[4].matched ? matches[4].str() : "";
                    registerTexture(material, filePath);
                }
            }
        }
        catch (const std::filesystem::filesystem_error& e)
        {
            throw std::runtime_error("Failed to initialize textures: " + std::string(e.what()));
        }

        m_initialized = true;
    }

    bool m_initialized = false;
    std::unordered_map<std::string, std::string> m_registry;
};
