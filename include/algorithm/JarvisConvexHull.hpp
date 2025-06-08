#pragma once

#include "ConvexHullBase.hpp"
#include "Point.hpp"
#include <cassert>
#include <vector>

class JarvisConvexHull : public ConvexHullBase {
public:
  JarvisConvexHull(const std::vector<Point> &points) : ConvexHullBase(points) {
    computeHull();
  }

  const std::vector<Point> &getHull() const override { return hull_; }

  size_t size() const override { return hull_size_; }

private:
  size_t hull_size_;

  void computeHull() override {
    const size_t points_number = points_.size();
    if (points_number < 3) {
      hull_ = points_;
      hull_size_ = points_number;
      return;
    }

    size_t leftmost = get_entry_point_idx();
    size_t current = leftmost;
    hull_.clear();
    do {
      hull_.push_back(points_[current]);
      size_t next = (current + 1) % points_number;
      for (size_t i = 0; i < points_number; ++i) {
        if (jarvis_compare(points_[i], points_[next], points_[current])) {
          next = i;
        }
      }
      current = next;
    } while (current != leftmost);
    hull_size_ = hull_.size();
    assert(hull_[0] == points_[leftmost]);
    assert(hull_.size() <= points_.size());
  }

  size_t get_entry_point_idx() {
    size_t entry_point_idx = 0;
    for (size_t i = 1; i != points_.size(); ++i) {
      bool y_less = (points_[i].y_ < points_[entry_point_idx].y_);
      bool x_less = (points_[i].x_ < points_[entry_point_idx].x_);
      bool y_eq = (std::abs(points_[i].y_ - points_[entry_point_idx].y_) < EPS);
      if (y_less || (y_eq && x_less))
        entry_point_idx = i;
    }
    return entry_point_idx;
  }

  bool jarvis_compare(const Point &candidate, const Point &current_candidate,
                      const Point &current_point) {
    double cross = cross_product(current_point, current_candidate, candidate);
    if (std::abs(cross) < EPS) {
      return (distance_squared(current_point, candidate) >
              distance_squared(current_point, current_candidate));
    }
    return cross < 0;
  }

  double cross_product(const Point &a, const Point &b, const Point &c) {
    return (b.x_ - a.x_) * (c.y_ - a.y_) - (b.y_ - a.y_) * (c.x_ - a.x_);
  }

  double distance_squared(const Point &a, const Point &b) {
    double dx = a.x_ - b.x_;
    double dy = a.y_ - b.y_;
    return dx * dx + dy * dy;
  }
};
