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

private:
  size_t partition_size_;
  std::vector<Point> set_;
  std::vector<std::vector<Point>> partitions_;
  void findConvexHull() { generatePartition(); }

  void findPartitionSize(size_t &partition_size, size_t &rem) {
    size_t set_size = set_.size();
    if (set_size < 6) {
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
    assert(partition_size + rem == set_size);
  }

  void generatePartition() {
    size_t partition_size = 0;
    size_t rem = 0;
    findPartitionSize(partition_size, rem);
    size_t partitions_number = set_.size() / partition_size + (rem != 0);
    partitions_.resize(partitions_number);
    size_t bucket_idx = 0;
    for (size_t point_idx = 0; point_idx < set_.size(); ++point_idx) {
      partitions_[bucket_idx].push_back(set_[point_idx]);
      if (point_idx && point_idx % partition_size == 0) {
        ++bucket_idx;
      }
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
