#pragma once
#include "../include/solution.h"
#include <limits>
#include <algorithm>

#define RESET "\033[0m"
#define RED "\033[31m"
#define CYAN "\033[36m"

template <typename T>
class Algorithm {
 public:
  typedef std::pair<int, T> farthestElement;
  typedef vector<vector<T>> Cluster;
  typedef vector<T> Punto;
  Algorithm() {};
  ~Algorithm() {};

  /**
   * Método para construir la solución inicial
   * mediante un algoritmo constructivo voraz
   * @param problem Instancia del problema
   * @param m Número de puntos a seleccionar
   * @return Solución inicial
  */
  Solution<T> constructivo_voraz(Problem<T> problem, int m = 2, int size_lrc = 1) {
    Cluster solution; // Tendré m puntos en la solucion
    Cluster puntos_disponibles = problem.get_points(); // Tendré todos los puntos disponibles
    // Primero calculo el centroide de la nube total de puntos
    vector<T> the_centroid = centroid(problem.get_points());

    // El punto más alejado del centroide en una lrc será el primer punto de la solución
    farthestElement choosedElement;
    int random_index_dist = rand() % size_lrc;
    vector<farthestElement> distances = get_lrc(puntos_disponibles, the_centroid, size_lrc);
    choosedElement = distances[random_index_dist];
    solution.push_back(puntos_disponibles[choosedElement.first]);
    // Elimino el punto de los disponibles
    puntos_disponibles.erase(puntos_disponibles.begin() + choosedElement.first);

    distances.clear();

    // Ahora, hasta que no se alcance el número de puntos en la solucion
    while (solution.size() < m) {
      // Calculo el centroide de mi solución
      the_centroid = centroid(solution);

      // Elijo aleatoriamente entre los X puntos más alejados
      random_index_dist = rand() % size_lrc;
      distances = get_lrc(puntos_disponibles, the_centroid, size_lrc);
      choosedElement = distances[random_index_dist];

      // Añado el punto elegido a los puntos de servicio
      solution.push_back(puntos_disponibles[choosedElement.first]);
      puntos_disponibles.erase(puntos_disponibles.begin() + choosedElement.first); // Elimino el punto de los disponibles
      distances.clear();
    }

    // Creo la solución
    Solution<T> solucion(solution);
    return solucion;
  };

  vector<farthestElement> get_lrc(Cluster puntos_disponibles, Punto gravity_center, int size = 3) {
    vector<farthestElement> distances;
    vector<farthestElement> bestKDistances;

    // Calculo la distancia de todos los puntos a los servicios
    for (int i = 0; i < puntos_disponibles.size(); i++) { // Para cada punto
      T min_distance = std::numeric_limits<T>::max(); // Inicializo la distancia mínima a infinito
        T distance = euclidean_distance(puntos_disponibles[i], gravity_center); // Calculo la distancia
        if (distance < min_distance) min_distance = distance; // Si es menor que la mínima, actualizo
      distances.push_back(std::make_pair(i, min_distance)); // Guardo el punto y su distancia
    }

    // Ordeno los puntos por distancia más alejada
    std::sort(distances.begin(), distances.end(), [](const farthestElement& a, const farthestElement& b) {
      return a.second > b.second;
    });

    // Elijo los k puntos más alejados
    for (int i = 0; i < size; i++) {
      bestKDistances.push_back(distances[i]);
    }

    // Retorno la lrc
    return bestKDistances;
  }
};