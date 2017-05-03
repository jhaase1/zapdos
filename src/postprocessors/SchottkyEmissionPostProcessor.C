#include "SchottkyEmissionPostProcessor.h"
#include "MooseVariable.h"

template <>
InputParameters
validParams<SchottkyEmissionPostProcessor>()
{
  InputParameters params = validParams<SideIntegralVariablePostprocessor>();

  params.addRequiredParam<Real>("r", "The reflection coefficient");

  params.addRequiredCoupledVar("potential", "The electric potential");
  params.addRequiredCoupledVar("ip", "The ion density.");

  params.addParam<Real>("position_units", 1, "The units of position.");
  params.addParam<Real>("time_units", 1, "The units of time.");
  params.addRequiredParam<std::string>("potential_units", "The potential units.");
  params.addRequiredParam<bool>("use_moles",
                                "Whether to use units of moles as opposed to # of molecules.");

  return params;
}

SchottkyEmissionPostProcessor::SchottkyEmissionPostProcessor(const InputParameters & params)
  : SideIntegralVariablePostprocessor(params),

    _use_moles(getParam<bool>("use_moles")),
    _r_units(1. / getParam<Real>("position_units")),
    _t_units(1. / getParam<Real>("time_units")),
    _r(getParam<Real>("r")),

    // Coupled Variables
    _grad_potential(coupledGradient("potential")),
    _ip_var(*getVar("ip", 0)),
    _ip(coupledValue("ip")),
    _grad_ip(coupledGradient("ip")),

    _sgnip(getMaterialProperty<Real>("sgn" + _ip_var.name())),
    _muip(getMaterialProperty<Real>("mu" + _ip_var.name())),
    _Dip(getMaterialProperty<Real>("diff" + _ip_var.name())),
    _se_coeff(getMaterialProperty<Real>("se_coeff")),

    _e(getMaterialProperty<Real>("e")),
    _N_A(getMaterialProperty<Real>("N_A")),

    _work_function(getMaterialProperty<Real>("work_function")),
    _field_enhancement(getMaterialProperty<Real>("field_enhancement")),
    _Richardson_coefficient(getMaterialProperty<Real>("Richardson_coefficient")),
    _cathode_temperature(getMaterialProperty<Real>("cathode_temperature"))

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
SchottkyEmissionPostProcessor::computeQpIntegral()
{
  return ((1.0 - _r) / (1 + _r)) * SchottkyEmissionPostProcessor::emission_current();
}

Real
SchottkyEmissionPostProcessor::emission_current()
{
  Real dPhi;
  Real kB = 8.617385E-5; // eV/K;
  Real _j_TE;
  Real _j_SE;
  Real F;

  _a = (_normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0) ? 1.0 : 0.0;
  if (_a == 1.0)
  {
    return 0.0;
  }

  _ion_flux = (_sgnip[_qp] * _muip[_qp] * -_grad_potential[_qp] * std::exp(_ip[_qp]) -
               _Dip[_qp] * std::exp(_ip[_qp]) * _grad_ip[_qp]) *
              (_use_moles ? _N_A[_qp] : 1);

  // Schottky emission
  // je = AR * T^2 * exp(-(wf - dPhi) / (kB * T))
  // dPhi = _dPhi_over_F * sqrt(F) // eV

  F = -_field_enhancement[_qp] * _normals[_qp] * _grad_potential[_qp] * _r_units * _voltage_scaling;

  dPhi = _dPhi_over_F * sqrt(F);

  _j_TE = _Richardson_coefficient[_qp] * std::pow(_cathode_temperature[_qp], 2) *
          std::exp(-(_work_function[_qp] - dPhi) /
                   (kB * _cathode_temperature[_qp] + std::numeric_limits<double>::epsilon()));
  _j_SE = _e[_qp] * _se_coeff[_qp] * _ion_flux * _normals[_qp];

  return (_t_units / _r_units) * (_j_TE + _j_SE) / (_use_moles ? _N_A[_qp] : 1);
}
