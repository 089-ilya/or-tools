// Copyright 2010-2018 Google LLC
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef OR_TOOLS_SAT_DIFFN_H_
#define OR_TOOLS_SAT_DIFFN_H_

#include <vector>

#include "ortools/base/int_type.h"
#include "ortools/base/integral_types.h"
#include "ortools/base/logging.h"
#include "ortools/base/macros.h"
#include "ortools/sat/integer.h"
#include "ortools/sat/intervals.h"
#include "ortools/sat/model.h"
#include "ortools/sat/sat_base.h"

namespace operations_research {
namespace sat {

// Non overlapping rectangles.
class NonOverlappingRectanglesPropagator : public PropagatorInterface {
 public:
  // The strict parameters indicates how to place zero width or zero height
  // boxes. If strict is true, these boxes must not 'cross' another box, and are
  // pushed by the other boxes.
  NonOverlappingRectanglesPropagator(const std::vector<IntervalVariable>& x,
                                     const std::vector<IntervalVariable>& y,
                                     bool strict, Model* model,
                                     IntegerTrail* integer_trail);
  ~NonOverlappingRectanglesPropagator() override;

  // TODO(user): Look at intervals to to store x and dx, y and dy.
  bool Propagate() final;
  void RegisterWith(GenericLiteralWatcher* watcher);

 private:
  bool FindBoxesThatMustOverlapAHorizontalLine(
      SchedulingConstraintHelper* x_dim, SchedulingConstraintHelper* y_dim);
  bool PropagateBoxesOvelappingAHorizontalLine(
      IntegerValue other_time, const std::vector<int>& boxes,
      SchedulingConstraintHelper* x_dim, SchedulingConstraintHelper* y_dim);
  void SortNeighbors(int box);
  bool FailWhenEnergyIsTooLarge(int box);

  const int num_boxes_;
  SchedulingConstraintHelper x_;
  SchedulingConstraintHelper y_;
  const bool strict_;
  IntegerTrail* integer_trail_;

  std::vector<int> neighbors_;
  std::vector<IntegerValue> cached_areas_;
  std::vector<IntegerValue> cached_distance_to_bounding_box_;

  DISALLOW_COPY_AND_ASSIGN(NonOverlappingRectanglesPropagator);
};

// Enforces that the boxes with corners in (x, y), (x + dx, y), (x, y + dy)
// and (x + dx, y + dy) do not overlap.
// If one box has a zero dimension, then it can be placed anywhere.
inline std::function<void(Model*)> NonOverlappingRectangles(
    const std::vector<IntervalVariable>& x,
    const std::vector<IntervalVariable>& y) {
  return [=](Model* model) {
    NonOverlappingRectanglesPropagator* constraint =
        new NonOverlappingRectanglesPropagator(
            x, y, false, model, model->GetOrCreate<IntegerTrail>());
    constraint->RegisterWith(model->GetOrCreate<GenericLiteralWatcher>());
    model->TakeOwnership(constraint);
  };
}

// Enforces that the boxes with corners in (x, y), (x + dx, y), (x, y + dy)
// and (x + dx, y + dy) do not overlap.
// If one box has a zero dimension, it still cannot intersect another box.
inline std::function<void(Model*)> StrictNonOverlappingRectangles(
    const std::vector<IntervalVariable>& x,
    const std::vector<IntervalVariable>& y) {
  return [=](Model* model) {
    NonOverlappingRectanglesPropagator* constraint =
        new NonOverlappingRectanglesPropagator(
            x, y, true, model, model->GetOrCreate<IntegerTrail>());
    constraint->RegisterWith(model->GetOrCreate<GenericLiteralWatcher>());
    model->TakeOwnership(constraint);
  };
}

}  // namespace sat
}  // namespace operations_research

#endif  // OR_TOOLS_SAT_DIFFN_H_
