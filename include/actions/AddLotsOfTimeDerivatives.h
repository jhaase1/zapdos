
#ifndef ADDLOTSOFTIMEDERIVATIVES_H
#define ADDLOTSOFTIMEDERIVATIVES_H

#include "AddVariableAction.h"
#include "Action.h"

class AddLotsOfTimeDerivatives;

template <>
InputParameters validParams<AddLotsOfTimeDerivatives>();

class AddLotsOfTimeDerivatives : public AddVariableAction
{
public:
  AddLotsOfTimeDerivatives(InputParameters params);

  virtual void act();
};

#endif // ADDLOTSOFTIMEDERIVATIVES_H
