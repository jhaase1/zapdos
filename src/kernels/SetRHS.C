#include "SetRHS.h"

template <>
InputParameters
validParams<SetRHS>()
{
  InputParameters params = validParams<Kernel>();
  params.addRequiredParam<Real>("value", "The potential units.");
  return params;
}

SetRHS::SetRHS(const InputParameters & parameters)
  : Kernel(parameters),

    _value(getParam<Real>("value"))
{
}

SetRHS::~SetRHS() {}

Real
SetRHS::computeQpResidual()
{
  return _test[_i][_qp] * (-_value);
}

Real
SetRHS::computeQpJacobian()
{
  return 0;
}

Real
SetRHS::computeQpOffDiagJacobian(unsigned int jvar)
{
  return 0.0;
}
