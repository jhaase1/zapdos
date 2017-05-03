#ifndef SCHOTTKYEMISSIONBC_H
#define SCHOTTKYEMISSIONBC_H

#include "IntegratedBC.h"

class SchottkyEmissionBC;

template <>
InputParameters validParams<SchottkyEmissionBC>();

class SchottkyEmissionBC : public IntegratedBC
{
public:
  SchottkyEmissionBC(const InputParameters & parameters);

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

  const VariableValue & _mean_en;
  unsigned int _mean_en_id;
  MooseVariable & _ip_var;
  const VariableValue & _ip;
  const VariableGradient & _grad_ip;
  unsigned int _ip_id;

  const MaterialProperty<Real> & _muem;
  const MaterialProperty<Real> & _d_muem_d_actual_mean_en;
  const MaterialProperty<Real> & _massem;
  const MaterialProperty<Real> & _e;
  const MaterialProperty<Real> & _eps;
  const MaterialProperty<Real> & _N_A;
  const MaterialProperty<Real> & _sgnip;
  const MaterialProperty<Real> & _muip;
  const MaterialProperty<Real> & _Dip;
  const MaterialProperty<Real> & _se_coeff;
  const MaterialProperty<Real> & _se_energy;

  const MaterialProperty<Real> & _work_function;
  const MaterialProperty<Real> & _field_enhancement;
  const MaterialProperty<Real> & _Richardson_coefficient;
  const MaterialProperty<Real> & _cathode_temperature;

  Real _a;

  RealVectorValue _ion_flux;

  RealVectorValue _d_ion_flux_d_potential;
  RealVectorValue _d_ion_flux_d_ip;

  Real _actual_mean_en;
  Real _tau;
  bool _relax;

  std::string _potential_units;

  // Unique variables
  Real _voltage_scaling;
  Real _dPhi_over_F;

  // System variables
  Real _relaxation_expr;

  Real _v_thermal;
  Real _d_v_thermal_d_u;

  Real _v_drift;
  Real _d_v_drift_d_u;

  Real _emission_flux;
  Real _d_emission_flux_d_u;

  Real _n_emitted;
  Real _d_n_emitted_d_u;

  Real _n_e_transported;
  Real _d_n_e_transported_d_u;

  Real _n_transported;
  Real _d_n_transported_d_u;
};

#endif // SCHOTTKYEMISSIONBC_H
