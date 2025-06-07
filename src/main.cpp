#include "../include/algorithm/ChanConvexHull.hpp"
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
  Button showMeshButton("Show mesh", font, {280, SF_HEIGHT - 70}, {250, 50});

  PointSet pointSet;
  pointSet.generate_points(POINTS_NUMBER);
  ChanConvexHull hull(pointSet.get_set());

  // JarvisConvexHull hull(pointSet.get_set());
  // GrahamConvexHull hull(pointSet.get_set());
  std::vector<sf::CircleShape> draw_points_vec;
  std::vector<sf::VertexArray> polygons;

  sf::VertexArray hull_lines;
  auto regenerate_set = [&]() {
    pointSet.generate_points(POINTS_NUMBER);
    hull = ChanConvexHull(pointSet.get_set());
    // hull = GrahamConvexHull(pointSet.get_set());
    // hull = JarvisConvexHull(pointSet.get_set());

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

    size_t partitions_number = hull.getPartitionsNumber();
    polygons.clear();
    polygons.resize(partitions_number);
    for (size_t polygon_idx = 0; polygon_idx < partitions_number;
         ++polygon_idx) {
      const auto &polygon_mesh = hull.getGrahamPartition(polygon_idx);
      polygons[polygon_idx] =
          sf::VertexArray(sf::LineStrip, polygon_mesh.size() + 1);
      if (!polygon_mesh.empty()) {
        for (size_t vertex_idx = 0; vertex_idx < polygon_mesh.size();
             ++vertex_idx) {
          polygons[polygon_idx][vertex_idx] =
              sf::Vertex(sf::Vector2f(polygon_mesh[vertex_idx].x_,
                                      polygon_mesh[vertex_idx].y_),
                         sf::Color(234, 239, 239));
          // TODO :
          std::cout << polygon_mesh[vertex_idx].x_ << ", "
                    << polygon_mesh[vertex_idx].y_ << std::endl;
        }
        polygons[polygon_idx][polygon_mesh.size()] =
            sf::Vector2f(polygon_mesh[0].x_, polygon_mesh[0].y_);
      }
    }
    const auto &vec_hull = hull.getChanHull();
    hull_lines = sf::VertexArray(sf::LineStrip, vec_hull.size() + 1);
    if (!vec_hull.empty()) {
      for (size_t i = 0; i < vec_hull.size(); ++i) {
        hull_lines[i] = sf::Vertex(sf::Vector2f(vec_hull[i].x_, vec_hull[i].y_),
                                   sf::Color(0, 255, 0));
      }
      hull_lines[vec_hull.size()] =
          sf::Vector2f(vec_hull[0].x_, vec_hull[0].y_);
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

  bool show_mesh = false;
  bool show_hull = true;
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
            // size_t points_number = pointSet.size();
            // size_t hull_size = hull.size();
            // double percentage =
            //     (static_cast<double>(hull_size) * 100.0) / points_number;
            // std::cout << "points number : " << points_number << std::endl;
            // std::cout << "hull size     : " << hull_size << std::endl;
            // std::cout << "percentage    : " << percentage << std::endl;
          } else if (showMeshButton.contains(mousePos)) {
            show_mesh = !show_mesh;
            show_hull = !show_hull;
          }
        }
      }
    }
    window.clear(sf::Color(51, 52, 70));
    window.draw(paddingArea);
    nextButton.draw(window);
    showMeshButton.draw(window);
    for (const auto &draw_point : draw_points_vec) {
      window.draw(draw_point);
    }

    if (show_mesh) {
      size_t partitions_number = hull.getPartitionsNumber();
      for (size_t polygon_idx = 0; polygon_idx < partitions_number;
           ++polygon_idx) {
        window.draw(polygons[polygon_idx]);
      }
    }
    if (show_hull) {
      if (hull.size() != 0) {
        window.draw(hull_lines);
      }
    }
    window.display();
  }
}
