//
// Created by dgmuller on 6/9/25.
//
#include "../include/input.hpp"

farcical::InputSystem::InputSystem(sf::RenderWindow& window, ui::Manager& manager):
    System(System::ID::InputSystem),
    window{window}, uiManager{manager} {
}

void farcical::InputSystem::Init() {
}

void farcical::InputSystem::Update() {
    if(window.isOpen()) {
      while(const std::optional event = window.pollEvent()) {
          if(event->is<sf::Event::Closed>()) {
              window.close();
              break;
          } // if(event == Closed)

          else if(const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {

          } // else if(event == KeyPressed)

          else if(const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {

          } // else if(event == MouseButtonPressed)
      } // while(event = pollEvent())
    } // if(window isOpen)
}

void farcical::InputSystem::Stop() {
}
