#include "InverseDensity.h"

template <>
InputParameters
validParams<InverseDensity>()
{
  InputParameters params = validParams<AuxKernel>();

  params.addRequiredCoupledVar("density", "The variable representing the density.");
  return params;
}

InverseDensity::InverseDensity(const InputParameters & parameters)
  : AuxKernel(parameters),

    _density(coupledValue("density"))
{
}

Real
InverseDensity::computeValue()
{
  return std::log(_density[_qp]);
}
