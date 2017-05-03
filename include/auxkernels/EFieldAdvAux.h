
#ifndef EFIELDADVAUX_H
#define EFIELDADVAUX_H

#include "AuxKernel.h"

class EFieldAdvAux;

template <>
InputParameters validParams<EFieldAdvAux>();

class EFieldAdvAux : public AuxKernel
{
public:
  EFieldAdvAux(const InputParameters & parameters);

protected:
  virtual Real computeValue();

  Real _r_units;

  // Coupled variables

  MooseVariable & _density_var;
  const VariableValue & _density_log;
  const VariableGradient & _grad_potential;

  // Material properties

  const MaterialProperty<Real> & _mu;
  const MaterialProperty<Real> & _sgn;
};

#endif // EFIELDADVAUX_H
