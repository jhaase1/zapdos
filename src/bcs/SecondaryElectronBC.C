#include "SecondaryElectronBC.h"

// MOOSE includes
#include "MooseVariable.h"

template <>
InputParameters
validParams<SecondaryElectronBC>()
{
  InputParameters params = validParams<IntegratedBC>();
  params.addRequiredParam<Real>("r", "The reflection coefficient");
  params.addRequiredCoupledVar("potential", "The electric potential");
  params.addRequiredCoupledVar("mean_en", "The mean energy.");
  params.addRequiredCoupledVar("ip", "The ion density.");
  params.addParam<Real>("position_units", 1.0, "Units of position.");
  params.addParam<Real>("time_units", 1.0, "Units of time.");
  return params;
}

SecondaryElectronBC::SecondaryElectronBC(const InputParameters & parameters)
  : IntegratedBC(parameters),

    _r_units(1. / getParam<Real>("position_units")),
    _t_units(1. / getParam<Real>("time_units")),
    _r(getParam<Real>("r")),

    // Coupled Variables
    _grad_potential(coupledGradient("potential")),
    _potential_id(coupled("potential")),
    _mean_en(coupledValue("mean_en")),
    _mean_en_id(coupled("mean_en")),
    _ip_var(*getVar("ip", 0)),
    _ip(coupledValue("ip")),
    _grad_ip(coupledGradient("ip")),
    _ip_id(coupled("ip")),

    _muem(getMaterialProperty<Real>("muem")),
    _d_muem_d_actual_mean_en(getMaterialProperty<Real>("d_muem_d_actual_mean_en")),
    _massem(getMaterialProperty<Real>("massem")),
    _e(getMaterialProperty<Real>("e")),
    _sgnip(getMaterialProperty<Real>("sgn" + _ip_var.name())),
    _muip(getMaterialProperty<Real>("mu" + _ip_var.name())),
    _Dip(getMaterialProperty<Real>("diff" + _ip_var.name())),
    _se_coeff(getMaterialProperty<Real>("se_coeff")),
    _a(0.5),
    _v_thermal(0),
    _ion_flux(0, 0, 0),
    _n_gamma(0),
    _d_v_thermal_d_u(0),
    _d_v_thermal_d_mean_en(0),
    _d_ion_flux_d_potential(0, 0, 0),
    _d_ion_flux_d_ip(0, 0, 0),
    _d_n_gamma_d_potential(0),
    _d_n_gamma_d_ip(0),
    _d_n_gamma_d_u(0),
    _d_n_gamma_d_mean_en(0),
    _actual_mean_en(0)
{
}

Real
SecondaryElectronBC::computeQpResidual()
{
	_a = (_normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0) ? 1.0 : 0.0;

  _actual_mean_en = std::exp(_mean_en[_qp] - _u[_qp]);
	
	_v_thermal = (_r_units / _t_units) *
      std::sqrt(8.0 * _e[_qp] * 2.0 / 3.0 * _actual_mean_en / (M_PI * _massem[_qp]));

  _ion_flux =  _sgnip[_qp] * _muip[_qp] * -_grad_potential[_qp] * std::exp(_ip[_qp]) -
              _Dip[_qp] * std::exp(_ip[_qp]) * _grad_ip[_qp];

  return -_test[_i][_qp] * 2. / (1. + _r) * (1. - _a) * _se_coeff[_qp] * _ion_flux * _normals[_qp] ;
}

Real
SecondaryElectronBC::computeQpJacobian()
{
	_a = (_normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0) ? 1.0 : 0.0;

  _actual_mean_en = std::exp(_mean_en[_qp] - _u[_qp]);
	
	_v_thermal = (_r_units / _t_units) *
      std::sqrt(8.0 * _e[_qp] * 2.0 / 3.0 * _actual_mean_en / (M_PI * _massem[_qp]));

	_d_v_thermal_d_u = 0.5 * _v_thermal * -_phi[_j][_qp];
		
  _ion_flux = _sgnip[_qp] * _muip[_qp] * -_grad_potential[_qp] * std::exp(_ip[_qp]) -
              _Dip[_qp] * std::exp(_ip[_qp]) * _grad_ip[_qp];

  return 0.;
}

Real
SecondaryElectronBC::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (_normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0)
    _a = 1.0;
  else
    _a = 0.0;

  if (jvar == _potential_id)
  {
    _d_ion_flux_d_potential = _sgnip[_qp] * _muip[_qp] * -_grad_phi[_j][_qp] * std::exp(_ip[_qp]);

    return -_test[_i][_qp] * 2. / (1. + _r) * (1. - _a) * _se_coeff[_qp] * _d_ion_flux_d_potential *
           _normals[_qp];
  }

  else if (jvar == _mean_en_id)
  {
    _v_thermal = std::sqrt(8 * _e[_qp] * 2.0 / 3 * std::exp(_mean_en[_qp] - _u[_qp]) /
                           (M_PI * _massem[_qp]));
    _d_v_thermal_d_mean_en = 0.5 / _v_thermal * 8 * _e[_qp] * 2.0 / 3 *
                             std::exp(_mean_en[_qp] - _u[_qp]) / (M_PI * _massem[_qp]) *
                             _phi[_j][_qp];
    _actual_mean_en = std::exp(_mean_en[_qp] - _u[_qp]);

    return 0.;
  }

  else if (jvar == _ip_id)
  {
    _d_ion_flux_d_ip =
        _sgnip[_qp] * _muip[_qp] * -_grad_potential[_qp] * std::exp(_ip[_qp]) * _phi[_j][_qp] -
        _Dip[_qp] * std::exp(_ip[_qp]) * _grad_phi[_j][_qp] -
        _Dip[_qp] * std::exp(_ip[_qp]) * _phi[_j][_qp] * _grad_ip[_qp];

    return -_test[_i][_qp] * 2. / (1. + _r) * (1. - _a) * _se_coeff[_qp] * _d_ion_flux_d_ip *
           _normals[_qp];
  }

  else
    return 0.0;
}
