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

  Solution(Cluster service_points, double sse) {
    service_points_ = service_points;
    sse_ = sse;
    sse_constructivo_ = 0;
  }

  void set_sse_constructivo(double sse_constructivo) {
    sse_constructivo_ = sse_constructivo;
  }

  double get_z() const { return sse_; }
  double get_z_constructivo() const { return sse_constructivo_; }
  int get_num_puntos() { return service_points_.size(); }
  int get_num_puntos() const { return service_points_.size(); }
  int get_k() { return service_points_[0].size(); }

  Cluster get_service_points() const { return service_points_; }

  void add_element(vector<T> element) {
    service_points_.push_back(element);
  }

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

  void parcial_evaluate(const double& old_sse, const Cluster& old_service_points) {
    /*cout << "Puntos antiguos: " << endl;
    for (int i = 0; i < old_service_points.size(); i++) {
      for (int j = 0; j < old_service_points[i].size(); j++) {
        cout << old_service_points[i][j] << " ";
      }
      cout << endl;
    }

    cout << "Puntos nuevos: " << endl;
    for (int i = 0; i < service_points_.size(); i++) {
      for (int j = 0; j < service_points_[i].size(); j++) {
        cout << service_points_[i][j] << " ";
      }
      cout << endl;
    }*/

    // Busco en qué posición difieren los vectores de puntos
    int pos = -1;
    for (int i = 0; i < old_service_points.size(); i++) {
      if (old_service_points[i] != service_points_[i]) {
        pos = i;
        break;
      }
    }

    //cout << "La posicion es: " << pos << endl;

    // Trabajo sobre una solución sse_ ya calculada
    sse_ = old_sse;
    //double comprobacion = old_sse;
    for (int i = 0; i < service_points_.size(); i++) {
      if (i == pos) continue;
      // Resto la distancia del antiguo
      sse_ -= euclidean_distance(service_points_[i], old_service_points[pos]);
      //comprobacion -= euclidean_distance(service_points_[i], old_service_points[pos]);
      // Sumo la distancia del nuevo
      sse_ += euclidean_distance(service_points_[i], service_points_[pos]);
      //comprobacion += euclidean_distance(service_points_[i], service_points_[pos]);
    }
    //cout << "Parcial: " << comprobacion << endl;
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

  bool operator==(const Solution<T>& other) const {
    if (sse_ != other.sse_) return false;
    if (service_points_ != other.service_points_) return false;
    if (sse_constructivo_ != other.sse_constructivo_) return false;
    return true;
  }

  vector<T>& operator[](int index) {
    if (index < 0 || index >= service_points_.size()) {
      throw out_of_range("Index out of range");
    }
    return service_points_[index];
  }

  vector<T> operator[](int index) const {
    if (index < 0 || index >= service_points_.size()) {
      throw out_of_range("Index out of range");
    }
    return service_points_[index];
  }

  void resize(int size) {
    service_points_.resize(size);
  }
};