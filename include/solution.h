#pragma once
#include <vector>
#include "formulas.h"

using namespace std;

template <typename T>
class Solution {
 private:
  typedef vector<vector<T>> Cluster;
  double sse_;
  Cluster service_points_;
 public:
  Solution() {
    sse_ = 0;
  }
  
  Solution(Cluster service_points) {
    service_points_ = service_points;
    sse_ = 0;
  }

  double get_sse() const { return sse_; }
  int get_num_puntos() { return service_points_.size(); }

  Cluster get_service_points() const { return service_points_; }

  void store_solution(const string& filename = "solution.txt") {
    string path = "solutions/" + filename;
    ofstream file;
    file.open(path);
    // Primero el resultado de la SSE
    file << sse_ << endl;
    // Luego el número de clusters
    file << get_num_puntos() << endl;
    // Luego los puntos de servicio
    Cluster centroids = get_service_points();
    for (int i = 0; i < centroids.size(); i++) {
      for (int j = 0; j < centroids[i].size(); j++) {
        file << centroids[i][j] << " ";
      }
      file << endl;
    }
    file.close();
  }

  Solution<T>& operator=(const Solution<T>& other) {
    if (this != &other) {
      sse_ = other.sse_;
      service_points_ = other.service_points_;
    }
    return *this;
  }
};