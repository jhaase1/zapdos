
#ifndef CONSTTIMESDIFFUSIONSUPG_H
#define CONSTTIMESDIFFUSIONSUPG_H

// Including the "Diffusion" Kernel here so we can extend it
#include "Diffusion.h"

class ConstTimesDiffusionSUPG;

template <>
InputParameters validParams<ConstTimesDiffusionSUPG>();

class ConstTimesDiffusionSUPG : public Diffusion
{
public:
  ConstTimesDiffusionSUPG(const InputParameters & parameters);
  virtual ~ConstTimesDiffusionSUPG();

protected:
  virtual Real computeQpResidual();

  virtual Real computeQpJacobian();

  MaterialProperty<Real> _alpha;
  MaterialProperty<RealVectorValue> _velocity_norm;
  MaterialProperty<Real> _diffusivity;
};

#endif /* CONSTTIMESDIFFUSIONSUPG_H */
