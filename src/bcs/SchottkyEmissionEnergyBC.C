#include "SchottkyEmissionEnergyBC.h"

template <>
InputParameters
validParams<SchottkyEmissionEnergyBC>()
{
  InputParameters params = validParams<SchottkyEmissionBC>();
  params.addRequiredCoupledVar("em", "The electron density.");
  return params;
}

SchottkyEmissionEnergyBC::SchottkyEmissionEnergyBC(const InputParameters & parameters)
  : SchottkyEmissionBC(parameters),

    // Variables
    _em_id(coupled("em")),
    _em(coupledValue("em"))
{
}

Real
SchottkyEmissionEnergyBC::computeQpResidual()
{
  _a = (_normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0) ? 1.0 : 0.0;
  _relaxation_expr = _relax ? std::tanh(_t / _tau) : 1.0;

  _actual_mean_en = std::exp(_u[_qp] - _em[_qp]);
  //	_v_thermal = (_r_units / _t_units) * std::sqrt(8.0 * _e[_qp] * 2.0 / 3.0 * _actual_mean_en /
  //(M_PI * _massem[_qp]));
  _v_drift = (1.0 - 2.0 * _a) * _muem[_qp] * -_grad_potential[_qp] * _normals[_qp];

  _emission_flux = (_a == 0.0) ? SchottkyEmissionBC::emission_flux() : 0.0;
  _n_emitted = _emission_flux / (_v_drift + std::numeric_limits<double>::epsilon());

  _n_e_transported = (5.0 / 3.0) * (std::exp(_u[_qp]) - _se_energy[_qp] * _n_emitted);

  return -_test[_i][_qp] *
         (((1.0 - _r) / (1 + _r)) * (_v_drift + 0.5 * _v_thermal) * _n_e_transported +
          (5.0 / 3.0) * _relaxation_expr * _se_energy[_qp] * _emission_flux);
}

Real
SchottkyEmissionEnergyBC::computeQpJacobian()
{
  _a = (_normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0) ? 1.0 : 0.0;
  _relaxation_expr = _relax ? std::tanh(_t / _tau) : 1.0;

  _actual_mean_en = std::exp(_u[_qp] - _em[_qp]);

  //	_v_thermal = (_r_units / _t_units) * std::sqrt(8.0 * _e[_qp] * 2.0 / 3.0 * _actual_mean_en /
  //(M_PI * _massem[_qp]));
  //	_d_v_thermal_d_u = 0.5 * _v_thermal * _phi[_j][_qp] ;

  _v_drift = (1.0 - 2.0 * _a) * _muem[_qp] * -_grad_potential[_qp] * _normals[_qp];
  _d_v_drift_d_u = (1.0 - 2.0 * _a) *
                   (_d_muem_d_actual_mean_en[_qp] * _actual_mean_en * _phi[_j][_qp]) *
                   -_grad_potential[_qp] * _normals[_qp];

  _emission_flux = (_a == 0.0) ? SchottkyEmissionBC::emission_flux() : 0.0;
  _d_emission_flux_d_u = (_a == 0.0) ? SchottkyEmissionBC::d_emission_flux_d_mean_en() : 0.0;

  _n_emitted = _emission_flux / (_v_drift + std::numeric_limits<double>::epsilon());
  _d_n_emitted_d_u = (_d_emission_flux_d_u * _v_drift - _d_v_drift_d_u * _emission_flux) /
                     pow((_v_drift + std::numeric_limits<double>::epsilon()), 2);

  _d_n_e_transported_d_u =
      (5.0 / 3.0) * (std::exp(_u[_qp]) * _phi[_j][_qp] - _se_energy[_qp] * _d_n_emitted_d_u);

  return -_test[_i][_qp] *
         (((1.0 - _r) / (1 + _r)) * (_d_v_drift_d_u + 0.5 * _d_v_thermal_d_u) * _n_e_transported +
          ((1.0 - _r) / (1 + _r)) * (_v_drift + 0.5 * _v_thermal) * _d_n_e_transported_d_u +
          (5.0 / 3.0) * _se_energy[_qp] * _relaxation_expr * _d_emission_flux_d_u);
}

Real
SchottkyEmissionEnergyBC::computeQpOffDiagJacobian(unsigned int jvar)
{
  _a = (_normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0) ? 1.0 : 0.0;
  _relaxation_expr = _relax ? std::tanh(_t / _tau) : 1.0;

  _actual_mean_en = std::exp(_u[_qp] - _em[_qp]);

  //	_v_thermal = (_r_units / _t_units) * std::sqrt(8.0 * _e[_qp] * 2.0 / 3.0 * _actual_mean_en /
  //(M_PI * _massem[_qp]));

  _v_drift = _muem[_qp] * -_grad_potential[_qp] * _normals[_qp];

  _emission_flux = (_a == 0.0) ? SchottkyEmissionBC::emission_flux() : 0.0;

  _n_emitted = _emission_flux / (_v_drift + std::numeric_limits<double>::epsilon());

  if (jvar == _potential_id)
  {
    _d_emission_flux_d_u = (_a == 0.0) ? SchottkyEmissionBC::d_emission_flux_d_potential() : 0.0;
    _d_v_drift_d_u = (1.0 - 2.0 * _a) * _muem[_qp] * -_grad_phi[_j][_qp] * _normals[_qp];
  }
  else if (jvar == _em_id)
  {
    _d_emission_flux_d_u = (_a == 0.0) ? SchottkyEmissionBC::d_emission_flux_d_em() : 0.0;
  }
  else if (jvar == _ip_id)
  {
    _d_emission_flux_d_u = (_a == 0.0) ? SchottkyEmissionBC::d_emission_flux_d_ip() : 0.0;
  }

  return -_test[_i][_qp] *
         (((1.0 - _r) / (1 + _r)) * ((1.0 - 2.0 * _a) * _d_v_drift_d_u + 0.5 * _d_v_thermal_d_u) *
              _n_e_transported +
          ((1.0 - _r) / (1 + _r)) * ((1.0 - 2.0 * _a) * _v_drift + 0.5 * _v_thermal) *
              _d_n_e_transported_d_u +
          (5.0 / 3.0) * _se_energy[_qp] * _relaxation_expr * _d_emission_flux_d_u);
}