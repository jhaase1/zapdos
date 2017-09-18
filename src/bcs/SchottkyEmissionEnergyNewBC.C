#include "SchottkyEmissionEnergyNewBC.h"

template <>
InputParameters
validParams<SchottkyEmissionEnergyNewBC>()
{
  InputParameters params = validParams<SchottkyEmissionNewBC>();
  params.addRequiredParam<Real>("r", "The reflection coefficient");
  params.addRequiredCoupledVar("em", "The electron density.");
  return params;
}

SchottkyEmissionEnergyNewBC::SchottkyEmissionEnergyNewBC(const InputParameters & parameters)
  : SchottkyEmissionNewBC(parameters),
		
    // Coupled Variables
    _em_id(coupled("em")),
    _em(coupledValue("em"))

{
}

Real
SchottkyEmissionEnergyNewBC::computeQpResidual()
{
  _a = (_normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0) ? 1.0 : 0.0;

  _relaxation_expr = _relax ? std::tanh(_t / _tau) : 1.0;

  _actual_mean_en = std::exp(_mean_en[_qp] - _u[_qp]);
	
  _v_thermal = (_r_units / _t_units) *
      std::sqrt(8.0 * _e[_qp] * 2.0 / 3.0 * _actual_mean_en / (M_PI * _massem[_qp]));
			
  _v_drift = _muem[_qp] * -_grad_potential[_qp] * _normals[_qp];

  _emission_flux = _relaxation_expr * ((_a == 0.0) ? SchottkyEmissionNewBC::emission_flux() : 0.0);
	
  _n_emitted = _emission_flux / (_v_drift + std::numeric_limits<double>::epsilon());

  return _test[_i][_qp] * (5.0/3.0) * _se_energy[_qp] *
         (((1.0 - _r) / (1 + _r)) * 0.5 * _v_thermal * _n_emitted + // Minus a minus is a plus
          (2.0 / (1.0 + _r)) * (1.0 - _a) * _emission_flux);
}

Real
SchottkyEmissionEnergyNewBC::computeQpJacobian()
{
	_a = (_normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0) ? 1.0 : 0.0;

  _relaxation_expr = _relax ? std::tanh(_t / _tau) : 1.0;

  _actual_mean_en = std::exp(_mean_en[_qp] - _u[_qp]);
	
  _v_thermal = (_r_units / _t_units) *
      std::sqrt(8.0 * _e[_qp] * 2.0 / 3.0 * _actual_mean_en / (M_PI * _massem[_qp]));
			
  _v_drift = _muem[_qp] * -_grad_potential[_qp] * _normals[_qp];

  _emission_flux = _relaxation_expr * ((_a == 0.0) ? SchottkyEmissionNewBC::emission_flux() : 0.0);
	
  _n_emitted = _emission_flux / (_v_drift + std::numeric_limits<double>::epsilon());
	
	  return _test[_i][_qp] * _phi[_j][_qp] * (5.0/3.0) * _se_energy[_qp] *
					(1.0 - _r) / (1 + _r) * _v_thermal * _n_emitted *
					(
					-_muem[_qp] +
					2.0 * _d_muem_d_actual_mean_en[_qp] * std::pow( _actual_mean_en , 2 )
					) / (
					4.0 * _muem[_qp]
					);
}

Real
SchottkyEmissionEnergyNewBC::computeQpOffDiagJacobian(unsigned int jvar)
{
	_a = (_normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0) ? 1.0 : 0.0;

  _relaxation_expr = _relax ? std::tanh(_t / _tau) : 1.0;

  _actual_mean_en = std::exp(_mean_en[_qp] - _u[_qp]);
	
  _v_thermal = (_r_units / _t_units) *
      std::sqrt(8.0 * _e[_qp] * 2.0 / 3.0 * _actual_mean_en / (M_PI * _massem[_qp]));
			
  _v_drift = _muem[_qp] * -_grad_potential[_qp] * _normals[_qp];

  _emission_flux = _relaxation_expr * ((_a == 0.0) ? SchottkyEmissionNewBC::emission_flux() : 0.0);
	
  _n_emitted = _emission_flux / (_v_drift + std::numeric_limits<double>::epsilon());

	
	if (jvar == _em_id)
  {
		return _test[_i][_qp] * _phi[_j][_qp] * (5.0/3.0) * _se_energy[_qp] *
						(1.0 - _r) / (1 + _r) * _v_thermal * _n_emitted *
						(
						_muem[_qp] +
						-2.0 * _d_muem_d_actual_mean_en[_qp] * std::pow( _actual_mean_en , 2 )
						) / (
						4.0 * _muem[_qp]
						);
		
  }
  else if (jvar == _potential_id)
  {
    _d_emission_flux_d_u = (_a == 0.0) ? SchottkyEmissionNewBC::d_emission_flux_d_potential() : 0.0;
		
		return _test[_i][_qp] * (5.0/3.0) * _se_energy[_qp] *
						(
						(1.0 - _r) / (1 + _r) * _v_thermal * _n_emitted / (2.0 * _grad_potential[_qp] * _normals[_qp]) * (_grad_phi[_j][_qp] * _normals[_qp]) +
						(1.0 - _r) / (1 + _r) * _v_thermal * _n_emitted * _d_emission_flux_d_u / (2.0 * _emission_flux) * (_grad_phi[_j][_qp] * _normals[_qp]) +
						-(2.0 / (1.0 + _r)) * (1.0 - _a) * SchottkyEmissionNewBC::d_emission_flux_d_potential() * (_grad_phi[_j][_qp] * _normals[_qp])
						);
  }
  else
  {
    return 0.0;
  }
}