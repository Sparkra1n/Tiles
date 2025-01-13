#pragma once
#include <raylib.h>
#include <string>

struct Modifier
{
    Modifier(std::string description, Vector4 rgba)
        : name(std::move(description)), rgba(rgba) {}

    Modifier() = default;

    static constexpr int colorClamp(const int value)
    {
        return value > 255 ? 255 : (value < 0 ? 0 : value);
    }

    std::string name;
    Vector4 rgba{};
};