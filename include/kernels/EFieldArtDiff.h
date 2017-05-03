
#ifndef EFIELDARTDIFF_H
#define EFIELDARTDIFF_H

// Including the "Diffusion" Kernel here so we can extend it
#include "Kernel.h"

class EFieldArtDiff;

template <>
InputParameters validParams<EFieldArtDiff>();

class EFieldArtDiff : public Kernel
{
public:
  EFieldArtDiff(const InputParameters & parameters);
  virtual ~EFieldArtDiff();

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  // Coupled variables

  const VariableGradient & _grad_potential;
  unsigned int _potential_id;

  Real _scale;
  Real _r_units;

  // Material Properties

  const MaterialProperty<Real> & _mu;
};

#endif /* EFIELDARTDIFF_H */
