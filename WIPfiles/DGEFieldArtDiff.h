
#ifndef DGEFIELDARTDIFF_H
#define DGEFIELDARTDIFF_H

#include "InterfaceKernel.h"

// Forward Declarations
class DGEFieldArtDiff;

template <>
InputParameters validParams<DGEFieldArtDiff>();

/**
 * DG kernel for interfacing advection on adjacent blocks
 */
class DGEFieldArtDiff : public InterfaceKernel
{
public:
  DGEFieldArtDiff(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual(Moose::DGResidualType type);
  virtual Real computeQpJacobian(Moose::DGJacobianType type);

  MooseVariable & _potential_var;
  MooseVariable & _potential_neighbor_var;
  VariableGradient & _grad_potential;
  VariableGradient & _grad_potential_neighbor;

  const MaterialProperty<Real> & _mu;
  const MaterialProperty<Real> & _mu_neighbor;
  const MaterialProperty<Real> & _sgn;
  const MaterialProperty<Real> & _sgn_neighbor;
};

#endif
