#include "HagelaarElectronBC.h"

template <>
InputParameters
validParams<HagelaarElectronBC>()
{
  InputParameters params = validParams<IntegratedBC>();
  params.addRequiredParam<Real>("r", "The reflection coefficient");
  params.addRequiredCoupledVar("potential", "The electric potential");
  params.addRequiredCoupledVar("mean_en", "The mean energy.");
  params.addParam<Real>("position_units", 1.0, "Units of position.");
  params.addParam<Real>("time_units", 1.0, "Units of time.");
  return params;
}

HagelaarElectronBC::HagelaarElectronBC(const InputParameters & parameters)
  : IntegratedBC(parameters),

    _r_units(1. / getParam<Real>("position_units")),
    _t_units(1. / getParam<Real>("time_units")),
    _r(getParam<Real>("r")),

    // Coupled Variables
    _grad_potential(coupledGradient("potential")),
    _potential_id(coupled("potential")),
    _mean_en(coupledValue("mean_en")),
    _mean_en_id(coupled("mean_en")),

    _muem(getMaterialProperty<Real>("muem")),
    _d_muem_d_actual_mean_en(getMaterialProperty<Real>("d_muem_d_actual_mean_en")),
		
    _massem(getMaterialProperty<Real>("massem")),
    _e(getMaterialProperty<Real>("e")),
		
    _a(0.5),

		_electron_flux(0.0),
		_d_electron_flux_d_u(0.0),
    _v_thermal(0.0),
    _actual_mean_en(0.0)
{
}

Real
HagelaarElectronBC::computeQpResidual()
{
	_a = (_normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0) ? 1.0 : 0.0;

  _actual_mean_en = std::exp(_mean_en[_qp] - _u[_qp]);

  _v_thermal = (_r_units / _t_units) *
      std::sqrt(8.0 * _e[_qp] * 2.0 / 3.0 * _actual_mean_en / (M_PI * _massem[_qp]));

	_electron_flux = (1.0 - _r) / (1.0 + _r) * (
         -(2.0 * _a - 1.0) * _muem[_qp] * -_grad_potential[_qp] * _normals[_qp] * std::exp(_u[_qp]) + 
				 0.5 * _v_thermal * std::exp(_u[_qp])
				 );

  return _test[_i][_qp] * _electron_flux;
}

Real
HagelaarElectronBC::computeQpJacobian()
{
	_a = (_normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0) ? 1.0 : 0.0;

  _actual_mean_en = std::exp(_mean_en[_qp] - _u[_qp]);

  _v_thermal = (_r_units / _t_units) *
      std::sqrt(8.0 * _e[_qp] * 2.0 / 3.0 * _actual_mean_en / (M_PI * _massem[_qp]));

	_electron_flux = (1.0 - _r) / (1.0 + _r) * (
         -(2.0 * _a - 1.0) * _muem[_qp] * -_grad_potential[_qp] * _normals[_qp] * std::exp(_u[_qp]) + 
				 0.5 * _v_thermal * std::exp(_u[_qp])
				 );

	_d_electron_flux_d_u = (1.0 - _r) / (1.0 + _r) * (
         -(2.0 * _a - 1.0) * _d_muem_d_actual_mean_en[_qp] * _actual_mean_en * -_grad_potential[_qp] * _normals[_qp] * std::exp(_mean_en[_qp])  * -_phi[_j][_qp] +
				 -(2.0 * _a - 1.0) * _muem[_qp]                                      * -_grad_potential[_qp] * _normals[_qp] * std::exp(_u[_qp])        *  _phi[_j][_qp] +
				 0.25 * _v_thermal * std::exp(_u[_qp]) * _phi[_j][_qp]
				 );
				 
  return _test[_i][_qp] * _d_electron_flux_d_u;

}

Real
HagelaarElectronBC::computeQpOffDiagJacobian(unsigned int jvar)
{
	_a = (_normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0) ? 1.0 : 0.0;

  _actual_mean_en = std::exp(_mean_en[_qp] - _u[_qp]);

  _v_thermal = (_r_units / _t_units) *
      std::sqrt(8.0 * _e[_qp] * 2.0 / 3.0 * _actual_mean_en / (M_PI * _massem[_qp]));

	_electron_flux = (1.0 - _r) / (1.0 + _r) * (
         -(2.0 * _a - 1.0) * _muem[_qp] * -_grad_potential[_qp] * _normals[_qp] * std::exp(_u[_qp]) + 
				 0.5 * _v_thermal * std::exp(_u[_qp])
				 );

  if (jvar == _potential_id)
  {
		_d_electron_flux_d_u = (1.0 - _r) / (1.0 + _r) * (
					 -(2.0 * _a - 1.0) * _muem[_qp] * -_grad_phi[_j][_qp] * _normals[_qp] * std::exp(_u[_qp])
					 );
				 
    return _test[_i][_qp] * _d_electron_flux_d_u;
		
  } else if (jvar == _mean_en_id)
	{
		_d_electron_flux_d_u = (1.0 - _r) / (1.0 + _r) * (
					 -(2.0 * _a - 1.0) * _d_muem_d_actual_mean_en[_qp] * _actual_mean_en * -_grad_potential[_qp] * _normals[_qp] * std::exp(_mean_en[_qp]) * _phi[_j][_qp] +
					 0.25 * _v_thermal * std::exp(_u[_qp]) * _phi[_j][_qp]
					 );
				 
		return _test[_i][_qp] * _d_electron_flux_d_u ;
  } else
	{
    return 0.0;
	}

}
