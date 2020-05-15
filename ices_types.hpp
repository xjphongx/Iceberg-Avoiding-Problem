///////////////////////////////////////////////////////////////////////////////
// ices_types.hpp
//
// Data types for the input and output of the iceberg avoiding problem.
//
// You should leave this file as-is; there are no TODO parts in this file.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include <optional>

namespace ices {

// Type for a row or column number.
using coordinate = size_t;

// Type for one element of the map grid.
enum cell_kind { CELL_WATER, CELL_ICEBERG};

// Type for a rectangular grid representing the map.
class grid {
private:
  std::vector<std::vector<cell_kind>> cells_;

public:

  // Create a grid with the given number of rows and columns, all initialized
  // to hold CELL_WATER.
  grid(coordinate rows, coordinate columns)
  : cells_(rows, std::vector<cell_kind>(columns, CELL_WATER)) {

    assert(rows > 0);
    assert(columns > 0);
  }

  // Accessors.
   coordinate rows() const { return cells_.size(); }
   coordinate columns() const { return cells_.front().size(); }

  // Test whether the given value is a valid row or column number.
   bool is_row(coordinate row) const { return row < rows(); }
   bool is_column(coordinate column) const { return column < columns(); }
   bool is_row_column(coordinate row, coordinate column) const {
    return is_row(row) && is_column(column);
  }

  // Return the cell at the given row and column.
   cell_kind get(coordinate row, coordinate column) const {
    assert(is_row_column(row, column));
    return cells_[row][column];
  }

  // Set the contents of the cell at the given row and column.
  // (0, 0) may only be CELL_WATER. Other coordinates may be any kind.
   void set(coordinate row, coordinate column, cell_kind kind) {
    assert(is_row_column(row, column));

    if ((row == 0) && (column == 0)) {
      assert(kind == CELL_WATER);
    }

    cells_[row][column] = kind;
  }

  // Return true if it is valid to step into the given row and column.
  // This is the case when those are valid row-column values, and also
  // that cell is not CELL_ICEBERG.
   bool may_step(coordinate row, coordinate column) const {
    return (is_row_column(row, column) &&
            (cells_[row][column] != CELL_ICEBERG));
  }

  // Return strings corresponding to lines of text in a human-readable
  // representation of the grid.
  std::vector<std::string> printable() const {
    std::vector<std::string> result(rows(), std::string(columns(), '.'));
    for (coordinate r = 0; r < rows(); ++r) {
      for (coordinate c = 0; c < columns(); ++c) {
        auto cell = get(r, c);
        if (cell == CELL_WATER) {
          result[r][c] = '.';
        } else if (cell == CELL_ICEBERG) {
          result[r][c] = 'X';
        }
      }
    }
    return result;
  }

  // Print the grid.
  void print() const {
    for (auto& line : printable()) {
      std::cout << line << std::endl;
    }
  }

  // Create a random grid with the given number of rows, columns, 
  // passable cells, thicket cells, and random number generator. 
  // rows and columns must both be positive. The number of thicket cells 
  // must be less than the number of total cells in the grid.
  template <typename URNG>
  static grid random(coordinate rows, coordinate columns, 
		     unsigned thicket_count, URNG&& gen) {

    assert(rows > 0);
    assert(columns > 0);
    assert((thicket_count) < (rows * columns));

    // The output grid, at this point all cells are earth.
    grid result(rows, columns);

    // Next we are going to shuffle all the positions that could contain the
    // thicket and use the shuffled order to place those cells randomly.
    // This approach makes it easy to prevent the thicket cells from colliding
    // or being placed at (0, 0) and (n-1, m-1)

    struct position {

      coordinate row, column;

      position(coordinate r, coordinate c)
      : row(r), column(c) { }
    };

    // Build a vector of every position except (0, 0) and (n-1, m-1)
    std::vector<position> positions;
    for (coordinate row = 0; row < rows; ++row) {
      for (coordinate column = 0; column < columns; ++column) {
        if (!(row == 0 && column == 0) && ! (row == rows-1 && column == columns-1)) {
          positions.emplace_back(row, column);
        }
      }
    }
    // Shuffle the vector randomly using the provided random number generator.
    shuffle(positions.begin(), positions.end(), gen);

    // Use the first thicket_count positions to place thicket cells.
    assert(positions.size() >= thicket_count);
    for (unsigned i = 0; i < thicket_count; ++i) {
      auto& p = positions.back();
      result.set(p.row, p.column, CELL_ICEBERG);
      positions.pop_back();
    }

    // done
    return result;
  }
};

// Type for a legal step direction; starting at (0, 0) counts as a step.
enum step_direction {
  STEP_DIRECTION_START,
  STEP_DIRECTION_RIGHT,
  STEP_DIRECTION_DOWN
};

// Type for one step in a path.
class step {
private:
  step_direction direction_;

public:

  // Create a step in the given direction.
  step(step_direction direction)
  : direction_(direction) { }

  // Accessor.
  step_direction direction() const { return direction_; }

  // Return the number of rows/columns that we move when we take this step.
  // Moving right increases the column by 1 and the row by 0.
  // Moving down increases the column by 0 and the row by 1.
  // Starting increases both by 0.
  coordinate delta_row() const {
    if (direction_ == STEP_DIRECTION_DOWN) {
      return 1;
    } else {
      return 0;
    }
  }
  coordinate delta_column() const {
    if (direction_ == STEP_DIRECTION_RIGHT) {
      return 1;
    } else {
      return 0;
    }
  }

  // Equality operator, for unit testing.
  bool operator== (const step& o) const {
    return (direction_ == o.direction_);
  }
};

  
// A path represents a sequence of valid steps in a particular grid.
//
// The first step must always be STEP_DIRECTION_START, and subsequent steps
// may not be STEP_DIRECTION_START.
//
// This class can only represent a valid path, so steps that would move off
// the grid, or step on a thicket cell, are prohibited with assertions.
//
// This class tracks the ending position, and the total number of the paths, 
// in order to make it easier to compare candidate solutions in the 
// exhaustive optimization algorithm.
class path {
private:
  const grid* setting_;
  std::vector<step> steps_;
  coordinate final_row_, final_column_;

  // Helper function to initialize all data members, called by the two
  // constructors below.
  void initialize(const grid& setting) {
    assert(steps_.empty());
    setting_ = &setting;
    steps_.emplace_back(STEP_DIRECTION_START);
    final_row_ = final_column_ = 0;
  }

public:

  // Create an empty path, containing only one STEP_DIRECTION_START step
  // and no other steps.
  path(const grid& setting) { initialize(setting); }

  // Create a path containing one STEP_DIRECTION_START step followed by the
  // steps in steps_after_start, which must all be valid. This constructor is
  // intended to make unit testing easier and probably does not need to be used
  // by the algorithms.
  path(const grid& setting, const std::vector<step_direction>& steps_after_start) {
    initialize(setting);
    for (auto& step : steps_after_start) {
      assert(is_step_valid(step));
      add_step(step);
    }
  }

  // Accessors.
  const grid& setting() const { return *setting_; }
  const std::vector<step>& steps() const { return steps_; }
  coordinate final_row() const { return final_row_; }
  coordinate final_column() const { return final_column_; }

  // Return the last step in the path.
  const step& last_step() const { return steps_.back(); }

  // Return the row/column number that we would be in if we took one more step
  // in the given direction.
  coordinate row_after(step_direction dir) const {
    return final_row() + step(dir).delta_row();
  }
  coordinate column_after(step_direction dir) const {
    return final_column() + step(dir).delta_column();
  }

  // Return true if adding the given step is valid. A step is valid when it
  // is not STEP_DIRECTION_START, it stays inside the grid, and it does not
  // try to step into a CELL_ICEBERG cell.
  bool is_step_valid(step_direction dir) const {
    auto row = row_after(dir), column = column_after(dir);
    return ((dir != STEP_DIRECTION_START) &&
            setting_->is_row_column(row, column) &&
            (setting_->get(row, column) != CELL_ICEBERG));
  }

  // Add one step, which must be valid as determined by is_step_valid.
  void add_step(step_direction dir) {

    assert(is_step_valid(dir));

    steps_.emplace_back(dir);

    // Update final row and column
    final_row_ = row_after(dir);
    final_column_ = column_after(dir);
  }

  // Equality operator, for unit testing.
  bool operator==(const path& o) const {
    return std::equal(steps_.begin(), steps_.end(), o.steps_.begin());
  }

};
 
}
