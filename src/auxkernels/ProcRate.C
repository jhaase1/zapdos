#include "ProcRate.h"

template <>
InputParameters
validParams<ProcRate>()
{
  InputParameters params = validParams<AuxKernel>();

  params.addRequiredCoupledVar("em", "The electron density");
  params.addRequiredCoupledVar("potential", "The potential");
  params.addRequiredParam<std::string>(
      "proc",
      "The process that we want to get the townsend coefficient for. Options are iz, ex, and el.");
  params.addParam<Real>("position_units", 1, "Units of position.");
  params.addParam<Real>("time_units", 1, "Units of time.");
  params.addRequiredParam<bool>("use_moles",
                                "Whether to use units of moles as opposed to # of molecules.");
  return params;
}

ProcRate::ProcRate(const InputParameters & parameters)
  : AuxKernel(parameters),

    _use_moles(getParam<bool>("use_moles")),
    _r_units(1. / getParam<Real>("position_units")),
    _t_units(1. / getParam<Real>("time_units")),

    _e(getMaterialProperty<Real>("e")),
    _N_A(getMaterialProperty<Real>("N_A")),
    _em(coupledValue("em")),
    _grad_em(coupledGradient("em")),
    _grad_potential(coupledGradient("potential")),
    _muem(getMaterialProperty<Real>("muem")),
    _sgnem(getMaterialProperty<Real>("sgnem")),
    _diffem(getMaterialProperty<Real>("diffem")),
    _alpha(getMaterialProperty<Real>("alpha_" + getParam<std::string>("proc"))),
    _current(0, 0, 0)
{
}

Real
ProcRate::computeValue()
{
  _current = (_sgnem[_qp] * _muem[_qp] * -_grad_potential[_qp] * std::exp(_em[_qp]) -
              _diffem[_qp] * std::exp(_em[_qp]) * _grad_em[_qp]);

  if (_use_moles)
  {
    _current *= _N_A[_qp];
  }

  _current *= (_t_units / _r_units);

  return _alpha[_qp] * _current.norm();
}
