#pragma once

#include "ConvexHullBase.hpp"
#include "GrahamConvexHull.hpp"
#include "JarvisConvexHull.hpp"
#include <cassert>
#include <stdexcept>

class ChanConvexHull : public ConvexHullBase {
public:
  ChanConvexHull(const std::vector<Point> &points) : ConvexHullBase(points) {
    if (points.size() < 3) {
      throw std::invalid_argument("At least 3 points to build convex hull");
    }
    computeHull();
  }

  const std::vector<Point> &getHull() const override { return hull_; }

  std::vector<GrahamConvexHull> getGrahamPartitions() const {
    return graham_partitions_;
  }

  size_t getPartitionsNumber() const { return partitions_.size(); }

  std::vector<Point> getGrahamPartition(size_t idx) const {
    if (idx >= graham_partitions_.size()) {
      throw std::invalid_argument("Wrong index for partitions!");
    }
    return graham_partitions_[idx].getHull();
  }

  size_t size() const override { return hull_.size(); }

private:
  std::vector<std::vector<Point>> partitions_;
  std::vector<GrahamConvexHull> graham_partitions_;

  void computeHull() override {
    if (points_.size() < 6) {
      GrahamConvexHull graham_convex_hull(points_);
      hull_ = graham_convex_hull.getHull();
      return;
    }

    // TODO : delete debug vector
    JarvisConvexHull jarvis_convex_hull(points_);
    std::vector<Point> debug_vec = jarvis_convex_hull.getHull();

    // for (const auto &point : points_) {
    // point.display_log();
    // std::cout << std::endl;
    // }

    generatePartitions();
    for (size_t bucket_idx = 0; bucket_idx < partitions_.size(); ++bucket_idx) {
      graham_partitions_.push_back({partitions_[bucket_idx]});
    }

    Point entry_point = findEntryPoint();
    hull_.clear();
    hull_.push_back(entry_point);

    assert(hull_.front() == debug_vec[0]);

    size_t max_hull_size = points_.size();
    for (size_t step = 0; step < max_hull_size; ++step) {
      Point current_point = hull_.back();
      Point best_candidate;
      bool first = true;
      for (const auto &partition : graham_partitions_) {
        const auto &subhull = partition.getHull();
        if (subhull.empty())
          continue;
        Point candidate = findNextPointInPartition(subhull, current_point);
        if (first || jarvis_compare(candidate, best_candidate, current_point)) {
          best_candidate = candidate;
          first = false;
        }
      }
      if (best_candidate == entry_point) {
        break;
      }
      // assert(debug_vec[step + 1] == best_candidate);
      hull_.push_back(best_candidate);
    }
    // for (const auto &point : hull_) {
    // point.display_log();
    // std::cout << std::endl;
    // }
    // assert(hull_.size() == jarvis_convex_hull.size());
  }

  // Point findNextPointInPartition(const std::vector<Point> &hull,
  //                                const Point &p) {
  //   int left = 0;
  //   int right = hull.size();
  //   while (right - left > 1) {
  //     int mid = (left + right) / 2;
  //     int mid_next = (mid + 1) % hull.size();
  //     int mid_prev = (mid - 1 + hull.size()) % hull.size();
  //
  //     double cross_next = cross_product(p, hull[mid], hull[mid_next]);
  //     double cross_prev = cross_product(p, hull[mid_prev], hull[mid]);
  //
  //     if (cross_prev >= 0 && cross_next >= 0 && hull[mid] != p) {
  //       return hull[mid];
  //     }
  //
  //     double cross_left = cross_product(p, hull[left], hull[mid]);
  //     if (cross_left > 0) {
  //       if (cross_prev < 0)
  //         right = mid;
  //       else
  //         left = mid;
  //     } else {
  //       if (cross_prev >= 0)
  //         left = mid;
  //       else
  //         right = mid;
  //     }
  //   }
  //
  //   return cross_product(p, hull[left], hull[right % hull.size()]) > 0
  //              ? hull[left]
  //              : hull[right % hull.size()];
  // }
  Point findNextPointInPartitionLinear(const std::vector<Point> &hull,
                                       const Point &p) {
    int n = hull.size();
    if (n == 0)
      return Point(); // Возвращаем пустую точку при пустой оболочке
    if (n == 1)
      return hull[0]; // Если оболочка из одной точки

    // Проверка, является ли p точкой на оболочке
    for (int i = 0; i < n; i++) {
      if (hull[i] == p) {
        return hull[(i + 1) % n]; // Следующая точка в обходе
      }
    }

    // Линейный поиск точки с минимальным полярным углом
    int best_index = 0;
    for (int i = 1; i < n; i++) {
      if (jarvis_compare(hull[i], hull[best_index], p)) {
        best_index = i;
      }
    }
    return hull[best_index];
  }

  Point findNextPointInPartition(const std::vector<Point> &hull,
                                 const Point &p) {
    int n = hull.size();
    if (n == 0)
      return Point(); // Возвращаем пустую точку при пустой оболочке
    if (n == 1)
      return hull[0]; // Если оболочка из одной точки

    // Проверка, является ли p точкой на оболочке
    for (int i = 0; i < n; i++) {
      if (hull[i] == p) {
        return hull[(i + 1) % n]; // Следующая точка в обходе
      }
    }

    // Линейный поиск точки с минимальным полярным углом
    int best_index = 0;
    for (int i = 1; i < n; i++) {
      if (jarvis_compare(hull[i], hull[best_index], p)) {
        best_index = i;
      }
    }
    return hull[best_index];
  }
  /*
   *
     Point findNextPointInPartition(const std::vector<Point> &partition,
                                 const Point &current_point) {
    size_t size = partition.size();
    if (size == 1)
      return partition[0];
    if (size == 2) {
      return jarvis_compare(partition[1], partition[0], current_point)
                 ? partition[1]
                 : partition[0];
    }

    auto get = [&](size_t idx) { return partition[idx % size]; };

    size_t l = 0, r = size;
    while (r - l > 1) {
      size_t m = (l + r) / 2;
      Point mid = get(m);
      Point mid_next = get(m + 1);

      double orient = cross_product(current_point, mid, mid_next);
      if (orient < 0) {
        r = m;
      } else {
        l = m;
      }
    }

    Point p1 = get(l);
    Point p2 = get(r % size);
    return cross_product(current_point, p1, p2) > 0 ? p2 : p1;
  }
  */

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

  Point findEntryPoint() {
    Point entryPoint = partitions_[0][0];
    for (size_t row = 0; row < partitions_.size(); ++row) {
      Point candidate = findEntryPointInPartition(partitions_[row]);
      if (y_less_compare(candidate, entryPoint)) {
        entryPoint = candidate;
      }
    }
    return entryPoint;
  }

  Point findEntryPointInPartition(const std::vector<Point> &partition) {
    size_t entry_point_idx = 0;
    for (size_t i = 1; i < partition.size(); ++i) {
      if (y_less_compare(partition[i], partition[entry_point_idx])) {
        entry_point_idx = i;
      }
    }
    return partition[entry_point_idx];
  }

  bool y_less_compare(const Point &p1, const Point &p2) {
    bool y_less = (p1.y_ < p2.y_);
    bool x_less = (p1.x_ < p2.x_);
    bool y_eq = (std::abs(p1.y_ - p2.y_) < EPS);
    return (y_less || (y_eq && x_less));
  }

  void findPartitionSize(size_t &partition_size, size_t &rem) {
    size_t set_size = points_.size();
    if (set_size < 6) {
      partition_size = set_size;
      rem = 0;
      assert(partition_size == set_size);
      assert(rem == 0);
      partitions_.push_back(points_);
      return;
    }
    partition_size = points_.size() * 2 / 10;
    if (partition_size < 3) {
      partition_size = 3;
    }
    rem = set_size % partition_size;
    while (rem && rem < 3) {
      ++partition_size;
      rem = set_size % partition_size;
    }
    assert(partition_size >= 3);
    assert(rem >= 3 || rem == 0);
  }

  void generatePartitions() {
    size_t partition_size = 0;
    size_t rem = 0;
    findPartitionSize(partition_size, rem);
    size_t partitions_number = points_.size() / partition_size + (rem != 0);
    for (size_t point_idx = 0; point_idx < points_.size(); ++point_idx) {
      if (point_idx % partition_size == 0) {
        partitions_.push_back(std::vector<Point>{});
      }
      partitions_.back().push_back(points_[point_idx]);
    }
  }
};
