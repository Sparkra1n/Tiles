#pragma once
#include <raylib.h>

struct GameState
{
	GameState() = default;
	Vector2 mousePosition{};
	float deltaTime{};
};
