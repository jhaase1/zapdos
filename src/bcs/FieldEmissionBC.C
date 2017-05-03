#include "FieldEmissionBC.h"

// MOOSE includes
#include "MooseVariable.h"

template <>
InputParameters
validParams<FieldEmissionBC>()
{
  InputParameters params = validParams<IntegratedBC>();
  params.addRequiredParam<Real>("r", "The reflection coefficient");
  params.addRequiredCoupledVar("potential", "The electric potential");
  params.addRequiredCoupledVar("mean_en", "The mean energy.");
  params.addRequiredCoupledVar("ip", "The ion density.");
  params.addParam<Real>("position_units", 1, "The units of position.");
  params.addParam<Real>("time_units", 1, "The units of time.");
  params.addRequiredParam<std::string>("potential_units", "The potential units.");
  params.addParam<Real>("tau", 1e-9, "The time constant for ramping the boundary condition.");
  params.addParam<bool>("relax", false, "Use relaxation for emission.");
  return params;
}

FieldEmissionBC::FieldEmissionBC(const InputParameters & parameters)
  : IntegratedBC(parameters),

    _use_moles(getParam<bool>("use_moles")),
    _r_units(1. / getParam<Real>("position_units")),
    _t_units(1. / getParam<Real>("position_units")),
    _r(getParam<Real>("r")),

    // Coupled Variables
    _grad_potential(coupledGradient("potential")),
    _potential_id(coupled("potential")),
    _mean_en(coupledValue("mean_en")),
    _mean_en_id(coupled("mean_en")),
    _ip_var(*getVar("ip", 0)),
    _ip(coupledValue("ip")),
    _grad_ip(coupledGradient("ip")),
    _ip_id(coupled("ip")),

    _muem(getMaterialProperty<Real>("muem")),
    _d_muem_d_actual_mean_en(getMaterialProperty<Real>("d_muem_d_actual_mean_en")),
    _massem(getMaterialProperty<Real>("massem")),
    _e(getMaterialProperty<Real>("e")),
    _N_A(getMaterialProperty<Real>("N_A")),
    _sgnip(getMaterialProperty<Real>("sgn" + _ip_var.name())),
    _muip(getMaterialProperty<Real>("mu" + _ip_var.name())),
    _Dip(getMaterialProperty<Real>("diff" + _ip_var.name())),
    _se_coeff(getMaterialProperty<Real>("se_coeff")),
    _work_function(getMaterialProperty<Real>("work_function")),
    _field_enhancement(getMaterialProperty<Real>("field_enhancement")),
    _a(0.5),
    _v_thermal(0),
    _ion_flux(0, 0, 0),
    _n_gamma(0),
    _d_v_thermal_d_u(0),
    _d_v_thermal_d_mean_en(0),
    _d_ion_flux_d_potential(0, 0, 0),
    _d_ion_flux_d_ip(0, 0, 0),
    _d_n_gamma_d_potential(0),
    _d_n_gamma_d_ip(0),
    _d_n_gamma_d_u(0),
    _d_n_gamma_d_mean_en(0),
    _actual_mean_en(0),
    _tau(getParam<Real>("tau")),
    _relax(getParam<bool>("relax")),
    _potential_units(getParam<std::string>("potential_units"))
{
  if (_potential_units.compare("V") == 0)
  {
    _voltage_scaling = 1.;
  }
  else if (_potential_units.compare("kV") == 0)
  {
    _voltage_scaling = 1000;
  }

  FE_a = 1.541434E-6 * pow(_voltage_scaling, 2) * pow(_r_units, 3) /
         _t_units;                                    // A eV/kV^2 (if _voltage_scaling == 1000)
  FE_b = 6.830890E9 / (_r_units * _voltage_scaling);  // kV/m-eV^1.5 (if _voltage_scaling == 1000)
  FE_c = 1.439964E-9 * (_r_units * _voltage_scaling); // eV^2*m/kV (if _voltage_scaling == 1000)
}

Real
FieldEmissionBC::computeQpResidual()
{
  Real v;
  Real f;
  Real _j_FE;
  Real _j_SE;
  Real F;
  Real _relaxation_Expr;

  if (_normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0)
  {
    _a = 1.0;
    return 0;
  }
  else
  {
    _a = 0.0;

    _ion_flux = _sgnip[_qp] * _muip[_qp] * -_grad_potential[_qp] * std::exp(_ip[_qp]) -
                _Dip[_qp] * std::exp(_ip[_qp]) * _grad_ip[_qp];

    // Fowler-Nordheim
    // _j_FE = (a / wf) * F^2 * exp(-v(f) * b * wf^1.5 / F)
    // a = 1.541434E-6 A eV/V^2
    // b = 6.830890E9 V/m-eV^1.5
    // c = 1.439964E-9 eV^2-m/V
    // v(f) = 1 - f + (f/6)*ln(f)
    // f = c*(F/wf^2)

    F = -(1 - _a) * _field_enhancement[_qp] * _normals[_qp] * _grad_potential[_qp];

    f = FE_c * F / pow(_work_function[_qp], 2);
    v = 1 - f + (f / 6) * std::log(f);

    _j_FE = (FE_a / (_work_function[_qp])) * pow(F, 2) *
            std::exp(-v * FE_b * pow(_work_function[_qp], 1.5) / F);
    _j_SE = _e[_qp] * _se_coeff[_qp] * _ion_flux * _normals[_qp];

    if (_use_moles)
      _j_SE = _j_SE * _N_A[_qp];

    if (_relax == true)
      _relaxation_Expr = std::tanh(_t / _tau);
    else
      _relaxation_Expr = 1.0;

    if (_use_moles)
      return _relaxation_Expr * _test[_i][_qp] * 2. / (1. + _r) * (1 - _a) * (-_j_FE - _j_SE) /
             (_e[_qp] * _N_A[_qp]);
    else
      return _relaxation_Expr * _test[_i][_qp] * 2. / (1. + _r) * (1 - _a) * (-_j_FE - _j_SE) /
             _e[_qp];
  }
}

Real
FieldEmissionBC::computeQpJacobian()
{
  if (_normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0)
  {
    _a = 1.0;
  }
  else
  {
    _a = 0.0;
  }

  _actual_mean_en = std::exp(_mean_en[_qp] - _u[_qp]);

  return 0.;
}

Real
FieldEmissionBC::computeQpOffDiagJacobian(unsigned int jvar)
{
  Real v;
  Real f;
  Real F;
  Real _j_FE;
  Real _d_j_FE_d_potential;
  Real _d_j_SE_d_potential;
  Real _relaxation_Expr;

  if (_normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0)
  {
    _a = 1.0;
  }
  else
  {
    _a = 0.0;
  }

  if (jvar == _potential_id)
  {
    _d_ion_flux_d_potential = _sgnip[_qp] * _muip[_qp] * -_grad_phi[_j][_qp] * std::exp(_ip[_qp]);
    _d_j_SE_d_potential = _e[_qp] * _se_coeff[_qp] * _d_ion_flux_d_potential * _normals[_qp];

    if (_use_moles)
      _d_j_SE_d_potential = _d_j_SE_d_potential * _N_A[_qp];

    // Fowler-Nordheim
    // _j_FE = (a / wf) * F^2 * exp(-v(f) * b * wf^1.5 / F)
    // v(f) = 1 - f + (f/6)*ln(f)
    // f = c*(F/wf^2)
    F = -(1 - _a) * _field_enhancement[_qp] * _normals[_qp] * _grad_potential[_qp];

    f = FE_c * F / pow(_work_function[_qp], 2);
    v = 1 - f + (f / 6) * std::log(f);

    _j_FE = (FE_a / (_work_function[_qp])) * pow(F, 2) *
            std::exp(-v * FE_b * pow(_work_function[_qp], 1.5) / F);
    _d_j_FE_d_potential = _j_FE * (2 - (FE_b * FE_c) / (6 * sqrt(_work_function[_qp])) +
                                   (FE_b * pow(_work_function[_qp], 1.5) / F)) *
                          (_grad_phi[_j][_qp] * _normals[_qp]) /
                          (_grad_potential[_qp] * _normals[_qp]);

    if (_relax == true)
      _relaxation_Expr = std::tanh(_t / _tau);
    else
      _relaxation_Expr = 1.0;

    if (_use_moles)
      return _relaxation_Expr * _test[_i][_qp] * 2. / (1. + _r) * (1 - _a) *
             (-_d_j_FE_d_potential - _d_j_SE_d_potential) / (_e[_qp] * _N_A[_qp]);
    else
      return _relaxation_Expr * _test[_i][_qp] * 2. / (1. + _r) * (1 - _a) *
             (-_d_j_FE_d_potential - _d_j_SE_d_potential) / _e[_qp];
  }
  else if (jvar == _mean_en_id)
  {
    _actual_mean_en = std::exp(_mean_en[_qp] - _u[_qp]);

    return 0.;
  }
  else if (jvar == _ip_id)
  {
    _d_ion_flux_d_ip =
        _sgnip[_qp] * _muip[_qp] * -_grad_potential[_qp] * std::exp(_ip[_qp]) * _phi[_j][_qp] -
        _Dip[_qp] * std::exp(_ip[_qp]) * _grad_phi[_j][_qp] -
        _Dip[_qp] * std::exp(_ip[_qp]) * _phi[_j][_qp] * _grad_ip[_qp];

    return 0;
  }
  else
  {
    return 0.0;
  }
}
