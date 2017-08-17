#ifndef SCHOTTKYEMISSIONPOSTPROCESSOR_H
#define SCHOTTKYEMISSIONPOSTPROCESSOR_H

// MOOSE includes
#include "SideIntegralVariablePostprocessor.h"

// Forward Declarations
class SchottkyEmissionPostProcessor;

template<>
InputParameters validParams<SchottkyEmissionPostProcessor>();

/**
 * This postprocessor computes a side integral of the mass flux.
 */
class SchottkyEmissionPostProcessor : public SideIntegralVariablePostprocessor
{
public:
  SchottkyEmissionPostProcessor(const InputParameters & params);

protected:
  virtual Real computeQpIntegral();
	Real emission_current();

	Real _use_moles;
	Real _r_units;
	Real _t_units;
	std::string _potential_units;
	Real _r;

	// Coupled Variables
	const VariableGradient & _grad_potential;
	
	MooseVariable & _ip_var;
	const VariableValue & _ip;
	const VariableGradient & _grad_ip;
	
	const MaterialProperty<Real> & _sgnip;
	const MaterialProperty<Real> & _muip;
	const MaterialProperty<Real> & _Dip;
	const MaterialProperty<Real> & _se_coeff;

	const MaterialProperty<Real> & _e;
	const MaterialProperty<Real> & _N_A;
	
	const MaterialProperty<Real> & _work_function;
	const MaterialProperty<Real> & _field_enhancement;
	const MaterialProperty<Real> & _Richardson_coefficient;
	const MaterialProperty<Real> & _cathode_temperature;

	Real _a;
	
	RealVectorValue _ion_flux;

	// Unique variables
	Real _voltage_scaling;
	Real _dPhi_over_F;
};

#endif // SCHOTTKYEMISSIONPOSTPROCESSOR_H
