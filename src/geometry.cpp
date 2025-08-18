//
// Created by dgmuller on 8/2/25.
//
#include "../include/geometry.hpp"

bool farcical::IsPointWithinRect(sf::Vector2i point, sf::IntRect rect) {
    bool isInRect{false};
    if(point.x >= rect.position.x &&
       point.y >= rect.position.y &&
       point.x < rect.position.x + rect.size.x &&
       point.y < rect.position.y + rect.size.y) {
        isInRect = true;
    } // if in bounds
    return isInRect;
}