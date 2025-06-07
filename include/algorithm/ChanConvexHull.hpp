#pragma once

#include "GrahamConvexHull.hpp"
#include "JarvisConvexHull.hpp"
#include <stdexcept>

class ChanConvexHull {
public:
  ChanConvexHull(const std::vector<Point> &set) : set_{set} {
    if (set.size() < 3) {
      throw std::invalid_argument("At least 3 points to build convex hull");
    }
    findConvexHull();
  }

private:
  size_t partition_size_;
  std::vector<Point> set_;
  std::vector<std::vector<Point>> partitions_;
  void findConvexHull() { calculatePartitionSize(); }

  void calculatePartitionSize() {}
};
