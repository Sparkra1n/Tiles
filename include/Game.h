#pragma once
#include "Player.h"
#include "Renderer.h"
#include "GameState.h"
#include <iostream>
#include <vector>
#include <memory>
#include <raylib.h>
#include "GameBoard.h"

class Game final
{
public:
    Game(const std::string& path, const std::string& playerName);
    ~Game() = default;
    void run();
    void handleLeftMouseButtonClick(const Vector2& mousePosition);
    void handleRightMouseButtonClick(const Vector2& mousePosition);
    void update(double deltaTime);
    void handleInputEvents();

private:
    GameState m_gameState;
    GameBoard m_gameBoard;
    Renderer m_renderer;
    std::vector<std::shared_ptr<Sprite>> m_backgroundSprites;
    std::vector<std::shared_ptr<Sprite>> m_foregroundSprites;
};
