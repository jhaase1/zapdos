
#ifndef EFIELDADVECTION_H
#define EFIELDADVECTION_H

#include "Kernel.h"

class EFieldAdvection;

template <>
InputParameters validParams<EFieldAdvection>();

class EFieldAdvection : public Kernel
{
public:
  EFieldAdvection(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  // Material properties

  Real _r_units;

  const MaterialProperty<Real> & _mu;
  const MaterialProperty<Real> & _sign;

private:
  // Coupled variables
  unsigned int _potential_id;
  const VariableGradient & _grad_potential;
};

#endif // EFIELDADVECTION_H
