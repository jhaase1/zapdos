
#ifndef ADDLOTSOFSOURCES_H
#define ADDLOTSOFSOURCES_H

#include "AddVariableAction.h"
#include "Action.h"

class AddLotsOfSources;

template <>
InputParameters validParams<AddLotsOfSources>();

class AddLotsOfSources : public AddVariableAction
{
public:
  AddLotsOfSources(InputParameters params);

  virtual void act();
};

#endif // ADDLOTSOFSOURCES_H
