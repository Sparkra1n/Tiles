#pragma once
#include <raylib.h>
#include <raymath.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "GameState.h"
#include "Modifier.h"
#include "CoordinateTransformer.h"

class Sprite
{
public:
    Sprite(const char* path, float speed = 0);
    ~Sprite();

    /**
     * @brief Callback when a sprite gets clicked
     */
    void onClick();

    /**
     * @brief Callback when the mouse hovers over the sprite
     */
    void onFocus();

    /**
     * Callback when the mouse stops hovering over the sprite
     */
    void onBlur();
    
    void setGameBoardCoordinates(Vector2 gameBoardCoordinates);
    void setGameBoardCoordinates(int x, int y);
    Vector2 getGameBoardCoordinates() const;
    void setWindowCoordinates(Vector2 windowCoordinates);
    void setWindowCoordinates(float x, float y);
    void setWindowXCoordinate(float value);
    void setWindowYCoordinate(float value);
    void setRotation(float degrees);
    void show();
    void hide();
    void resetSurface();
    void update(const GameState& state);
    float getRotation() const;
    bool getRenderFlag() const;
    Rectangle getRect() const;
    Vector2 getWindowCoordinates() const;
    Texture2D getTexture() const;
    Shader getShader() const;
    void removeModifierByName(const std::string& name);
    void applyAllModifiers();

    /**
     * @brief Push a new modifier onto the stack and rebuilds the modifier stack
     * @param Modifier new modifier
     */
    void pushModifier(const Modifier& modifier);

    /**
     * @brief Removes the modifier on the top of the stack and rebuilds the modifier stack
     * @return Removed modifier 
     */
    Modifier popModifier();

    /**
     * @brief Set a path that will be traversed by the sprite
     * @param path vector of Vector2 in GameBoard coordinates
     */
    void walkPath(const std::vector<Vector2>& path);

protected:
    bool m_renderFlag{};
    Rectangle m_rect{};
    Texture2D m_texture;
    Texture2D m_textureOriginal;                  // Used to restore a Sprite to original state
    Shader m_shader;
    Vector4 m_colorOffset{};
    std::vector<Modifier> m_modifierStack;        // Collection of active modifiers
    std::vector<Vector2> m_path;                  // Collection of points it will walk to
    float m_speed;
    float m_rotation{};
};
