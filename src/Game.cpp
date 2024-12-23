#include "Game.h"

Game::Game(const std::string& path, const std::string& playerName) 
    : m_gameState(), 
      m_gameBoard(),
      m_renderer()
{
    loadLevel(path, playerName);
}

void Game::loadLevel(const std::string& path, const std::string& playerName)
{
    // Initialize Raylib
    InitWindow(1000, 1000, "TilePuzzle");
    SetTargetFPS(60);

    // Initialize GameBoard
    m_gameBoard.loadBoard(path, playerName);

    // Load general resources
    for (auto& entity : m_gameBoard.getTiles())
        addBackgroundEntity(entity);

    addForegroundEntity(m_gameBoard.getPlayer());
}

void Game::run()
{
    while (!WindowShouldClose())
    {
        handleInputEvents();

        double deltaTime = GetFrameTime();
        update(deltaTime);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        m_renderer.renderAll(m_backgroundSprites);
        m_renderer.renderAll(m_foregroundSprites);
        EndDrawing();
    }
    CloseWindow();
}

void Game::handleInputEvents()
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        Vector2 mousePosition = GetMousePosition();
        handleLeftMouseButtonClick(mousePosition);
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
    {
        Vector2 mousePosition = GetMousePosition();
        handleRightMouseButtonClick(mousePosition);
    }
}

void Game::handleLeftMouseButtonClick(const Vector2& mousePosition)
{
    m_gameBoard.onClick(m_gameState);
}

void Game::handleRightMouseButtonClick(const Vector2& mousePosition)
{
    // Handle right mouse button click logic
}

void Game::update(const double deltaTime) {
    Vector2 mousePosition = GetMousePosition();
    m_gameState.mousePosition = mousePosition;
    m_gameState.deltaTime = deltaTime;
    m_gameBoard.update(m_gameState);
}

void Game::addBackgroundEntity(const std::shared_ptr<Sprite>& entity)
{
    m_backgroundSprites.push_back(entity);
}

void Game::addForegroundEntity(const std::shared_ptr<Sprite>& entity)
{
    m_foregroundSprites.push_back(entity);
}
