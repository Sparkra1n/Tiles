#pragma once
#include <iostream>
#include <cmath>
#include <iomanip>
#include "Factory.h"
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

class Tile : public Sprite
{
public:
    static constexpr int getHeight() { return 128; }

    Tile(const std::string& texturePath,
        const std::shared_ptr<Sprite>& residingEntity = nullptr,
        bool isGoalTile = false
    );

    void setAsGoalTile() { m_isGoalTile = true; }
    void setResidingSprite(const std::shared_ptr<Sprite>& residingEntity);
    bool isGoalTile() const { return m_isGoalTile; }
    std::shared_ptr<Sprite> getResidingSprite() const;

private:
    std::shared_ptr<Sprite> m_residingSprite;
    bool m_isGoalTile;
};

class GameBoard
{
public:
    GameBoard() = default;
    void loadBoard(const std::string& path, const std::string& playerName);
    void update(const GameState& state);
    void onClick(const GameState& state);
    void pushTile(const std::shared_ptr<Sprite>& entity, const Vector2& playerPosition) const;
    void readDimensions(const std::string& path);
    std::shared_ptr<Sprite> getPlayer() const;
    static Vector2 snapToGrid(Vector2 coordinates);
    static Vector2 centerScreenCoordinates(Vector2 coordinates, Rectangle rect);
    Vector2 getGameBoardCoordinates(Vector2 coordinates) const;
    std::shared_ptr<Tile> getEnclosingTile(const Vector2& position) const;
    std::shared_ptr<Tile> getTile(int x, int y) const;
    std::vector<std::shared_ptr<Tile>> getTiles() const;
    // std::shared_ptr<Tile> getClosestAvailableTile(const Vector2& tilePosition, const Vector2& playerCoordinates) const;
    std::vector<std::shared_ptr<Tile>> getPathToTile(const std::shared_ptr<Tile>& startTile, const std::shared_ptr<Tile>& goalTile) const;
    bool isSolved();
    int getBoardRows() const { return m_boardRows; }
    int getBoardColumns() const { return m_boardColumns; }
    Vector2 getBoardBounds() const { return m_boardBounds; }
    static constexpr int MAX_ROWS = 7;
    static constexpr int MAX_COLUMNS = 7;

private:
    int m_boardRows{};
    int m_boardColumns{};
    Vector2 m_boardBounds{};

    std::shared_ptr<Sprite> m_hoveredSprite{};
    std::shared_ptr<Sprite> m_player{};
    std::vector<std::vector<std::shared_ptr<Tile>>> m_tiles;

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
        std::shared_ptr<Tile> getTile() { return m_tile; }
        std::shared_ptr<AStarNode> getParent() { return m_parent; }
        bool operator>(const AStarNode& other) const { return getFValue() > other.getFValue(); }

    private:
        std::shared_ptr<Tile> m_tile;
        std::shared_ptr<AStarNode> m_parent;
        float m_gValue;
        float m_hValue;
    };

    static std::vector<std::vector<std::string>> loadMatrix(const std::string& path, int offset, int expectedRows, int expectedColumns);
    static std::vector<std::shared_ptr<Tile>> reversePath(const std::shared_ptr<AStarNode>& node);
    static float heuristic(Vector2 a, Vector2 b);
    std::vector<std::shared_ptr<Tile>> getNeighborTiles(const std::shared_ptr<Tile>& tile) const;
};
