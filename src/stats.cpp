#include "../include/algorithm/ChanConvexHull.hpp"
#include "../include/algorithm/GrahamConvexHull.hpp"
#include "../include/algorithm/JarvisConvexHull.hpp"
#include "../include/algorithm/PointSet.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>

int main() {
  // Configuration
  bool circular = false;
  const std::vector<size_t> sizes_random{100,   500,   1000, 5000,
                                         10000, 25000, 50000};
  const std::vector<size_t> sizes_circular{100, 200, 300, 400, 500, 600, 700};
  std::vector<size_t> sizes;
  if (circular) {
    sizes = sizes_circular;
  } else {
    sizes = sizes_random;
  }
  const int trials = 5; // Runs per size
  const std::string csv_path = "timings.csv";

  std::ofstream outfile(csv_path);
  outfile << "n,trial,jarvis_time,graham_time,chan_time\n";

  for (size_t n : sizes) {
    for (int t = 0; t < trials; ++t) {
      // Generate new point set
      PointSet point_set;

      if (circular) {
        point_set.generate_circular_points(n);
      } else {
        point_set.generate_points(n);
      }

      // Measure Jarvis
      auto start_jarvis = std::chrono::high_resolution_clock::now();
      JarvisConvexHull jarvis(point_set.get_set());
      auto end_jarvis = std::chrono::high_resolution_clock::now();
      long jarvis_time = std::chrono::duration_cast<std::chrono::microseconds>(
                             end_jarvis - start_jarvis)
                             .count();

      // Measure Graham
      auto start_graham = std::chrono::high_resolution_clock::now();
      GrahamConvexHull graham(point_set.get_set());
      auto end_graham = std::chrono::high_resolution_clock::now();
      long graham_time = std::chrono::duration_cast<std::chrono::microseconds>(
                             end_graham - start_graham)
                             .count();

      // Measure Chan
      auto start_chan = std::chrono::high_resolution_clock::now();
      ChanConvexHull chan(point_set.get_set());
      auto end_chan = std::chrono::high_resolution_clock::now();
      long chan_time = std::chrono::duration_cast<std::chrono::microseconds>(
                           end_chan - start_chan)
                           .count();

      // Save results
      outfile << n << "," << t << "," << jarvis_time << "," << graham_time
              << "," << chan_time << "\n";
      // outfile << n << "," << t << "," << 0 << "," << graham_time << ","
      //         << chan_time << "\n";

      std::cout << "n=" << n << " trial=" << t << " | Jarvis: " << jarvis_time
                << "μs"
                << " | Graham: " << graham_time << "μs"
                << " | Chan: " << chan_time << "μs" << std::endl;
      // std::cout << "n=" << n << " trial=" << t << " | Jarvis: " << 0 << "μs"
      //           << " | Graham: " << graham_time << "μs"
      //           << " | Chan: " << chan_time << "μs" << std::endl;
    }
  }
  outfile.close();
  std::cout << "Results saved to " << csv_path << std::endl;
  return 0;
}
