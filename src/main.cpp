#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

#define SF_WIDTH 1900
#define SF_HEIGHT 1000
#define SF_PADDING 100
#define SF_POINT_RADIUS 6.0
#define POINTS_NUMBER 50
#define EPS 1e-6

struct Point {
  double x_;
  double y_;

  Point(double x, double y) : x_{x}, y_{y} {}

  void display() const { std::cout << "Point : (" << x_ << ", " << y_ << ")"; }

  bool operator==(const Point &other) const {
    return std::abs(x_ - other.x_) < EPS && std::abs(y_ - other.y_) < EPS;
  }

  bool operator!=(const Point &other) const { return !(*this == other); }
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

  void display() const {
    for (const auto &point : set_) {
      point.display();
      std::cout << std::endl;
    }
  }

  const std::vector<Point> &getSet() { return set_; }

private:
  std::vector<Point> set_;

  void CheckSetSize() const {
    if (set_.size() < 3) {
      throw std::invalid_argument("Point set must contain at least 3 points!");
    }
  }
};

class ConvexHull {
public:
  ConvexHull(const std::vector<Point> &set) : set_{set} {
    findJarvisConvexHull();
  }

  std::vector<Point> getConvexHull() {
    if (size_ > set_.size()) {
      throw std::out_of_range("Convex hull size out of set range!");
    }
    std::vector<Point> result(set_.begin(), set_.begin() + size_);
    return result;
  }

private:
  std::vector<Point> set_;
  size_t size_;

  void findJarvisConvexHull() {
    const size_t n = set_.size();
    if (n < 3) {
      this->size_ = n;
      return;
    }

    size_t entry_idx = getEntryPointIdx();
    std::swap(set_[0], set_[entry_idx]);
    Point p0 = set_[0];
    Point pi = p0;
    int k = -1;
    do {
      ++k;
      for (size_t i = k; i != set_.size(); ++i) {
        if (JarvisCompare(set_[i], set_[k], pi)) {
          std::swap(set_[i], set_[k]);
        }
      }
      pi = set_[k];
    } while (pi != p0);

    this->size_ = k + 1;
  }

  size_t getEntryPointIdx() {
    size_t res = 0;
    for (size_t i = 1; i != set_.size(); ++i) {
      if (set_[i].y_ < set_[res].y_ ||
          (std::abs(set_[i].y_ - set_[res].y_) < EPS &&
           set_[i].x_ < set_[res].x_)) {
        res = i;
      }
    }
    return res;
  }

  bool JarvisCompare(const Point &candidate, const Point &current_candidate,
                     const Point &current_point) {
    double cross = crossProduct(current_point, current_candidate, candidate);
    if (std::abs(cross) < EPS) {
      return (distanceSquared(current_point, candidate) >
              distanceSquared(current_point, current_candidate));
    }
    return cross > 0;
  }

  double crossProduct(const Point &a, const Point &b, const Point &c) {
    return (b.x_ - a.x_) * (c.y_ - a.y_) - (b.y_ - a.y_) * (c.x_ - a.x_);
  }

  double distanceSquared(const Point &a, const Point &b) {
    double dx = a.x_ - b.x_;
    double dy = a.y_ - b.y_;
    return dx * dx + dy * dy; // ?
  }
};

std::vector<Point> generate_points(size_t size, double width = SF_WIDTH,
                                   double height = SF_HEIGHT,
                                   double padding = SF_PADDING) {
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
  // PointSet pointSet({{1316.96, 827.68}, {1257.43, 302.405}, {1438.57,
  // 439.66}});
  // PointSet pointSet(generate_points(POINTS_NUMBER));
  // PointSet pointSet(generate_points(4));
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> pt_dist(3, 10);
  PointSet pointSet(generate_points(pt_dist(gen)));
  pointSet.display();
  ConvexHull hull(pointSet.getSet());
  const auto &vec_hull = hull.getConvexHull();
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
