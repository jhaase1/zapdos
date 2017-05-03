
#include "Position.h"

template <>
InputParameters
validParams<Position>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addRequiredParam<Real>("position_units", "Units of position.");
  return params;
}

Position::Position(const InputParameters & parameters)
  : AuxKernel(parameters), _r_units(1. / getParam<Real>("position_units"))
{
}

Real
Position::computeValue()
{
  if (isNodal())
    return (*_current_node)(0) / _r_units;
  else
    return _q_point[_qp](0) / _r_units;
}
