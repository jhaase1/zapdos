#ifndef SCHOTTKYEMISSIONENERGYNEWBC_H
#define SCHOTTKYEMISSIONENERGYNEWBC_H

#include "SchottkyEmissionNewBC.h"

class SchottkyEmissionEnergyNewBC;

template <>
InputParameters validParams<SchottkyEmissionEnergyNewBC>();

class SchottkyEmissionEnergyNewBC : public SchottkyEmissionNewBC
{
public:
  SchottkyEmissionEnergyNewBC(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);
};

#endif /* SCHOTTKYEMISSIONENERGYNEWBC_H */
