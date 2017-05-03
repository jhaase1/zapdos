
#ifndef LOGSTABILIZATIONMOLES_H
#define LOGSTABILIZATIONMOLES_H

#include "Kernel.h"

class LogStabilizationMoles;

template <>
InputParameters validParams<LogStabilizationMoles>();

class LogStabilizationMoles : public Kernel
{
public:
  LogStabilizationMoles(const InputParameters & parameters);
  virtual ~LogStabilizationMoles();

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

  Real _offset;
};

#endif /* LOGSTABILIZATIONMOLES_H */
