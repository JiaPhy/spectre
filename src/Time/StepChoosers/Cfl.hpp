// Distributed under the MIT License.
// See LICENSE.txt for details.

#pragma once

#include <cmath>
#include <cstddef>
#include <limits>
#include <pup.h>
#include <utility>

#include "DataStructures/DataBox/DataBox.hpp"
#include "Domain/MinimumGridSpacing.hpp"
#include "Options/String.hpp"
#include "Time/StepChoosers/StepChooser.hpp"
#include "Time/TimeStepRequest.hpp"
#include "Time/TimeSteppers/TimeStepper.hpp"
#include "Utilities/Serialization/CharmPupable.hpp"
#include "Utilities/TMPL.hpp"

/// \cond
namespace Tags {
template <typename StepperInterface>
struct TimeStepper;
}  // namespace Tags
namespace domain {
namespace Tags {
template <size_t Dim, typename Frame>
struct MinimumGridSpacing;
}  // namespace Tags
}  // namespace domain

/// \endcond

namespace StepChoosers {
/// Sets a goal based on the CFL stability criterion.
template <typename StepChooserUse, typename Frame, typename System>
class Cfl : public StepChooser<StepChooserUse> {
 public:
  /// \cond
  Cfl() = default;
  explicit Cfl(CkMigrateMessage* /*unused*/) {}
  using PUP::able::register_constructor;
  WRAPPED_PUPable_decl_template(Cfl);  // NOLINT
  /// \endcond

  struct SafetyFactor {
    using type = double;
    static constexpr Options::String help{"Multiplier for computed step"};
    static type lower_bound() { return 0.0; }
  };

  static constexpr Options::String help{
      "Sets a goal based on the CFL stability criterion."};
  using options = tmpl::list<SafetyFactor>;

  explicit Cfl(const double safety_factor) : safety_factor_(safety_factor) {}

  using argument_tags =
      tmpl::list<domain::Tags::MinimumGridSpacing<System::volume_dim, Frame>,
                 ::Tags::TimeStepper<TimeStepper>,
                 typename System::compute_largest_characteristic_speed>;

  using compute_tags = tmpl::list<
      domain::Tags::MinimumGridSpacingCompute<System::volume_dim, Frame>,
      typename System::compute_largest_characteristic_speed>;

  std::pair<TimeStepRequest, bool> operator()(const double minimum_grid_spacing,
                                              const TimeStepper& time_stepper,
                                              const double speed,
                                              const double last_step) const {
    const double time_stepper_stability_factor = time_stepper.stable_step();
    const double step_size = safety_factor_ * time_stepper_stability_factor *
                             minimum_grid_spacing /
                             (speed * System::volume_dim);
    // Reject the step if the CFL condition is violated.
    return {{.size_goal = std::copysign(step_size, last_step)},
            abs(last_step) <= step_size};
  }

  bool uses_local_data() const override { return true; }
  bool can_be_delayed() const override { return true; }

  // NOLINTNEXTLINE(google-runtime-references)
  void pup(PUP::er& p) override { p | safety_factor_; }

 private:
  double safety_factor_ = std::numeric_limits<double>::signaling_NaN();
};

/// \cond
template <typename StepChooserUse, typename Frame, typename System>
PUP::able::PUP_ID Cfl<StepChooserUse, Frame, System>::my_PUP_ID = 0;  // NOLINT
/// \endcond
}  // namespace StepChoosers
