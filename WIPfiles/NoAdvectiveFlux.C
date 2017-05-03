
#include "NoDiffusiveFlux.h"

template <>
InputParameters
validParams<NoDiffusiveFlux>()
{
  InputParameters params = validParams<IntegratedBC>();
  return params;
}

NoDiffusiveFlux::NoDiffusiveFlux(const std::string & name, InputParameters parameters)
  : IntegratedBC(name, parameters),

    // Material Properties

    _velocity(getMaterialProperty<RealVectorValue>("velocity"))
{
}

Real
NoDiffusiveFlux::computeQpResidual()
{
  return _test[_i][_qp] * _u[_qp] * _velocity[_qp] * _normals[_qp];
}

Real
NoDiffusiveFlux::computeQpJacobian()
{
  return _test[_i][_qp] * _phi[_j][_qp] * _velocity[_qp] * _normals[_qp];
}
