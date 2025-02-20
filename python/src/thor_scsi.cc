#include <pybind11/pybind11.h>
#include <thor_scsi/version.h>
#include <thor_scsi/core/constants.h>
#include <thor_scsi/elements/constants.h>

#include "thor_scsi.h"
#include <gtpsa/lielib.hpp>

#include <string>
#include <sstream>

namespace py = pybind11;
namespace tse = thor_scsi::elements;
namespace tsc = thor_scsi::core;


PYBIND11_MODULE(lib, m) {
    m.doc() = "Self-Consistent Symplectic Integrator (SCSI)";

    py_thor_scsi_init_field_interpolation(m);
    py_thor_scsi_init_elements(m);
    py_thor_scsi_init_aperture(m);
    py_thor_scsi_init_radiation(m);
    py_thor_scsi_init_observers(m);
    py_thor_scsi_init_accelerator(m);
    py_thor_scsi_init_config_type(m);
    // py_thor_scsi_init_lattice(scsi);


    // Beamline class.

    // Constants.
    //scsi.attr("HOMmax") = HOMmax;
    m.attr("c0")     = tse::speed_of_light;
    // scsi.attr("q_e")    = tsc::q_e;
    // scsi.attr("m_e")    = tsc::m_e;
    // scsi.attr("mu_0")   = tsc::mu_0;
    // scsi.attr("eps_0")  = tsc::eps_0;
    // scsi.attr("r_e")    = tsc::r_e;
    // scsi.attr("h_bar")  = tsc::h_bar;
    /*
    */
    // Enums.

    m.def("inv", &gtpsa::minv);
    m.def("pinv", &gtpsa::pminv);
    m.def("compose", &gtpsa::compose);
    // m.def("integ", &gtpsa::integ);
    m.def("M_to_h_DF", &gtpsa::M_to_h_DF);

    py::enum_<tsc::spatial_index>(m, "spatial_index")
	.value("X", tsc::X_)
	.value("Y", tsc::Y_)
	.value("Z", tsc::Z_);

    // does not follow standard convention
    py::enum_<tsc::phase_space_index>(m, "phase_space_index_internal")
	.value("x",     tsc::x_)
	.value("px",    tsc::px_)
	.value("y",     tsc::y_)
	.value("py",    tsc::py_)
	.value("delta", tsc::delta_)
	.value("ct",    tsc::ct_);

}
