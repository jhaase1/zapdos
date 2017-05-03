#include "SetValue.h"

template <>
InputParameters
validParams<SetValue>()
{
  InputParameters params = validParams<Kernel>();
  params.addRequiredParam<Real>("value", "The potential units.");
  return params;
}

SetValue::SetValue(const InputParameters & parameters)
  : Kernel(parameters),

    _value(getParam<Real>("value"))
{
}

SetValue::~SetValue() {}

Real
SetValue::computeQpResidual()
{
  return _test[_i][_qp] * (_u[_qp] - _value);
}

Real
SetValue::computeQpJacobian()
{
  return _test[_i][_qp] * _phi[_j][_qp];
}

Real
SetValue::computeQpOffDiagJacobian(unsigned int jvar)
{
  return 0.0;
}
