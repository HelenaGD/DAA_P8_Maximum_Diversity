#pragma once
#include "solution.h"
#include "node.h"

#include <limits>
#include <algorithm>
#include <tuple>
#include <vector>

#define RESET "\033[0m"
#define RED "\033[31m"
#define CYAN "\033[36m"

template <typename T>
class Algorithm {
 private:
  int numberOfNodes_;
  std::vector<Node<Solution<T>>> activeNodes_;
  int option_;

 public:
  typedef std::pair<int, T> farthestElement;
  typedef vector<vector<T>> Cluster;
  typedef vector<T> Punto;

  Algorithm() {
    numberOfNodes_ = 0;
    option_ = 0;
  };
  ~Algorithm() {};

  /**
   * @brief Establece la opción con la que se debe ejecutar el algoritmo
   * @param opcion Opción de algoritmo a ejecutar (0: SmallestZ, 1: Deeper)
  */
  void set_option(int option) {
    option_ = option;
  }

  int getNumberOfNodes() const { return numberOfNodes_; }

  // Modificación
  static bool sortByZ(const Node<Solution<T>>& node1, const Node<Solution<T>>& node2) {
    return (node1.get_solution().get_z() > node2.get_solution().get_z()); // Mayor en lugar de menor
  }

  // Modificación
  static bool sortByDepth(const Node<Solution<T>>& node1, const Node<Solution<T>>& node2) {
    return (node1.get_depth() > node2.get_depth()); // Mayor en profundidad
  }

  std::vector<Node<Solution<T>>> sortByOption(vector<Node<Solution<T>>>& activeNodes) {
    if (option_ == 0) { // SmallestZ. GreatterZ en la modificación
      std::sort(activeNodes.begin(), activeNodes.end(), sortByZ);
    } else { // Deeper
      std::sort(activeNodes.begin(), activeNodes.end(), sortByDepth);
    }
    return activeNodes;
  }

  /**
   * @brief Ejecuta el algoritmo para un problema dado
   * @param problem Problema a resolver
   * @param initial_solution Solución inicial
   * @return Solución óptima
  */
  Solution<T> run_branch_and_bound(const Problem<T>& problem, const Solution<T>& initial_solution, const int& m = 2) {
    // Tengo una solucion inicial que me llega de greedy o grasp
    Solution<double> cota_inferior = initial_solution;
    Node<Solution<T>> initial_node(cota_inferior, 0);

    // Creo un conjunto de nodos activos a ser expandidos
    activeNodes_.push_back(initial_node);
    numberOfNodes_ = 1;
    Node<Solution<T>> actual_Node = activeNodes_[0];

    // Nodos activos a verificar por su límite superior y que podrían expandirse
    while (activeNodes_.size() > 0) {
      //cout << RED << "Nodos activos: " << RESET << activeNodes_.size() << endl;
      sortByOption(activeNodes_);
      actual_Node = activeNodes_[activeNodes_.size() - 1];

      // Expando el nodo actual
      while (actual_Node.get_depth() < m) {
        //cout << "Generando hojas..." << endl;
        vector<Node<Solution<T>>> newNodes = generateLeaf(problem, cota_inferior, actual_Node.get_depth(), m);
        //cout << "Hojas generadas: " << newNodes.size() << endl;
        typename vector<Node<Solution<T>>>::iterator activeNodes_it = std::find(activeNodes_.begin(), activeNodes_.end(), actual_Node);
        activeNodes_.erase(activeNodes_it);
        sortByOption(newNodes);
        for (int i = 0; i < newNodes.size(); i++) {
          activeNodes_.push_back(newNodes[i]);
          numberOfNodes_++;
        }

        // La mejor de las hojas
        actual_Node = activeNodes_[activeNodes_.size() - 1];
      }

      // Calculo z
      double actual_Node_z = actual_Node.get_solution().get_z(); // Revisar esto
      if (actual_Node_z > cota_inferior.get_z()) {
        cota_inferior = actual_Node.get_solution();
      }

      for (int i = 0; i < activeNodes_.size(); i++) {
        if (activeNodes_[i].get_solution().get_z() <= cota_inferior.get_z()) {
          typename vector<Node<Solution<T>>>::iterator activeNodes_it = std::find(activeNodes_.begin(), activeNodes_.end(), activeNodes_[i]);
          activeNodes_.erase(activeNodes_it);
          i--;
        }
      }
    }
    return cota_inferior;
  }

  /**
   * Genera todas las hojas/hijos de un nodo padre, avanzando a travñes de cada una de sus
   * ramas y, dependiendo de la profundidad, establece elementos para obtener todas las
   * posibles combinaciones
  */
  std::vector<Node<Solution<T>>> generateLeaf(const Problem<T>& problem, const Solution<T>& parent, const int& depth, const int& m) {
    std::vector<Node<Solution<T>>> children;
    Solution<T> selectedElement;
    
    // Agrega a la posible solución los elementos correspondientes del padre debido a la profundidad actual.
    for (int i = 0; i < depth; i++) {
      selectedElement.add_element(parent[i]);
    }

    // Eliminar cada elemento de la solución generada que se encuentre dentro del conjunto inicial de elementos.
    std::vector<vector<T>> initialX = problem.get_points();
    for (int i = 0; i < selectedElement.get_num_puntos(); i++) {
      typename Cluster::iterator initialXIterator = std::find(initialX.begin(), initialX.end(), selectedElement.get_service_points()[i]);
      initialX.erase(initialXIterator);
    }

    // Establece un eje de coordenadas
    selectedElement.resize(depth + 1);
    for (int i = 0; i < initialX.size(); i++) {
      selectedElement[depth] = initialX[i];
      Cluster initialWithoutSelected = initialX;
      initialWithoutSelected.erase(initialWithoutSelected.begin() + i);
      Node<Solution<T>> newLeaf(generateBestSolution(initialWithoutSelected, selectedElement, m), depth + 1);
      children.push_back(newLeaf);
    }

    return children;
  }

  /**
   * Genera cada posible solución con las coordenadas restances y luego solo elige la solución
   * que tenga el mejor valor de función objetivo
  */
  Solution<T> generateBestSolution(const Cluster& initialX, const Solution<T>& selectedElement, const int& m) {
    std::pair<vector<T>, double> bestCandidate; // Cada posición corresponde a un elemento y su valor de z
    bestCandidate.second = 0.0;
    Solution<T> auxSolution;
    double candidate;
    Solution<T> bestSolution = selectedElement;

    while(bestSolution.get_num_puntos() < m) {
      for (int i = 0; i < initialX.size(); i++) {
        auxSolution = bestSolution;
        auxSolution.add_element(initialX[i]);
        auxSolution.evaluate();
        candidate = auxSolution.get_z(); // revisar
        if (candidate > bestCandidate.second) {
          bestCandidate.first = initialX[i];
          bestCandidate.second = candidate;
        }
      }
      bestSolution.add_element(bestCandidate.first);
    }
    bestSolution.evaluate();
    return bestSolution;
  }

  /**
   * Realiza la búsqueda local para un problema dado.
   * Realiza una fase constructiva inicial con el algoritmo voraz y luego una búsqueda local
   * @param problem Problema a resolver
   * @param m Número de puntos de servicio
   * @return Solución óptima
  */
  Solution<T> run_local_search(Problem<T> problem, const int& m = 2, const int& lrc_size = 1) {
    // Fase constructiva inicial
    Solution<T> solution = constructivo_voraz(problem, m, lrc_size);
    double sse_constructivo = solution.get_z();

    // BÚSQUEDA LOCAL
    //cout << endl << "Inicia búsqueda local" << endl;
    Solution<T> optimo_local = local_search(problem, solution, 0);
    optimo_local.set_sse_constructivo(sse_constructivo);
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
      double old_z = best_z;
      Cluster old_points = best_serv_points;
      best_serv_points = best_solution.get_service_points();

      // Evaluación
      Solution<T> new_solution(best_serv_points);
      new_solution.parcial_evaluate(old_z, old_points);
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
    Solution<T> initial_solution_obj(initial_solution);
    initial_solution_obj.evaluate();
    double reference_z = initial_solution_obj.get_z();

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
      new_solution.parcial_evaluate(reference_z, initial_solution);
      //new_solution.evaluate();
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

    /*cout << "Solucion inicial para el intercambio: " << endl;
    for (int i = 0; i < initial_solution.size(); i++) {
      cout << "Cluster " << i << ": ";
      for (int j = 0; j < initial_solution[i].size(); j++) {
        cout << initial_solution[i][j] << " ";
      }
      cout << endl;
    }*/
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