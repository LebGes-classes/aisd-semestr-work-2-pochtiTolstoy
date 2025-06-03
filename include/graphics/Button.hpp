#pragma once

#include "../sfml.hpp"

class Button {
public:
  Button(const std::string &text, const sf::Font &font,
         const sf::Vector2f &position, const sf::Vector2f &size)
      : font_{font} {
    shape_.setSize(size);
    shape_.setPosition(position);
    shape_.setFillColor(sf::Color(70, 130, 180)); // Steel blue
    shape_.setOutlineThickness(2);
    shape_.setOutlineColor(sf::Color::White);

    text_.setString(text);
    text_.setFont(font);
    text_.setCharacterSize(20);
    text_.setFillColor(sf::Color::White);

    sf::FloatRect textRect = text_.getLocalBounds();
    text_.setOrigin(textRect.left + textRect.width / 2.0f,
                    textRect.top + textRect.height / 2.0f);
    text_.setPosition(position.x + size.x / 2.0f, position.y + size.y / 2.0f);
  }

  bool contains(const sf::Vector2f &point) const {
    return shape_.getGlobalBounds().contains(point);
  }

  void draw(sf::RenderWindow &window) const {
    window.draw(shape_);
    window.draw(text_);
  }

private:
  sf::RectangleShape shape_;
  sf::Text text_;
  const sf::Font &font_;
};
