#pragma once

#include "Point.hpp"
#include <vector>

class JarvisConvexHull {
public:
  JarvisConvexHull(const std::vector<Point> &set) : set_{set} {
    find_jarvis_convex_hull();
  }

  std::vector<Point> getConvexHull() {
    if (hull_size_ > set_.size()) {
      throw std::out_of_range("Convex hull size out of set range!");
    }
    std::vector<Point> result(set_.begin(), set_.begin() + hull_size_);
    return result;
  }

  size_t size() const { return hull_size_; }

private:
  std::vector<Point> set_;
  size_t hull_size_;

  void find_jarvis_convex_hull() {
    const size_t set_size = set_.size();
    if (set_size < 3) {
      this->hull_size_ = set_size;
      return;
    }
    // TODO : jarvis swap entry point, should be at [0] index
    std::swap(set_[0], set_[get_entry_point_idx()]);
    Point entry_point = set_[0];
    Point current_point = entry_point;
    size_t set_cell_to_update = 0;
    do {
      for (size_t candidate = set_cell_to_update; candidate != set_size;
           ++candidate) {
        // set_[set_cell_to_update] - best candidate
        // set_[cnadidate] - current candidate
        if (jarvis_compare(set_[candidate], set_[set_cell_to_update],
                           current_point)) {
          std::swap(set_[candidate], set_[set_cell_to_update]);
        }
      }
      current_point = set_[set_cell_to_update++];
    } while (current_point != entry_point);
    this->hull_size_ = set_cell_to_update;
  }

  size_t get_entry_point_idx() {
    size_t entry_point_idx = 0;
    for (size_t i = entry_point_idx + 1; i != set_.size(); ++i) {
      bool y_less = (set_[i].y_ < set_[entry_point_idx].y_);
      bool x_less = (set_[i].x_ < set_[entry_point_idx].x_);
      bool y_eq = (std::abs(set_[i].y_ - set_[entry_point_idx].y_) < EPS);
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
