
#include "DiffusiveFlux.h"

template <>
InputParameters
validParams<DiffusiveFlux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addRequiredCoupledVar("density_log", "The variable representing the log of the density.");
  params.addParam<Real>("position_units", 1, "The units of position.");
  params.addParam<Real>("time_units", 1, "The units of time.");
  params.addRequiredParam<bool>("use_moles",
                                "Whether to use units of moles as opposed to # of molecules.");
  params.addParam<int>(
      "component", 0, "The component of the electric field to access. Accepts an integer");
  return params;
}

DiffusiveFlux::DiffusiveFlux(const InputParameters & parameters)
  : AuxKernel(parameters),
    _component(getParam<int>("component")),
    _use_moles(getParam<bool>("use_moles")),
    _r_units(1. / getParam<Real>("position_units")),
    _t_units(1. / getParam<Real>("time_units")),

    _N_A(getMaterialProperty<Real>("N_A")),

    // Coupled variables

    _grad_density_log(coupledGradient("density_log")),
    _density_var(*getVar("density_log", 0)),
    _density_log(coupledValue("density_log")),

    // Material properties

    _diff(getMaterialProperty<Real>("diff" + _density_var.name()))
{
}

Real
DiffusiveFlux::computeValue()
{
  if (_use_moles)
    return -_diff[_qp] * std::exp(_density_log[_qp]) * _grad_density_log[_qp](_component) *
           (_r_units / _t_units) * _N_A[_qp];
  else
    return -_diff[_qp] * std::exp(_density_log[_qp]) * _grad_density_log[_qp](_component) *
           (_r_units / _t_units);
}
