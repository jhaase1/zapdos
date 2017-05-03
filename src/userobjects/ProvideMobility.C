
#include "ProvideMobility.h"

template <>
InputParameters
validParams<ProvideMobility>()
{
  InputParameters params = validParams<GeneralUserObject>();
  params.addRequiredParam<Real>("electrode_area", "The area of the electrode or plasma.");
  params.addRequiredParam<Real>("ballast_resist", "The magnitude of the ballasting resistance.");
  return params;
}

ProvideMobility::ProvideMobility(const InputParameters & parameters)
  : GeneralUserObject(parameters),
    _electrode_area(getParam<Real>(
        "electrode_area")), // in square meters. Taken from Comsol's 1D corona discharge
    _ballast_resist(
        getParam<Real>("ballast_resist")) // Also taken from Comsol's 1D corona discharge
// _electrode_area(1.26e-5), // in square meters. Taken from Comsol's 1D corona discharge. Used in
// my golden simulation results
// _ballast_resist(1e6), // Also taken from Comsol's 1D corona discharge. Used in my golden
// simulation results
//_e(getMaterialProperty<Real>("e"))
{
}

Real
ProvideMobility::electrode_area() const
{
  return _electrode_area;
}
Real
ProvideMobility::ballast_resist() const
{
  return _ballast_resist;
}
/*Real
ProvideMobility::coulomb_charge() const
{
  return _e;
}
*/
void
ProvideMobility::initialize()
{
}

void
ProvideMobility::execute()
{
}

void
ProvideMobility::finalize()
{
}
