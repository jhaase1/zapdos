#include "DensityMoles.h"

template <>
InputParameters
validParams<DensityMoles>()
{
  InputParameters params = validParams<Density>();

  params.addRequiredParam<bool>("use_moles", "Whether densities are stored in units of moles.");
  params.addParam<bool>("convert_units", false, "Whether to convert units (moles to #) or (# to moles).");
  return params;
}

DensityMoles::DensityMoles(const InputParameters & parameters)
  : Density(parameters),

	_use_moles(getParam<bool>("use_moles")),
	_convert_units(getParam<bool>("convert_units")),
	_N_A(getMaterialProperty<Real>("N_A"))
{
}

Real
DensityMoles::computeValue()
{
  if (!(_convert_units)) {
		return Density::computeValue();
	} else {
		if (_use_moles) {
			return Density::computeValue() * _N_A[_qp];
		} else {
			return Density::computeValue() / _N_A[_qp];
		}
	}
}
