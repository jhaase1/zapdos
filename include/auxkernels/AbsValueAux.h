
#ifndef ABSVALUEAUX_H
#define ABSVALUEAUX_H

#include "AuxKernel.h"

class AbsValueAux;

template <>
InputParameters validParams<AbsValueAux>();

class AbsValueAux : public AuxKernel
{
public:
  AbsValueAux(const InputParameters & parameters);

protected:
  virtual Real computeValue();

private:
  const VariableValue & _u;
};

#endif // ABSVALUEAUX_H
