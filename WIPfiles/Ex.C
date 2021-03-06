
#include "Ex.h"

template <>
InputParameters
validParams<Ex>()
{
  InputParameters params = validParams<AuxKernel>();

  params.addRequiredCoupledVar("potential", "The potential");
  params.addRequiredParam<Real>("position_units", "Units of position.");
  params.addRequiredParam<std::string>("potential_units", "The potential units.");
  params.addRequiredParam<int>("component",
                               "The component of the electric field to access. Accepts an integer");
  return params;
}

Ex::Ex(const InputParameters & parameters)
  : AuxKernel(parameters),

    _r_units(1. / getParam<Real>("position_units")),
    _potential_units(getParam<std::string>("potential_units")),
    _grad_potential(coupledGradient("potential"))
{
  if (_potential_units.compare("V") == 0)
    _voltage_scaling = 1.;
  else if (_potential_units.compare("kV") == 0)
    _voltage_scaling = 1000;
}

Efield::~Efield() {}

Real
Ex::computeValue()
{
  return -_grad_potential[_qp](0) * _r_units * _voltage_scaling;
}
