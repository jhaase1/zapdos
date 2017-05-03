
#ifndef ANALYTICALDIFFINDICATOR_H
#define ANALYTICALDIFFINDICATOR_H

#include "ElementIntegralIndicator.h"

class AnalyticalDiffIndicator;

template <>
InputParameters validParams<AnalyticalDiffIndicator>();

class AnalyticalDiffIndicator : public ElementIntegralIndicator
{
public:
  AnalyticalDiffIndicator(const InputParameters & parameters);
  virtual ~AnalyticalDiffIndicator(){};

protected:
  virtual Real computeQpIntegral();

  Function & _func;
};

#endif /* ANALYTICALDIFFINDICATOR_H */
