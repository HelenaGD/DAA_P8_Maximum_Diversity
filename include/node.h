#pragma once

#include <iostream>
#include "solution.h"

template <typename T>
class Node {
private:
  int depth_;
  Solution<T> solution_;
 public:
  Node() {};
  Node(Solution<T> solution, int depth = 0) {
    depth_ = depth;
    solution_ = solution;
  }

  int get_depth() const { return depth_; }
  Solution<T> get_solution() const { return solution_; }

  void set_depth(int depth) { depth_ = depth; }

  bool operator == (const Node<T>& node) const {
    return ((solution_ == node.get_solution()) && (depth_ == node.get_depth()));
  }
};