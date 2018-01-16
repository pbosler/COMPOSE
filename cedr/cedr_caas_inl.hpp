#ifndef INCLUDE_CEDR_CAAS_INL_HPP
#define INCLUDE_CEDR_CAAS_INL_HPP

#include "cedr_util.hpp"

namespace cedr {
// ClipAndAssuredSum.
namespace caas {

template <typename ES> KOKKOS_INLINE_FUNCTION
void CAAS<ES>::set_rhom (const Int& lclcellidx, const Real& rhom) {
  cedr_kernel_assert(lclcellidx >= 0 && lclcellidx < nlclcells_);
  d_(lclcellidx) = rhom;
}

template <typename ES> KOKKOS_INLINE_FUNCTION
void CAAS<ES>
::set_Qm (const Int& lclcellidx, const Int& tracer_idx,
          const Real& Qm, const Real& Qm_min, const Real& Qm_max,
          const Real Qm_prev) {
  cedr_kernel_assert(lclcellidx >= 0 && lclcellidx < nlclcells_);
  cedr_kernel_assert(tracer_idx >= 0 && tracer_idx < tracers_.extent_int(0));
  const Int nt = tracers_.size();
  d_((1 +               tracer_idx)*nlclcells_ + lclcellidx) = Qm;
  d_((1 +   nt + tracer_idx)*nlclcells_ + lclcellidx) = Qm_min;
  d_((1 + 2*nt + tracer_idx)*nlclcells_ + lclcellidx) = Qm_max;
  if (need_conserve_)
    d_((1 + 3*nt + tracer_idx)*nlclcells_ + lclcellidx) = Qm_prev;
}

template <typename ES> KOKKOS_INLINE_FUNCTION
Real CAAS<ES>::get_Qm (const Int& lclcellidx, const Int& tracer_idx) {
  cedr_kernel_assert(lclcellidx >= 0 && lclcellidx < nlclcells_);
  cedr_kernel_assert(tracer_idx >= 0 && tracer_idx < tracers_.extent_int(0));
  return d_((1 + tracer_idx)*nlclcells_ + lclcellidx);
}

} // namespace caas
} // namespace cedr

#endif
