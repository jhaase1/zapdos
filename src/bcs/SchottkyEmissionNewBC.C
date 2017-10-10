#include "SchottkyEmissionNewBC.h"

// MOOSE includes
#include "MooseVariable.h"

template <>
InputParameters
validParams<SchottkyEmissionNewBC>()
{
  InputParameters params = validParams<IntegratedBC>();
  params.addRequiredParam<Real>("r", "The reflection coefficient");
  params.addRequiredCoupledVar("potential", "The electric potential");
	
	params.addRequiredCoupledVar("em", "The full electron density.");
  params.addRequiredCoupledVar("mean_en", "The mean energy.");
	
  params.addParam<Real>("position_units", 1, "The units of position.");
  params.addParam<Real>("time_units", 1, "The units of time.");
	
	params.addRequiredParam<Real>("work_function", "The work function of the emitter (eV).");
	params.addRequiredParam<Real>("field_enhancement", "The field enhancement factor, β.");
	params.addRequiredParam<Real>("Richardson_coefficient", "The Richardson coefficient (A/(m^2 K^2)).");
	params.addRequiredParam<Real>("temperature", "The absolute temperature of the emitter (K).");
		
  params.addRequiredParam<std::string>("potential_units", "The potential units.");
  params.addRequiredParam<bool>("use_moles",
                                "Whether to use units of moles as opposed to # of molecules.");
  params.addParam<Real>("tau", 1e-9, "The time constant for ramping the boundary condition.");
  params.addParam<bool>("relax", false, "Use relaxation for emission.");
  return params;
}

SchottkyEmissionNewBC::SchottkyEmissionNewBC(const InputParameters & parameters)
  : IntegratedBC(parameters),

    _use_moles(getParam<bool>("use_moles")),
    _r_units(1. / getParam<Real>("position_units")),
    _t_units(1. / getParam<Real>("time_units")),
    _r(getParam<Real>("r")),

    // Coupled Variables
    _grad_potential(coupledGradient("potential")),
    _potential_id(coupled("potential")),

    _em(coupledValue("em")),
		_em_id(coupled("em")),
		
    _mean_en(coupledValue("mean_en")),
    _mean_en_id(coupled("mean_en")),

    _muem(getMaterialProperty<Real>("muem")),
    _d_muem_d_actual_mean_en(getMaterialProperty<Real>("d_muem_d_actual_mean_en")),
    _massem(getMaterialProperty<Real>("massem")),

    _e(getMaterialProperty<Real>("e")),
    _eps(getMaterialProperty<Real>("eps")),
    _N_A(getMaterialProperty<Real>("N_A")),

    _se_energy(getMaterialProperty<Real>("se_energy")),

    _work_function(getParam<Real>("work_function")),
    _field_enhancement(getParam<Real>("field_enhancement")),
    _Richardson_coefficient(getParam<Real>("Richardson_coefficient")),
    _temperature(getParam<Real>("temperature")),

    _a(0.5),

    _actual_mean_en(0),

    _tau(getParam<Real>("tau")),
    _relax(getParam<bool>("relax")),
    _potential_units(getParam<std::string>("potential_units")),

    // System variables
    _relaxation_expr(1),

    _v_thermal(0),
    _d_v_thermal_d_u(0),

    _v_drift(0),
    _d_v_drift_d_u(0),

    _emission_flux(0),
    _d_emission_flux_d_u(0)
{
  if (_potential_units.compare("V") == 0)
  {
    _voltage_scaling = 1.;
  }
  else if (_potential_units.compare("kV") == 0)
  {
    _voltage_scaling = 1000;
  }

  _dPhi_over_F = 0.00003794686; // sqrt(q / (4*pi*E_0) [eV / ( V / m )]
}

Real
SchottkyEmissionNewBC::computeQpResidual()
{
  _a = (_normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0) ? 1.0 : 0.0;

  _relaxation_expr = _relax ? std::tanh(_t / _tau) : 1.0;

  _actual_mean_en = std::exp(_mean_en[_qp] - _em[_qp]);
	
  _v_thermal = (_r_units / _t_units) *
      std::sqrt(8.0 * _e[_qp] * 2.0 / 3.0 * _actual_mean_en / (M_PI * _massem[_qp]));
			
  _emission_flux = _relaxation_expr * (SchottkyEmissionNewBC::emission_flux());

  return _test[_i][_qp] *
         (
          (1.0 - _a) * _emission_flux
					);
}

Real
SchottkyEmissionNewBC::computeQpJacobian()
{
	_a = (_normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0) ? 1.0 : 0.0;

  _relaxation_expr = _relax ? std::tanh(_t / _tau) : 1.0;

  _actual_mean_en = std::exp(_mean_en[_qp] - _em[_qp]);
	
  _v_thermal = (_r_units / _t_units) *
      std::sqrt(8.0 * _e[_qp] * 2.0 / 3.0 * _actual_mean_en / (M_PI * _massem[_qp]));
			
  _emission_flux = _relaxation_expr * (SchottkyEmissionNewBC::emission_flux());
	
  return 0;
}

Real
SchottkyEmissionNewBC::computeQpOffDiagJacobian(unsigned int jvar)
{
	_a = (_normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0) ? 1.0 : 0.0;

  _relaxation_expr = _relax ? std::tanh(_t / _tau) : 1.0;

  _actual_mean_en = std::exp(_mean_en[_qp] - _em[_qp]);
	
  _v_thermal = (_r_units / _t_units) *
      std::sqrt(8.0 * _e[_qp] * 2.0 / 3.0 * _actual_mean_en / (M_PI * _massem[_qp]));
			
  _emission_flux = _relaxation_expr * (SchottkyEmissionNewBC::emission_flux());
	
	if (jvar == _mean_en_id)
  {
  return 0;
		
  }
  else if (jvar == _potential_id)
  {
    _d_emission_flux_d_u = SchottkyEmissionNewBC::d_emission_flux_d_potential();
		
		return _test[_i][_qp] *
					 (
						(1.0 - _a) * _d_emission_flux_d_u
						);
  }
  else
  {
    return 0.0;
  }
}

Real
SchottkyEmissionNewBC::emission_flux()
{
  Real dPhi;
  Real kB = 8.617385E-5; // eV/K;
  Real _j_TE;
  Real F;

  _a = (_normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0) ? 1.0 : 0.0;
	
  // Schottky emission
  // je = AR * T^2 * exp(-(wf - dPhi) / (kB * T))
  // dPhi = _dPhi_over_F * sqrt(F) // eV

  F = -_field_enhancement * _normals[_qp] * _grad_potential[_qp] * _r_units * _voltage_scaling;

  dPhi = (1.0 - 2.0 * _a) * _dPhi_over_F * sqrt(std::abs(F));

  _j_TE = _Richardson_coefficient * std::pow(_temperature, 2) *
          std::exp(-(_work_function - dPhi) /
                   (kB * _temperature + std::numeric_limits<double>::epsilon()));

  return (_r_units / _t_units) * _j_TE / (-_e[_qp] * (_use_moles ? _N_A[_qp] : 1.0)) * -( _normals[_qp](0) + _normals[_qp](1) + _normals[_qp](2) );
}

Real
SchottkyEmissionNewBC::d_emission_flux_d_potential()
{
  Real dPhi;
  Real kB = 8.617385E-5; // eV/K;
  Real _j_TE;
  Real _d_j_TE_d_potential;
  Real F;

  _a = (_normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0) ? 1.0 : 0.0;

  // Schottky emission
  // je = AR * T^2 * exp(-(wf - dPhi) / (kB * T))
  // dPhi = _dPhi_over_F * sqrt(F) // eV

  F = -_field_enhancement * _normals[_qp] * _grad_potential[_qp];

	dPhi = (1.0 - 2.0 * _a) * _dPhi_over_F * sqrt(std::abs(F));

  _j_TE = _Richardson_coefficient * std::pow(_temperature, 2) *
          exp(-(_work_function - dPhi) /
              (kB * _temperature + std::numeric_limits<double>::epsilon()));

  _d_j_TE_d_potential =
      _j_TE *
      (dPhi / (2 * kB * _temperature + std::numeric_limits<double>::epsilon())) /
      (_grad_potential[_qp] * _normals[_qp] + std::numeric_limits<double>::epsilon());

  return (_r_units / _t_units) * _d_j_TE_d_potential / (-_e[_qp] * (_use_moles ? _N_A[_qp] : 1.0)) * -( _normals[_qp](0) + _normals[_qp](1) + _normals[_qp](2) );
}

Real
SchottkyEmissionNewBC::d_emission_flux_d_em()
{
  return 0.0;
}

Real
SchottkyEmissionNewBC::d_emission_flux_d_mean_en()
{
  return 0.0;
}