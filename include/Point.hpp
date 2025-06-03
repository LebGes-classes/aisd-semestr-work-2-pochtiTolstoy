#pragma once

#include "Constants.hpp"
#include <iostream>

struct Point {
  double x_;
  double y_;

  Point() : x_{0.0}, y_{0.0} {}

  Point(double x, double y) : x_{x}, y_{y} {}

  void display() const { std::cout << "Point : (" << x_ << ", " << y_ << ")"; }

  bool operator==(const Point &other) const {
    return std::abs(x_ - other.x_) < EPS && std::abs(y_ - other.y_) < EPS;
  }

  bool operator!=(const Point &other) const { return !(*this == other); }

  Point operator+(const Point &other) const noexcept {
    return Point(x_ + other.x_, y_ + other.y_);
  }

  Point operator-(const Point &other) const noexcept {
    return Point(x_ - other.x_, y_ - other.y_);
  }
};
