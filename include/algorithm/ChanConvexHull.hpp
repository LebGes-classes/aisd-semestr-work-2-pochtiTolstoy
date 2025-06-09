#pragma once

#include "ConvexHullBase.hpp"
#include "GrahamConvexHull.hpp"
#include "JarvisConvexHull.hpp"
#include <cassert>
#include <stdexcept>
#include <tuple>

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

  // point_tuple_t : Point, graham_partition, position in partition
  using point_tuple_t = std::tuple<Point, size_t, size_t>;

  void computeHull() override {
    if (points_.size() < 6) {
      GrahamConvexHull graham_convex_hull(points_);
      hull_ = graham_convex_hull.getHull();
      return;
    }

    // TODO : delete debug vector
    JarvisConvexHull jarvis_convex_hull(points_);
    std::vector<Point> debug_vec = jarvis_convex_hull.getHull();

    // TODO : delete output
    std::cout << "Original points : " << std::endl;
    for (const auto &point : points_) {
      point.display_log();
      std::cout << std::endl;
    }

    // generate graham partitions
    generatePartitions();
    for (size_t bucket_idx = 0; bucket_idx < partitions_.size(); ++bucket_idx) {
      graham_partitions_.push_back({partitions_[bucket_idx]});
    }

    // Find entry point
    point_tuple_t entry_point = findEntryPoint();
    std::vector<point_tuple_t> hull_tuple;
    hull_tuple.push_back(entry_point);

    assert(std::get<0>(entry_point) == debug_vec[0]);

    // Create convex hull based on graham partitions
    size_t max_hull_size = points_.size();
    for (size_t step = 0; step < max_hull_size; ++step) {
      point_tuple_t current_point = hull_tuple.back();
      point_tuple_t best_candidate = current_point;
      bool first = true;

      for (size_t partition_idx = 0; partition_idx < graham_partitions_.size();
           ++partition_idx) {
        const auto &subhull = graham_partitions_[partition_idx].getHull();
        assert(!subhull.empty());

        point_tuple_t candidate;
        if (std::get<1>(current_point) == partition_idx) {
          // O(1)
          size_t pos_in_partition = std::get<2>(current_point);
          size_t next_pos = (pos_in_partition + 1) % subhull.size();
          Point next_point = subhull[next_pos];
          candidate = {next_point, partition_idx, next_pos};
          // assert(0 && "branch: same graham set");
        } else {
          // O(log(m))
          candidate = findNextPointInPartitionLinear(
              subhull, std::get<0>(current_point));
          std::get<1>(candidate) = partition_idx;
        }

        // O(1)
        if (first ||
            jarvis_compare(std::get<0>(candidate), std::get<0>(best_candidate),
                           std::get<0>(current_point))) {
          best_candidate = candidate;
          first = false;
        }
      }

      // Close convex hull
      if (std::get<0>(best_candidate) == std::get<0>(entry_point)) {
        break;
      }

      assert(debug_vec[step + 1] == std::get<0>(best_candidate));

      hull_tuple.push_back(best_candidate);
    }

    hull_.clear();
    for (const auto &[point, i, j] : hull_tuple) {
      hull_.push_back(point);
    }

    std::cout << "Chan algorithm : " << std::endl;
    for (const auto &point : hull_) {
      point.display_log();
      std::cout << std::endl;
    }

    std::cout << "Correct answer : " << std::endl;
    for (const auto &point : debug_vec) {
      point.display_log();
      std::cout << std::endl;
    }

    assert(hull_.size() == jarvis_convex_hull.size());
  }

  point_tuple_t findNextPointInPartition(const std::vector<Point> &hull,
                                         const Point &point) {

    assert(hull.size() >= 3);
    auto isRightTurn = [](const Point &a, const Point &b,
                          const Point &pivot) -> bool {
      return cross_product(pivot, a, b) < 0;
    };

    int low = 0, high = static_cast<int>(hull.size());
    auto next = [&](int i) { return (i + 1) % hull.size(); };

    while (low < high) {
      int mid = (low + high) / 2;
      int mid_next = next(mid);

      if (isRightTurn(hull[mid], hull[mid_next], point)) {
        high = mid;
      } else {
        low = mid + 1;
      }
    }

    size_t next_pos = low % hull.size();
    return {hull[next_pos], 0, next_pos};
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

  static double cross_product(const Point &a, const Point &b, const Point &c) {
    return (b.x_ - a.x_) * (c.y_ - a.y_) - (b.y_ - a.y_) * (c.x_ - a.x_);
  }

  double distance_squared(const Point &a, const Point &b) {
    double dx = a.x_ - b.x_;
    double dy = a.y_ - b.y_;
    return dx * dx + dy * dy;
  }

  point_tuple_t findEntryPoint() {
    const Point &entry_point = graham_partitions_.front().getHull()[0];
    point_tuple_t entry_point_tuple{entry_point, 0, 0};
    for (size_t i = 1; i < graham_partitions_.size(); ++i) {
      const Point &curr_point = graham_partitions_[i].getHull()[0];
      const Point &best_point = std::get<0>(entry_point_tuple);
      if (y_less_compare(curr_point, best_point)) {
        entry_point_tuple = std::make_tuple(curr_point, i, 0);
      }
    }
    return entry_point_tuple;
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

  point_tuple_t findNextPointInPartitionLinear(const std::vector<Point> &hull,
                                               const Point &p) {
    int size = hull.size();
    assert(size >= 3);

    for (int i = 0; i < size; ++i) {
      if (hull[i] == p) {
        assert(0 && "point can't be in hull, processed in other branch");
        size_t next_pos = (i + 1) % size;
        return {hull[next_pos], 0, next_pos};
      }
    }

    int best_index = 0;
    for (int i = 1; i < size; ++i) {
      if (jarvis_compare(hull[i], hull[best_index], p)) {
        best_index = i;
      }
    }
    return {hull[best_index], 0, best_index};
  }
};
