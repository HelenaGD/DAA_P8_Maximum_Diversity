#pragma once
#include <vector>
#include "formulas.h"

using namespace std;

template <typename T>
class Solution {
 private:
  typedef vector<vector<T>> Cluster;
  vector<Cluster> clusters_;
  int num_clusters_;
  double sse_;
  Cluster service_points_;
 public:
  Solution() {
    num_clusters_ = 0;
    sse_ = 0;
  }
  Solution(vector<Cluster> clusters) {
    num_clusters_ = clusters.size();
    clusters_ = clusters;
    sse_ = 0;
  }

  Solution(vector<Cluster> clusters, Cluster service_points) {
    num_clusters_ = clusters.size();
    clusters_ = clusters;
    service_points_ = service_points;
    sse_ = 0;
  }

  void evaluate() {
    sse_ = 0;
    vector<T> a_service_point;
    // Primero itero sobre los puntos de servicio
    //cout << "El numero de puntos de servicio es: " << service_points_.size() << endl;
    for (int k = 0; k < service_points_.size(); k++) {
      a_service_point = service_points_[k];
      // Punto de servicio
      /*cout << "El punto de servicio en sse es: ";
      for (int i = 0; i < a_service_point.size(); i++) {
        cout << a_service_point[i] << " ";
      }
      cout << endl;*/

      // Para todos los puntos del cluster
      for (int i = 0; i < clusters_[k].size(); i++) {
        // Calculo la distancia euclidea entre el punto de servicio y el punto
        sse_ += euclidean_distance(a_service_point, clusters_[k][i]);
      }
      //cout << "Punto evaluado" << endl;
    }
    // añado una penalización por cantidad de puntos
    sse_ += 4 * service_points_.size();
  }

  /**
   * Método para evaluar la solución en función
   * de la distancia de los puntos de cada cluster
   * a su centroide
  */
  void evaluate_old() {
    sse_ = 0;
    vector<T> the_centroid;
    // Primero itero sobre los clusters
    for (int k = 0; k < clusters_.size(); k++) {
      the_centroid = centroid(clusters_[k]);
      // Centroide
      /*cout << "El centroides calculado en sse es: ";
      for (int i = 0; i < the_centroid.size(); i++) {
        cout << the_centroid[i] << " ";
      }
      cout << endl;*/

      // Para todos los puntos del cluster
      for (int i = 0; i < clusters_[k].size(); i++) {
        // Calculo la distancia euclidea entre el centroide y el punto
        sse_ += pow(euclidean_distance(the_centroid, clusters_[k][i]), 2);
      }
    }
  }

  double get_sse() const { return sse_; }
  vector<Cluster> get_clusters() { return clusters_; }
  int get_num_clusters() { return num_clusters_; }

  Cluster get_centroids() {
    Cluster centroids;
    for (int i = 0; i < clusters_.size(); i++) {
      centroids.push_back(centroid(clusters_[i]));
    }
    return centroids;
  }

  Cluster get_service_points() const { return service_points_; }

  void print_centroids() {
    Cluster centroids = get_centroids();
    for (int i = 0; i < centroids.size(); i++) {
      cout << "Centroid " << i << ": ";
      for (int j = 0; j < centroids[i].size(); j++) {
        cout << centroids[i][j] << " ";
      }
      cout << endl;
    }
  }

  void print_clusters() {
    for (int i = 0; i < clusters_.size(); i++) {
      cout << "Cluster " << i << ": ";
      for (int j = 0; j < clusters_[i].size(); j++) {
        cout << "(";
        for (int k = 0; k < clusters_[i][j].size(); k++) {
          cout << clusters_[i][j][k] << " ";
        }
        cout << ") " << endl;
      }
      cout << endl;
    }
  }

  void store_solution(const string& filename = "solution.txt") {
    string path = "solutions/" + filename;
    ofstream file;
    file.open(path);
    // Primero el resultado de la SSE
    file << sse_ << endl;
    // Luego el número de clusters
    file << clusters_.size() << endl;
    // Luego los puntos de servicio
    Cluster centroids = get_service_points();
    for (int i = 0; i < centroids.size(); i++) {
      for (int j = 0; j < centroids[i].size(); j++) {
        file << centroids[i][j] << " ";
      }
      file << endl;
    }
    // Finalmente los clusters
    for (int i = 0; i < clusters_.size(); i++) {
      file << "Cluster " << i << ": " << endl;
      for (int j = 0; j < clusters_[i].size(); j++) {
        for (int k = 0; k < clusters_[i][j].size(); k++) {
          file << clusters_[i][j][k] << " ";
        }
        file << endl;
      }
      file << endl;
    }
    file.close();
  }

  void store_solution_old(const string& filename = "solution.txt") {
    string path = "solutions/" + filename;
    ofstream file;
    file.open(path);
    // Primero el resultado de la SSE
    file << sse_ << endl;
    // Luego el número de clusters
    file << clusters_.size() << endl;
    // Luego los centroides
    Cluster centroids = get_centroids();
    for (int i = 0; i < centroids.size(); i++) {
      for (int j = 0; j < centroids[i].size(); j++) {
        file << centroids[i][j] << " ";
      }
      file << endl;
    }
    // Finalmente los clusters
    for (int i = 0; i < clusters_.size(); i++) {
      file << "Cluster " << i << ": " << endl;
      for (int j = 0; j < clusters_[i].size(); j++) {
        for (int k = 0; k < clusters_[i][j].size(); k++) {
          file << clusters_[i][j][k] << " ";
        }
        file << endl;
      }
      file << endl;
    }
    file.close();
  }

  Solution<T>& operator=(const Solution<T>& other) {
    if (this != &other) {
      num_clusters_ = other.num_clusters_;
      clusters_ = other.clusters_;
      sse_ = other.sse_;
      service_points_ = other.service_points_;
    }
    return *this;
  }
};