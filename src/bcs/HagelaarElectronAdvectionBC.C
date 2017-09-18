#include "HagelaarElectronAdvectionBC.h"

template <>
InputParameters
validParams<HagelaarElectronAdvectionBC>()
{
  InputParameters params = validParams<IntegratedBC>();
  params.addRequiredParam<Real>("r", "The reflection coefficient");
  params.addRequiredCoupledVar("potential", "The electric potential");
  params.addRequiredCoupledVar("mean_en", "The mean energy.");
  params.addParam<Real>("position_units", 1.0, "Units of position.");
  params.addParam<Real>("time_units", 1.0, "Units of time.");
  return params;
}

HagelaarElectronAdvectionBC::HagelaarElectronAdvectionBC(const InputParameters & parameters)
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
    _actual_mean_en(0.0)
{
}

Real
HagelaarElectronAdvectionBC::computeQpResidual()
{
  _a = (_normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0) ? 1.0 : 0.0;

  _actual_mean_en = std::exp(_mean_en[_qp] - _u[_qp]);

  return _test[_i][_qp] * (1.0 - _r) / (1.0 + _r) * (
         -(2.0 * _a - 1.0) * _muem[_qp] * -_grad_potential[_qp] * _normals[_qp] * std::exp(_u[_qp])
				 );
}

Real
HagelaarElectronAdvectionBC::computeQpJacobian()
{
  _a = (_normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0) ? 1.0 : 0.0;

  _actual_mean_en = std::exp(_mean_en[_qp] - _u[_qp]);

  return _test[_i][_qp] * (1.0 - _r) / (1.0 + _r) * (
          -(2.0 * _a - 1.0) * _muem[_qp]                                      * -_grad_potential[_qp] * _normals[_qp] * std::exp(_u[_qp])       *  _phi[_j][_qp] +
          -(2.0 * _a - 1.0) * _d_muem_d_actual_mean_en[_qp] * _actual_mean_en * -_grad_potential[_qp] * _normals[_qp] * std::exp(_mean_en[_qp]) * -_phi[_j][_qp]
					);
}

Real
HagelaarElectronAdvectionBC::computeQpOffDiagJacobian(unsigned int jvar)
{
  _a = (_normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0) ? 1.0 : 0.0;

  _actual_mean_en = std::exp(_mean_en[_qp] - _u[_qp]);
	
  if (jvar == _potential_id)
  {
    return _test[_i][_qp] * (1.0 - _r) / (1.0 + _r) * (
           -(2.0 * _a - 1.0) * _muem[_qp] * -_grad_phi[_j][_qp] * _normals[_qp] * std::exp(_u[_qp])
					 );
  } else if (jvar == _mean_en_id)
  {
    _actual_mean_en = std::exp(_mean_en[_qp] - _u[_qp]);

    return _test[_i][_qp] * (1.0 - _r) / (1.0 + _r) * (
           -(2.0 * _a - 1.0) * _d_muem_d_actual_mean_en[_qp] * _actual_mean_en * -_grad_potential[_qp] * _normals[_qp] * std::exp(_mean_en[_qp]) * _phi[_j][_qp]
					 );
  }

  else
    return 0.0;
}
