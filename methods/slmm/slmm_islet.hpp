#ifndef INCLUDE_SLMM_ISLET_HPP
#define INCLUDE_SLMM_ISLET_HPP

#include "slmm_gll.hpp"

/* Generate the weights using:
     ./slmm_test -c islet_compute
 */

namespace slmm {
namespace islet {

void eval(const Int& np, const Real* const xnodes,
          const Int* subnp, Int const* const* nodes,
          const Real& x, Real* const v);

template <int np, int nreg>
bool evalon (const Real* const xnode,
             const std::array<int, nreg>& subnp,
             const std::array<int, nreg>& os,
             const Real& x, Real* const v) {
  if (x > 0) {
    evalon<np, nreg>(xnode, subnp, os, -x, v);
    for (int i = 0; i < np/2; ++i)
      std::swap(v[i], v[np-i-1]);
    return true;
  }
  bool done = false;
  for (int i = 0; i < nreg; ++i) {
    if (x > xnode[i+1]) continue;
    assert(i == 0 || x >= xnode[i]);
    assert( ! done);
    done = true;
    if (subnp[i] == np) {
      Basis::eval_lagrange_poly(np, xnode, x, v);
    } else {
      std::fill(v, v + np, 0);
      Basis::eval_lagrange_poly(subnp[i], xnode + os[i], x, v + os[i]);
    }
    break;
  }
  if ( ! done)
    Basis::eval_lagrange_poly(np, xnode, x, v);
  return true;
}

struct GllOffsetNodal : public GLL {
  const char* name () const override { return "islet::GllOffsetNodal"; }
  bool get_w(const Int& np, const Real*& wt) const override;
  bool eval(const Int& np, const Real& x, Real* const v) const override;
  bool eval_derivative(const Int& np, const Real& x, Real* const v) const override;
  Int max_degree(const Int& np) const override;

protected:
  const Real w_np2[2] = { 1.0000000000000000e+00, 1.0000000000000000e+00};
  const Real w_np3[3] = { 3.3333333333333331e-01, 1.3333333333333333e+00, 3.3333333333333331e-01};
  const Real w_np4[4] = { 1.6666666666666666e-01, 8.3333333333333337e-01, 8.3333333333333337e-01, 1.6666666666666666e-01};
  const Real w_np5[5] = { 7.2438673929622860e-02, 6.0875420527532442e-01, 6.3761424159010549e-01, 6.0875420527532442e-01, 7.2438673929622860e-02};
  const Real w_np6[6] = { 6.6666666666666624e-02, 3.7847495629784700e-01, 5.5485837703548646e-01, 5.5485837703548646e-01, 3.7847495629784700e-01, 6.6666666666666624e-02};
  const Real w_np7[7] = { 5.6454983633034334e-02, 2.5552182504453469e-01, 4.5835116513528573e-01, 4.5934405237429038e-01, 4.5835116513528573e-01, 2.5552182504453469e-01, 5.6454983633034334e-02};
  const Real w_np8[8] = { 4.3144193831569533e-02, 1.9497214769017937e-01, 3.5470956393990549e-01, 4.0717409453834563e-01, 4.0717409453834563e-01, 3.5470956393990549e-01, 1.9497214769017937e-01, 4.3144193831569533e-02};
  const Real w_np9[9] = { 4.1812271854496312e-02, 1.3123902435694160e-01, 3.1866016571917827e-01, 2.9686582599803263e-01, 4.2284542414270215e-01, 2.9686582599803263e-01, 3.1866016571917827e-01, 1.3123902435694160e-01, 4.1812271854496312e-02};
  const Real w_np10[10] = { 1.5509733280217758e-02, 1.4842357596604355e-01, 2.0911374516621034e-01, 3.0367249606634206e-01, 3.2328044952118629e-01, 3.2328044952118629e-01, 3.0367249606634206e-01, 2.0911374516621034e-01, 1.4842357596604355e-01, 1.5509733280217758e-02};
  const Real w_np11[11] = { 1.4115415593113077e-02, 1.1746481483677482e-01, 1.8251645617210899e-01, 2.4597010811609454e-01, 2.9538296815410536e-01, 2.8910047425560659e-01, 2.9538296815410536e-01, 2.4597010811609454e-01, 1.8251645617210899e-01, 1.1746481483677482e-01, 1.4115415593113077e-02};
  const Real w_np12[12] = { 9.2548354381213702e-03, 1.0539058985971034e-01, 1.4237539955323250e-01, 2.2648452767205887e-01, 2.4168148450452953e-01, 2.7481316297234754e-01, 2.7481316297234754e-01, 2.4168148450452953e-01, 2.2648452767205887e-01, 1.4237539955323250e-01, 1.0539058985971034e-01, 9.2548354381213702e-03};
  const Real w_np13[13] = { 1.5986387115823793e-02, 7.1039463009726772e-02, 1.4100673941822789e-01, 1.8090611106261884e-01, 2.1922209886060423e-01, 2.4908449372434635e-01, 2.4550941361730400e-01, 2.4908449372434635e-01, 2.1922209886060423e-01, 1.8090611106261884e-01, 1.4100673941822789e-01, 7.1039463009726772e-02, 1.5986387115823793e-02};
  const Real w_np16[16] = { 6.6054381853532362e-03, 5.4731980471730592e-02, 8.5313396530798766e-02, 1.2750075018473614e-01, 1.5206243160880162e-01, 1.7830331300698002e-01, 1.9354559587015541e-01, 2.0193709414144417e-01, 2.0193709414144417e-01, 1.9354559587015541e-01, 1.7830331300698002e-01, 1.5206243160880162e-01, 1.2750075018473614e-01, 8.5313396530798766e-02, 5.4731980471730592e-02, 6.6054381853532362e-03};
};

struct GllNodal : public GllOffsetNodal {
  const char* name () const override { return "islet::GllNodal"; }
  bool get_w(const Int& np, const Real*& wt) const override;
  bool eval(const Int& np, const Real& x, Real* const v) const override;
  bool eval_derivative(const Int& np, const Real& x, Real* const v) const override;

private:
  const Real w_np2[2] = { 1.0000000000000000e+00, 1.0000000000000000e+00};
  const Real w_np3[3] = { 3.3333333333333331e-01, 1.3333333333333333e+00, 3.3333333333333331e-01};
  const Real w_np4[4] = { 1.6666666666666666e-01, 8.3333333333333337e-01, 8.3333333333333337e-01, 1.6666666666666666e-01};
  const Real w_np5[5] = { 7.2438673929622860e-02, 6.0875420527532442e-01, 6.3761424159010549e-01, 6.0875420527532442e-01, 7.2438673929622860e-02};
  const Real w_np6[6] = { 6.6666666666666624e-02, 3.7847495629784705e-01, 5.5485837703548646e-01, 5.5485837703548646e-01, 3.7847495629784705e-01, 6.6666666666666624e-02};
  const Real w_np7[7] = { 5.6454983633034334e-02, 2.5552182504453469e-01, 4.5835116513528573e-01, 4.5934405237429038e-01, 4.5835116513528573e-01, 2.5552182504453469e-01, 5.6454983633034334e-02};
  const Real w_np8[8] = { 4.3144193831569533e-02, 1.9497214769017937e-01, 3.5470956393990549e-01, 4.0717409453834563e-01, 4.0717409453834563e-01, 3.5470956393990549e-01, 1.9497214769017937e-01, 4.3144193831569533e-02};
  const Real w_np9[9] = { 3.6046050775536347e-02, 1.4531360464413259e-01, 3.0053239765036854e-01, 3.1722918197442412e-01, 4.0175752991107733e-01, 3.1722918197442412e-01, 3.0053239765036854e-01, 1.4531360464413259e-01, 3.6046050775536347e-02};
  const Real w_np10[10] = { 1.5509733280217758e-02, 1.4842357596604355e-01, 2.0911374516621034e-01, 3.0367249606634206e-01, 3.2328044952118629e-01, 3.2328044952118629e-01, 3.0367249606634206e-01, 2.0911374516621034e-01, 1.4842357596604355e-01, 1.5509733280217758e-02};
  const Real w_np11[11] = { 1.4115415593113077e-02, 1.1746481483677482e-01, 1.8251645617210899e-01, 2.4597010811609454e-01, 2.9538296815410536e-01, 2.8910047425560659e-01, 2.9538296815410536e-01, 2.4597010811609454e-01, 1.8251645617210899e-01, 1.1746481483677482e-01, 1.4115415593113077e-02};
  const Real w_np12[12] = { 9.2548354381213702e-03, 1.0539058985971034e-01, 1.4237539955323250e-01, 2.2648452767205887e-01, 2.4168148450452953e-01, 2.7481316297234754e-01, 2.7481316297234754e-01, 2.4168148450452953e-01, 2.2648452767205887e-01, 1.4237539955323250e-01, 1.0539058985971034e-01, 9.2548354381213702e-03};
  const Real w_np13[13] = { 1.5986387115823793e-02, 7.1039463009726772e-02, 1.4100673941822789e-01, 1.8090611106261884e-01, 2.1922209886060423e-01, 2.4908449372434635e-01, 2.4550941361730400e-01, 2.4908449372434635e-01, 2.1922209886060423e-01, 1.8090611106261884e-01, 1.4100673941822789e-01, 7.1039463009726772e-02, 1.5986387115823793e-02};
};

struct FreeNodal : public Basis {
  const char* name () const override { return "islet::FreeNodal"; }
  bool gll_nodes () const override { return false; }
  bool get_x(const Int& np, const Real*& coord) const override;
  bool get_w(const Int& np, const Real*& wt) const override;
  bool eval(const Int& np, const Real& x, Real* const v) const override;
  bool eval_derivative(const Int& np, const Real& x, Real* const v) const override;

private:
  const Real x_np4[4] = {-1, -4.4721359549995793e-01, 4.4721359549995793e-01, 1};
  const Real x_np5[5] = {-1, -6.6678658540509828e-01, 0, 6.6678658540509828e-01, 1};
  const Real x_np6[6] = {-1, -7.6692663677851514e-01, -3.0080515728048823e-01, 3.0080515728048823e-01, 7.6692663677851514e-01, 1};
  const Real x_np7[7] = {-1, -9.0990710644769845e-01, -5.2121920370139296e-01, 0, 5.2121920370139296e-01, 9.0990710644769845e-01, 1};
  const Real x_np8[8] = {-1, -8.5140924689985531e-01, -6.8076136583943381e-01, -3.3295319583926342e-01, 3.3295319583926342e-01, 6.8076136583943381e-01, 8.5140924689985531e-01, 1};
  const Real x_np10[10] = {-1, -9.1953390816645886e-01, -7.3979280618087628e-01, -5.5608644784645889e-01, -2.3500601793189407e-01, 2.3500601793189407e-01, 5.5608644784645889e-01, 7.3979280618087628e-01, 9.1953390816645886e-01, 1};

  const Real w_np4[4] = { 1.6666666666666666e-01, 8.3333333333333326e-01, 8.3333333333333326e-01, 1.6666666666666666e-01};
  const Real w_np5[5] = { 4.9870438822580979e-02, 6.3756212508301224e-01, 6.2513487218881347e-01, 6.3756212508301224e-01, 4.9870438822580979e-02};
  const Real w_np6[6] = { 7.2085444326295170e-02, 3.5621719740552549e-01, 5.7169735826817936e-01, 5.7169735826817936e-01, 3.5621719740552549e-01, 7.2085444326295170e-02};
  const Real w_np7[7] = { 6.5052894249013657e-02, 1.3476067847565915e-01, 5.7683298128860117e-01, 4.4670689197345193e-01, 5.7683298128860117e-01, 1.3476067847565915e-01, 6.5052894249013657e-02};
  const Real w_np8[8] = { 1.2725008869179433e-02, 3.3555941579644766e-01, 1.4505431324948675e-02, 6.3721014400942422e-01, 6.3721014400942422e-01, 1.4505431324948675e-02, 3.3555941579644766e-01, 1.2725008869179433e-02};
  const Real w_np10[10] = { 2.8881226543594377e-02, 1.1714238963320928e-01, 2.3127422749964027e-01, 1.7494931493842375e-01, 4.4775284138513227e-01, 4.4775284138513227e-01, 1.7494931493842375e-01, 2.3127422749964027e-01, 1.1714238963320928e-01, 2.8881226543594377e-02};
};

struct UniformOffsetNodal : public Basis {
  const char* name () const override { return "UniformOffsetNodal"; }
  Int max_degree(const Int& np) const override;
  bool gll_nodes () const override { return false; }
  bool get_x(const Int& np, const Real*& x) const override;
  bool get_w(const Int& np, const Real*& wt) const override;
  bool eval(const Int& np, const Real& x, Real* const v) const override;
  bool eval_derivative(const Int& np, const Real& x, Real* const v) const override;
private:
  const Real w_np2[2] = { 1.0000000000000000e+00, 1.0000000000000000e+00};
  const Real w_np3[3] = { 3.3333333333333331e-01, 1.3333333333333333e+00, 3.3333333333333331e-01};
  const Real w_np4[4] = { 2.4999999999999992e-01, 7.5000000000000000e-01, 7.5000000000000000e-01, 2.4999999999999992e-01};
  const Real w_np5[5] = { 1.8750000000000006e-01, 5.8333333333333337e-01, 4.5833333333333343e-01, 5.8333333333333337e-01, 1.8750000000000006e-01};
  const Real w_np6[6] = { 1.5305555555555561e-01, 4.5750000000000002e-01, 3.8944444444444448e-01, 3.8944444444444448e-01, 4.5750000000000002e-01, 1.5305555555555561e-01};
  const Real w_np7[7] = { 1.2754629629629630e-01, 3.8379629629629636e-01, 3.1689814814814821e-01, 3.4351851851851856e-01, 3.1689814814814821e-01, 3.8379629629629636e-01, 1.2754629629629630e-01};
  const Real w_np8[8] = { 9.5238095238095247e-02, 3.6904761904761907e-01, 2.3809523809523803e-01, 2.9761904761904762e-01, 2.9761904761904762e-01, 2.3809523809523803e-01, 3.6904761904761907e-01, 9.5238095238095247e-02};
  const Real w_np9[9] = { 8.3333333333333329e-02, 3.2291666666666669e-01, 2.0833333333333337e-01, 2.6041666666666669e-01, 2.5000000000000006e-01, 2.6041666666666669e-01, 2.0833333333333337e-01, 3.2291666666666669e-01, 8.3333333333333329e-02};
  const Real w_np10[10] = { 7.7469135802469141e-02, 2.7345679012345675e-01, 2.0555555555555555e-01, 2.1790123456790123e-01, 2.2561728395061736e-01, 2.2561728395061736e-01, 2.1790123456790123e-01, 2.0555555555555555e-01, 2.7345679012345675e-01, 7.7469135802469141e-02};
  const Real w_np11[11] = { 6.9722222222222227e-02, 2.4611111111111111e-01, 1.8500000000000008e-01, 1.9611111111111118e-01, 2.0305555555555560e-01, 1.9999999999999998e-01, 2.0305555555555560e-01, 1.9611111111111118e-01, 1.8500000000000008e-01, 2.4611111111111111e-01, 6.9722222222222227e-02};
  const Real w_np12[12] = { 6.3383838383838390e-02, 2.2651515151515156e-01, 1.5707070707070694e-01, 1.9494949494949498e-01, 1.7348484848484846e-01, 1.8459595959595959e-01, 1.8459595959595959e-01, 1.7348484848484846e-01, 1.9494949494949498e-01, 1.5707070707070694e-01, 2.2651515151515156e-01, 6.3383838383838390e-02};
  const Real w_np13[13] = { 5.9374999999999990e-02, 2.0127314814814809e-01, 1.5671296296296305e-01, 1.6597222222222227e-01, 1.6539351851851855e-01, 1.6793981481481482e-01, 1.6666666666666663e-01, 1.6793981481481482e-01, 1.6539351851851855e-01, 1.6597222222222227e-01, 1.5671296296296305e-01, 2.0127314814814809e-01, 5.9374999999999990e-02};
};

namespace impl { struct Nodes; }

struct GllNodalFromString : public GLL {
  GllNodalFromString();
  ~GllNodalFromString();

  bool init(const std::string& basis);

  const char* name () const override { return "islet::GllNodalFromString"; }
  bool gll_nodes () const override { return true; }
  bool get_w(const Int& np, const Real*& wt) const override;
  bool eval(const Int& np, const Real& x, Real* const v) const override;
  bool eval_derivative(const Int& np, const Real& x, Real* const v) const override;

private:
  impl::Nodes* nodes;
  Real wts[Basis::np_max];
};

struct FreeNodalFromString : public Basis {
  FreeNodalFromString();
  ~FreeNodalFromString();

  bool init(const std::string& basis);

  const char* name () const override { return "islet::FreeNodalFromString"; }
  bool gll_nodes () const override { return false; }
  bool get_x(const Int& np, const Real*& coord) const override;
  bool get_w(const Int& np, const Real*& wt) const override;
  bool eval(const Int& np, const Real& x, Real* const v) const override;
  bool eval_derivative(const Int& np, const Real& x, Real* const v) const override;

private:
  impl::Nodes* nodes;
  Real xnodes[Basis::np_max], wts[Basis::np_max];
};

void unittest_Nodes();

} // namespace islet
} // namespace slmm

#endif
