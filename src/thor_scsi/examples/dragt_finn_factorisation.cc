#include <gtpsa/ss_vect.h>
#include <gtpsa/lielib.hpp>
#include <thor_scsi/elements/sextupole.h>
#include <iostream>

namespace tsc = thor_scsi::core;
namespace tse = thor_scsi::elements;

int main(int argc, char *argv[])
{
  const int mo = 4, nv = 6;
  auto desc = std::make_shared<gtpsa::desc>(nv, mo);
  //const auto t =
  Config C;
  C.set<std::string>("name", "test");
  C.set<double>("K", 1e0);
  C.set<double>("L", 0e0);
  C.set<double>("N", 1);

  tsc::ConfigType calc_config;
  auto sext = tse::SextupoleType(C);

  auto ps = gtpsa::ss_vect<gtpsa::tpsa>(desc, 4);
  ps.set_identity();
  sext.propagate(calc_config, ps);


  std::cout << "after sextupole\n" << ps << std::endl;

  for (int i = 0; i < ps.size(); i++) {
    ps[i].print();
  }

  auto lie_factors = gtpsa::M_to_h_DF(ps);
  std::cout << "lie factors\n" << lie_factors << std::endl;
  lie_factors.print();
}
