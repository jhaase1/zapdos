#ifndef HAGELAARENERGYBC_H
#define HAGELAARENERGYBC_H

#include "IntegratedBC.h"

class HagelaarEnergyBC;

template <>
InputParameters validParams<HagelaarEnergyBC>();

class HagelaarEnergyBC : public IntegratedBC
{
public:
  HagelaarEnergyBC(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  Real _r;
	Real _r_units;
	Real _t_units;

  // Coupled variables

  const VariableGradient & _grad_potential;
  unsigned int _potential_id;
  const VariableValue & _em;
  unsigned int _em_id;

  const MaterialProperty<Real> & _muem;
  const MaterialProperty<Real> & _d_muem_d_actual_mean_en;
  const MaterialProperty<Real> & _massem;
  const MaterialProperty<Real> & _e;

  Real _a;
	Real _electron_flux;
  Real _d_electron_flux_d_u;
	Real _v_thermal;
  Real _actual_mean_en;
};

#endif // HAGELAARENERGYBC_H
