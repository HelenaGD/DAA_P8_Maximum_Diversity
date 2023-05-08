#pragma once

#include <iostream>
#include <vector>

using namespace std;

struct Iteracion {
  string Identificador_;
  string estrategia_;
  int n_;
  int k_;
  int m_;
  int ejecuciones_;
  int lrc_size_;
  int num_Iter_;
  double z_constructivo_;
  double z_;
  vector<vector<double>> solution_;
  double tiempo_;
  int nodos_;
};