#include "HagelaarEnergyBC.h"

// MOOSE includes
#include "MooseVariable.h"

template <>
InputParameters
validParams<HagelaarEnergyBC>()
{
  InputParameters params = validParams<IntegratedBC>();
  params.addRequiredParam<Real>("r", "The reflection coefficient");
  params.addRequiredCoupledVar("potential", "The electric potential");
  params.addRequiredCoupledVar("em", "The electron density.");
  params.addParam<Real>("position_units", 1.0, "Units of position.");
  params.addParam<Real>("time_units", 1.0, "Units of time.");
  return params;
}

HagelaarEnergyBC::HagelaarEnergyBC(const InputParameters & parameters)
  : IntegratedBC(parameters),

    _r(getParam<Real>("r")),
    _r_units(1. / getParam<Real>("position_units")),
    _t_units(1. / getParam<Real>("time_units")),

    // Coupled Variables
    _grad_potential(coupledGradient("potential")),
    _potential_id(coupled("potential")),
    _em(coupledValue("em")),
    _em_id(coupled("em")),
    
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
HagelaarEnergyBC::computeQpResidual()
{
	_a = (_normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0) ? 1.0 : 0.0;
	
	_actual_mean_en = std::exp(_u[_qp] - _em[_qp]);
	
  _v_thermal = (_r_units / _t_units) *
      std::sqrt(8.0 * _e[_qp] * 2.0 / 3.0 * _actual_mean_en / (M_PI * _massem[_qp]));
			
	_electron_flux = (1.0 - _r) / (1.0 + _r) * (
         -(2.0 * _a - 1.0) * _muem[_qp] * -_grad_potential[_qp] * _normals[_qp] * std::exp(_em[_qp]) + 
				 0.5 * _v_thermal * std::exp(_em[_qp])
				 );

  return _test[_i][_qp] * (5.0/3.0) * _actual_mean_en * _electron_flux ;
}

Real
HagelaarEnergyBC::computeQpJacobian()
{
	_a = (_normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0) ? 1.0 : 0.0;
	
	_actual_mean_en = std::exp(_u[_qp] - _em[_qp]);
	
  _v_thermal = (_r_units / _t_units) *
      std::sqrt(8.0 * _e[_qp] * 2.0 / 3.0 * _actual_mean_en / (M_PI * _massem[_qp]));
			
	_electron_flux = (1.0 - _r) / (1.0 + _r) * (
         -(2.0 * _a - 1.0) * _muem[_qp] * -_grad_potential[_qp] * _normals[_qp] * std::exp(_em[_qp]) + 
				 0.5 * _v_thermal * std::exp(_em[_qp])
				 );

  _d_electron_flux_d_u = (1.0 - _r) / (1.0 + _r) * (
         -(2.0 * _a - 1.0) * _d_muem_d_actual_mean_en[_qp] * _actual_mean_en * -_grad_potential[_qp] * _normals[_qp] * std::exp(_u[_qp]) * _phi[_j][_qp] +
				 0.25 * _v_thermal * std::exp(_em[_qp]) * _phi[_j][_qp]
				 );
				 
  return _test[_i][_qp] * (
				(5.0/3.0) * _actual_mean_en * _d_electron_flux_d_u +
				(5.0/3.0) * _actual_mean_en * _electron_flux       * _phi[_j][_qp]
				);
}

Real
HagelaarEnergyBC::computeQpOffDiagJacobian(unsigned int jvar)
{
  _a = (_normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0) ? 1.0 : 0.0;

  _actual_mean_en = std::exp(_u[_qp] - _em[_qp]);

  _v_thermal = (_r_units / _t_units) *
      std::sqrt(8.0 * _e[_qp] * 2.0 / 3.0 * _actual_mean_en / (M_PI * _massem[_qp]));

	_electron_flux = (1.0 - _r) / (1.0 + _r) * (
         -(2.0 * _a - 1.0) * _muem[_qp] * -_grad_potential[_qp] * _normals[_qp] * std::exp(_em[_qp]) + 
				 0.5 * _v_thermal * std::exp(_em[_qp])
				 );
	
  if (jvar == _potential_id)
  {
    _d_electron_flux_d_u = (1.0 - _r) / (1.0 + _r) * (
         -(2.0 * _a - 1.0) * _muem[_qp] * -_grad_phi[_j][_qp]   * _normals[_qp] * std::exp(_em[_qp])
				 );

    return _test[_i][_qp] * (5.0/3.0) * _actual_mean_en * _d_electron_flux_d_u ;
  } else if (jvar == _em_id)
  {
    _d_electron_flux_d_u = (1.0 - _r) / (1.0 + _r) * (
         -(2.0 * _a - 1.0) * _d_muem_d_actual_mean_en[_qp] * _actual_mean_en * -_grad_potential[_qp] * _normals[_qp] * std::exp(_u[_qp])  * -_phi[_j][_qp] +
				 -(2.0 * _a - 1.0) * _muem[_qp]                                      * -_grad_potential[_qp] * _normals[_qp] * std::exp(_em[_qp]) *  _phi[_j][_qp] +
				 0.25 * _v_thermal * std::exp(_em[_qp]) * _phi[_j][_qp]
				 );

    return _test[_i][_qp] * (
		         (5.0/3.0) * _actual_mean_en * _d_electron_flux_d_u +
						-(5.0/3.0) * _actual_mean_en * _electron_flux *  _phi[_j][_qp]
						);
  } else
	{
    return 0.0;
	}
}
