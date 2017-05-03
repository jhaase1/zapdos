#ifndef SCHOTTKYEMISSIONENERGYBC_H
#define SCHOTTKYEMISSIONENERGYBC_H

#include "SchottkyEmissionBC.h"

class SchottkyEmissionEnergyBC;

template <>
InputParameters validParams<SchottkyEmissionEnergyBC>();

class SchottkyEmissionEnergyBC : public SchottkyEmissionBC
{
public:
  SchottkyEmissionEnergyBC(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  // Variables
  unsigned int _em_id;
  const VariableValue & _em;
};

#endif /* SCHOTTKYEMISSIONENERGYBC_H */
