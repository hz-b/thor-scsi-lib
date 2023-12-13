#include <pybind11/stl.h>
#include <pybind11/complex.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include "thor_scsi.h"
#include <thor_scsi/elements/field_kick.h>
#include <thor_scsi/elements/drift.h>
#include <thor_scsi/elements/marker.h>
#include <thor_scsi/elements/bpm.h>
#include <thor_scsi/elements/bending.h>
#include <thor_scsi/elements/quadrupole.h>
#include <thor_scsi/elements/sextupole.h>
#include <thor_scsi/elements/octupole.h>
#include <thor_scsi/elements/corrector.h>
#include <thor_scsi/elements/cavity.h>
#include <gtpsa/python/utils.h>

namespace tse = thor_scsi::elements;
namespace tsc = thor_scsi::core;
namespace py = pybind11;

#warning "implement using cdbl properly (templating  elemtype  etc)"
using cdbl = std::complex<double>;
using cdbl_intern = std::complex<double>;

class PyCellVoid: public tsc::CellVoid{
public:
        PyCellVoid(const Config& config)
		: tsc::CellVoid(config)
		{}
	const char * type_name(void) const override {
		PYBIND11_OVERRIDE_PURE(
			const char *, /* Return type */
			tsc::CellVoid,      /* Parent class */
			type_name,          /* Name of function in C++ (must
					       match Python name) */
			/* Argument(s) */
			);
	}
};

// template<class C>
class PyElemType: public tsc::ElemTypeKnobbed/*<C>*/{
	using base = tse::ElemTypeKnobbed /*<C>*/;
public:
	// typedef tsc::ElemTypeKnobbed/*<C>*/ _ElemType;
        PyElemType(const Config& config)
		: tsc::ElemTypeKnobbed/*<C>*/(config)
		{}

	void propagate
	(thor_scsi::core::ConfigType & conf, gtpsa::ss_vect<double>& ps)
		override {
		PYBIND11_OVERRIDE_PURE(void, base, propagate, conf, ps);
	}
	/*
	void propagate
	(thor_scsi::core::ConfigType & conf, gtpsa::ss_vect<tps>& ps) override {
		PYBIND11_OVERRIDE_PURE(void, base, propagate, conf, ps);
	}
	*/
	void propagate
	(thor_scsi::core::ConfigType & conf, gtpsa::ss_vect<gtpsa::tpsa>& ps)
		override {
		PYBIND11_OVERRIDE_PURE(void, base, propagate, conf, ps);
	}
	const char * type_name(void) const override {
		PYBIND11_OVERRIDE_PURE(
			const char *, /* Return type */
			base,         /* Parent class */
			type_name,    /* Name of function in C++ (must match
					 Python name)                         */
			/* Argument(s) */
			);
	}
};

static const char pass_d_doc[] = "pass the element (state as doubles)";
static const char pass_tps_doc[] = "pass the element (state as tps)";
static const char pass_tpsa_doc[] = "pass the element (state as gtpsa)";


template<class C>
struct PyClassicalMagnet : public tse::ClassicalMagnetWithKnob<C> {
	using base = tse::ClassicalMagnetWithKnob <C>;
        PyClassicalMagnet(const Config& config)
		: base(config)
		{}

	int getMainMultipoleNumber(void) const override {
		PYBIND11_OVERRIDE_PURE(
			int,                     /* Return type */
			base,                    /* Parent class */
			getMainMultipoleNumber,  /* Name of function in C++
						    (must match Python name)  */
			/* Argument(s) */
			);
	}
	bool isSkew(void) const override {
		PYBIND11_OVERRIDE_PURE(
			bool,   /* Return type */
			base,   /* Parent class */
			isSkew, /* Name of function in C++ (must match Python
				   name)                                      */
			/* Argument(s) */
			);
	}
};


#if 0
class PyFieldKick: public tse::FieldKick {
	using tse::FieldKick::FieldKick;

	void show(std::ostream& strm, int level) const override {
		const std::string txt = (level <= 1) ? this->pstr() :
			this->repr();
		strm << txt;
	}

	std::string repr(void) const {
		PYBIND11_OVERRIDE_NAME
			(std::string, tsc::FieldKick, "__repr__", repr);
	}

	std::string pstr(void) const {
		PYBIND11_OVERRIDE_NAME
			(std::string, tsc::FieldKick, "__str__", pstr);
	}
};
#endif


// not using the wrapper but directly the double type:
// TpsaOrComplex  also understands of double of gtpsa::tpsa objects are passed
template<typename double_type, typename Class>
void field_kick_add_set_methods(py::class_<Class> t_mapper)
{
	t_mapper
		.def("set_dx", [](Class &kick, const double_type& dx) { kick.getTransform()->setDx(dx) ; })
		.def("set_dy", [](Class &kick, const double_type& dy) { kick.getTransform()->setDy(dy) ; })
		/*
		 * Todo: seems to crash the interpreter
		*/
		.def("set_roll", [](Class &kick, const double_type& roll)
			{kick.getTransform()->setRoll(roll);},
			py::keep_alive<1, 2>())
		;
}

namespace gpy = gtpsa::python;


template<typename Class>
void field_kick_add_get_methods(py::class_<Class> t_mapper)
{
	t_mapper
		.def("get_dx",   [](Class &kick) { return gpy::to_pyobject( kick.getTransform()->getDx());   })
		.def("get_dy",   [](Class &kick) { return gpy::to_pyobject( kick.getTransform()->getDy());   })
		.def("get_roll", [](Class &kick) { return gpy::to_pyobject( kick.getTransform()->getRoll()); })
		;
}

template<typename double_type, typename Class>
void field_kick_add_methods(py::class_<Class> t_mapper)
{

	//using double_type;
	field_kick_add_set_methods<double_type, Class>(t_mapper);
	field_kick_add_get_methods(t_mapper);

	t_mapper
		.def("is_thick",		        &Class::isThick                      )
                .def("as_thick",                        &Class::asThick                      )
                .def("get_number_of_integration_steps", &Class::getNumberOfIntegrationSteps  )
                .def("set_number_of_integration_steps", &Class::setNumberOfIntegrationSteps  )
                .def("get_integration_method",          &Class::getIntegrationMethod         )
                .def("assuming_curved_trajectory",      &Class::assumingCurvedTrajectory     )
                .def("set_curvature",                   &Class::setCurvature                 )
                .def("set_bending_angle",               &Class::setBendingAngle              )
                .def("set_entrance_angle",              &Class::setEntranceAngle             )
                .def("set_exit_angle",                  &Class::setExitAngle                 )
                .def("get_radiation_delegate",          &Class::getRadiationDelegate         )
                .def("set_radiation_delegate",          &Class::setRadiationDelegate         )
                .def("get_field_interpolator",          &Class::getFieldInterpolator         )
                .def("set_field_interpolator",          &Class::setFieldInterpolator         )

                .def("get_curvature",      [](const Class& kick) { return gpy::to_pyobject( kick.getCurvature()     ); })
                .def("get_bending_angle",  [](const Class& kick) { return gpy::to_pyobject( kick.getBendingAngle()  ); })
                .def("get_entrance_angle", [](const Class& kick) { return gpy::to_pyobject( kick.getEntranceAngle() ); })
                .def("get_exit_angle",     [](const Class& kick) { return gpy::to_pyobject( kick.getExitAngle()     ); })
		;
}


template<typename complex_type, typename Class>
void classical_magnet_add_set_methods(py::class_<Class> t_mapper)
{
	t_mapper
		.def("set_main_multipole_strength",
		     [](Class &inst, const complex_type v)
			     {inst.setMainMultipoleStrength(v);})
		;
}
template<typename Types, typename Class>
void classical_magnet_add_methods(py::class_<Class> t_mapper)
{
	// using double_type = typename Types::double_type;
	using complex_type = typename Types::complex_type;

	classical_magnet_add_set_methods<complex_type, Class>(t_mapper);
	t_mapper
		.def("get_multipoles",
		     &Class::getMultipoles)
		// .def("getBegninMultipoles",
		//      &tse::ClassicalMagnet::getBegninMultipoles)
		// .def("set_multipoles",&Class::setMultipoles)
		.def("get_main_multipole_number",
		     &Class::getMainMultipoleNumber)
		.def("get_main_multipole_strength", [](Class& inst){
			return gpy::to_pyobject(inst.getMainMultipoleStrength());
		})
		.def("get_main_multipole_strength_component",
		     &Class::getMainMultipoleStrengthComponent)

		// .def("set_main_multipole_strength",
		//      py::overload_cast<const double_type>
		//      (&Class::setMainMultipoleStrength))
		// .def("set_main_multipole_strength",
		//      [](Class &inst, const double_type v)
		// 	     {inst.setMainMultipoleStrength(v);})
		.def("propagate",
		     py::overload_cast<tsc::ConfigType&,
		     gtpsa::ss_vect<double>&>(&Class::propagate), pass_d_doc)
		// .def("propagate",
		//      py::overload_cast<tsc::ConfigType&,
		//      gtpsa::ss_vect<tps>&>
		//      (&tse::ClassicalMagnet::propagate),
		//      pass_tps_doc)
		.def("propagate",
		     py::overload_cast<tsc::ConfigType&,
		     gtpsa::ss_vect<gtpsa::tpsa>&> (&Class::propagate),
		     pass_tps_doc)
		;
}


template<class C>
struct TemplatedClasses
{

	std::string m_suffix;
	py::module& m_module;

	TemplatedClasses(py::module &m, std::string suffix)
		: m_suffix(suffix)
		, m_module(m)
  		{}

	// auto buildClasses
	// (py::class_<tsc::CellVoid,
	//  std::shared_ptr<tsc::CellVoid>>& cell_void)
	typedef tse::FieldKickKnobbed<C> FieldKickK;

	auto buildClasses
	(py::class_<tsc::ElemType, PyElemType, std::shared_ptr<tsc::ElemType>>& elem_type,
	 py::class_<FieldKickK, std::shared_ptr<FieldKickK>>& field_kick){

		typedef tse::DriftTypeWithKnob<C> DriftTypeK;
		std::string drift_type_name = "Drift" + this->m_suffix;
		py::class_<DriftTypeK, std::shared_ptr<DriftTypeK>> drift
			(this->m_module, drift_type_name.c_str(), elem_type);
		drift
			.def(py::init<const Config &>());

		field_kick_add_methods<typename C::double_type, FieldKickK>(field_kick);
		field_kick
			/*
			.def("getTransform",
			      &tse::FieldKick::getTransform) // causes segfault
			*/
			.def(py::init<const Config &>());

		std::string mpole_name = "Mpole" + this->m_suffix;
		typedef tse::MpoleTypeWithKnob<C> MpoleK;
		py::class_<MpoleK, std::shared_ptr<MpoleK>> mpole_type
			(this->m_module, mpole_name.c_str(), field_kick);
		mpole_type
			.def("getFieldInterpolator",
			     &MpoleK::getFieldInterpolator)
			.def(py::init<const Config &>());

		//typedef  ClassicalMagnetK;
		std::string cm_name = "ClassicalMagnet" + this->m_suffix;
		py::class_<
			tse::ClassicalMagnetWithKnob<C>,
			PyClassicalMagnet<C>,
			std::shared_ptr<tse::ClassicalMagnetWithKnob<C>>
			> cm(this->m_module, cm_name.c_str(), mpole_type);
		classical_magnet_add_methods<C, tse::ClassicalMagnetWithKnob<C>>(cm);
		cm
			.def(py::init<const Config &>());

		std::string quad_name = "Quadrupole" + this->m_suffix;
		typedef tse::QuadrupoleTypeWithKnob<C> QuadK;
		py::class_<QuadK, std::shared_ptr<QuadK>>
			(this->m_module, quad_name.c_str(), cm)
			.def(py::init<const Config &>());

		std::string sext_name = "Sextupole" + this->m_suffix;
		typedef tse::SextupoleTypeWithKnob<C> SextK;
		py::class_<SextK, std::shared_ptr<SextK>>
			(this->m_module, sext_name.c_str(), cm)
			.def(py::init<const Config &>());

		std::string oct_name = "Octupole" + this->m_suffix;
		typedef tse::OctupoleTypeWithKnob<C> OctK;
		py::class_<OctK, std::shared_ptr<OctK>>
			(this->m_module, oct_name.c_str(), cm)
			.def(py::init<const Config &>());

		std::string bending_name = "Bending" + this->m_suffix;
		typedef tse::BendingTypeWithKnob<C> BendingK;
		py::class_<BendingK, std::shared_ptr<BendingK>>
			(this->m_module, bending_name.c_str(), cm)
			.def(py::init<const Config &>());

		std::string hor_st_name = "HorizontalSteerer" + this->m_suffix;
		typedef tse::HorizontalSteererTypeWithKnob<C> HorStK;
		py::class_<HorStK, std::shared_ptr<HorStK>>
			(this->m_module, hor_st_name.c_str(), cm)
			.def(py::init<const Config &>());

		std::string ver_st_name = "VerticalSteerer" + this->m_suffix;
		typedef tse::VerticalSteererTypeWithKnob<C> VerStK;
		py::class_<VerStK, std::shared_ptr<VerStK>>
			(this->m_module, ver_st_name.c_str(), cm)
			.def(py::init<const Config &>());


		return cm;
	}
};

/*
 * I tested if the derived classes have to mention that their memory needs to
 * be managed by shared pointers. Worked for me only if it is done by shared
 * pointers
 */
void py_thor_scsi_init_elements(py::module &m)
{
	py::class_<tsc::CellVoid, std::shared_ptr<tsc::CellVoid>> cell_void
		(m, "CellVoid");
	cell_void
		.def_readonly("name",  &tsc::CellVoid::name)
		.def_readonly("index", &tsc::CellVoid::index)
		.def("config", &tsc::CellVoid::conf)
		;

	std::string elem_type_name = "ElemType";
	py::class_<tsc::ElemTypeKnobbed/*<C>*/, PyElemType/*<C>*/,  std::shared_ptr<tsc::ElemTypeKnobbed/*<C>*/>> elem_type
		(m, elem_type_name.c_str(), cell_void);
	elem_type.def(py::init<const Config &>());

	elem_type
		.def("__str__",      &tsc::ElemTypeKnobbed/*<C>*/::pstr)
		.def("__repr__",     &tsc::ElemTypeKnobbed/*<C>*/::repr)
		.def("get_length",   &tsc::ElemTypeKnobbed/*<C>*/::getLength)
		.def("set_length",   &tsc::ElemTypeKnobbed/*<C>*/::setLength)
		.def("get_observer", &tsc::ElemTypeKnobbed/*<C>*/::observer)
		.def("set_observer",
		     &tsc::ElemTypeKnobbed/*<C>*/::set_observer)
		.def("get_aperture",
		     &tsc::ElemTypeKnobbed/*<C>*/::getAperture)
		.def("set_aperture",
		     &tsc::ElemTypeKnobbed/*<C>*/::setAperture)
		.def("propagate",
		     py::overload_cast<tsc::ConfigType&,
		     gtpsa::ss_vect<double>&>
		     (&tse::ElemTypeKnobbed/*<C>*/::propagate), pass_d_doc)
		.def("propagate",
		     py::overload_cast<tsc::ConfigType&,
		     gtpsa::ss_vect<gtpsa::tpsa>&>
		     (&tse::ElemTypeKnobbed/*<C>*/::propagate), pass_d_doc)
		// .def("propagate",
		//      py::overload_cast<tsc::ConfigType&,
		//      gtpsa::ss_vect<tps>&>
		//      (&tse::ElemType::propagate), pass_tpsa_doc)
                ;

	typedef tse::FieldKickKnobbed<tsc::StandardDoubleType> FieldKickKD;
	py::class_<FieldKickKD, std::shared_ptr<FieldKickKD>> field_kick
		(m, "FieldKick", elem_type);

	typedef tse::FieldKickKnobbed<tsc::TpsaVariantType> FieldKickKT;
	py::class_<FieldKickKT, std::shared_ptr<FieldKickKT>> field_kick_tpsa
		(m, "FieldKickTpsa", elem_type);

	// make it directly callable by doubles
	field_kick_add_set_methods<double, FieldKickKT>(field_kick_tpsa);
	// and gtpsa::tpsa objects
	field_kick_add_set_methods<gtpsa::tpsa, FieldKickKT>(field_kick_tpsa);

	TemplatedClasses<tsc::StandardDoubleType> templated_classes_std(m, "");
	// required as marker and bpm are not knobbed yet
	auto classical_magnet =	templated_classes_std.buildClasses(elem_type, field_kick);

	// Device classes / types with knobs ... handled by these two lines
	TemplatedClasses<tsc::TpsaVariantType> templated_classes_tpsa
		(m, "Tpsa");
	auto classical_magnet_tpsa = templated_classes_tpsa.buildClasses(elem_type, field_kick_tpsa);
	classical_magnet_add_set_methods<double,      tse::ClassicalMagnetWithKnob<tsc::StandardDoubleType>> (classical_magnet_tpsa);
	classical_magnet_add_set_methods<gtpsa::tpsa, tse::ClassicalMagnetWithKnob<tsc::TpsaVariantType   >> (classical_magnet_tpsa);



	// classes without knobs follow

	py::class_<tse::MarkerType, std::shared_ptr<tse::MarkerType>>
		(m, "Marker", elem_type)
		.def("get_radiation_delegate",
		     &tse::MarkerType::getRadiationDelegate)
		.def("set_radiation_delegate",
		     &tse::MarkerType::setRadiationDelegate)
		.def(py::init<const Config &>());

	py::class_<tse::BPMType, std::shared_ptr<tse::BPMType>>
		(m, "BPM", elem_type)
		.def(py::init<const Config &>());


	//, std::shared_ptr<tse::>
	py::class_<tse::CavityType, std::shared_ptr<tse::CavityType>>
		(m, "Cavity", elem_type)
		.def("set_frequency",       &tse::CavityType::setFrequency)
		.def("get_frequency",       &tse::CavityType::getFrequency)
		.def("set_voltage",         &tse::CavityType::setVoltage)
		.def("get_voltage",         &tse::CavityType::getVoltage)
		.def("set_phase",           &tse::CavityType::setPhase)
		.def("get_phase",           &tse::CavityType::getPhase)
		.def("set_harmonic_number", &tse::CavityType::setHarmonicNumber)
		.def("get_harmonic_number", &tse::CavityType::getHarmonicNumber)
		.def(py::init<const Config &>());

	/*
	 * Needs to be defined as shared ptr class as it is returned as shared
	 * pointer  by classical magnet's method getMultipoles
	 */


#if 0
	py::class_<WigglerType,    ElemType>(m, "WigglerType")
		.def(py::init<>());

	py::class_<InsertionType,  ElemType>(m, "InsertionType")
		.def(py::init<>());

	py::class_<FieldMapType,   ElemType>(m, "FieldMapType")
		.def(py::init<>());

	py::class_<SpreaderType,   ElemType>(m, "SpreaderType")
		.def(py::init<>());

	py::class_<RecombinerType, ElemType>(m, "RecombinerType")
		.def(py::init<>());

	py::class_<SolenoidType,   ElemType>(m, "SolenoidType")
		.def(py::init<>());

	py::class_<MapType,        ElemType>(m, "MapType")
		.def(py::init<>());
#endif
}
/*
 * Local Variables:
 * mode: c++
 * c-file-style: "python"
 * End:
 */
