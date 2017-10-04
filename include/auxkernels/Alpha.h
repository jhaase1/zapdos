#ifndef ALPHA_H
#define ALPHA_H

#include "AuxKernel.h"

class Alpha;

template <>
InputParameters validParams<Alpha>();

class Alpha : public AuxKernel
{
public:
  Alpha(const InputParameters & parameters);

  virtual ~Alpha() {}
  virtual Real computeValue();

protected:
  Real _r_units;
  const MaterialProperty<Real> & _alpha;
};

#endif // ALPHA_H
