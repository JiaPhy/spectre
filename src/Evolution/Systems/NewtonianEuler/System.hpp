// Distributed under the MIT License.
// See LICENSE.txt for details.

#pragma once

#include <cstddef>

#include "DataStructures/VariablesTag.hpp"
#include "Evolution/Systems/NewtonianEuler/BoundaryConditions/BoundaryCondition.hpp"
#include "Evolution/Systems/NewtonianEuler/BoundaryCorrections/BoundaryCorrection.hpp"
#include "Evolution/Systems/NewtonianEuler/Characteristics.hpp"
#include "Evolution/Systems/NewtonianEuler/ConservativeFromPrimitive.hpp"
#include "Evolution/Systems/NewtonianEuler/PrimitiveFromConservative.hpp"
#include "Evolution/Systems/NewtonianEuler/Tags.hpp"
#include "Evolution/Systems/NewtonianEuler/TimeDerivativeTerms.hpp"
#include "PointwiseFunctions/Hydro/Tags.hpp"
#include "Utilities/TMPL.hpp"

/// \ingroup EvolutionSystemsGroup
/// \brief Items related to evolving the Newtonian Euler system
namespace NewtonianEuler {

template <size_t Dim, typename InitialDataType>
struct System {
  static constexpr bool is_in_flux_conservative_form = true;
  static constexpr bool has_primitive_and_conservative_vars = true;
  static constexpr size_t volume_dim = Dim;

  using boundary_conditions_base = BoundaryConditions::BoundaryCondition<Dim>;
  using boundary_correction_base = BoundaryCorrections::BoundaryCorrection<Dim>;

  using variables_tag = ::Tags::Variables<tmpl::list<
      Tags::MassDensityCons, Tags::MomentumDensity<Dim>, Tags::EnergyDensity>>;
  using flux_variables =
      tmpl::list<Tags::MassDensityCons, Tags::MomentumDensity<Dim>,
                 Tags::EnergyDensity>;
  using non_conservative_variables = tmpl::list<>;
  using gradient_variables = tmpl::list<>;
  // Compute item for pressure not currently implemented in SpECTRE,
  // so its simple tag is passed along with the primitive variables.
  using primitive_variables_tag = ::Tags::Variables<
      tmpl::list<hydro::Tags::RestMassDensity<DataVector>,
                 hydro::Tags::SpatialVelocity<DataVector, Dim>,
                 hydro::Tags::SpecificInternalEnergy<DataVector>,
                 hydro::Tags::Pressure<DataVector>>>;

  using compute_volume_time_derivative_terms =
      TimeDerivativeTerms<Dim, InitialDataType>;

  using conservative_from_primitive = ConservativeFromPrimitive<Dim>;
  using primitive_from_conservative = PrimitiveFromConservative<Dim>;

  using compute_largest_characteristic_speed =
      Tags::ComputeLargestCharacteristicSpeed<Dim>;
};

}  // namespace NewtonianEuler
