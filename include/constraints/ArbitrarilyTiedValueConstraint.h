
#ifndef ARBITRARILYTIEDVALUECONSTRAINT_H
#define ARBITRARILYTIEDVALUECONSTRAINT_H

// MOOSE includes
#include "NodeFaceConstraint.h"

// Forward Declarations
class ArbitrarilyTiedValueConstraint;

template <>
InputParameters validParams<ArbitrarilyTiedValueConstraint>();

/**
 * A ArbitrarilyTiedValueConstraint forces the value of a variable to be the same on both sides of
 * an interface.
 */
class ArbitrarilyTiedValueConstraint : public NodeFaceConstraint
{
public:
  ArbitrarilyTiedValueConstraint(const InputParameters & parameters);
  virtual ~ArbitrarilyTiedValueConstraint() {}

  virtual Real computeQpSlaveValue();

  virtual Real computeQpResidual(Moose::ConstraintType type);

  virtual Real computeQpJacobian(Moose::ConstraintJacobianType type);

protected:
  const Real _scaling;
  const Real _H;
  NumericVector<Number> & _residual_copy;
};

#endif
