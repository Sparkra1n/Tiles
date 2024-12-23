#include "GameBoard.h"

Tile::Tile(const std::string& texturePath,
           const std::shared_ptr<Sprite>& residingEntity,
           bool isGoalTile)
    : Sprite(texturePath.c_str()), 
      m_residingSprite(residingEntity),
      m_isGoalTile(isGoalTile) {}


void GameBoard::loadBoard(const std::string& path, const std::string& playerName)
{
    using Matrix = std::vector<std::vector<std::string>>;
    m_player = Factory::create<Sprite>(playerName);
    readDimensions(path);

    // Determine offsets for matrices in the file
    int offset = 1; // First line is dimensions
    Matrix tileKeys = loadMatrix(path, offset, m_boardRows, m_boardColumns);
    offset += m_boardRows;
    Matrix immovableKeys = loadMatrix(path, offset, m_boardRows, m_boardColumns);
    offset += m_boardRows;
    Matrix movableKeys = loadMatrix(path, offset, m_boardRows, m_boardColumns);

    // Lay tiles on the board
    for (int i = 0; i < tileKeys.size(); ++i)
    {
        std::vector<std::shared_ptr<Tile>> convertedRow;
        for (int j = 0; j < tileKeys[i].size(); ++j)
        {
            const std::string& textureKey = tileKeys[i][j];
            try {
                std::shared_ptr<Tile> tile = Factory::create<Tile>(textureKey);
                tile->setWindowCoordinates({
                    static_cast<float>(Tile::getHeight() * i),
                    static_cast<float>(Tile::getHeight() * j)
            });
                convertedRow.push_back(tile);
            }
            catch (const std::out_of_range&) {
                throw std::runtime_error("Invalid tile texture key: " + textureKey);
            }
        }
        m_tiles.push_back(convertedRow);
    }

    // Place immovable objects
    for (int i = 0; i < immovableKeys.size(); ++i)
    {
        for (int j = 0; j < immovableKeys[i].size(); ++j)
        {
            const std::string& textureKey = immovableKeys[i][j];
            if (textureKey != "Empty")
            {
                try {
                    std::shared_ptr<Sprite> sprite = Factory::create<Sprite>(textureKey);
                    sprite->setWindowCoordinates({
                        static_cast<float>(Tile::getHeight() * i),
                        static_cast<float>(Tile::getHeight() * j)
                        });
                }
                catch (const std::out_of_range&) {
                    throw std::runtime_error("Invalid immovable texture key: " + textureKey);
                }
            }
        }
    }

    // Place movable objects
    for (int i = 0; i < movableKeys.size(); ++i)
    {
        for (int j = 0; j < movableKeys[i].size(); ++j)
        {
            const std::string& textureKey = movableKeys[i][j];
            if (textureKey != "Empty")
            {
                try {
                    auto gameObject = Factory::create<Sprite>(textureKey, 5.0);
                    gameObject->setWindowCoordinates({
                        static_cast<float>(Tile::getHeight() * i),
                        static_cast<float>(Tile::getHeight() * j)
                        });
                }
                catch (const std::out_of_range&) {
                    throw std::runtime_error("Invalid movable texture key: " + textureKey);
                }
            }
        }
    }
}

void GameBoard::readDimensions(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Could not open file: " + path);

    std::string line;
    std::getline(file, line);
    std::stringstream dimensionsStream(line);
    dimensionsStream >> m_boardRows;
    dimensionsStream.ignore(1);
    dimensionsStream >> m_boardColumns;

    if (m_boardRows <= 0 || m_boardColumns <= 0 || m_boardRows > MAX_ROWS || m_boardColumns > MAX_COLUMNS)
        throw std::runtime_error("Invalid board dimensions in file: " + path);

    m_boardBounds =
    {
        static_cast<float>(m_boardRows * Tile::getHeight() - 5),
        static_cast<float>(m_boardColumns * Tile::getHeight() - 5)
    };
}

std::vector<std::vector<std::string>> GameBoard::loadMatrix(const std::string& path, int offset, int expectedRows, int expectedColumns)
{
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Could not open file: " + path);

    // Skip lines up to the matrix start
    std::string line;
    for (int i = 0; i < offset; ++i)
    {
        if (!std::getline(file, line))
            throw std::runtime_error("Unexpected end of file before matrix data");
    }

    // Read the matrix
    std::vector<std::vector<std::string>> matrix;
    for (int i = 0; i < expectedRows; ++i)
    {
        if (!std::getline(file, line))
            throw std::runtime_error("Unexpected end of file in matrix data");

        if (line.empty()) continue;

        std::vector<std::string> rowElements;
        std::stringstream ss(line);
        std::string cell;

        while (std::getline(ss, cell, ','))
            rowElements.push_back(cell);

        if (static_cast<int>(rowElements.size()) != expectedColumns)
            throw std::runtime_error("Row size mismatch in matrix data");

        matrix.push_back(rowElements);
    }

    return matrix;
}



void GameBoard::onClick(const GameState& state)
{
    if (state.mousePosition.x > m_boardBounds.x || state.mousePosition.y > m_boardBounds.y)
        return;

    std::cout << "Clicked\n";
    //const Vector2 destination = centerScreenCoordinates(state.mousePosition, m_player->getRect());
    //std::shared_ptr<Tile> tile = getEnclosingTile(destination);

    //// Unoccupied destination tile
    //if (tile->getResidingSprite() == nullptr)
    //{
    //    std::vector<std::shared_ptr<Tile>> tiles = getPathToTile(getEnclosingTile(m_player->getWindowCoordinates()), tile);
    //    std::vector<Vector2> path;
    //    path.reserve(tiles.size());
    //    for (const auto& i : tiles)
    //        path.push_back(centerScreenCoordinates(
    //            i->getWindowCoordinates(),
    //            m_player->getSdlRect())
    //        );
    //    m_player->walk(path);
    //}

    //FIXME: Go to a neighboring tile and push the slab
    //else
    //{
    //    std::shared_ptr<Tile> nextTileChoice = getClosestAvailableTile(state.mousePosition, m_player->getWindowCoordinates());
    //    if (nextTileChoice)
    //    {
    //        std::vector<std::shared_ptr<Tile>> tiles = getPathToTile(getEnclosingTile(m_player->getWindowCoordinates()), tile);
    //        std::vector<Vector2> path;
    //        path.reserve(tiles.size());
    //        for (const auto& i : tiles)
    //            path.push_back(centerScreenCoordinates(i->getWindowCoordinates(), m_player->getSdlRect()));

    //        m_player->walk(path);
    //        pushTile(getEnclosingTile(destination)->getResidingSprite(), m_player->getWindowCoordinates());
    //    }
    //}
    //m_hoverTracker.getFocused()->onClick();
}

std::shared_ptr<Sprite> GameBoard::getPlayer() const 
{
    return m_player;
}

void GameBoard::update(const GameState& state)
{
    Vector2 mousePosition = state.mousePosition;

    if (mousePosition.x > m_boardBounds.x)
        mousePosition.x = m_boardBounds.x;

    if (mousePosition.y > m_boardBounds.y)
        mousePosition.y = m_boardBounds.y;

    std::shared_ptr<Tile> hoveredTile = getEnclosingTile(mousePosition);
    std::shared_ptr<Sprite> residingSprite = hoveredTile->getResidingSprite();
    if (residingSprite)
    {
        if (residingSprite != m_hoveredSprite)
        {
            residingSprite->onFocus();
            if (m_hoveredSprite)
                m_hoveredSprite->onBlur();
            m_hoveredSprite = residingSprite;
        }
    }
    else
    {
        if (hoveredTile != m_hoveredSprite)
        {
            hoveredTile->onFocus();
            if (m_hoveredSprite)
                m_hoveredSprite->onBlur();
            m_hoveredSprite = hoveredTile;
        }
    }

    m_player->update(state);
}

void Tile::setResidingSprite(const std::shared_ptr<Sprite>& residingEntity)
{
    m_residingSprite = residingEntity;
}

std::shared_ptr<Sprite> Tile::getResidingSprite() const
{
    return m_residingSprite;
}

Vector2 GameBoard::snapToGrid(Vector2 coordinates)
{
    constexpr int tileHeight = Tile::getHeight();
    float x = std::floor(coordinates.x / tileHeight) * tileHeight;
    float y = std::floor(coordinates.y / tileHeight) * tileHeight;
    return { x, y };
}

Vector2 GameBoard::centerScreenCoordinates(Vector2 coordinates, Rectangle rect)
{
    constexpr int tileHeight = Tile::getHeight();
    float x = std::floor(coordinates.x / tileHeight) * tileHeight + (tileHeight / 2.0f) - (rect.x / 2.0f);
    float y = std::floor(coordinates.y / tileHeight) * tileHeight + (tileHeight / 2.0f) - (rect.y / 2.0f);
    return { x, y };
}

Vector2 GameBoard::getGameBoardCoordinates(Vector2 coordinates) const
{
    constexpr int tileHeight = Tile::getHeight();
    return { coordinates.x / tileHeight, coordinates.y / tileHeight };
}

std::shared_ptr<Tile> GameBoard::getEnclosingTile(const Vector2& position) const
{
    Vector2 tileCoordinates = snapToGrid(position);
    constexpr int tileHeight = Tile::getHeight();
    int xIndex = static_cast<int>(tileCoordinates.x / tileHeight);
    int yIndex = static_cast<int>(tileCoordinates.y / tileHeight);

    if (xIndex < 0 || yIndex < 0 || xIndex >= m_boardColumns || yIndex >= m_boardRows)
        return nullptr;

    return m_tiles[xIndex][yIndex];
}


std::shared_ptr<Tile> GameBoard::getTile(int x, int y) const
{
    if (x >= m_boardColumns || y >= m_boardRows || x < 0 || y < 0)
        return nullptr;
    return m_tiles[x][y];
}

std::vector<std::shared_ptr<Tile>> GameBoard::getTiles() const
{
    size_t total_size = 0;
    for (const auto& v : m_tiles)
        total_size += v.size();

    std::vector<std::shared_ptr<Tile>> tiles;
    tiles.reserve(total_size);

    for (const auto& v : m_tiles)
        tiles.insert(tiles.end(), v.begin(), v.end());

    return tiles;
}

void GameBoard::pushTile(const std::shared_ptr<Sprite>& entity, const Vector2& playerPosition) const
{
    //std::shared_ptr<Tile> playerTile = getEnclosingTile(playerPosition);
    //std::shared_ptr<Tile> entityTile = getEnclosingTile(entity->getWindowCoordinates());

    //if (!playerTile || !entityTile)
    //    return;

    //int dX = playerTile->getWindowCoordinates().x - entityTile->getWindowCoordinates().x;
    //int dY = playerTile->getWindowCoordinates().y - entityTile->getWindowCoordinates().y;

    //if (std::abs(dX) > Tile::TILE_DIMENSIONS.x || std::abs(dY) > Tile::TILE_DIMENSIONS.y)
    //    return;

    //int dirX = 0, dirY = 0;
    //if (std::abs(dX) > std::abs(dY))
    //    dirX = (dX > 0) ? -1 : 1;
    //else
    //    dirY = (dY > 0) ? -1 : 1;

    //int currentX = entityTile->getWindowCoordinates().x / Tile::TILE_DIMENSIONS.x;
    //int currentY = entityTile->getWindowCoordinates().y / Tile::TILE_DIMENSIONS.y;

    //std::shared_ptr<Tile> targetTile = nullptr;
    //while (true)
    //{
    //    int nextX = currentX + dirX;
    //    int nextY = currentY + dirY;

    //    if (nextX < 0 || nextX >= m_boardColumns || nextY < 0 || nextY >= m_boardRows)
    //        break;

    //    std::shared_ptr<Tile> nextTile = m_tiles[nextX][nextY];
    //    if (nextTile->getResidingSprite() != nullptr)
    //        break;

    //    targetTile = nextTile;
    //    currentX = nextX;
    //    currentY = nextY;
    //}

    //if (targetTile)
    //{
    //    entityTile->setResidingSprite(nullptr);
    //    targetTile->setResidingSprite(entity);
    //    Vector2 destination = centerScreenCoordinates(targetTile->getWindowCoordinates(), entity->getRect());
    //    entity->walk({ destination });
    //}
}

//std::shared_ptr<Tile> GameBoard::getClosestAvailableTile(const Vector2&tilePosition, const Vector2&playerCoordinates) const
//{
    //static const std::vector<Vector2> directions =
    //{
    //    { 0, -1 },
    //    { -1, 0 },
    //    { 1, 0 },
    //    { 0, 1 }
    //};

    //Vector2 coordinates = snapToGrid(tilePosition);
    //int tileX = coordinates.x / Tile::TILE_DIMENSIONS.x;
    //int tileY = coordinates.y / Tile::TILE_DIMENSIONS.y;

    //std::shared_ptr<Tile> closestTile = nullptr;
    //double minDistance = std::numeric_limits<double>::max();

    //for (const auto& dir : directions)
    //{
    //    int newX = tileX + dir.x;
    //    int newY = tileY + dir.y;

    //    if (newX >= 0 && newX < m_boardColumns && newY >= 0 && newY < m_boardRows)
    //    {
    //        std::shared_ptr<Tile> adjacentTile = m_tiles[newX][newY];

    //        if (adjacentTile->getResidingSprite() == nullptr)
    //        {
    //            double distance = std::sqrt(std::pow(newX * Tile::TILE_DIMENSIONS.x - playerCoordinates.x, 2) +
    //                std::pow(newY * Tile::TILE_DIMENSIONS.y - playerCoordinates.y, 2));

    //            if (distance < minDistance)
    //            {
    //                closestTile = adjacentTile;
    //                minDistance = distance;
    //            }
    //        }
    //    }
    //}

    //return closestTile;
//}

std::vector<std::shared_ptr<Tile>> GameBoard::reversePath(const std::shared_ptr<AStarNode>& node)
{
    std::vector<std::shared_ptr<Tile>> path;
    std::shared_ptr<AStarNode> current = node;
    while (current)
    {
        path.push_back(current->getTile());
        current = current->getParent();
    }
    std::reverse(path.begin(), path.end());
    return path;
}

float GameBoard::heuristic(const Vector2 a, const Vector2 b)
{
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

std::vector<std::shared_ptr<Tile>> GameBoard::getNeighborTiles(const std::shared_ptr<Tile>& tile) const
{
    static const std::vector<Vector2> directions =
    {
        Vector2{ 0, -1 }, Vector2{ -1, 0 },
        Vector2{ 1, 0  }, Vector2{  0, 1 }
    };

    Vector2 position = getGameBoardCoordinates(tile->getWindowCoordinates());

    std::vector<std::shared_ptr<Tile>> neighbors;
    neighbors.reserve(directions.size());

    for (const auto& d : directions)
    {
        if (auto neighbor = getTile(position.x + d.x, position.y + d.y))
            neighbors.push_back(neighbor);
    }

    return neighbors;
}

std::vector<std::shared_ptr<Tile>> GameBoard::getPathToTile(const std::shared_ptr<Tile>& startTile, const std::shared_ptr<Tile>& goalTile) const
{
    if (!startTile || !goalTile)
        return {};

    auto compare = [](const std::shared_ptr<AStarNode>& a, const std::shared_ptr<AStarNode>& b) -> bool
        {
            return *a > *b;
        };

    std::priority_queue<std::shared_ptr<AStarNode>, std::vector<std::shared_ptr<AStarNode>>, decltype(compare)> openList(compare);
    std::unordered_map<std::shared_ptr<Tile>, std::shared_ptr<AStarNode>> allNodes;
    std::unordered_set<std::shared_ptr<Tile>> closedList;

    auto startNode = std::make_shared<AStarNode>(startTile,
        nullptr,
        0.0,
        heuristic(startTile->getWindowCoordinates(), goalTile->getWindowCoordinates())
    );

    openList.push(startNode);
    allNodes[startTile] = startNode;

    while (!openList.empty())
    {
        auto current = openList.top();
        openList.pop();
        closedList.insert(current->getTile());

        if (current->getTile() == goalTile)
            return reversePath(current);

        auto neighbors = getNeighborTiles(current->getTile());
        for (const auto& neighbor : neighbors)
        {
            if (closedList.count(neighbor))
                continue;

            if (neighbor->getResidingSprite() != nullptr)
                continue;

            double tentativeG = current->getGValue() + 1.0;
            double hCost = heuristic(neighbor->getWindowCoordinates(), goalTile->getWindowCoordinates());
            auto neighborNode = std::make_shared<AStarNode>(neighbor, current, tentativeG, hCost);

            if (allNodes.find(neighbor) == allNodes.end() || tentativeG < allNodes[neighbor]->getGValue())
            {
                openList.push(neighborNode);
                allNodes[neighbor] = neighborNode;
            }
        }
    }
    return {};
}

bool GameBoard::isSolved()
{
    for (const auto& column : m_tiles)
    {
        for (const auto& tile : column)
        {
            if (!tile->isGoalTile())
                continue;

            if (tile->getResidingSprite() == nullptr)
                return false;
        }
    }
    return true;
}
