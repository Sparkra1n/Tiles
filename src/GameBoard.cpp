#include "GameBoard.h"

GameBoard::GameBoard(const std::string& path, const std::string& playerName)
{
    readDimensions(path);
    
    // Reserve space in vectors
    m_tiles.reserve(m_boardRows);
    for (auto& row : m_tiles)
        row.reserve(m_boardColumns);

    m_residingSprites.reserve((m_boardRows * m_boardColumns) / 2); // Estimate of how many sprites are on the board

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
            std::shared_ptr<Tile> tile = SpriteFactory::create<Tile>(textureKey);
            tile->setWindowCoordinates(i * Tile::getSize(), j * Tile::getSize());
            tile->setRotation(90.0f * generateRandomRotation(i, j));
            convertedRow.push_back(tile);
        }
        m_tiles.push_back(convertedRow);
    }

    // Place immovable objects
    for (int i = 0; i < immovableKeys.size(); ++i)
    {
        for (int j = 0; j < immovableKeys[i].size(); ++j)
        {
            const std::string& textureKey = immovableKeys[i][j];
            if (textureKey == "Empty")
                continue;

            auto sprite = SpriteFactory::create<Sprite>(textureKey);
            auto tile = getTile(i, j);
            sprite->setGameBoardCoordinates(i, j);
            tile->setResidingSprite(sprite);
            m_residingSprites.push_back(sprite);
        }
    }

    // Place movable objects
    for (int i = 0; i < movableKeys.size(); ++i)
    {
        for (int j = 0; j < movableKeys[i].size(); ++j)
        {
            const std::string& textureKey = movableKeys[i][j];
            if (textureKey == "Empty")
                continue;

            auto sprite = SpriteFactory::create<Sprite>(textureKey, 5.0f);
            auto tile = getTile(i, j);
            sprite->setGameBoardCoordinates(i, j);
            tile->setResidingSprite(sprite);
            m_residingSprites.push_back(sprite);
        }
    }

    m_player = SpriteFactory::create<Sprite>(playerName, 100.0f);
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

    if (m_boardRows <= 0 || m_boardColumns <= 0 || 
            m_boardRows > getMaxRows() || 
            m_boardColumns > getMaxColumns())
        throw std::runtime_error("Invalid board dimensions in file: " + path);

    m_boardBounds =
    {
        static_cast<float>(m_boardRows * Tile::getSize() - 5),
        static_cast<float>(m_boardColumns * Tile::getSize() - 5)
    };
}

std::vector<std::vector<std::string>> GameBoard::loadMatrix(const std::string& path,
    int offset, int expectedRows, int expectedColumns)
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
        // Skip lines that contain only whitespace or invisible characters
        do {
            if (!std::getline(file, line))
                throw std::runtime_error("Unexpected end of file in matrix data");
        } while (line.find_first_not_of(" \t\n\r") == std::string::npos);

        std::vector<std::string> rowElements;
        std::stringstream ss(line);
        std::string cell;

        // Parse the line as a CSV row
        while (std::getline(ss, cell, ','))
        {
            // Trim whitespace around the cell if necessary
            cell.erase(cell.find_last_not_of(" \t\n\r") + 1);
            cell.erase(0, cell.find_first_not_of(" \t\n\r"));
            rowElements.push_back(cell);
        }

        // Validate the row size
        int size = static_cast<int>(rowElements.size());
        if (size != expectedColumns)
        {
            throw std::runtime_error(
                "Row size mismatch in matrix data; row size: " + std::to_string(size) +
                ", expected: " + std::to_string(expectedColumns));
        }

        matrix.push_back(rowElements);
    }

    // Validate the number of rows read
    if (static_cast<int>(matrix.size()) != expectedRows)
    {
        throw std::runtime_error(
            "Row count mismatch in matrix data; rows read: " + std::to_string(matrix.size()) +
            ", expected: " + std::to_string(expectedRows));
    }

    return matrix;
}

int GameBoard::generateRandomRotation(int x, int y) const
{
    std::seed_seq seed{ x, y };
    std::mt19937 rng(seed); // Mersenne Twister RNG
    std::uniform_int_distribution<int> dist(0, 3); // Range [0, 3]
    return dist(rng);
}

std::shared_ptr<Tile> GameBoard::getEnclosingTile(const std::shared_ptr<Sprite>& sprite) const
{
    return getEnclosingTile(sprite->getWindowCoordinates());
}

std::shared_ptr<Tile> GameBoard::getEnclosingTile(Vector2 windowCoordinates) const
{
    Vector2 gameBoardCoordinates = CoordinateTransformer::toGameBoardCoordinates(windowCoordinates);

    if (gameBoardCoordinates.x < 0 || gameBoardCoordinates.y < 0 ||
        gameBoardCoordinates.x >= m_boardColumns || gameBoardCoordinates.y >= m_boardRows)
    {
        throw std::out_of_range("getEnclosingTile: Sprite is out of board bounds.");
    }

    return m_tiles[static_cast<int>(gameBoardCoordinates.x)][static_cast<int>(gameBoardCoordinates.y)];
}

void GameBoard::onClick(const GameState& state)
{
    if (state.mousePosition.x > m_boardBounds.x || state.mousePosition.y > m_boardBounds.y)
        return;

    std::shared_ptr<Tile> destinationTile = getEnclosingTile(state.mousePosition);

    // Unoccupied destination tile
    if (destinationTile->getResidingSprite() == nullptr)
    {
        std::shared_ptr<Tile> playerTile = getEnclosingTile(m_player);
        std::vector<std::shared_ptr<Tile>> tilePath = getPathToTile(playerTile, destinationTile);
        std::vector<Vector2> coordinates;
        coordinates.reserve(tilePath.size());
        for (const auto& tile : tilePath) 
            coordinates.push_back(tile->getGameBoardCoordinates());
        m_player->walkPath(coordinates);
    }
}

std::shared_ptr<Sprite> GameBoard::getPlayer() const 
{
    return m_player;
}

void GameBoard::update(const GameState& state)
{
    //TODO:: Clean up update function
    if (state.mousePosition.x > m_boardBounds.x)
        return;

    if (state.mousePosition.y > m_boardBounds.y)
        return;

    std::shared_ptr<Tile> hoveredTile = getEnclosingTile(state.mousePosition);
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

std::shared_ptr<Tile> GameBoard::getTile(int x, int y) const
{
    if (x < 0 || y < 0 || x > m_boardColumns || y > m_boardRows)
        throw std::runtime_error("getTile: Invalid coordinates");
    return m_tiles[x][y];
}

std::vector<std::shared_ptr<Sprite>> GameBoard::getResidingSprites() const
{
    return m_residingSprites;
}

std::vector<std::shared_ptr<Tile>> GameBoard::getTiles() const
{
    size_t totalSize = getBoardRows() * getBoardColumns();
    std::vector<std::shared_ptr<Tile>> tiles;
    tiles.reserve(totalSize);

    for (const auto& v : m_tiles)
        tiles.insert(tiles.end(), v.begin(), v.end());
    return tiles;
}

void GameBoard::pushObject(const std::shared_ptr<Sprite>& object, const std::shared_ptr<Sprite>& player)
{
    std::shared_ptr<Tile> playerTile = getEnclosingTile(player);
    std::shared_ptr<Tile> objectTile = getEnclosingTile(object);
    Vector2 playerCoordinates = player->getGameBoardCoordinates();
    Vector2 objectCoordinates = object->getGameBoardCoordinates();

    int dX = playerCoordinates.x - objectCoordinates.x;
    int dY = playerCoordinates.y - objectCoordinates.y;

    // Ensure player and object are adjacent in tile units
    if (std::abs(dX) > 1 || std::abs(dY) > 1)
    {
        // TODO: Make player walk to the location clicked and then try again
        return;
    }

    // Calculate push direction
    int dirX = 0;
    int dirY = 0;

    if (std::abs(dX) > std::abs(dY)) 
    {
        if (dX > 0) 
            dirX = -1; // Player is to the right of the object
        else 
            dirX = 1;  // Player is to the left of the object
    }
    else 
    {
        if (dY > 0) 
            dirY = -1; // Player is below the object
        else 
            dirY = 1;  // Player is above the object
    }

    // Early exit if no direction
    if (dirX == 0 && dirY == 0)
        return;

    int x = objectCoordinates.x;
    int y = objectCoordinates.y;

    std::shared_ptr<Tile> targetTile = nullptr;
    while (true)
    {
        int nextX = x + dirX;
        int nextY = y + dirY;

        // Validate board boundaries
        if (nextX < 0 || nextX >= m_boardColumns || nextY < 0 || nextY >= m_boardRows)
            break;

        std::shared_ptr<Tile> nextTile = m_tiles[nextX][nextY];
        if (nextTile->getResidingSprite() != nullptr)
            break; // Cannot move further

        targetTile = nextTile; // Valid target
        x = nextX;
        y = nextY;
    }

    // If a valid target tile is found
    if (targetTile)
    {
        objectTile->setResidingSprite(nullptr);  // Clear current tile
        targetTile->setResidingSprite(object);   // Set new tile
        object->setGameBoardCoordinates(x, y);   // Update object position
    }
}

std::shared_ptr<Tile> GameBoard::getClosestAvailableTile(const std::shared_ptr<Tile>& start, const std::shared_ptr<Tile>& destination) const
{
    constexpr int tileSize = Tile::getSize();
    const std::array<Vector2, 4> directions = {
        Vector2{0, -1},
        Vector2{-1, 0},
        Vector2{1, 0},
        Vector2{0, 1}
    };

    Vector2 startCoordinates = start->getGameBoardCoordinates();
    int x = startCoordinates.x / tileSize;
    int y = startCoordinates.y / tileSize;

    for (const auto& dir : directions)
    {
        int newX = x + dir.x;
        int newY = y + dir.y;

        if (newX < 0 || newX >= m_boardColumns || newY < 0 || newY >= m_boardRows)
            continue;

        const auto& adjacentTile = m_tiles[newX][newY];
        if (adjacentTile && adjacentTile->getResidingSprite() == nullptr)
            return adjacentTile;
    }
    return nullptr;
}


std::vector<std::shared_ptr<Tile>> GameBoard::reversePath(const std::shared_ptr<AStarNode>& node)
{
    std::vector<std::shared_ptr<Tile>> path;
    std::shared_ptr<AStarNode> current = node;
    while (current)
    {
        path.push_back(current->getCorrespondingTile());
        current = current->getParent();
    }
    std::reverse(path.begin(), path.end());
    return path;
}

float GameBoard::heuristic(const std::shared_ptr<Tile>& first, const std::shared_ptr<Tile> second)
{
    Vector2 f = first->getGameBoardCoordinates();
    Vector2 s = second->getGameBoardCoordinates();
    return std::abs(f.x - s.x) + std::abs(f.y - s.y);
}

std::vector<std::shared_ptr<Tile>> GameBoard::getNeighborTiles(const std::shared_ptr<Tile>& tile) const
{
    static const std::array<Vector2, 4> directions =
    {
        Vector2{ 0, -1 },
        Vector2{ -1, 0 },
        Vector2{ 1, 0  },
        Vector2{  0, 1 }
    };

    Vector2 tileCoordinate = tile->getGameBoardCoordinates();

    std::vector<std::shared_ptr<Tile>> neighbors;
    neighbors.reserve(4);

    for (const auto& direction : directions)
    {
        Vector2 adjacentCoordinate = tileCoordinate + direction;
        if (adjacentCoordinate.x < 0 || adjacentCoordinate.x >= m_boardColumns || 
                adjacentCoordinate.y < 0 || adjacentCoordinate.y >= m_boardRows)
            continue;
        auto neighbor = m_tiles[adjacentCoordinate.x][adjacentCoordinate.y];
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

    auto startNode = std::make_shared<AStarNode>(startTile, nullptr, 0.0, heuristic(startTile, goalTile));
    openList.push(startNode);
    allNodes[startTile] = startNode;

    while (!openList.empty())
    {
        auto current = openList.top();
        openList.pop();
        closedList.insert(current->getCorrespondingTile());

        if (current->getCorrespondingTile() == goalTile)
            return reversePath(current);

        auto neighbors = getNeighborTiles(current->getCorrespondingTile());
        auto a = current->getCorrespondingTile()->getGameBoardCoordinates();
        for (const auto& neighbor : neighbors)
        {
            if (closedList.count(neighbor))
                continue;

            if (neighbor->getResidingSprite() != nullptr)
                continue;

            double tentativeG = current->getGValue() + 1.0;
            double hCost = heuristic(neighbor, goalTile);
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
