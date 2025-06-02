#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

#define SF_WIDTH 1900
#define SF_HEIGHT 1000

struct Point {
  double x_;
  double y_;

  Point(double x, double y) : x_{x}, y_{y} {}
};

class PointSet {
public:
  PointSet() = default;

  PointSet(const std::vector<Point> &set) : set_{set} { CheckSetSize(); }

  PointSet(std::vector<Point> &&set) : set_{std::move(set)} { CheckSetSize(); }

  void movePointSet(double x, double y) {
    for (auto &point : set_) {
      point.x_ += x;
      point.y_ += y;
    }
  }

  std::vector<Point> &getSet() { return set_; }

private:
  std::vector<Point> set_;

  void CheckSetSize() const {
    if (set_.size() < 3) {
      throw std::invalid_argument("Point set must contain at least 3 points!");
    }
  }
};

std::vector<Point> generate_points(size_t size, double width = SF_WIDTH,
                                   double height = SF_HEIGHT,
                                   double padding = 100.0) {
  if (size < 3) {
    throw std::invalid_argument("Should be generated at least 3 points!");
  }
  if (width < 200 || width > 1920) {
    throw std::invalid_argument("Wrong width size!");
  }
  if (height < 200 || height > 1080) {
    throw std::invalid_argument("Wrong height size!");
  }
  if (padding < 0 || padding >= width / 2 || padding >= height / 2) {
    throw std::invalid_argument("Invalid padding value!");
  }
  std::vector<Point> result;
  result.reserve(size);
  std::random_device rd;
  std::mt19937 gen(rd());
  double min_x = padding;
  double max_x = width - padding;
  double min_y = padding;
  double max_y = height - padding;
  std::uniform_real_distribution<double> x_dist(min_x, max_x);
  std::uniform_real_distribution<double> y_dist(min_y, max_y);
  for (size_t i = 0; i != size; ++i) {
    result.emplace_back(x_dist(gen), y_dist(gen));
  }
  return result;
}

int main() {
  PointSet pointSet(generate_points(50));

  // SFML
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8; // Request 8x anti-aliasing

  // Create window with these settings
  sf::RenderWindow window(sf::VideoMode(SF_WIDTH, SF_HEIGHT),
                          "Convex Hull - SFML", sf::Style::Default, settings);

  std::vector<sf::CircleShape> draw_points_vec;
  constexpr double point_radius = 8.0;
  for (const auto &point : pointSet.getSet()) {
    sf::CircleShape draw_point(point_radius);
    draw_point.setPointCount(100);
    draw_point.setFillColor(sf::Color(127, 140, 170));
    draw_point.setOutlineColor(sf::Color::Black);
    draw_point.setOutlineThickness(2);
    draw_point.setPosition(static_cast<float>(point.x_) - point_radius,
                           static_cast<float>(point.y_) - point_radius);
    draw_points_vec.push_back(draw_point);
  }

  sf::RectangleShape paddingArea(sf::Vector2f(SF_WIDTH - 200, SF_HEIGHT - 200));
  paddingArea.setPosition(100, 100);
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
    window.display();
  }
}
