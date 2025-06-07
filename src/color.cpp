//
// Created by dgmuller on 6/7/25.
//

#include "../include/color.hpp"

sf::Color farcical::GetColorByName(std::string_view name) {
    sf::Color color{sf::Color::Black};
    if(name == "black") {
        color = sf::Color::Black;
    }
    else if(name == "white") {
        color = sf::Color::White;
    }
    else if(name == "red") {
        color = sf::Color::Red;
    }
    else if(name == "green") {
        color = sf::Color::Green;
    }
    else if(name == "blue") {
        color = sf::Color::Blue;
    }
    else if(name == "yellow") {
        color = sf::Color::Yellow;
    }
    else if(name == "magenta") {
        color = sf::Color::Magenta;
    }
    else if(name == "cyan") {
        color = sf::Color::Cyan;
    }
    return color;
}

std::string_view farcical::GetColorName(sf::Color color) {
    std::string name{"unknown"};
    if(color == sf::Color::Black) {
        name = "black";
    }
    else if(color == sf::Color::White) {
        name = "white";
    }
    else if(color == sf::Color::Red) {
        name = "red";
    }
    else if(color == sf::Color::Green) {
        name = "green";
    }
    else if(color == sf::Color::Blue) {
        name = "blue";
    }
    else if(color == sf::Color::Yellow) {
        name = "yellow";
    }
    else if(color == sf::Color::Magenta) {
        name = "magenta";
    }
    else if(color == sf::Color::Cyan) {
        name = "cyan";
    }
    return std::string_view{name};
}
