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
   * Realiza la búsqueda local para un problema dado.
   * Realiza una fase constructiva inicial con el algoritmo voraz y luego una búsqueda local
   * @param problem Problema a resolver
   * @param m Número de puntos de servicio
   * @return Solución óptima
  */
  Solution<T> run_local_search(Problem<T> problem, int m = 2) {
    // Fase constructiva inicial
    Solution<T> solution = constructivo_voraz(problem, m, 1);

    // BÚSQUEDA LOCAL
    //cout << endl << "Inicia búsqueda local" << endl;
    Solution<T> optimo_local = local_search(problem, solution, 0);
    //cout << "Finaliza búsqueda local" << endl << endl;

    // Retorno la solución
    return optimo_local;
  }

  /**
   * @brief Realiza una búsqueda local. Mientras encuentre una solución mejor,
   * se seguirá buscando en el entorno con el movimiento indicado
   * @param problem Problema a resolver
   * @param m Número de puntos de servicio
   * @param metodo Método de generación de solución inicial (0: Voraz)
   * @return Solución inicial
  */
  Solution<T> local_search(const Problem<T>& problem, const Solution<T>& initial_solution, const int& movimiento = 0) {
    double best_z = initial_solution.get_z(); // La mejor solucion hasta el momento
    
    Solution<T> best_solution;
    Cluster best_serv_points = initial_solution.get_service_points();

    // Ahora, debo buscar la mejor solución vecina, y si cambia
    // con respecto a la mejor solución hasta el momento, busco
    // en ese vecindario

    Solution<T> current_best_solution = initial_solution;
    while (true) {
      // Encuentro la mejor de las soluciones del entorno
      // Si ya no queda entorno por generar, termino
      if (best_serv_points.size() == problem.get_m()) {
        break;
      }
      best_solution = best_vecino(problem, best_serv_points, movimiento);
      best_serv_points = best_solution.get_service_points();

      // Evaluación
      Solution<T> new_solution(best_serv_points);
      new_solution.evaluate();
      double new_z = new_solution.get_z();

      /*cout << "Evaluación hecha" << endl;
      cout << "SSE actual: " << new_sse << endl;
      cout << "SSE mejor: " << best_sse << endl;*/

      if (new_z > best_z) { // Se maximiza
        best_z = new_z;
        current_best_solution = new_solution; // Actualizo la mejor solución actual
      } else {
        break;
      }
    }

    return current_best_solution;
  }

  /**
   * @brief Encuentra la mejor solución vecina
   * Dado un movimiento definido, genera todas las soluciones vecinas
   * y encuentra la mejor de ellas
   * @param problem Problema a resolver
   * @param initial_solution Solución inicial
   * @param movimiento Movimiento a realizar  (0: Intercambio)
   * @return Mejor solución vecina
  */
  Solution<T> best_vecino(const Problem<T>& problem, const Cluster& initial_solution, const int& movimiento = 0) {
    Solution<T> the_solution;
    // De todas las soluciones vecinas, encuentro la mejor
    // Genero espacio de soluciones vecinas
    vector<Cluster> vecinos;
    switch (movimiento) {
      case 0:
        vecinos = intercambio(problem, initial_solution);
        break;
    }

    // Encuentro la mejor solución vecina
    double best_z = std::numeric_limits<T>::min();
    Cluster best_solution;
    //cout << "Encuentro la mejor solución vecina" << endl << endl;
    for (int i = 0; i < vecinos.size(); i++) {
      // Evaluación (Total. Intentar parcial)
      Solution<T> new_solution(vecinos[i]);
      new_solution.evaluate();
      double new_z = new_solution.get_z();
      if (new_z > best_z) { // Ahora maximizo
        best_solution = vecinos[i];
        best_z = new_z;
        the_solution = new_solution;
      }
    }

    return the_solution;
  }

  /**
   * Método para generar el entorno por intercambio en una solución
   * @param problem Instancia del problema
   * @param initial_solution Solución inicial
   * @return Entorno generado
  */
  vector<Cluster> intercambio(const Problem<T>& problem, const Cluster& initial_solution) {
    Cluster vecindario = problem.get_points();
    vector<Cluster> grupos;
    Cluster dummy;
    // Ahora debo intercambiar los puntos de servicio por alguno de los puntos del entorno

    /////////////////////
    cout << "Solucion inicial para el intercambio: " << endl;
    for (int i = 0; i < initial_solution.size(); i++) {
      cout << "Cluster " << i << ": ";
      for (int j = 0; j < initial_solution[i].size(); j++) {
        cout << initial_solution[i][j] << " ";
      }
      cout << endl;
    }
    for (int i = 0; i < initial_solution.size(); i++) {
      for (int j = 0; j < vecindario.size(); j++) {
        // Si el punto es uno de los clusteres, no lo añado
        //if (find(initial_solution.begin(), initial_solution.end(), vecindario[j]) != initial_solution.end()) {
          if (initial_solution[i] == vecindario[j]) {
          continue;
        }
        // Intercambio el punto de servicio por el punto del entorno
        dummy = initial_solution;
        dummy[i] = vecindario[j];
        grupos.push_back(dummy); // Añado la nueva solución vecina
      }
    }
    return grupos;
  }

  /**
   * Método para construir la solución inicial
   * mediante un algoritmo constructivo voraz
   * @param problem Instancia del problema
   * @param m Número de puntos a seleccionar
   * @param size_lrc Tamaño de la lista restringida de candidatos
   * @return Solución inicial evaluada
  */
  Solution<T> constructivo_voraz(Problem<T> problem, const int& m = 2, const int& size_lrc = 1) {
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
    solucion.evaluate();
    return solucion;
  };

  /**
   * Método para construir una LRC teniendo en cuenta
   * la distancia de un punto al centro de gravedad
   * @param puntos_disponibles Puntos disponibles para elegir e incluir en la LRC
   * @param gravity_center Centro de gravedad de los puntos de la solución
   * @param size Tamaño de la LRC
   * @return Lista restringida de candidatos
  */
  vector<farthestElement> get_lrc(Cluster puntos_disponibles, Punto gravity_center, int size = 3) {
    vector<farthestElement> distances;
    vector<farthestElement> bestKDistances;

    // Calculo la distancia de todos los puntos al centro de gravedad
    for (int i = 0; i < puntos_disponibles.size(); i++) { // Para cada punto
      T min_distance = std::numeric_limits<T>::max(); // Inicializo la distancia máxima al mínimo
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