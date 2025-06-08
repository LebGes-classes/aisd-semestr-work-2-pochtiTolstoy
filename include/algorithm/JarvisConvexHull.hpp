#pragma once

#include "ConvexHullBase.hpp"
#include "Point.hpp"
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
    const size_t set_size = points_.size();
    if (set_size < 3) {
      this->hull_size_ = set_size;
      this->hull_.assign(points_.begin(), points_.begin() + hull_size_);
      return;
    }
    // TODO : jarvis swap entry point, should be at [0] index
    std::swap(points_[0], points_[get_entry_point_idx()]);
    Point entry_point = points_[0];
    Point current_point = entry_point;
    size_t set_cell_to_update = 0;
    do {
      for (size_t candidate = set_cell_to_update; candidate != set_size;
           ++candidate) {
        // set_[set_cell_to_update] - best candidate
        // set_[cnadidate] - current candidate
        if (jarvis_compare(points_[candidate], points_[set_cell_to_update],
                           current_point)) {
          std::swap(points_[candidate], points_[set_cell_to_update]);
        }
      }
      current_point = points_[set_cell_to_update++];
    } while (current_point != entry_point);
    this->hull_size_ = set_cell_to_update;
    this->hull_.assign(points_.begin(),
                       points_.begin() + std::min(hull_size_, points_.size()));
  }

  size_t get_entry_point_idx() {
    size_t entry_point_idx = 0;
    for (size_t i = entry_point_idx + 1; i != points_.size(); ++i) {
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
    if (candidate == current_candidate)
      return false;
    double cross = cross_product(current_point, current_candidate, candidate);
    if (std::abs(cross) < EPS) {
      return (distance_squared(current_point, candidate) >
              distance_squared(current_point, current_candidate));
    }
    return cross > 0;
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
