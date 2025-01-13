#include "Game.h"

Game::Game(const std::string& path, const std::string& playerName) 
{
    // Initialize Raylib
    InitWindow(1000, 1000, "TilePuzzle");
    SetTargetFPS(60);

    m_gameBoard = GameBoard(path, playerName);
    m_renderer = Renderer();

    for (auto& sprite : m_gameBoard.getTiles())
        m_backgroundSprites.push_back(sprite);

    for (auto& sprite : m_gameBoard.getResidingSprites())
        m_foregroundSprites.push_back(sprite);

    m_foregroundSprites.push_back(m_gameBoard.getPlayer());
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
