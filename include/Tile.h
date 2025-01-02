#pragma once
#include <raylib.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <regex>
#include "Sprite.h"

class Tile : public Sprite
{
public:
    Tile(const std::string& texturePath,
         const std::shared_ptr<Sprite>& residingEntity = nullptr,
         bool isGoalTile = false)
         : Sprite(texturePath.c_str()), 
           m_residingSprite(residingEntity),
           m_isGoalTile(isGoalTile) {}

    static constexpr int getHeight() { return 128; }
    void setAsGoalTile() { m_isGoalTile = true; }
    void setResidingSprite(const std::shared_ptr<Sprite>& residingEntity);
    bool isGoalTile() const { return m_isGoalTile; }
    std::shared_ptr<Sprite> getResidingSprite() const;

private:
    std::shared_ptr<Sprite> m_residingSprite;
    bool m_isGoalTile;
};

