
#include "AnalyticalDiffIndicator.h"
#include "Function.h"

template <>
InputParameters
validParams<AnalyticalDiffIndicator>()
{
  InputParameters params = validParams<ElementIntegralIndicator>();
  params.addRequiredParam<FunctionName>("function", "The analytic solution to compare against");
  return params;
}

AnalyticalDiffIndicator::AnalyticalDiffIndicator(const InputParameters & parameters)
  : ElementIntegralIndicator(parameters), _func(getFunction("function"))
{
}

Real
AnalyticalDiffIndicator::computeQpIntegral()
{
  Real diff = _u[_qp] - _func.value(_t, _q_point[_qp]);
  return diff;
}
