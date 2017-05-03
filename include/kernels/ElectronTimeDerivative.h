
#ifndef ELECTRONTIMEDERIVATIVE_H
#define ELECTRONTIMEDERIVATIVE_H

#include "TimeKernel.h"

// Forward Declaration
class ElectronTimeDerivative;

template <>
InputParameters validParams<ElectronTimeDerivative>();

class ElectronTimeDerivative : public TimeKernel
{
public:
  ElectronTimeDerivative(const InputParameters & parameters);

  /* virtual void computeJacobian(); */

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  /* virtual Real computeQpOffDiagJacobian(unsigned int jvar); */

  bool _lumping;
};

#endif // ELECTRONTIMEDERIVATIVE_H
