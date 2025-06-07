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

private:
  size_t partition_size_;
  std::vector<Point> set_;
  std::vector<std::vector<Point>> partitions_;
  std::vector<GrahamConvexHull> graham_partitions_;

  void findConvexHull() {
    generatePartition();
    for (size_t bucket_idx = 0; bucket_idx < partitions_.size(); ++bucket_idx) {
      graham_partitions_.push_back({partitions_[bucket_idx]});
    }
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
