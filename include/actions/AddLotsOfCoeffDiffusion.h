
#ifndef ADDLOTSOFCOEFFDIFFUSION_H
#define ADDLOTSOFCOEFFDIFFUSION_H

#include "AddVariableAction.h"
#include "Action.h"

class AddLotsOfCoeffDiffusion;

template <>
InputParameters validParams<AddLotsOfCoeffDiffusion>();

class AddLotsOfCoeffDiffusion : public AddVariableAction
{
public:
  AddLotsOfCoeffDiffusion(InputParameters params);

  virtual void act();
};

#endif // ADDLOTSOFCOEFFDIFFUSION_H
