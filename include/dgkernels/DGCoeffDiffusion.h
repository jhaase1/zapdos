
#ifndef DGCOEFFDIFFUSION_H
#define DGCOEFFDIFFUSION_H

#include "DGKernel.h"

// Forward Declarations
class DGCoeffDiffusion;

template <>
InputParameters validParams<DGCoeffDiffusion>();

/**
 * DG kernel for diffusion
 *
 * General DG kernel that this class can handle is:
 * \f$ { \nabla u * n_e} [v] + epsilon { \nabla v * n_e } [u] + (sigma / |e| * [u][v]) \f$
 *
 * \f$ [a] = [ a_1 - a_2 ] \f$
 * \f$ {a} = 0.5 * (a_1 + a_2) \f$
 *
 */
class DGCoeffDiffusion : public DGKernel
{
public:
  DGCoeffDiffusion(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual(Moose::DGResidualType type);
  virtual Real computeQpJacobian(Moose::DGJacobianType type);

  Real _epsilon;
  Real _sigma;
  const MaterialProperty<Real> & _diff;
  const MaterialProperty<Real> & _diff_neighbor;
};

#endif
