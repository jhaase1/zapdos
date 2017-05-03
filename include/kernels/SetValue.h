#ifndef SETVALUE_H
#define SETVALUE_H

#include "Kernel.h"

class SetValue;

template <>
InputParameters validParams<SetValue>();

class SetValue : public Kernel
{
public:
  SetValue(const InputParameters & parameters);
  virtual ~SetValue();

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  // coupled vars

  Real _value;
};

#endif /* SETVALUE_H */
