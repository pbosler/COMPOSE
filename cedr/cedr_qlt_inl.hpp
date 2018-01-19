#ifndef INCLUDE_CEDR_QLT_INL_HPP
#define INCLUDE_CEDR_QLT_INL_HPP

#include <cassert>

#include "cedr_local.hpp"

namespace cedr {
namespace qlt {

template <typename ES> KOKKOS_INLINE_FUNCTION
void QLT<ES>::set_rhom (const Int& lclcellidx, const Int& rhomidx, const Real& rhom) {
  const Int ndps = md_.a_d.prob2bl2r[md_.nprobtypes];
  bd_.l2r_data(ndps*lclcellidx) = rhom;  
}

template <typename ES> KOKKOS_INLINE_FUNCTION
void QLT<ES>::set_Qm (const Int& lclcellidx, const Int& tracer_idx,
                      const Real& Qm,
                      const Real& Qm_min, const Real& Qm_max,
                      const Real Qm_prev) {
  const Int ndps = md_.a_d.prob2bl2r[md_.nprobtypes];
  Real* bd; {
    const Int bdi = md_.a_d.trcr2bl2r(tracer_idx);
    bd = &bd_.l2r_data(ndps*lclcellidx + bdi);
  }
  bd[1] = Qm;
  {
    const Int problem_type = md_.a_d.trcr2prob(tracer_idx);
    if (problem_type & ProblemType::shapepreserve) {
      bd[0] = Qm_min;
      bd[2] = Qm_max;
    } else if (problem_type & ProblemType::consistent) {
      const Real rhom = bd_.l2r_data(ndps*lclcellidx);
      bd[0] = Qm_min / rhom;
      bd[2] = Qm_max / rhom;
    } else {
      cedr_kernel_throw_if(true, "set_Q: invalid problem_type.");
    }
    if (problem_type & ProblemType::conserve) {
      cedr_kernel_throw_if(Qm_prev < 0, "Qm_prev was not provided to set_Q.");
      bd[3] = Qm_prev;
    }
  }
}

template <typename ES> KOKKOS_INLINE_FUNCTION
Real QLT<ES>::get_Qm (const Int& lclcellidx, const Int& tracer_idx) {
  const Int ndps = md_.a_d.prob2br2l[md_.nprobtypes];
  const Int bdi = md_.a_d.trcr2br2l(tracer_idx);
  return bd_.r2l_data(ndps*lclcellidx + bdi);
}

//todo Replace this and the calling code with ReconstructSafely.
KOKKOS_INLINE_FUNCTION
void r2l_nl_adjust_bounds (Real Qm_bnd[2], const Real rhom[2], Real Qm_extra) {
  Real q[2];
  for (Int i = 0; i < 2; ++i) q[i] = Qm_bnd[i] / rhom[i];
  if (Qm_extra < 0) {
    Int i0, i1;
    if (q[0] >= q[1]) { i0 = 0; i1 = 1; } else { i0 = 1; i1 = 0; }
    const Real Qm_gap = (q[i1] - q[i0])*rhom[i0];
    if (Qm_gap <= Qm_extra) {
      Qm_bnd[i0] += Qm_extra;
      return;
    }
  } else {
    Int i0, i1;
    if (q[0] <= q[1]) { i0 = 0; i1 = 1; } else { i0 = 1; i1 = 0; }
    const Real Qm_gap = (q[i1] - q[i0])*rhom[i0];
    if (Qm_gap >= Qm_extra) {
      Qm_bnd[i0] += Qm_extra;
      return;
    }
  }
  { // Have to adjust both. Adjust so that the q bounds are the same. This
    // procedure assures that as long as rhom is conservative, then the
    // adjustment never pushes q_{min,max} out of the safety bounds.
    const Real Qm_tot = Qm_bnd[0] + Qm_bnd[1] + Qm_extra;
    const Real rhom_tot = rhom[0] + rhom[1];
    const Real q_tot = Qm_tot / rhom_tot;
    for (Int i = 0; i < 2; ++i)
      Qm_bnd[i] = q_tot*rhom[i];
  }
}

namespace impl {
KOKKOS_INLINE_FUNCTION
void solve_node_problem (const Real& rhom, const Real* pd, const Real& Qm,
                         const Real& rhom0, const Real* k0d, Real& Qm0,
                         const Real& rhom1, const Real* k1d, Real& Qm1) {
  Real Qm_min_kids [] = {k0d[0], k1d[0]};
  Real Qm_orig_kids[] = {k0d[1], k1d[1]};
  Real Qm_max_kids [] = {k0d[2], k1d[2]};
  { // The ideal problem is not assuredly feasible. Test for feasibility. If not
    // feasible, adjust bounds to solve the safety problem, which is assuredly
    // feasible if the total density field rho is mass conserving (Q doesn't
    // have to be mass conserving, of course; achieving mass conservation is one
    // use for QLT).
    const Real Qm_min = pd[0], Qm_max = pd[2];
    const bool lo = Qm < Qm_min, hi = Qm > Qm_max;
    if (lo || hi) {
      // If the discrepancy is numerical noise, don't act on it.
      const Real tol = 10*std::numeric_limits<Real>::epsilon();
      const Real discrepancy = lo ? Qm_min - Qm : Qm - Qm_max;
      if (discrepancy > tol*Qm_max) {
        const Real rhom_kids[] = {rhom0, rhom1};
        r2l_nl_adjust_bounds(lo ? Qm_min_kids : Qm_max_kids,
                             rhom_kids,
                             Qm - (lo ? Qm_min : Qm_max));
      }
    } else {
      // Quick exit if everything is OK as is. This is a speedup, and it also
      // lets the subnode solver make ~1 ulp changes instead of having to keep x
      // = y if y satisfies the conditions. Without this block, the
      // no_change_should_hold tests can fail.
      if (Qm == pd[1] && // Was our total tracer mass adjusted?
          // Are the kids' problems feasible?
          Qm_orig_kids[0] >= Qm_min_kids[0] && Qm_orig_kids[0] <= Qm_max_kids[0] &&
          Qm_orig_kids[1] >= Qm_min_kids[1] && Qm_orig_kids[1] <= Qm_max_kids[1]) {
        // Don't need to do anything, so skip even the math-based quick exits in
        // solve_node_problem.
        Qm0 = Qm_orig_kids[0];
        Qm1 = Qm_orig_kids[1];
        return;
      }
    }
  }
  { // Solve the node's QP.
    static const Real ones[] = {1, 1};
    const Real w[] = {1/rhom0, 1/rhom1};
    Real Qm_kids[2] = {k0d[1], k1d[1]};
    local::solve_1eq_bc_qp_2d(w, ones, Qm, Qm_min_kids, Qm_max_kids,
                              Qm_orig_kids, Qm_kids);
    Qm0 = Qm_kids[0];
    Qm1 = Qm_kids[1];
  }
}
} // namespace impl

template <typename ES> KOKKOS_INLINE_FUNCTION
void QLT<ES>::solve_node_problem (const Int problem_type,
                                  const Real& rhom, const Real* pd, const Real& Qm,
                                  const Real& rhom0, const Real* k0d, Real& Qm0,
                                  const Real& rhom1, const Real* k1d, Real& Qm1) {
  if ( ! (problem_type & ProblemType::shapepreserve)) {      
    Real mpd[3], mk0d[3], mk1d[3];
    mpd[0]  = pd [0]*rhom ; mpd [1] = pd[1] ; mpd [2] = pd [2]*rhom ;
    mk0d[0] = k0d[0]*rhom0; mk0d[1] = k0d[1]; mk0d[2] = k0d[2]*rhom0;
    mk1d[0] = k1d[0]*rhom1; mk1d[1] = k1d[1]; mk1d[2] = k1d[2]*rhom1;
    impl::solve_node_problem(rhom, mpd, Qm, rhom0, mk0d, Qm0, rhom1, mk1d, Qm1);
    return;
  }
  impl::solve_node_problem(rhom, pd, Qm, rhom0, k0d, Qm0, rhom1, k1d, Qm1);
}

} // namespace qlt
} // namespace cedr

#endif
