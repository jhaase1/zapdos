#ifndef SETEQUALTOLOGSUM_H
#define SETEQUALTOLOGSUM_H

#include "Kernel.h"

class SetEqualToLogSum;

template <>
InputParameters validParams<SetEqualToLogSum>();

class SetEqualToLogSum : public Kernel
{
public:
  SetEqualToLogSum(const InputParameters & parameters);
  virtual ~SetEqualToLogSum();

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  // coupled vars
  const VariableValue & _q1;
	unsigned int _q1_id;
	
	const VariableValue & _q2;
	unsigned int _q2_id;
};

#endif /* SETEQUALTOLOGSUM_H */
