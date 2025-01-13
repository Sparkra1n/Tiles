#pragma once
#include <iostream>
#include <cmath>
#include <iomanip>
#include "SpriteFactory.h"
#include "Player.h"
#include <queue>
#include <unordered_set>
#include <raylib.h>
#include <vector>
#include <random>
#include <algorithm>
#include <fstream>
#include "GameState.h"
#include "Sprite.h"
#include "Player.h"
#include "Tile.h"

class GameBoard
{
public:
    GameBoard() = default;
    GameBoard(const std::string& path, const std::string& playerName);
    void update(const GameState& state);
    void onClick(const GameState& state);
    void pushObject(const std::shared_ptr<Sprite>& object, const std::shared_ptr<Sprite>& player);
    void readDimensions(const std::string& path);
    std::shared_ptr<Sprite> getPlayer() const;
    std::shared_ptr<Tile> getTile(int x, int y) const;
    std::shared_ptr<Tile> getClosestAvailableTile(const std::shared_ptr<Tile>& start, const std::shared_ptr<Tile>& destination) const;
    std::vector<std::shared_ptr<Sprite>> getResidingSprites() const;
    std::vector<std::shared_ptr<Tile>> getTiles() const;
    std::vector<std::shared_ptr<Tile>> getPathToTile(const std::shared_ptr<Tile>& startTile, const std::shared_ptr<Tile>& goalTile) const;
    std::shared_ptr<Tile> getEnclosingTile(const std::shared_ptr<Sprite>& sprite) const;
    std::shared_ptr<Tile> getEnclosingTile(Vector2 windowCoordinates) const;
    bool isSolved();
    int generateRandomRotation(int x, int y) const;
    int getBoardRows() const { return m_boardRows; }
    int getBoardColumns() const { return m_boardColumns; }
    Vector2 getBoardBounds() const { return m_boardBounds; }
    static constexpr int getMaxRows() { return 7; }
    static constexpr int getMaxColumns() { return 7; }

private:
    int m_boardRows{};
    int m_boardColumns{};
    Vector2 m_boardBounds{};
    std::shared_ptr<Sprite> m_hoveredSprite{};
    std::shared_ptr<Sprite> m_player{};
    std::vector<std::vector<std::shared_ptr<Tile>>> m_tiles;
    std::vector<std::shared_ptr<Sprite>> m_residingSprites;

    struct AStarNode
    {
        AStarNode(std::shared_ptr<Tile> tile, std::shared_ptr<AStarNode> parent, float gCost, float hCost)
            : m_tile(std::move(tile)),
            m_parent(std::move(parent)),
            m_gValue(gCost),
            m_hValue(hCost) {}

        float getFValue() const { return m_gValue + m_hValue; }
        float getGValue() const { return m_gValue; }
        float getHValue() const { return m_hValue; }
        std::shared_ptr<Tile> getCorrespondingTile() { return m_tile; }
        std::shared_ptr<AStarNode> getParent() { return m_parent; }
        bool operator>(const AStarNode& other) const { return getFValue() > other.getFValue(); }

    private:
        std::shared_ptr<Tile> m_tile;
        std::shared_ptr<AStarNode> m_parent;
        float m_gValue;
        float m_hValue;
    };

    using Matrix = std::vector<std::vector<std::string>>;
    static Matrix loadMatrix(const std::string& path, int offset, int expectedRows, int expectedColumns);
    static std::vector<std::shared_ptr<Tile>> reversePath(const std::shared_ptr<AStarNode>& node);
    static float heuristic(const std::shared_ptr<Tile>& first, const std::shared_ptr<Tile> second);
    std::vector<std::shared_ptr<Tile>> getNeighborTiles(const std::shared_ptr<Tile>& tile) const;
};
