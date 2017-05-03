
#ifndef ADDLOTSOFEFIELDADVECTION_H
#define ADDLOTSOFEFIELDADVECTION_H

#include "AddVariableAction.h"
#include "Action.h"

class AddLotsOfEFieldAdvection;

template <>
InputParameters validParams<AddLotsOfEFieldAdvection>();

class AddLotsOfEFieldAdvection : public AddVariableAction
{
public:
  AddLotsOfEFieldAdvection(InputParameters params);

  virtual void act();
};

#endif // ADDLOTSOFEFIELDADVECTION_H
