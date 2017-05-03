
#ifndef IONIZATIONSOURCE_H
#define IONIZATIONSOURCE_H

#include "Kernel.h"

// Forward Declaration
class IonizationSource;

template <>
InputParameters validParams<IonizationSource>();

class IonizationSource : public Kernel
{
public:
  IonizationSource(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  Real _ionization_coeff;

  unsigned int _potential_id;

private:
  VariableGradient & _grad_potential;
};
#endif // IONIZATIONSOURCE_H
