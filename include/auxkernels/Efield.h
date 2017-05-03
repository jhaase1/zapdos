
#ifndef EFIELD_H
#define EFIELD_H

#include "AuxKernel.h"

// Forward Declarations
class Efield;

template <>
InputParameters validParams<Efield>();

/**
 * Constant auxiliary value
 */
class Efield : public AuxKernel
{
public:
  Efield(const InputParameters & parameters);

  virtual ~Efield() {}

protected:
  /**
   * AuxKernels MUST override computeValue.  computeValue() is called on
   * every quadrature point.  For Nodal Auxiliary variables those quadrature
   * points coincide with the nodes.
   */
  virtual Real computeValue();

  int _component;
  Real _r_units;
  std::string _potential_units;

  /// The gradient of a coupled variable
  const VariableGradient & _grad_potential;
  Real _voltage_scaling;
};

#endif // EFIELD_H
