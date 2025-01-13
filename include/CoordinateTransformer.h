// CoordinateTranslator.h
#pragma once
#include "raylib.h"
#include <cmath>
#include <iostream>

class CoordinateTransformer 
{
public:
    static Vector2 toGameBoardCoordinates(Vector2 windowCoordinates) 
    {
        // Offset the center by half the sprite's width and height
        float x = std::floor(windowCoordinates.x / m_tileSize);
        float y = std::floor(windowCoordinates.y / m_tileSize);
        return { x, y };
    }

    static Vector2 toWindowCoordinates(Vector2 gameBoardCoordinates, Rectangle rectangle) 
    {
        float offsetX = rectangle.width / 2.0f;
        float offsetY = rectangle.height / 2.0f;
        float x = gameBoardCoordinates.x * m_tileSize + offsetX;
        float y = gameBoardCoordinates.y * m_tileSize + offsetY;
        return { x, y };
    }

    static Vector2 toTileOrigin(Vector2 windowCoordinates) 
    {
        float x = std::floor(windowCoordinates.x / m_tileSize) * m_tileSize;
        float y = std::floor(windowCoordinates.y / m_tileSize) * m_tileSize;
        return { x, y };
    }

    static Vector2 toTileCenter(Vector2 windowCoordinates)
    {
        float offset = m_tileSize / 2.0f;
        float x = windowCoordinates.x * m_tileSize + offset;
        float y = windowCoordinates.y * m_tileSize + offset;
        return { x, y };
    }

private:
    CoordinateTransformer() = default;
    static constexpr int m_tileSize = 128;
};




