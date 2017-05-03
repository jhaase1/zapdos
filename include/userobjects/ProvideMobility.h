#ifndef PROVIDEMOBILITY_H
#define PROVIDEMOBILITY_H

#include "GeneralUserObject.h"

// Forward Declarations
class ProvideMobility;

template <>
InputParameters validParams<ProvideMobility>();

class ProvideMobility : public GeneralUserObject
{
public:
  ProvideMobility(const InputParameters & parameters);

  Real electrode_area() const;
  Real ballast_resist() const;
  // Real coulomb_charge() const;

  virtual void initialize();

  virtual void execute();

  virtual void finalize();

protected:
  Real _electrode_area;
  Real _ballast_resist;
  // const MaterialProperty<Real> & _e;
};

#endif
