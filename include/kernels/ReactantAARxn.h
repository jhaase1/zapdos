
#ifndef REACTANTAARXN_H
#define REACTANTAARXN_H

#include "Kernel.h"

// Forward Declaration
class ReactantAARxn;

template <>
InputParameters validParams<ReactantAARxn>();

class ReactantAARxn : public Kernel
{
public:
  ReactantAARxn(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

  // The reaction coefficient
  const MaterialProperty<Real> & _reaction_coeff;
};
#endif // REACTANTAARXN_H
