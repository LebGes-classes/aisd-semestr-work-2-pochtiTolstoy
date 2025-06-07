#include "../include/algorithm/GrahamConvexHull.hpp"
#include "../include/algorithm/JarvisConvexHull.hpp"
#include "../include/algorithm/PointSet.hpp"
#include "../include/graphics/Button.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <iostream>
#include <vector>

int main() {
  sf::Font font;
  if (!font.loadFromFile("../res/AgaveNerdFontMono-Bold.ttf")) {
    std::cerr << "Failed to load font!" << std::endl;
    return EXIT_FAILURE;
  }

  Button nextButton("Next generation", font, {20, SF_HEIGHT - 70}, {250, 50});

  PointSet pointSet;
  pointSet.generate_points(3);

  GrahamConvexHull hull(pointSet.get_set());
  std::vector<sf::CircleShape> draw_points_vec;
  sf::VertexArray lines;

  auto regenerate_set = [&]() {
    pointSet.generate_points();
    hull = GrahamConvexHull(pointSet.get_set());

    draw_points_vec.clear();
    for (const auto &point : pointSet.get_set()) {
      sf::CircleShape draw_point(SF_POINT_RADIUS);
      draw_point.setPointCount(100);
      draw_point.setFillColor(sf::Color(127, 140, 170));
      draw_point.setOutlineColor(sf::Color::Black);
      draw_point.setOutlineThickness(2);
      draw_point.setPosition(static_cast<float>(point.x_) - SF_POINT_RADIUS,
                             static_cast<float>(point.y_) - SF_POINT_RADIUS);
      draw_points_vec.push_back(draw_point);
    }

    const auto &vec_hull = hull.getConvexHull();
    lines = sf::VertexArray(sf::LineStrip, vec_hull.size() + 1);
    if (!vec_hull.empty()) {
      for (size_t i = 0; i < vec_hull.size(); ++i) {
        lines[i] = sf::Vertex(sf::Vector2f(vec_hull[i].x_, vec_hull[i].y_),
                              sf::Color(234, 239, 239));
      }
      lines[vec_hull.size()] = sf::Vector2f(vec_hull[0].x_, vec_hull[0].y_);
    }
  };

  regenerate_set();

  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;
  sf::RenderWindow window(sf::VideoMode(SF_WIDTH, SF_HEIGHT),
                          "Convex hull - SFML", sf::Style::Default, settings);
  window.setFramerateLimit(60);

  sf::RectangleShape paddingArea(
      sf::Vector2f(SF_WIDTH - (SF_PADDING * 2), SF_HEIGHT - (SF_PADDING * 2)));
  paddingArea.setPosition(SF_PADDING, SF_PADDING);
  paddingArea.setFillColor(sf::Color::Transparent);
  paddingArea.setOutlineColor(sf::Color(35, 36, 48));
  paddingArea.setOutlineThickness(2);

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }

      if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left) {
          sf::Vector2f mousePos = window.mapPixelToCoords(
              {event.mouseButton.x, event.mouseButton.y});
          if (nextButton.contains(mousePos)) {
            regenerate_set();
          }
        }
      }
    }
    window.clear(sf::Color(51, 52, 70));
    window.draw(paddingArea);
    nextButton.draw(window);
    for (const auto &draw_point : draw_points_vec) {
      window.draw(draw_point);
    }

    if (!hull.getConvexHull().empty()) {
      window.draw(lines);
    }
    window.display();
  }
}
