#pragma once

#include "Point.hpp"
#include <random>
#include <vector>

class PointSet {
public:
  PointSet() = default;

  PointSet(const std::vector<Point> &set) : set_{set} { check_set_size(); }

  PointSet(std::vector<Point> &&set) : set_{std::move(set)} {
    check_set_size();
  }

  void move_point_set(double x, double y) {
    for (auto &point : set_) {
      point.x_ += x;
      point.y_ += y;
    }
  }

  void display_log() const {
    for (const auto &point : set_) {
      point.display_log();
      std::cout << std::endl;
    }
  }

  const std::vector<Point> &get_set() { return set_; }

  size_t size() const { return set_.size(); }

  void generate_points() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> pt_dist(MIN_POINTS, MAX_POINTS);
    generate_points(pt_dist(gen));
  }

  void generate_points(size_t size, double width = SF_WIDTH,
                       double height = SF_HEIGHT, double padding = SF_PADDING) {
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
    set_.clear();
    set_.reserve(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    double min_x = padding;
    double max_x = width - padding;
    double min_y = padding;
    double max_y = height - padding;
    std::uniform_real_distribution<double> x_dist(min_x, max_x);
    std::uniform_real_distribution<double> y_dist(min_y, max_y);
    for (size_t i = 0; i != size; ++i) {
      set_.emplace_back(x_dist(gen), y_dist(gen));
    }
  }

private:
  std::vector<Point> set_;
  static constexpr size_t MIN_POINTS = 3;
  static constexpr size_t MAX_POINTS = 50;

  void check_set_size() const {
    if (set_.size() < 3) {
      throw std::invalid_argument("Point set must contain at least 3 points!");
    }
  }
};
