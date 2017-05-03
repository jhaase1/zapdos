
#ifndef ADDLOTSOFPOTENTIALDRIVENARTIFICIALDIFF_H
#define ADDLOTSOFPOTENTIALDRIVENARTIFICIALDIFF_H

#include "AddVariableAction.h"
#include "Action.h"

class AddLotsOfPotentialDrivenArtificialDiff;

template <>
InputParameters validParams<AddLotsOfPotentialDrivenArtificialDiff>();

class AddLotsOfPotentialDrivenArtificialDiff : public AddVariableAction
{
public:
  AddLotsOfPotentialDrivenArtificialDiff(InputParameters params);

  virtual void act();
};

#endif // ADDLOTSOFPOTENTIALDRIVENARTIFICIALDIFF_H
