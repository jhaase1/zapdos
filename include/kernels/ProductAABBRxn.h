
#ifndef PRODUCTAABBRXN_H
#define PRODUCTAABBRXN_H

#include "Kernel.h"

// Forward Declaration
class ProductAABBRxn;

template <>
InputParameters validParams<ProductAABBRxn>();

class ProductAABBRxn : public Kernel
{
public:
  ProductAABBRxn(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  MooseVariable & _coupled_var;
  const VariableValue & _v;
  unsigned int _v_id;

  // The reaction coefficient
  const MaterialProperty<Real> & _reaction_coeff;
};
#endif // PRODUCTAABBRXN_H
