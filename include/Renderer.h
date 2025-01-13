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
    static void renderAll(const std::vector<std::shared_ptr<Sprite>>& sprites)
    {
        for (const auto& sprite : sprites)
            render(sprite);
    }

    // Render a single sprite
    static void render(const std::shared_ptr<Sprite>& sprite)
    {
        if (!sprite->getRenderFlag())
            return;

        Rectangle spriteRect = sprite->getRect(); // Source rectangle from sprite
        Vector2 spriteCenter = { spriteRect.width / 2.0f, spriteRect.height / 2.0f };

        if (sprite->getRotation() != 0.0f)
        {
            // Define the destination rectangle
            Rectangle dest = {
                spriteRect.x + spriteCenter.x,  // Center of the sprite on screen (X)
                spriteRect.y + spriteCenter.y,  // Center of the sprite on screen (Y)
                spriteRect.width,               // Width
                spriteRect.height               // Height
            };

            BeginShaderMode(sprite->getShader());
            DrawTexturePro(
                sprite->getTexture(),            // Texture to draw
                spriteRect,                      // Source rectangle
                dest,                            // Destination rectangle
                spriteCenter,                    // Origin of rotation (center of sprite)
                sprite->getRotation(),           // Rotation angle
                WHITE                            // Tint
            );
            EndShaderMode();
        }
        else
        {
            BeginShaderMode(sprite->getShader());
            DrawTexture(
                sprite->getTexture(),
                spriteRect.x,
                spriteRect.y,
                WHITE
            );
            EndShaderMode();
        }
    }
};

