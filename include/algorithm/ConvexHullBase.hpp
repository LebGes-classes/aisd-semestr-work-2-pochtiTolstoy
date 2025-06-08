#pragma once
#include "Point.hpp"
#include <vector>

class ConvexHullBase {
public:
  virtual ~ConvexHullBase() = default;

  virtual void computeHull() = 0;

  virtual const std::vector<Point> &getHull() const = 0;

  virtual size_t size() const = 0;

protected:
  explicit ConvexHullBase(const std::vector<Point> &points) : points_(points) {}

  std::vector<Point> points_;
  std::vector<Point> hull_;
};
