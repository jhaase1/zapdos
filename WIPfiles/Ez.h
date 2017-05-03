
#ifndef EZ_H
#define EZ_H

#include "AuxKernel.h"

// Forward Declarations
class Ez;

template <>
InputParameters validParams<Ez>();

/**
 * Constant auxiliary value
 */
class Ez : public AuxKernel
{
public:
  Ez(const InputParameters & parameters);

  virtual ~Ez() {}

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

#endif // EZ_H
