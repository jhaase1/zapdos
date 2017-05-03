
#ifndef ADDLOTSOFVARIABLES_H
#define ADDLOTSOFVARIABLES_H

#include "AddVariableAction.h"
#include "Action.h"

class AddLotsOfVariables;

template <>
InputParameters validParams<AddLotsOfVariables>();

class AddLotsOfVariables : public AddVariableAction
{
public:
  AddLotsOfVariables(InputParameters params);

  virtual void act();
};

#endif // ADDLOTSOFVARIABLES_H
