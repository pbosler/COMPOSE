#ifndef INCLUDE_CEDR_CAAS_HPP
#define INCLUDE_CEDR_CAAS_HPP

#include "cedr_cdr.hpp"

namespace cedr {
// ClipAndAssuredSum.
namespace caas {

template <typename ExeSpace = Kokkos::DefaultExecutionSpace>
class CAAS : public CDR {
public:
  typedef typename cedr::impl::DeviceType<ExeSpace>::type Device;
  typedef CAAS<ExeSpace> Me;
  typedef std::shared_ptr<Me> Ptr;

public:
  CAAS(const mpi::Parallel::Ptr& p, const Int nlclcells);

  void declare_tracer(int problem_type) override;

  void end_tracer_declarations() override;

  int get_problem_type(const Int& tracer_idx) const override;

  Int get_num_tracers() const override;

  // lclcellidx is trivial; it is the user's index for the cell.
  KOKKOS_INLINE_FUNCTION
  void set_rhom(const Int& lclcellidx, const Real& rhom) override;

  KOKKOS_INLINE_FUNCTION
  void set_Qm(const Int& lclcellidx, const Int& tracer_idx,
              const Real& Qm, const Real& Qm_min, const Real& Qm_max,
              const Real Qm_prev = -1) override;

  void run() override;

  KOKKOS_INLINE_FUNCTION
  Real get_Qm(const Int& lclcellidx, const Int& tracer_idx) override;

private:
  typedef Kokkos::View<Real*, Kokkos::LayoutLeft, Device> RealList;
  typedef cedr::impl::Unmanaged<RealList> UnmanagedRealList;
  typedef Kokkos::View<Int*, Kokkos::LayoutLeft, Device> IntList;

  mpi::Parallel::Ptr p_;
  
  Int nlclcells_;
  std::shared_ptr<std::vector<Int> > tracer_decls_;
  bool need_conserve_;
  IntList tracers_;
  RealList d_, send_, recv_;

  void reduce_locally();
  void reduce_globally();
  void finish_locally();
};

namespace test {
Int unittest(const mpi::Parallel::Ptr& p);
} // namespace test
} // namespace caas
} // namespace cedr

#include "cedr_caas_inl.hpp"

#endif
