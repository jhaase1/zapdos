#include "SetEqualToLogSum.h"

template <>
InputParameters
validParams<SetEqualToLogSum>()
{
  InputParameters params = validParams<Kernel>();
  params.addRequiredCoupledVar("q1", "The first value to add.");
	params.addRequiredCoupledVar("q2", "The second value to add.");
  return params;
}

SetEqualToLogSum::SetEqualToLogSum(const InputParameters & parameters)
  : Kernel(parameters),

    _q1(coupledValue("q1")),
		_q1_id(coupled("q1")),
		
		_q2(coupledValue("q2")),
		_q2_id(coupled("q2"))
{
}

SetEqualToLogSum::~SetEqualToLogSum() {}

Real
SetEqualToLogSum::computeQpResidual()
{
  return _test[_i][_qp] * (_u[_qp] - std::log(std::exp(_q1[_qp]) + std::exp(_q2[_qp])));
}

Real
SetEqualToLogSum::computeQpJacobian()
{
  return _test[_i][_qp] * _phi[_j][_qp];
}

Real
SetEqualToLogSum::computeQpOffDiagJacobian(unsigned int jvar)
{
	if (jvar == _q1_id)
  {
		return _test[_i][_qp] * _phi[_j][_qp] * (std::exp(_q1[_qp])) / (std::exp(_q1[_qp]) + std::exp(_q2[_qp]));
  }
  else if (jvar == _q2_id)
	{
		return _test[_i][_qp] * _phi[_j][_qp] * (std::exp(_q2[_qp])) / (std::exp(_q1[_qp]) + std::exp(_q2[_qp]));
	}
	else
	{
  return 0.0;
	}
}
