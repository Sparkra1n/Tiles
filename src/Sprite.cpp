#include "Sprite.h"

Sprite::Sprite(const char* path, const CoordinateTransformer* transformer, float speed)
    : m_renderFlag(true), m_transformer(transformer), m_speed(speed)
{
    std::cout << path << "\n";
    m_textureOriginal = LoadTexture(path);
    m_texture = m_textureOriginal;
    m_rect = {
        0.0f,
        0.0f,
        static_cast<float>(m_texture.width),
        static_cast<float>(m_texture.height)
    };

    m_shader = LoadShader(nullptr, "resources/ColorModifier.fs");
    if (m_shader.id == 0)  // Shader loading failed
    {
        TraceLog(LOG_ERROR, "Failed to load shader. Defaulting to no shader.");
    }
    else
    {
        Vector4 initialOffset = { 0.0f, 0.0f, 0.0f, 0.0f };
        int loc = GetShaderLocation(m_shader, "colorOffset");
        SetShaderValue(m_shader, loc, &initialOffset, SHADER_UNIFORM_VEC4);
    }
}

Sprite::~Sprite()
{
    UnloadTexture(m_textureOriginal);
    UnloadTexture(m_texture);
    UnloadShader(m_shader);
}

void Sprite::setGameBoardCoordinates(Vector2 gameBoardCoordinates)
{
    if (!m_transformer)
        return;

    Vector2 windowCoordinates = m_transformer->toWindowCoordinates(gameBoardCoordinates, m_rect);
    setWindowCoordinates(windowCoordinates);
}

void Sprite::setGameBoardCoordinates(int x, int y)
{
    if (!m_transformer)
        return;

    Vector2 gameBoardCoordinates = Vector2{ static_cast<float>(x), static_cast<float>(y) };
    Vector2 windowCoordinates = m_transformer->toWindowCoordinates(gameBoardCoordinates, m_rect);
    setWindowCoordinates(windowCoordinates);
}

Vector2 Sprite::getGameBoardCoordinates() const
{
    if (!m_transformer)
        return {};

    return m_transformer->toGameBoardCoordinates({ m_rect.x, m_rect.y }, m_rect);
}

void Sprite::update(const GameState& state)
{
    if (m_speed == 0.0f)
        return;

    if (!m_path.empty())
    {
        Vector2 target = m_path.front();
        Vector2 coordinates = { m_rect.x, m_rect.y };

        // Move horizontally if x coordinates are different
        if (abs(target.x - m_rect.x) > 1)
        {
            const int sign = coordinates.x < target.x ? 1 : -1;
            coordinates.x += m_speed * state.deltaTime * sign;
            setWindowCoordinates(coordinates);
        }

        // Move vertically if y coordinates are different
        else if (abs(target.y - m_rect.y) > 1)
        {
            const int sign = coordinates.y < target.y ? 1 : -1;
            coordinates.y += m_speed * state.deltaTime * sign;
            setWindowCoordinates(coordinates);
        }

        // Reached the checkpoint
        else
        {
            setWindowCoordinates(target);
            m_path.erase(m_path.begin());
        }
    }
}

void Sprite::pushModifier(const Modifier& modifier)
{
    m_modifierStack.push_back(modifier);     // Add the modifier to the back
    applyAllModifiers();                     // Apply all modifiers in order
}

void Sprite::removeModifierByName(const std::string& name)
{
    for (int i = 0; i != m_modifierStack.size(); ++i)
    {
        if (m_modifierStack[i].name == name)
        {
            m_modifierStack.erase(std::next(m_modifierStack.begin(), i));
            break;
        }
    }
    applyAllModifiers();
}

Modifier Sprite::popModifier()
{
    if (!m_modifierStack.empty())
    {
        Modifier topModifier = m_modifierStack.back();
        m_modifierStack.pop_back();

        // Apply remaining modifiers in order
        applyAllModifiers();
        return topModifier;
    }

    return {};
}

void Sprite::applyAllModifiers()
{
    // Combine all modifiers into one
    Vector4 combinedOffset{};
    for (const auto& modifier : m_modifierStack)
        combinedOffset += modifier.rgba;

    // Clamp values to ensure valid range
    combinedOffset.x = std::clamp(combinedOffset.x, 0.0f, 255.0f);
    combinedOffset.y = std::clamp(combinedOffset.y, 0.0f, 255.0f);
    combinedOffset.z = std::clamp(combinedOffset.z, 0.0f, 255.0f);
    combinedOffset.w = std::clamp(combinedOffset.w, 0.0f, 255.0f);

    // Normalize
    combinedOffset /= 255.0f;

    int loc = GetShaderLocation(m_shader, "colorOffset");
    SetShaderValue(m_shader, loc, &combinedOffset, SHADER_UNIFORM_VEC4);
}

void Sprite::walkPath(const std::vector<Vector2>& path)
{
    m_path = path;
}

void Sprite::resetSurface()
{
    UnloadTexture(m_texture);
    m_texture = m_textureOriginal;
}

bool Sprite::getRenderFlag() const
{
    return m_renderFlag;
}

Shader Sprite::getShader() const
{
    return m_shader;
}

void Sprite::onClick()
{

}

void Sprite::onFocus()
{
    pushModifier({
        "Highlight",
        { 0, 50, 0, 0 }
    });
    applyAllModifiers();
}

void Sprite::onBlur()
{
    removeModifierByName("Highlight");
}

Rectangle Sprite::getRect() const
{
    return m_rect;
}

Texture2D Sprite::getTexture() const
{
    return m_texture;
}

void Sprite::setWindowCoordinates(const Vector2 windowCoordinates)
{
    m_rect.x = windowCoordinates.x;
    m_rect.y = windowCoordinates.y;
}

void Sprite::setWindowXCoordinate(const float value)
{
    m_rect.x = value;
}

void Sprite::setWindowYCoordinate(const float value)
{
    m_rect.y = value;
}

void Sprite::show()
{
    m_renderFlag = true;
}

void Sprite::hide()
{
    m_renderFlag = false;
}

Vector2 Sprite::getWindowCoordinates() const
{
    return { m_rect.x, m_rect.y };
}

void Sprite::setRotation(float degrees)
{
    m_rotation = degrees;
}

float Sprite::getRotation() const
{
    return m_rotation;
}