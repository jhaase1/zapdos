#ifndef PROCRATE_H
#define PROCRATE_H

#include "AuxKernel.h"

class ProcRate;

template <>
InputParameters validParams<ProcRate>();

class ProcRate : public AuxKernel
{
public:
  ProcRate(const InputParameters & parameters);

  virtual ~ProcRate() {}
  virtual Real computeValue();

protected:
  int _component;
  bool _use_moles;

  Real _r_units;
  Real _t_units;

  const MaterialProperty<Real> & _e;
  const MaterialProperty<Real> & _N_A;

  const VariableValue & _em;
  const VariableGradient & _grad_em;
  const VariableGradient & _grad_potential;
  const MaterialProperty<Real> & _muem;
  const MaterialProperty<Real> & _sgnem;
  const MaterialProperty<Real> & _diffem;
  const MaterialProperty<Real> & _alpha;

  RealVectorValue _current;
};

#endif // PROCRATE_H
