#ifndef NEUMANNCIRCUITVOLTAGENEW_H
#define NEUMANNCIRCUITVOLTAGENEW_H

#include "NonlocalIntegratedBC.h"
#include "CurrentDensityShapeSideUserObject.h"
#include "ProvideMobility.h"

class NeumannCircuitVoltageNew;

template <>
InputParameters validParams<NeumannCircuitVoltageNew>();

/**
 * Boundary condition of a Dirichlet type
 *
 * Sets the value at the node to the value of a Postprocessor
 */
class NeumannCircuitVoltageNew : public NonlocalIntegratedBC
{
public:
  NeumannCircuitVoltageNew(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);
  virtual Real computeQpNonlocalJacobian(dof_id_type dof_index);
  virtual Real computeQpNonlocalOffDiagJacobian(unsigned int jvar, dof_id_type dof_index);

  const CurrentDensityShapeSideUserObject & _current_uo;
  const Real & _current;
  const std::vector<Real> & _current_jac;
  Function & _source_voltage;
  std::string _surface;
  Real _current_sign;
  const ProvideMobility & _data;
  const std::vector<dof_id_type> & _var_dofs;
  unsigned int _em_id;
  const std::vector<dof_id_type> & _em_dofs;
  unsigned int _ip_id;
  const std::vector<dof_id_type> & _ip_dofs;
  unsigned int _mean_en_id;
  const std::vector<dof_id_type> & _mean_en_dofs;
  Real _r_units;
  Real _resistance;
  Real _area;
  bool _use_area;
  Real _voltage_scaling;
};

#endif /* NEUMANNCIRCUITVOLTAGENEW_H */
