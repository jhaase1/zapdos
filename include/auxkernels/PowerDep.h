#ifndef POWERDEP_H
#define POWERDEP_H

#include "AuxKernel.h"

class PowerDep;

template <>
InputParameters validParams<PowerDep>();

class PowerDep : public AuxKernel
{
public:
  PowerDep(const InputParameters & parameters);

  virtual ~PowerDep() {}
  virtual Real computeValue();

protected:
  bool _power_used;
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
  std::string _potential_units;
  RealVectorValue _current;
  Real _voltage_scaling;
};

#endif // POWERDEP_H
