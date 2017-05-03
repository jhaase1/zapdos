
#ifndef ELECTRONENERGYDIFFUSION_H
#define ELECTRONENERGYDIFFUSION_H

// Including the "Diffusion" Kernel here so we can extend it
#include "Diffusion.h"

class ElectronEnergyDiffusion;

template <>
InputParameters validParams<ElectronEnergyDiffusion>();

class ElectronEnergyDiffusion : public Diffusion
{
public:
  ElectronEnergyDiffusion(const InputParameters & parameters);
  virtual ~ElectronEnergyDiffusion();

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  // Material Properties

  const MaterialProperty<Real> & _muem;

  // Coupled Variables

  VariableValue & _em;
  unsigned int _em_id;

  // Unique variables

  Real _T_e;
  Real _D_Te;
  Real _mu_Te;
  Real _d_D_Te_d_Te;
};

#endif /* ELECTRONENERGYDIFFUSION_H */
