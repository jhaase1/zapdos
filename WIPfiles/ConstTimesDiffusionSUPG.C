
#include "ConstTimesDiffusionSUPG.h"

template <>
InputParameters
validParams<ConstTimesDiffusionSUPG>()
{
  // Start with the parameters from our parent
  InputParameters params = validParams<Diffusion>();

  // No parameters are necessary here because we're going to get
  // permeability and viscosity from the Material
  // so we just return params...

  return params;
}

ConstTimesDiffusionSUPG::ConstTimesDiffusionSUPG(const InputParameters & parameters)
  : Diffusion(parameters),

    _alpha(getMaterialProperty<Real>("alpha")),
    _velocity_norm(getMaterialProperty<RealVectorValue>("velocity_norm")),
    _diffusivity(getMaterialProperty<Real>("diffusivity"))

{
}

ConstTimesDiffusionSUPG::~ConstTimesDiffusionSUPG() {}

Real
ConstTimesDiffusionSUPG::computeQpResidual()
{
  // Use the MaterialProperty references we stored earlier
  return _diffusivity[_qp] * Diffusion::computeQpResidual();
}

Real
ConstTimesDiffusionSUPG::computeQpJacobian()
{
  // Use the MaterialProperty references we stored earlier
  return _diffusivity[_qp] * Diffusion::computeQpJacobian();
}
