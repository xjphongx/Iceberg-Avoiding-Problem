///////////////////////////////////////////////////////////////////////////////
// crossing_algs.hpp
//
// Algorithms that solve the iceberg avoiding problem.
//
// All of the TODO sections for this project reside in this file.
//
// This file builds on ices_types.hpp, so you should familiarize yourself
// with that file before working on this file.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cassert>
#include <iostream>

#include "ices_types.hpp"

namespace ices {
// Solve the iceberg avoiding problem for the given grid, using an exhaustive
// optimization algorithm.
//
// This algorithm is expected to run in exponential time, so the grid's
// width+height must be small enough to fit in a 64-bit int; this is enforced
// with an assertion.
//
// The grid must be non-empty.
unsigned int iceberg_avoiding_exhaustive(const grid& setting) {
    
  // grid must be non-empty.
  assert(setting.rows() > 0);
  assert(setting.columns() > 0);

  // Compute the path length, and check that it is legal.
  const size_t steps = setting.rows() + setting.columns() - 2;
  assert(steps < 64);

  unsigned int count_paths = 0;
    
  // TODO: implement the exhaustive optimization algorithm, then delete this
  // comment.

  return count_paths;
}

// Solve the iceberg avoiding problem for the given grid, using a dynamic
// programming algorithm.
//
// The grid must be non-empty.
unsigned int iceberg_avoiding_dyn_prog(const grid& setting) {

  // grid must be non-empty.
  assert(setting.rows() > 0);
  assert(setting.columns() > 0);


  const int DIM=100;
  std::vector<std::vector<unsigned>> A(DIM, std::vector<unsigned>(DIM));

  A[0][0] = 1;
    
  // TODO: implement the dynamic programming algorithm, then delete this
  // comment.
  
  return A[setting.rows()-1][setting.columns()-1];
}

}
