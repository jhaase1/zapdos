#include "Current.h"

template <>
InputParameters
validParams<Current>()
{
  InputParameters params = validParams<AuxKernel>();

  params.addRequiredCoupledVar("density_log", "The electron density");
  params.addRequiredCoupledVar("potential", "The potential");
  params.addParam<bool>(
      "art_diff", false, "Whether there is a current contribution from artificial diffusion.");
  params.addParam<Real>("position_units", 1, "The units of position.");
  params.addParam<Real>("time_units", 1, "The units of time.");
  params.addRequiredParam<bool>("use_moles",
                                "Whether to use units of moles as opposed to # of molecules.");
  params.addParam<int>(
      "component", 0, "The component of the electric field to access. Accepts an integer");
  return params;
}

Current::Current(const InputParameters & parameters)
  : AuxKernel(parameters),

    _component(getParam<int>("component")),
    _use_moles(getParam<bool>("use_moles")),
    _r_units(1. / getParam<Real>("position_units")),
    _t_units(1. / getParam<Real>("time_units")),

    _e(getMaterialProperty<Real>("e")),
    _N_A(getMaterialProperty<Real>("N_A")),

    _density_var(*getVar("density_log", 0)),
    _density_log(coupledValue("density_log")),
    _grad_density_log(coupledGradient("density_log")),
    _grad_potential(coupledGradient("potential")),
    _mu(getMaterialProperty<Real>("mu" + _density_var.name())),
    _sgn(getMaterialProperty<Real>("sgn" + _density_var.name())),
    _diff(getMaterialProperty<Real>("diff" + _density_var.name())),
    _art_diff(getParam<bool>("art_diff"))
{
}

Real
Current::computeValue()
{
  Real _current =
      _sgn[_qp] * _e[_qp] *
      (_sgn[_qp] * _mu[_qp] * -_grad_potential[_qp](_component) * std::exp(_density_log[_qp]) -
       _diff[_qp] * std::exp(_density_log[_qp]) * _grad_density_log[_qp](_component));

  if (_art_diff)
  {
    Real vd_mag = _mu[_qp] * _grad_potential[_qp].norm();
    Real delta = vd_mag * _current_elem->hmax() / 2.;
    _current += _sgn[_qp] * _e[_qp] * -delta * std::exp(_density_log[_qp]) *
                _grad_density_log[_qp](_component);
  }

  if (_use_moles)
  {
    _current *= _N_A[_qp];
  }

  _current *= (_t_units / _r_units);

  return _current;
}
