
#include "WDAtomicDensity.h"

template <>
InputParameters
validParams<WDAtomicDensity>()
{
  InputParameters params = validParams<AuxKernel>();

  params.addRequiredCoupledVar("potential", "The potential");

  return params;
}

WDAtomicDensity::WDAtomicDensity(const InputParameters & parameters)
  : AuxKernel(parameters),

    // Get the gradient of the variable
    _grad_potential(coupledGradient("potential"))
{
}

Real
WDAtomicDensity::computeValue()
{
  // Access the gradient of the pressure at this quadrature point
  // Then pull out the "component" of it we are looking for (x, y or z)
  // Note that getting a particular component of a gradient is done using the
  // parenthesis operator
  return _grad_potential[_qp].size();
  // return _grad_potential[_qp](_component);
}
