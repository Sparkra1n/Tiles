
#include <memory>
#include <vector>
#include "Sprite.h"
#include <raylib.h>

class Renderer
{
public:
    Renderer() = default;
    ~Renderer() = default;

    // Clears the screen with a given color
    static void clear(Color color = RAYWHITE)
    {
        BeginDrawing();
        ClearBackground(color);
        EndDrawing();
    }

    // Renders all sprites in a layer
    static void renderAll(const std::vector<std::shared_ptr<Sprite>>& entities)
    {
        for (const auto& entity : entities)
            render(entity);
    }

    // Renders a single sprite
    static void render(const std::shared_ptr<Sprite>& entity)
    {
        if (!entity->getRenderFlag())
            return;

        Rectangle entityRect = entity->getRect();
        BeginShaderMode(entity->getShader());
        DrawTexture(
            entity->getTexture(),
            static_cast<int>(entityRect.x),
            static_cast<int>(entityRect.y),
            WHITE
        );
        EndShaderMode();
    }
};

