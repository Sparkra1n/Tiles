#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include "GameBoard.h"
#include "Player.h"
#include "Renderer.h"
#include "GameState.h"
#include "raylib.h"

class Game final
{
public:
    Game(const std::string& path, const std::string& playerName);
    ~Game() = default;
    void loadLevel(const std::string& path, const std::string& playerName);
    void run();
    void handleLeftMouseButtonClick(const Vector2& mousePosition);
    void handleRightMouseButtonClick(const Vector2& mousePosition);
    void update(double deltaTime);
    void addBackgroundEntity(const std::shared_ptr<Sprite>& entity);
    void addForegroundEntity(const std::shared_ptr<Sprite>& entity);
    void handleInputEvents();

private:
    GameState m_gameState;
    GameBoard m_gameBoard;
    Renderer m_renderer;
    std::vector<std::shared_ptr<Sprite>> m_backgroundSprites;
    std::vector<std::shared_ptr<Sprite>> m_foregroundSprites;
};
