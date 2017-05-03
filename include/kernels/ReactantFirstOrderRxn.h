
#ifndef REACTANTFIRSTORDERRXN_H
#define REACTANTFIRSTORDERRXN_H

#include "Kernel.h"

// Forward Declaration
class ReactantFirstOrderRxn;

template <>
InputParameters validParams<ReactantFirstOrderRxn>();

class ReactantFirstOrderRxn : public Kernel
{
public:
  ReactantFirstOrderRxn(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

  // The reaction coefficient
  const MaterialProperty<Real> & _reaction_coeff;
};
#endif // REACTANTFIRSTORDERRXN_H
