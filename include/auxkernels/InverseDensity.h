#ifndef INVERSEDENSITY_H
#define INVERSEDENSITY_H

#include "AuxKernel.h"

class InverseDensity;

template <>
InputParameters validParams<InverseDensity>();

class InverseDensity : public AuxKernel
{
public:
  InverseDensity(const InputParameters & parameters);

  virtual ~InverseDensity() {}

protected:
  virtual Real computeValue();

  const VariableValue & _density;
};

#endif // INVERSEDENSITY_H
