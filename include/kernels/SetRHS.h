#ifndef SETRHS_H
#define SETRHS_H

#include "Kernel.h"

class SetRHS;

template <>
InputParameters validParams<SetRHS>();

class SetRHS : public Kernel
{
public:
  SetRHS(const InputParameters & parameters);
  virtual ~SetRHS();

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  // coupled vars

  Real _value;
};

#endif /* SETSRH_H */
