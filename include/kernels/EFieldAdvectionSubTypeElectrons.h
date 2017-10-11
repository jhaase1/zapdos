#ifndef EFIELDADVECTIONSUBTYPEELECTRONS_H
#define EFIELDADVECTIONSUBTYPEELECTRONS_H

#include "Kernel.h"

class EFieldAdvectionSubTypeElectrons;

template <>
InputParameters validParams<EFieldAdvectionSubTypeElectrons>();

class EFieldAdvectionSubTypeElectrons : public Kernel
{
public:
  EFieldAdvectionSubTypeElectrons(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  Real _r_units;

  // Material properties

  const MaterialProperty<Real> & _muem;
  const MaterialProperty<Real> & _d_muem_d_actual_mean_en;
  const MaterialProperty<Real> & _sign;

private:
  // Coupled variables
  unsigned int _potential_id;
  const VariableGradient & _grad_potential;
  const VariableValue & _mean_en;
  unsigned int _mean_en_id;
	
	const VariableValue & _em;

  Real _d_actual_mean_en_d_mean_en;
  Real _d_muem_d_mean_en;
  Real _d_actual_mean_en_d_u;
  Real _d_muem_d_u;
	Real _actual_mean_en;
};

#endif // EFIELDADVECTIONSUBTYPEELECTRONS_H
