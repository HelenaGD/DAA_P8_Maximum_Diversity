#include "../include/problem.h"
#include "../include/formulas.h"
#include "../include/solution.h"
#include "../include/algorithm.h"
#include "../include/voraz.h"
#include "../include/generator.h"

#include <filesystem>

using namespace std;

namespace fs = std::filesystem;

void execute(const string& file, Algorithm<double>* algorithm, const Problem<double>& problem) {
  unsigned t0, t1;
  t0 = clock();
  //Solution<double> solution = algorithm->run(problem);
  //solution.evaluate();
  t1 = clock();
  double time = (double(t1-t0)/CLOCKS_PER_SEC);
  //cout << "SSE: " << solution.get_sse() << endl;
  cout << "CPU Time: " << time << " seconds" << endl << endl;
  /*cout << "Clusters: " << solution.get_num_clusters() << endl;
  cout << "Centroids: " << endl;
  solution.print_centroids();*/
  //solution.store_solution(file);
}

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " <directory_path>" << std::endl;
    return 1;
  }
  string path = argv[1]; // directorio

  fs::path directory_path(argv[1]);
  if (!fs::is_directory(directory_path)) {
      std::cerr << "Error: " << directory_path << " is not a directory" << std::endl;
      return 1;
  }

  // VORAZ
  //ofstream archivo("solutions/voraz.csv");
  // Escribir encabezado
  //archivo << "Problema,n,K,m,z,S,CPU" << endl;

  // GRASP
  ofstream archivo2("solutions/grasp.csv");
  // Escribir encabezado
  archivo2 << "Problema,n,K,m,Iter,|LRC|,z,S,CPU" << endl;

  for (const auto& entry : fs::directory_iterator(directory_path)) {
    if (entry.is_regular_file()) {
      // Para cada archivo de entrada
      string nombre_archivo = string(entry.path().filename());
      //cout << nombre_archivo << endl;
      Problem<double> problem(directory_path.string() + "/" + nombre_archivo);
      //problem.print();

      GENERATOR generator;

      // Para cada archivo de las instancias. Hago las iteraciones correspondientes
      //generator.generate(nombre_archivo, archivo, problem, 1, 4);
      generator.generate(nombre_archivo, archivo2, problem, 2, 4);
    }  
  }
  //archivo.close();
  archivo2.close();
}