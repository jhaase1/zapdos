
#ifndef CIRCUITVOLTAGE_H
#define CIRCUITVOLTAGE_H

#include "NodalBC.h"
#include "ProvideMobility.h"

// Forward Declarations
class CircuitVoltage;
class Function;

template <>
InputParameters validParams<CircuitVoltage>();

/**
 * Defines a boundary condition that forces the value to be a user specified
 * function at the boundary.
 */
class CircuitVoltage : public NodalBC
{
public:
  CircuitVoltage(const std::string & name, InputParameters parameters);

protected:
  /**
   * Evaluate the function at the current quadrature point and timestep.
   */
  Real f();

  /**
   * returns (u - the function)
   */
  virtual Real computeQpResidual();

  /// The function being used for evaluation
  Function & _func;
  VariableValue & _ip;
  VariableGradient & _grad_ip;

  const ProvideMobility & _data;
};

#endif // CIRCUITVOLTAGE_H
