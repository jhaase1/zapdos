#include "SchottkyEmissionEnergyNewBC.h"

template <>
InputParameters
validParams<SchottkyEmissionEnergyNewBC>()
{
  InputParameters params = validParams<SchottkyEmissionNewBC>();
  params.addRequiredParam<Real>("r", "The reflection coefficient");
	params.addRequiredCoupledVar("em_emitted", "The emitted electron density.");
  return params;
}

SchottkyEmissionEnergyNewBC::SchottkyEmissionEnergyNewBC(const InputParameters & parameters)
  : SchottkyEmissionNewBC(parameters),
		
    // Coupled Variables    
    _em_emitted(coupledValue("em_emitted"))

{
}

Real
SchottkyEmissionEnergyNewBC::computeQpResidual()
{
  _a = (_normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0) ? 1.0 : 0.0;

  _relaxation_expr = _relax ? std::tanh(_t / _tau) : 1.0;

  _actual_mean_en = std::exp(_mean_en[_qp] - _em[_qp]);
	
  _v_thermal = (_r_units / _t_units) *
      std::sqrt(8.0 * _e[_qp] * 2.0 / 3.0 * _actual_mean_en / (M_PI * _massem[_qp]));
			
  _v_drift = _muem[_qp] * -_grad_potential[_qp] * _normals[_qp];

  _emission_flux = _relaxation_expr * SchottkyEmissionNewBC::emission_flux();

  return _test[_i][_qp] * (5.0/3.0) * _se_energy[_qp] * (1.0 - _a) * _emission_flux;
}

Real
SchottkyEmissionEnergyNewBC::computeQpJacobian()
{
	_a = (_normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0) ? 1.0 : 0.0;

  _relaxation_expr = _relax ? std::tanh(_t / _tau) : 1.0;

  _actual_mean_en = std::exp(_mean_en[_qp] - _em[_qp]);
	
  _v_thermal = (_r_units / _t_units) *
      std::sqrt(8.0 * _e[_qp] * 2.0 / 3.0 * _actual_mean_en / (M_PI * _massem[_qp]));
			
  _v_drift = _muem[_qp] * -_grad_potential[_qp] * _normals[_qp];

  _emission_flux = _relaxation_expr * SchottkyEmissionNewBC::emission_flux();
	
	  return 0;
}

Real
SchottkyEmissionEnergyNewBC::computeQpOffDiagJacobian(unsigned int jvar)
{
	_a = (_normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0) ? 1.0 : 0.0;

  _relaxation_expr = _relax ? std::tanh(_t / _tau) : 1.0;

  _actual_mean_en = std::exp(_mean_en[_qp] - _em[_qp]);
	
  _v_thermal = (_r_units / _t_units) *
      std::sqrt(8.0 * _e[_qp] * 2.0 / 3.0 * _actual_mean_en / (M_PI * _massem[_qp]));
			
  _v_drift = _muem[_qp] * -_grad_potential[_qp] * _normals[_qp];

  _emission_flux = _relaxation_expr * SchottkyEmissionNewBC::emission_flux();
	
	if (jvar == _em_emitted_id)
  {
		return 0;
		
  }
  else if (jvar == _potential_id)
  {
    _d_emission_flux_d_u = (_a == 0.0) ? SchottkyEmissionNewBC::d_emission_flux_d_potential() : 0.0;
		
		return _test[_i][_qp] *
					 (
						(1.0 - _a) * _d_emission_flux_d_u
						);
  }
  else
  {
    return 0.0;
  }
}