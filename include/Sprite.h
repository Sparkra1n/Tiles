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

class Sprite
{
public:
    Sprite(const char* path, float speed = 0);
    ~Sprite();
    void onClick();
    void onFocus();
    void onBlur();
    void setWindowCoordinates(Vector2 windowCoordinates);
    void setWindowXCoordinate(float value);
    void setWindowYCoordinate(float value);
    void show();
    void hide();
    void resetSurface();
    void update(const GameState& state);
    bool getRenderFlag() const;
    Rectangle getRect() const;
    Vector2 getWindowCoordinates() const;
    Texture2D getTexture() const;
    Shader getShader() const;

    void removeModifierByName(const std::string& name);
    void applyAllModifiers();
    void pushModifier(const Modifier& modifier);
    Modifier popModifier();

protected:
    bool m_renderFlag{};
    Rectangle m_rect{};
    Texture2D m_texture{};
    Texture2D m_textureOriginal;                  // Used to restore a Sprite to original state
    Shader m_shader;
    Vector4 m_colorOffset{};
    std::vector<Modifier> m_modifierStack;        // Collection of active modifiers
    std::vector<Vector2> m_path;                  // Collection of points it will walk to
    float m_speed;
};
