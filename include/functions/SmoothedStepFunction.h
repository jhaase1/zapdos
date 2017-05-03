
#ifndef SMOOTHSTEPFUNCTION_H
#define SMOOTHSTEPFUNCTION_H

#include "Function.h"

class SmoothedStepFunction;

template <>
InputParameters validParams<SmoothedStepFunction>();

class SmoothedStepFunction : public Function
{
public:
  SmoothedStepFunction(const InputParameters & parameters);

  virtual Real value(Real t, const Point & x) override;

protected:
  Real _vLow;
  Real _vHigh;
  Real _period;
  Real _duty;
  Real _rise;

  Real DoubleTanh(Real t);
  Real RoundedStep(Real t);
};

#endif // SMOOTHSTEPFUNCTION_H
