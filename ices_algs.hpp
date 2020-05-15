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

  for(unsigned bits = 0; bits <= (pow(2, steps)-1); bits++)
  {
    //initialize candidate path
    path candidatePath(setting);

    //loop through grid
    for(unsigned k = 0; k <= steps-1; k++)
    {
      unsigned bit = (bits>>k)&1;

      if(bit == 1) // columns
      {
        if(candidatePath.is_step_valid(STEP_DIRECTION_RIGHT))
        { //go right if valid
          candidatePath.add_step(STEP_DIRECTION_RIGHT);
        }
      }else // rows
      {
        if(candidatePath.is_step_valid(STEP_DIRECTION_DOWN))
        { // go down if valid
          candidatePath.add_step(STEP_DIRECTION_DOWN);
        }
      }
    }

    // if candidate stays inside the grid and never crosses an X cell:
    if(candidatePath.final_row() == setting.rows()-1 &&
    candidatePath.final_column() == setting.columns()-1)
    { // increment total number of paths
      count_paths++;
    }
  }
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

  A[0][0] = 1; // base case

  //loop through rows
  for(unsigned i = 0; i <= (setting.rows()-1); i++)
  {
    //loop through columns
    for(unsigned j = 0; j <= (setting.columns()-1); j++)
    {
      //variables to hold number of paths
      //coming in from above and left
      unsigned from_above = 0;
      unsigned from_left = 0;

      //setting values of variables to hold number
      //of paths coming in from above and left
      //after the starting cell
      if (i > 0)
      {
        from_above = A[i-1][j];
      }
      if (j > 0)
      {
        from_left = A[i][j-1];
      }
      //adding together number of paths from
      //both directions into the current cell
      A[i][j] += from_above + from_left;

      //if current cell is an iceberg,
      //disregard everything above
      if(setting.get(i,j) == CELL_ICEBERG)
      {
        A[i][j] = 0;
      }
    }
  }
  return A[setting.rows()-1][setting.columns()-1];
}

}
