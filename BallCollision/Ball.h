#pragma once

#include "SFML/Graphics.hpp"


/// <summary>
/// Represents 2D ball
/// </summary>
struct Ball
{
    sf::Vector2f p;
    sf::Vector2f dir;
    float r = 0;
    float speed = 0;
};