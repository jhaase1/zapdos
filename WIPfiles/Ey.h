
#ifndef EY_H
#define EY_H

#include "AuxKernel.h"

// Forward Declarations
class Ey;

template <>
InputParameters validParams<Ey>();

/**
 * Constant auxiliary value
 */
class Ey : public AuxKernel
{
public:
  Ey(const InputParameters & parameters);

  virtual ~Ey() {}

protected:
  /**
   * AuxKernels MUST override computeValue.  computeValue() is called on
   * every quadrature point.  For Nodal Auxiliary variables those quadrature
   * points coincide with the nodes.
   */
  virtual void compute();
  virtual Real computeValue();
  virtual void computeVarValues(std::vector<Real> & values);

  // int _component;
  Real _r_units;
  std::string _potential_units;

  /// The gradient of a coupled variable
  const VariableGradient & _grad_potential;
  Real _voltage_scaling;
};

#endif // EY_H
