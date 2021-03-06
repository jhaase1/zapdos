#include "ArbitrarilyTiedValueConstraint.h"

// MOOSE includes
#include "MooseVariable.h"
#include "SystemBase.h"

// libmesh includes
#include "libmesh/sparse_matrix.h"

template <>
InputParameters
validParams<ArbitrarilyTiedValueConstraint>()
{
  InputParameters params = validParams<NodeFaceConstraint>();
  params.addParam<Real>("scaling", 1, "scaling factor to be applied to constraint equations");
  params.addRequiredParam<Real>("H", "The value of u_slave/u_master.");
  params.set<bool>("use_displaced_mesh") = true;
  return params;
}

ArbitrarilyTiedValueConstraint::ArbitrarilyTiedValueConstraint(const InputParameters & parameters)
  : NodeFaceConstraint(parameters),
    _scaling(getParam<Real>("scaling")),
    _H(getParam<Real>("H")),
    _residual_copy(_sys.residualGhosted())
{
}

Real
ArbitrarilyTiedValueConstraint::computeQpSlaveValue()
{
  return _H * _u_master[_qp];
}

Real
ArbitrarilyTiedValueConstraint::computeQpResidual(Moose::ConstraintType type)
{
  Real scaling_factor = _var.scalingFactor();
  Real slave_resid = 0;
  Real retVal = 0;
  switch (type)
  {
    case Moose::Slave:
      retVal = (_u_slave[_qp] - _H * _u_master[_qp]) * _test_slave[_i][_qp] * _scaling;
      break;
    case Moose::Master:
      slave_resid = _residual_copy(_current_node->dof_number(0, _var.number(), 0)) / scaling_factor;
      retVal = slave_resid * _test_master[_i][_qp];
      break;
    default:
      break;
  }
  return retVal;
}

Real
ArbitrarilyTiedValueConstraint::computeQpJacobian(Moose::ConstraintJacobianType type)
{
  Real scaling_factor = _var.scalingFactor();
  Real slave_jac = 0;
  Real retVal = 0;
  switch (type)
  {
    case Moose::SlaveSlave:
      retVal = _phi_slave[_j][_qp] * _test_slave[_i][_qp] * _scaling;
      break;
    case Moose::SlaveMaster:
      retVal = -_H * _phi_master[_j][_qp] * _test_slave[_i][_qp] * _scaling;
      break;
    case Moose::MasterSlave:
      slave_jac =
          (*_jacobian)(_current_node->dof_number(0, _var.number(), 0), _connected_dof_indices[_j]);
      retVal = slave_jac * _test_master[_i][_qp] / scaling_factor;
      break;
    case Moose::MasterMaster:
      retVal = 0;
      break;
  }
  return retVal;
}
