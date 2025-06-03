#include "../include/ConvexHull.hpp"
#include "../include/PointSet.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <iostream>
#include <vector>

int main() {
  // PointSet pointSet({{1316.96, 827.68}, {1257.43, 302.405}, {1438.57,
  // 439.66}});
  PointSet pointSet;
  pointSet.generate_points(POINTS_NUMBER);
  // PointSet pointSet(generate_points(4));
  // std::random_device rd;
  // std::mt19937 gen(rd());
  // std::uniform_int_distribution<int> pt_dist(3, 10);
  // PointSet pointSet(generate_points(pt_dist(gen)));
  pointSet.display();
  ConvexHull hull(pointSet.get_set());
  const auto &vec_hull = hull.get_convex_hull();
  std::cout << "hull size : " << vec_hull.size() << std::endl;
  sf::VertexArray lines(sf::LineStrip, vec_hull.size() + 1); // +1 ???
  if (!vec_hull.empty()) {
    for (size_t i = 0; i < vec_hull.size(); ++i) {
      lines[i] = sf::Vertex(sf::Vector2f(vec_hull[i].x_, vec_hull[i].y_),
                            sf::Color(234, 239, 239));
    }
    lines[vec_hull.size()] = sf::Vector2f(vec_hull[0].x_, vec_hull[0].y_);
  }

  // SFML
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8; // Request 8x anti-aliasing

  // Create window with these settings
  sf::RenderWindow window(sf::VideoMode(SF_WIDTH, SF_HEIGHT),
                          "Convex Hull - SFML", sf::Style::Default, settings);

  std::vector<sf::CircleShape> draw_points_vec;
  constexpr double point_radius = SF_POINT_RADIUS;
  for (const auto &point : pointSet.get_set()) {
    sf::CircleShape draw_point(point_radius);
    draw_point.setPointCount(100);
    draw_point.setFillColor(sf::Color(127, 140, 170));
    draw_point.setOutlineColor(sf::Color::Black);
    draw_point.setOutlineThickness(2);
    draw_point.setPosition(static_cast<float>(point.x_) - point_radius,
                           static_cast<float>(point.y_) - point_radius);
    draw_points_vec.push_back(draw_point);
  }

  sf::RectangleShape paddingArea(
      sf::Vector2f(SF_WIDTH - (SF_PADDING * 2), SF_HEIGHT - (SF_PADDING * 2)));
  paddingArea.setPosition(SF_PADDING, SF_PADDING);
  paddingArea.setFillColor(sf::Color::Transparent);
  paddingArea.setOutlineColor(sf::Color(35, 36, 48));
  paddingArea.setOutlineThickness(2);

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }
    window.clear(sf::Color(51, 52, 70));
    window.draw(paddingArea);
    for (const auto &draw_point : draw_points_vec) {
      window.draw(draw_point);
    }
    if (!vec_hull.empty()) {
      window.draw(lines);
    }
    window.display();
  }
}
