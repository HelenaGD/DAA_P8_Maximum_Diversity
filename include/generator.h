#include "iteration.h"
#include "problem.h"
#include "algorithm.h"

#include <unistd.h>
#include <cstdlib>
#include <ctime>

using namespace std;

class GENERATOR {
 private:
 public:
  GENERATOR() {};
  ~GENERATOR() {};
  
  void generate(const string& name, ofstream& archivo, Problem<double> problem, const int& type, const int& num_executions = 10) {
    // Obtener el tiempo actual
    time_t seed = time(NULL);
    vector<Iteracion> iteraciones;
    unsigned t0, t1;
    int m = 2; // Número de puntos en la solución
    Algorithm<double> algorithm;

    for (int i = 0; i < num_executions; i++) {
      //cout << "Iteracion: " << i + 1 << endl;
      // Cambiar la semilla aleatoria
      srand(seed + i);
      Solution<double> solution;

      if (type == 1) { // Voraz
        t0 = clock();
        solution = algorithm.constructivo_voraz(problem, m, 1);
        solution.evaluate();
        t1 = clock();

        double time = (double(t1-t0)/CLOCKS_PER_SEC);
        Iteracion iteracion;
        iteracion.Identificador_ = name;
        iteracion.n_ = problem.get_m(); // Number of points originally
        iteracion.k_ = solution.get_k(); // Dimension of points
        iteracion.m_ = solution.get_num_puntos(); // Number of points on solution
        iteracion.ejecuciones_ = i + 1;
        iteracion.z_ = solution.get_z();
        iteracion.solution_ = solution.get_service_points();
        iteracion.tiempo_ = time;
        iteraciones.push_back(iteracion);
        sleep(1);
      } else if (type == 2) iterar_grasp(m, algorithm, problem, archivo, name);
      else {
        cout << "Error: Tipo de algoritmo no válido" << endl;
        return;
      }
      m++; // Aumentar el número de puntos en la solución
    }
    if (type == 1) guardar_voraz_csv(archivo, iteraciones);
    //else if (type == 2) guardar_grasp_csv(archivo, iteraciones);
  }

  void iterar_grasp(const int& m, Algorithm<double> algorithm, const Problem<double>& problem, ofstream& archivo, const string& name) {
    // Llega con la m fijada
    // Para cada m debo
    // Hacer x ejecuciones con 2 y 3 de tamaño LRC
    int num_Iter = 10; // 10 y 20
    while (num_Iter <= 20) {
      cout << "Iteraciones: " << num_Iter << endl;
      int lrc_size = 2; // 2 y 3
      while (lrc_size <= 3) {
        cout << "LRC: " << lrc_size << endl;
        Iteracion mejor_iteracion = generar_mejor_iteracion(m, lrc_size, num_Iter, algorithm, problem, name);
        // Guardar sólo la mejor iteracion
        guardar_grasp_csv(archivo, mejor_iteracion);
        lrc_size++;
      }
      num_Iter += 10; // Ahora con 20 iteraciones
    }
  }

  Iteracion generar_mejor_iteracion (const int& m, const int& lrc_size, const int& num_Iter, Algorithm<double> algorithm, const Problem<double>& problem, const string& name) {
    vector<Iteracion> iteraciones;
    Solution<double> solution;

    unsigned t0, t1;
    for (int i = 0; i < num_Iter; i++) {
      cout << "Iteracion: " << i + 1 << endl;
      Iteracion iteracion;
      // Ejecutar el algoritmo
      t0 = clock();
      solution = algorithm.constructivo_voraz(problem, m, lrc_size);
      solution.evaluate();
      t1 = clock();
      double time = (double(t1-t0)/CLOCKS_PER_SEC);

      iteracion.Identificador_ = name;
      iteracion.n_ = problem.get_m(); // Number of points originally
      iteracion.k_ = solution.get_k(); // Dimension of points
      iteracion.m_ = solution.get_num_puntos(); // Number of points on solution
      iteracion.ejecuciones_ = i + 1;
      iteracion.lrc_size_ = lrc_size;
      iteracion.num_Iter_ = num_Iter;
      iteracion.z_ = solution.get_z();
      iteracion.solution_ = solution.get_service_points();
      iteracion.tiempo_ = time;
      iteraciones.push_back(iteracion);
      sleep(1);
    }
    // Guardar la mejor solución
    std::sort(iteraciones.begin(), iteraciones.end(), [](Iteracion a, Iteracion b) { return a.z_ > b.z_; });
    return iteraciones[0];
  }

  void guardar_grasp_csv(ofstream& archivo, const Iteracion& iteracion) {
    // Escribir cada fila
      archivo << iteracion.Identificador_ << ","
              << iteracion.n_ << ","
              << iteracion.k_ << ","
              << iteracion.m_ << ","
              << iteracion.num_Iter_ << ","
              << iteracion.lrc_size_ << ","
              << iteracion.z_ << ","
              << to_string_solution(iteracion.solution_) << ","
              << iteracion.tiempo_ << endl;
  }

  void guardar_voraz_csv(ofstream& archivo, const vector<Iteracion>& iteraciones) {
    // Escribir cada fila
    for (auto iteracion : iteraciones) {
      archivo << iteracion.Identificador_ << ","
              << iteracion.n_ << ","
              << iteracion.k_ << ","
              << iteracion.m_ << ","
              << iteracion.z_ << ","
              << to_string_solution(iteracion.solution_) << ","
              << iteracion.tiempo_ << endl;
    }
  }

  string to_string_solution(vector<vector<double>> vector) {
    string solution = "";
    for (int i = 0; i < vector.size(); i++) {
      solution += "(";
      for (int j = 0; j < vector[i].size(); j++) {
        if (j == vector[i].size() - 1) solution += to_string(float(vector[i][j]));
        else solution += to_string(float(vector[i][j])) + " ";
      }
      solution += ")";
    }
    return solution;
  }

};