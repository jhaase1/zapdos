#ifndef DRIFTDIFFUSIONFLUXAUX_H_
#define DRIFTDIFFUSIONFLUXAUX_H_

#include "AuxKernel.h"

class DriftDiffusionFluxAux;

template <>
InputParameters validParams<DriftDiffusionFluxAux>();

class DriftDiffusionFluxAux : public AuxKernel
{
public:
  DriftDiffusionFluxAux(const InputParameters & parameters);

protected:
  virtual Real computeValue();

private:
  const Real _sgn;
  const VariableGradient & _grad_potential;
  const VariableValue & _u;
  const VariableGradient & _grad_u;
  const int _component;
};

#endif // DRIFTDIFFUSIONFLUXAUX_H
