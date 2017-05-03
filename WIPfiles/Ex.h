
#ifndef EX_H
#define EX_H

#include "AuxKernel.h"

// Forward Declarations
class Ex;

template <>
InputParameters validParams<Ex>();

/**
 * Constant auxiliary value
 */
class Ex : public AuxKernel
{
public:
  Ex(const InputParameters & parameters);

  virtual ~Ex() {}

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

#endif // EX_H
