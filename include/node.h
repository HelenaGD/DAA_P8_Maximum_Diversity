#pragma once

#include <iostream>
#include "solution.h"

template <typename TYPE>
class Node {
 private:
  int depth_;
  Solution<double> solution_;
 public:
  Node() {};
  Node(Solution<double> solution, int depth = 0) {
    depth_ = depth;
    solution_ = solution;
  }

  int get_depth() const { return depth_; }
  Solution<double> get_solution() const { return solution_; }

  void set_depth(int depth) { depth_ = depth; }

  bool operator == (const Node<TYPE>& node) const {
    return ((solution_ == node.get_solution()) && (depth_ == node.get_depth()));
  }
};