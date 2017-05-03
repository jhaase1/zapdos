#ifndef CURRENT_H
#define CURRENT_H

#include "AuxKernel.h"

class Current;

template <>
InputParameters validParams<Current>();

class Current : public AuxKernel
{
public:
  Current(const InputParameters & parameters);

  virtual ~Current() {}
  virtual Real computeValue();

protected:
  int _component;
  bool _use_moles;

  Real _r_units;
  Real _t_units;

  const MaterialProperty<Real> & _e;
  const MaterialProperty<Real> & _N_A;

  MooseVariable & _density_var;
  const VariableValue & _density_log;
  const VariableGradient & _grad_density_log;
  const VariableGradient & _grad_potential;
  const MaterialProperty<Real> & _mu;
  const MaterialProperty<Real> & _sgn;
  const MaterialProperty<Real> & _diff;
  bool _art_diff;
};

#endif // CURRENT_H
