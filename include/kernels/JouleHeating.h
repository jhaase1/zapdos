
#ifndef JOULEHEATING_H
#define JOULEHEATING_H

#include "Kernel.h"

class JouleHeating;

template <>
InputParameters validParams<JouleHeating>();

class JouleHeating : public Kernel
{
public:
  JouleHeating(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();

  virtual Real computeQpJacobian();

  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  // Input file scalars
  Real _r_units;

  // Material properties

  const MaterialProperty<Real> & _muem;
  const MaterialProperty<Real> & _d_muem_d_actual_mean_en;
  const MaterialProperty<Real> & _diffem;
  const MaterialProperty<Real> & _d_diffem_d_actual_mean_en;

  std::string _potential_units;

  // Coupled variables

  unsigned int _potential_id;
  const VariableGradient & _grad_potential;
  const VariableValue & _em;
  const VariableGradient & _grad_em;
  unsigned int _em_id;

  // Unique variables

  Real _voltage_scaling;
};

#endif // JOULEHEATING_H
