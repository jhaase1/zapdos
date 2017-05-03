
#ifndef NODEANDSIDESETBETWEENSUBDOMAINS_H
#define NODEANDSIDESETBETWEENSUBDOMAINS_H

#include "MeshModifier.h"

class NodeAndSidesetBetweenSubdomains;

template <>
InputParameters validParams<NodeAndSidesetBetweenSubdomains>();

class NodeAndSidesetBetweenSubdomains : public MeshModifier
{
public:
  NodeAndSidesetBetweenSubdomains(const InputParameters & parameters);

  virtual ~NodeAndSidesetBetweenSubdomains();

  virtual void modify();

protected:
};

#endif /* NODEANDSIDESETBETWEENSUBDOMAINS_H */
