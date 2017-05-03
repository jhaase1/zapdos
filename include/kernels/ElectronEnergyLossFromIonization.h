
#ifndef ELECTRONENERGYLOSSFROMIONIZATION_H
#define ELECTRONENERGYLOSSFROMIONIZATION_H

#include "Kernel.h"

class ElectronEnergyLossFromIonization;

template <>
InputParameters validParams<ElectronEnergyLossFromIonization>();

class ElectronEnergyLossFromIonization : public Kernel
{
public:
  ElectronEnergyLossFromIonization(const InputParameters & parameters);
  virtual ~ElectronEnergyLossFromIonization();

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  Real _r_units;

  const MaterialProperty<Real> & _diffem;
  const MaterialProperty<Real> & _muem;
  const MaterialProperty<Real> & _alpha_iz;
  const MaterialProperty<Real> & _d_iz_d_actual_mean_en;
  const MaterialProperty<Real> & _d_muem_d_actual_mean_en;
  const MaterialProperty<Real> & _d_diffem_d_actual_mean_en;
  const MaterialProperty<Real> & _Eiz;

  const VariableGradient & _grad_potential;
  const VariableValue & _em;
  const VariableGradient & _grad_em;
  unsigned int _potential_id;
  unsigned int _em_id;
};

#endif /* ELECTRONENERGYLOSSFROMIONIZATION_H */
