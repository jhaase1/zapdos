
#ifndef ELECTRONENERGYLOSSFROMEXCITATION_H
#define ELECTRONENERGYLOSSFROMEXCITATION_H

#include "Kernel.h"

class ElectronEnergyLossFromExcitation;

template <>
InputParameters validParams<ElectronEnergyLossFromExcitation>();

class ElectronEnergyLossFromExcitation : public Kernel
{
public:
  ElectronEnergyLossFromExcitation(const InputParameters & parameters);
  virtual ~ElectronEnergyLossFromExcitation();

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  Real _r_units;

  const MaterialProperty<Real> & _diffem;
  const MaterialProperty<Real> & _muem;
  const MaterialProperty<Real> & _alpha_ex;
  const MaterialProperty<Real> & _d_ex_d_actual_mean_en;
  const MaterialProperty<Real> & _d_muem_d_actual_mean_en;
  const MaterialProperty<Real> & _d_diffem_d_actual_mean_en;
  const MaterialProperty<Real> & _Eex;

  const VariableGradient & _grad_potential;
  const VariableValue & _em;
  const VariableGradient & _grad_em;
  unsigned int _potential_id;
  unsigned int _em_id;
};

#endif /* ELECTRONENERGYLOSSFROMEXCITATION_H */
