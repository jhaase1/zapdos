#include "SmoothedStepFunction.h"

template <>
InputParameters
validParams<SmoothedStepFunction>()
{
  InputParameters params = validParams<Function>();
  params.addParam<Real>("vLow", "The low value");
  params.addParam<Real>("vHigh", "The high value");
  params.addParam<Real>("period", "The cycle period");
  params.addParam<Real>("duty", "The duty cycle");
  params.addParam<Real>("rise", 80.0, "The rise factor of the step function");
  return params;
}

SmoothedStepFunction::SmoothedStepFunction(const InputParameters & parameters)
  : Function(parameters),
    _vLow(getParam<Real>("vLow")),
    _vHigh(getParam<Real>("vHigh")),
    _period(getParam<Real>("period")),
    _duty(getParam<Real>("duty")),
    _rise(getParam<Real>("rise"))
{
}

Real
SmoothedStepFunction::DoubleTanh(Real t)
{
  return (tanh(_rise * ((fmod((t / _period) + 0.5, 1.0) - 0.5))) -
          tanh(_rise * (((fmod((t / _period) + 0.5 - _duty, 1.0)) - 0.5)))) /
         2.0;
}

Real
SmoothedStepFunction::RoundedStep(Real t)
{
  if (DoubleTanh(t) >= 0)
  {
    return DoubleTanh(t);
  }
  else
  {
    return 0.0;
  }
}

Real
SmoothedStepFunction::value(Real t, const Point & x)
{
  return (_vHigh - _vLow) * RoundedStep(t) + _vLow;
}
