#ifndef SCHOTTKYEMISSIONNEWBC_H
#define SCHOTTKYEMISSIONNEWBC_H

#include "IntegratedBC.h"

class SchottkyEmissionNewBC;

template <>
InputParameters validParams<SchottkyEmissionNewBC>();

class SchottkyEmissionNewBC : public IntegratedBC
{
public:
  SchottkyEmissionNewBC(const InputParameters & parameters);

protected:
  Real emission_flux();
  Real d_emission_flux_d_em();
  Real d_emission_flux_d_potential();
  Real d_emission_flux_d_mean_en();
  Real d_emission_flux_d_ip();

  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  bool _use_moles;
  Real _r_units;
  Real _t_units;
  Real _r;

  // Coupled variables

  const VariableGradient & _grad_potential;
  unsigned int _potential_id;

	const VariableValue & _em;
	unsigned int _em_id;
	
  const VariableValue & _mean_en;
  unsigned int _mean_en_id;

  const MaterialProperty<Real> & _muem;
  const MaterialProperty<Real> & _d_muem_d_actual_mean_en;
  const MaterialProperty<Real> & _massem;
  const MaterialProperty<Real> & _e;
  const MaterialProperty<Real> & _eps;
  const MaterialProperty<Real> & _N_A;
//	const MaterialProperty<Real> & _k_boltz;

  const MaterialProperty<Real> & _se_energy;

  Real _work_function;
  Real _field_enhancement;
  Real _Richardson_coefficient;
  Real _temperature;

  Real _a;

  Real _actual_mean_en;
  Real _tau;
  bool _relax;

  std::string _potential_units;

  // Unique variables
  Real _voltage_scaling;
  Real _dPhi_over_F;
	Real kB;

  // System variables
  Real _relaxation_expr;

  Real _v_thermal;
  Real _d_v_thermal_d_u;

  Real _v_drift;
  Real _d_v_drift_d_u;

  Real _emission_flux;
  Real _d_emission_flux_d_u;

  Real _n_emitted;
};

#endif // SCHOTTKYEMISSIONNEWBC_H
