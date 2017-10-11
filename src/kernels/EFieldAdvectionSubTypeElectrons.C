#include "EFieldAdvectionSubTypeElectrons.h"

template <>
InputParameters
validParams<EFieldAdvectionSubTypeElectrons>()
{
  InputParameters params = validParams<Kernel>();
  params.addRequiredCoupledVar(
      "potential", "The gradient of the potential will be used to compute the advection velocity.");
  params.addRequiredCoupledVar("mean_en", "The log of the mean energy.");
	params.addRequiredCoupledVar("em", "The log of the total electron density.");
  params.addRequiredParam<Real>("position_units", "The units of position.");
  return params;
}

EFieldAdvectionSubTypeElectrons::EFieldAdvectionSubTypeElectrons(const InputParameters & parameters)
  : Kernel(parameters),

    _r_units(1. / getParam<Real>("position_units")),

    _muem(getMaterialProperty<Real>("muem")),
    _d_muem_d_actual_mean_en(getMaterialProperty<Real>("d_muem_d_actual_mean_en")),
    _sign(getMaterialProperty<Real>("sgnem")),

    // Coupled variables

    _potential_id(coupled("potential")),
    _grad_potential(coupledGradient("potential")),
    _mean_en(coupledValue("mean_en")),
    _mean_en_id(coupled("mean_en")),
		
    _em(coupledValue("em")),

    _d_actual_mean_en_d_mean_en(0),
    _d_muem_d_mean_en(0),
    _d_actual_mean_en_d_u(0),
    _d_muem_d_u(0),
		_actual_mean_en(0)
{
}

Real
EFieldAdvectionSubTypeElectrons::computeQpResidual()
{
  return _sign[_qp] * _muem[_qp] * -_grad_potential[_qp] * std::exp(_u[_qp]) * -_grad_test[_i][_qp];
}

Real
EFieldAdvectionSubTypeElectrons::computeQpJacobian()
{
  _actual_mean_en = std::exp(_mean_en[_qp] - _em[_qp]);

	return (
					_sign[_qp] * _muem[_qp]                                      * -_grad_potential[_qp] * std::exp(_u[_qp]) * _phi[_j][_qp] +
					_sign[_qp] * _actual_mean_en * _d_muem_d_actual_mean_en[_qp] * -_grad_potential[_qp] * std::exp(_u[_qp]) * _phi[_j][_qp]
					) * -_grad_test[_i][_qp];
}

Real
EFieldAdvectionSubTypeElectrons::computeQpOffDiagJacobian(unsigned int jvar)
{
  _actual_mean_en = std::exp(_mean_en[_qp] - _em[_qp]);
	
  if (jvar == _potential_id)
  {
    return _sign[_qp] * _muem[_qp] * -_grad_phi[_j][_qp] * std::exp(_u[_qp]) * -_grad_test[_i][_qp];
  }

  if (jvar == _mean_en_id)
  {
    return _sign[_qp] * _actual_mean_en * _d_muem_d_actual_mean_en[_qp] * -_grad_potential[_qp] * std::exp(_u[_qp]) * -_grad_test[_i][_qp];
  }

  else
    return 0.0;
}
