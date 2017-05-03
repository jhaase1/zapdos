#ifndef POTENTIAL_H
#define POTENTIAL_H

#include "AuxKernel.h"

class Potential;

template <>
InputParameters validParams<Potential>();

class Potential : public AuxKernel
{
public:
  Potential(const InputParameters & parameters);

  virtual ~Potential() {}

protected:
  virtual Real computeValue();

  const VariableValue & _potential;
  std::string _potential_units;
  Real _voltage_scaling;
};

#endif // POTENTIAL_H
