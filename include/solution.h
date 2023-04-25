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
  double sse_constructivo_;
 public:
  Solution() {
    sse_ = 0;
  }
  
  Solution(Cluster service_points) {
    service_points_ = service_points;
    sse_ = 0;
    sse_constructivo_ = 0;
  }

  void set_sse_constructivo(double sse_constructivo) {
    sse_constructivo_ = sse_constructivo;
  }

  double get_z() const { return sse_; }
  double get_z_constructivo() const { return sse_constructivo_; }
  int get_num_puntos() { return service_points_.size(); }
  int get_k() { return service_points_[0].size(); }

  Cluster get_service_points() const { return service_points_; }

  void evaluate() {
    sse_ = 0;
    // Primero itero sobre los puntos de servicio
    //cout << "El numero de puntos de servicio es: " << service_points_.size() << endl;
    for (int i = 0; i < service_points_.size(); i++) {
      for (int j = i + 1; j < service_points_.size(); j++) {
        // Calculo la distancia euclidea entre el punto de servicio y el punto
        sse_ += euclidean_distance(service_points_[i], service_points_[j]);
      }
      //cout << "Punto evaluado" << endl;
    }
  }

  void reduced_evaluate(const double& old_sse, const Cluster& old_service_points) {
    sse_ = old_sse;
    // solucion actual
    Cluster new_service_points = get_service_points();
    // Trabajo sobre una solución sse_ ya calculada

    // Miro qué puntos han cambiado
    for (int i = 0; i < new_service_points.size(); i++) {
      for (int j = 0; j < new_service_points.size(); j++) {
        // Si el punto de servicio es el mismo, no hago nada
        if (new_service_points[i][j] == old_service_points[i][j]) {
          continue;
        }
        // Si no, calculo la distancia euclidea entre el punto de servicio y el punto
        sse_ += euclidean_distance(new_service_points[i], old_service_points[i]);
        sse_ -= euclidean_distance(new_service_points[i], new_service_points[j]);
      }
    }
  }

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
      sse_constructivo_ = other.sse_constructivo_;
    }
    return *this;
  }
};