#include "Potential.h"

template <>
InputParameters
validParams<Potential>()
{
  InputParameters params = validParams<AuxKernel>();

  params.addRequiredCoupledVar("potential", "The electric potential");
  params.addRequiredParam<std::string>("potential_units", "The potential units.");

  return params;
}

Potential::Potential(const InputParameters & parameters)
  : AuxKernel(parameters),

    _potential(coupledValue("potential")),
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
}

Real
Potential::computeValue()
{
  return _potential[_qp] * _voltage_scaling;
}
