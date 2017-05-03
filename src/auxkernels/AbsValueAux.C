
#include "AbsValueAux.h"

template <>
InputParameters
validParams<AbsValueAux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addRequiredCoupledVar("u", "Variable we want absolute value of.");
  return params;
}

AbsValueAux::AbsValueAux(const InputParameters & parameters)
  : AuxKernel(parameters), _u(coupledValue("u"))
{
}

Real
AbsValueAux::computeValue()
{
  return std::abs(_u[_qp]);
}
