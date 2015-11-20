/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*           (c) 2010 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#include "IonsFromIonization.h"


template<>
InputParameters validParams<IonsFromIonization>()
{
  InputParameters params = validParams<Kernel>();
  params.addRequiredCoupledVar("mean_en","The electron mean energy.");
  params.addRequiredCoupledVar("potential","The potential.");
  params.addRequiredCoupledVar("em", "The electron density.");
  return params;
}


IonsFromIonization::IonsFromIonization(const InputParameters & parameters) :
  Kernel(parameters),
    
  _diffem(getMaterialProperty<Real>("diffem")),
  _muem(getMaterialProperty<Real>("muem")),
  _alpha_iz(getMaterialProperty<Real>("alpha_iz")),
  _d_iz_d_actual_mean_en(getMaterialProperty<Real>("d_iz_d_actual_mean_en")),
  _d_muem_d_actual_mean_en(getMaterialProperty<Real>("d_muem_d_actual_mean_en")),
  _d_diffem_d_actual_mean_en(getMaterialProperty<Real>("d_diffem_d_actual_mean_en")),

  _mean_en(coupledValue("mean_en")),
  _grad_potential(coupledGradient("potential")),
  _em(coupledValue("em")),
  _grad_em(coupledGradient("em")),
  _mean_en_id(coupled("mean_en")),
  _potential_id(coupled("potential")),
  _em_id(coupled("em"))
{
}

IonsFromIonization::~IonsFromIonization()
{
}

Real
IonsFromIonization::computeQpResidual()
{
  Real electron_flux_mag = (-_muem[_qp]*-_grad_potential[_qp]*std::exp(_em[_qp])-_diffem[_qp]*std::exp(_em[_qp])*_grad_em[_qp]).size();
  Real iz_term = _alpha_iz[_qp] * electron_flux_mag;

  return -_test[_i][_qp]*iz_term;
}

Real
IonsFromIonization::computeQpJacobian()
{
  return 0.0;
}

Real
IonsFromIonization::computeQpOffDiagJacobian(unsigned int jvar)
{
  Real actual_mean_en = std::exp(_mean_en[_qp]-_em[_qp]);
  Real d_actual_mean_en_d_mean_en = std::exp(_mean_en[_qp]-_em[_qp])*_phi[_j][_qp];
  Real d_actual_mean_en_d_em = -std::exp(_mean_en[_qp]-_em[_qp])*_phi[_j][_qp];
  Real d_iz_d_mean_en = _d_iz_d_actual_mean_en[_qp] * d_actual_mean_en_d_mean_en;
  Real d_iz_d_em = _d_iz_d_actual_mean_en[_qp] * d_actual_mean_en_d_em;
  Real d_muem_d_mean_en = _d_muem_d_actual_mean_en[_qp] * d_actual_mean_en_d_mean_en;
  Real d_diffem_d_mean_en = _d_diffem_d_actual_mean_en[_qp] * d_actual_mean_en_d_mean_en;
  Real d_muem_d_em = _d_muem_d_actual_mean_en[_qp] * d_actual_mean_en_d_em;
  Real d_diffem_d_em = _d_diffem_d_actual_mean_en[_qp] * d_actual_mean_en_d_em;

  RealVectorValue electron_flux = -_muem[_qp]*-_grad_potential[_qp]*std::exp(_em[_qp])-_diffem[_qp]*std::exp(_em[_qp])*_grad_em[_qp];
  RealVectorValue d_electron_flux_d_potential = -_muem[_qp]*-_grad_phi[_j][_qp]*std::exp(_em[_qp]);
  RealVectorValue d_electron_flux_d_mean_en = -d_muem_d_mean_en*-_grad_potential[_qp]*std::exp(_em[_qp])-d_diffem_d_mean_en*std::exp(_em[_qp])*_grad_em[_qp];
  RealVectorValue d_electron_flux_d_em = -d_muem_d_em*-_grad_potential[_qp]*std::exp(_em[_qp])-_muem[_qp]*-_grad_potential[_qp]*std::exp(_em[_qp])*_phi[_j][_qp]-d_diffem_d_em*std::exp(_em[_qp])*_grad_em[_qp]-_diffem[_qp]*std::exp(_em[_qp])*_phi[_j][_qp]*_grad_em[_qp]-_diffem[_qp]*std::exp(_em[_qp])*_grad_phi[_j][_qp];
  Real electron_flux_mag = electron_flux.size();
  Real d_electron_flux_mag_d_potential = electron_flux*d_electron_flux_d_potential/(electron_flux_mag+std::numeric_limits<double>::epsilon());
  Real d_electron_flux_mag_d_mean_en = electron_flux*d_electron_flux_d_mean_en/(electron_flux_mag+std::numeric_limits<double>::epsilon());
  Real d_electron_flux_mag_d_em = electron_flux*d_electron_flux_d_em/(electron_flux_mag+std::numeric_limits<double>::epsilon());

  Real d_iz_term_d_potential = (_alpha_iz[_qp] * d_electron_flux_mag_d_potential);
  Real d_iz_term_d_mean_en = (electron_flux_mag * d_iz_d_mean_en + _alpha_iz[_qp] * d_electron_flux_mag_d_mean_en);
  Real d_iz_term_d_em = (electron_flux_mag * d_iz_d_em + _alpha_iz[_qp] * d_electron_flux_mag_d_em);

  if (jvar == _potential_id)
    return -_test[_i][_qp]*d_iz_term_d_potential;

  else if (jvar == _mean_en_id)
    return -_test[_i][_qp]*d_iz_term_d_mean_en;

  else if (jvar == _em_id)
    return -_test[_i][_qp] * d_iz_term_d_em;
  
  else
    return 0.0;
}