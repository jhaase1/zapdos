#include "NeumannCircuitVoltageNew.h"
#include "MooseMesh.h"
#include "Function.h"

template <>
InputParameters
validParams<NeumannCircuitVoltageNew>()
{
  InputParameters params = validParams<NonlocalIntegratedBC>();
  params.addRequiredParam<UserObjectName>(
      "current",
      "The postprocessor response for calculating the current passing through the needle surface.");
  params.addRequiredParam<FunctionName>(
      "source_voltage",
      "The electrical potential applied to the surface if no current was flowing in the circuit.");
  params.addRequiredParam<std::string>("surface",
                                       "Whether you are specifying the potential on the "
                                       "anode or the cathode with the requirement that "
                                       "the other metal surface be grounded.");
  //	params.addRequiredParam<UserObjectName>("data_provider", "The name of the UserObject that can
  // provide some data to materials, bcs, etc.");
  params.addRequiredCoupledVar("em", "The electron variable.");
  params.addRequiredCoupledVar("ip", "The ion variable.");
  params.addRequiredCoupledVar("mean_en", "The ion variable.");
  params.addParam<Real>("area", "Must be provided when the number of dimensions equals 1.");
  params.addRequiredParam<std::string>("potential_units", "The potential units.");
  params.addParam<Real>("position_units", 1, "The units of position.");
	params.addParam<Real>("time_units", 1, "The units of time.");
  params.addRequiredParam<Real>("resistance", "The ballast resistance in Ohms.");
  return params;
}

NeumannCircuitVoltageNew::NeumannCircuitVoltageNew(const InputParameters & parameters)
  : NonlocalIntegratedBC(parameters),
    _current_uo(getUserObject<CurrentDensityShapeSideUserObject>("current")),
    _current(_current_uo.getIntegral()),
    _current_jac(_current_uo.getJacobian()),
    _source_voltage(getFunction("source_voltage")),
    _surface(getParam<std::string>("surface")),

    //_data(getUserObject<ProvideMobility>("data_provider")),
    _var_dofs(_var.dofIndices()),
    _em_id(coupled("em")),
    _em_dofs(getVar("em", 0)->dofIndices()),
    _ip_id(coupled("ip")),
    _ip_dofs(getVar("ip", 0)->dofIndices()),
    _mean_en_id(coupled("mean_en")),
    _mean_en_dofs(getVar("mean_en", 0)->dofIndices()),
		
    _r_units(1. / getParam<Real>("position_units")),
    _t_units(1. / getParam<Real>("time_units")),
    _resistance(getParam<Real>("resistance"))
{
  if (_surface.compare("anode") == 0)
    _current_sign = -1.;
  else if (_surface.compare("cathode") == 0)
    _current_sign = 1.;

  if (_mesh.dimension() == 1 && isParamValid("area"))
  {
    _area = getParam<Real>("area");
    _use_area = true;
  }
  else if (_mesh.dimension() == 1 && !(isParamValid("area")))
    mooseError("In a one-dimensional simulation, the area parameter must be set.");
  else
    _use_area = false;

  if (getParam<std::string>("potential_units").compare("V") == 0)
    _voltage_scaling = 1.;
  else if (getParam<std::string>("potential_units").compare("kV") == 0)
    _voltage_scaling = 1000;
  else
    mooseError("Potential specified must be either 'V' or 'kV'.");
}

Real
NeumannCircuitVoltageNew::computeQpResidual()
{
  Real curr_times_resist = _current_sign * _current * _resistance / (_t_units * _voltage_scaling);
  if (_use_area)
    curr_times_resist *= _area;

  return _test[_i][_qp] * _r_units * (_source_voltage.value(_t, _q_point[_qp]) - _u[_qp] + curr_times_resist);
}

Real
NeumannCircuitVoltageNew::computeQpJacobian()
{
  Real d_curr_times_resist_d_potential =
      _current_sign * _current_jac[_var_dofs[_j]] * _resistance / (_t_units * _voltage_scaling);
  if (_use_area)
    d_curr_times_resist_d_potential *= _area;

  return _test[_i][_qp] * _r_units * (-_phi[_j][_qp] + d_curr_times_resist_d_potential);
}

Real
NeumannCircuitVoltageNew::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (jvar == _em_id)
  {
    Real d_curr_times_resist_d_em =
        _current_sign * _current_jac[_em_dofs[_j]] * _resistance / (_t_units * _voltage_scaling);
    if (_use_area)
      d_curr_times_resist_d_em *= _area;

    return _test[_i][_qp] * _r_units * d_curr_times_resist_d_em;
  }

  else if (jvar == _ip_id)
  {
    Real d_curr_times_resist_d_ip =
        _current_sign * _current_jac[_ip_dofs[_j]] * _resistance / (_t_units * _voltage_scaling);
    if (_use_area)
      d_curr_times_resist_d_ip *= _area;

    return _test[_i][_qp] * _r_units * d_curr_times_resist_d_ip;
  }

  else if (jvar == _mean_en_id)
  {
    Real d_curr_times_resist_d_mean_en = _current_sign * _current_jac[_mean_en_dofs[_j]]
                                         * _resistance / (_t_units * _voltage_scaling);
    if (_use_area)
      d_curr_times_resist_d_mean_en *= _area;

    return _test[_i][_qp] * _r_units * d_curr_times_resist_d_mean_en;
  }

  else
    return 0;
}

Real
NeumannCircuitVoltageNew::computeQpNonlocalJacobian(dof_id_type dof_index)
{
  Real d_curr_times_resist_d_potential =
      _current_sign * _current_jac[dof_index] * _resistance / (_t_units * _voltage_scaling);
  if (_use_area)
    d_curr_times_resist_d_potential *= _area;

  return _test[_i][_qp] * _r_units * d_curr_times_resist_d_potential;
}

Real
NeumannCircuitVoltageNew::computeQpNonlocalOffDiagJacobian(unsigned int jvar, dof_id_type dof_index)
{
  if (jvar == _em_id || jvar == _ip_id || jvar == _mean_en_id)
  {
    Real d_curr_times_resist_d_coupled_var =
        _current_sign * _current_jac[dof_index] * _resistance / (_t_units * _voltage_scaling);
    if (_use_area)
      d_curr_times_resist_d_coupled_var *= _area;

    return _test[_i][_qp] * _r_units * d_curr_times_resist_d_coupled_var;
  }

  return 0;
}
