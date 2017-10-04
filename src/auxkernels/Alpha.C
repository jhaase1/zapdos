#include "Alpha.h"

template <>
InputParameters
validParams<Alpha>()
{
  InputParameters params = validParams<AuxKernel>();

  params.addRequiredParam<std::string>(
      "proc",
      "The process that we want to get the townsend coefficient for. Options are iz, ex, and el.");
  params.addParam<Real>("position_units", 1, "The units of position.");
  return params;
}

Alpha::Alpha(const InputParameters & parameters)
  : AuxKernel(parameters),
		_r_units(1. / getParam<Real>("position_units")),
    _alpha(getMaterialProperty<Real>("alpha_" + getParam<std::string>("proc")))
{
}

Real
Alpha::computeValue()
{
  return _alpha[_qp] * _r_units ;
}
