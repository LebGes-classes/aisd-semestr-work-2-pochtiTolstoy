#pragma once

#include "GrahamConvexHull.hpp"
#include "JarvisConvexHull.hpp"
#include <cassert>
#include <stdexcept>

class ChanConvexHull {
public:
  ChanConvexHull(const std::vector<Point> &set) : set_{set} {
    if (set.size() < 3) {
      throw std::invalid_argument("At least 3 points to build convex hull");
    }
    findConvexHull();
  }

  std::vector<Point> getChanHull() const { return final_hull_; }

  std::vector<GrahamConvexHull> getGrahamPartitions() const {
    return graham_partitions_;
  }

  size_t getPartitionsNumber() const { return partitions_.size(); }

  std::vector<Point> getGrahamPartition(size_t idx) const {
    if (idx >= graham_partitions_.size()) {
      throw std::invalid_argument("Wrong index for partitions!");
    }
    return graham_partitions_[idx].getConvexHull();
  }

  size_t size() const { return final_hull_.size(); }

private:
  size_t partition_size_;
  std::vector<Point> set_;
  std::vector<std::vector<Point>> partitions_;
  std::vector<GrahamConvexHull> graham_partitions_;
  std::vector<Point> final_hull_;

  void findConvexHull() {
    // TODO : BINARY SEARCH INSIDE BUCKET INSTEAD OF LINEAR TIME
    generatePartition();
    // TODO : delete debug vector
    std::vector<Point> pure_graham_hull;
    for (size_t bucket_idx = 0; bucket_idx < partitions_.size(); ++bucket_idx) {
      graham_partitions_.push_back({partitions_[bucket_idx]});
      // TODO : BIN SEARCH
      const std::vector<Point> &graham_hull =
          graham_partitions_[bucket_idx].getConvexHull();
      for (const auto &point : graham_hull) {
        pure_graham_hull.push_back(point);
      }
    }

    // TODO : delete debug pure_jarvis_hull vector
    GrahamConvexHull grahamHull(pure_graham_hull);
    pure_graham_hull = grahamHull.getConvexHull();

    Point entryPoint = findEntryPoint();
    std::vector<Point> &current_bucket = partitions_[0];

    std::cout << "Points on hull : " << std::endl;
    for (const auto &elem : pure_graham_hull) {
      elem.display_log();
      std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << "First point on hull" << std::endl;
    entryPoint.display_log();
    std::cout << std::endl;
    pure_graham_hull[0].display_log();
    std::cout << std::endl;
    assert(entryPoint == pure_graham_hull[0]);

    Point best_bucket_point =
        findNextPointInPartition(current_bucket, entryPoint);

    std::cout << "Second point on hull : " << std::endl;
    best_bucket_point.display_log();
    std::cout << std::endl;
    pure_graham_hull[1].display_log();
    std::cout << std::endl;
    assert(best_bucket_point == pure_graham_hull[1]);

    // std::vector<Point> hull;
    // const Point &entryPoint = findEntryPoint();
    // std::cout << "ENTRY POINT : " << entryPoint.x_ << ", " << entryPoint.y_
    //           << std::endl;
    // hull.push_back(entryPoint);

    // TODO : BIN SEARCH
    // const Point& nextHullPoint = findNextHullPoint();
    // while (entryPoint != nextHullPoint) {
    //   hull.push_back(nextHullPoint);
    //   nextHullPoint = findNextHullPoint();
    // }

    // 1. Есть entry point, теперь нужно взять какой-то бакет
    // 2. Найти бин поиском лучшего кандидата, относительно угла
    // 3. Сделать то же самое с остальными бакетами
    // 4. из них выбрать лучшего кандидата
    // 5. добавить его в вектор результат

    // LINEAR TIME TO FIND NEXT CANDIDATE IN BUCKET, REWRITE TO BIN SEARCH
    // 1. Слить все в один вектор
    // 2. по этому вектору запустить Jarvis

    // TODO : если в [0] каждого бакета лежит entry point этого бакета, то entry
    // по всему множеству можно перебирая [0] бакетов
  }

  Point findNextPointInPartition(const std::vector<Point> &partition,
                                 const Point &current_point) {
    size_t l = 1;
    size_t r = partition.size() - 1;
    Point current_candidate = partition[0];
    Point best_candidate = current_point;
    while (l < r) {
      size_t m = l + (r - l) / 2;
      if (jarvis_compare(current_candidate, best_candidate, current_point)) {
        best_candidate = current_candidate;
        l = m + 1;
      } else {
        r = m;
      }
    }
    return best_candidate;
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
    size_t set_size = set_.size();
    if (set_size < 6) {
      partition_size = set_size;
      rem = 0;

      assert(partition_size == set_size);
      assert(rem == 0);

      partitions_.push_back(set_);
      return;
    }
    partition_size = set_.size() * 2 / 10;
    std::cout << "partition size : " << partition_size << std::endl;
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

  void generatePartition() {
    size_t partition_size = 0;
    size_t rem = 0;
    findPartitionSize(partition_size, rem);
    size_t partitions_number = set_.size() / partition_size + (rem != 0);

    if (rem) {
      assert((partitions_number - 1) * partition_size + rem == set_.size());
    } else {
      assert(partitions_number * partition_size == set_.size());
    }

    for (size_t point_idx = 0; point_idx < set_.size(); ++point_idx) {
      if (point_idx % partition_size == 0) {
        partitions_.push_back(std::vector<Point>{});
      }
      // partitions_[bucket_idx][point_idx % partition_size] = set_[point_idx];
      partitions_.back().push_back(set_[point_idx]);
    }

    for (size_t i = 0; i < partitions_number - 1; ++i) {
      assert(partitions_[i].size() == partition_size);
    }

    if (rem) {
      assert(partitions_.back().size() == rem);
    } else {
      assert(partitions_.back().size() == partition_size);
    }
  }
};
