#include "HagelaarIonDiffusionBC.h"

// MOOSE includes
#include "MooseVariable.h"

template <>
InputParameters
validParams<HagelaarIonDiffusionBC>()
{
  InputParameters params = validParams<IntegratedBC>();
  params.addRequiredParam<Real>("r", "The reflection coefficient");

  params.addParam<Real>("position_units", 1, "The units of position.");
  params.addParam<Real>("time_units", 1, "The units of time.");

  params.addParam<Real>("user_velocity",
                        -1.,
                        "Optional parameter if user wants to specify the thermal velocity, in m/s");

  return params;
}

HagelaarIonDiffusionBC::HagelaarIonDiffusionBC(const InputParameters & parameters)
  : IntegratedBC(parameters),

    _r_units(1. / getParam<Real>("position_units")),
    _t_units(1. / getParam<Real>("time_units")),

    _r(getParam<Real>("r")),

    _kb(getMaterialProperty<Real>("k_boltz")),
    _T(getMaterialProperty<Real>("T" + _var.name())),
    _mass(getMaterialProperty<Real>("mass" + _var.name())),
    _v_thermal(0),
    _user_velocity(getParam<Real>("user_velocity"))
{
}

Real
HagelaarIonDiffusionBC::computeQpResidual()
{
  if (_user_velocity > 0.)
    _v_thermal = _user_velocity * (_r_units / _t_units);
  else
    _v_thermal = std::sqrt(8 * _kb[_qp] * _T[_qp] / (M_PI * _mass[_qp])) * (_r_units / _t_units);

  return _test[_i][_qp] * (1. - _r) / (1. + _r) * 0.5 * _v_thermal * std::exp(_u[_qp]);
}

Real
HagelaarIonDiffusionBC::computeQpJacobian()
{
  if (_user_velocity > 0.)
    _v_thermal = _user_velocity * (_r_units / _t_units);
  else
    _v_thermal = std::sqrt(8 * _kb[_qp] * _T[_qp] / (M_PI * _mass[_qp])) * (_r_units / _t_units);

  return _test[_i][_qp] * (1. - _r) / (1. + _r) * 0.5 * _v_thermal * std::exp(_u[_qp]) *
         _phi[_j][_qp];
}
