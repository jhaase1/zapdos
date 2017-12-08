resistance = 1 #Ohms
area = 1E-4 # Formerly 3.14e-6

[GlobalParams]
#	offset = 25
	time_units = 1E-9
	position_units = 1E-6
	potential_units = kV
#	 potential_units = V
	use_moles = true
#	use_moles = false
[]

[Mesh]
	type = GeneratedMesh	# Can generate simple lines, rectangles and rectangular prisms
	dim = 1								# Dimension of the mesh
	nx = 5000							# Number of elements in the x direction
	xmax = 10				# Length of test chamber
[]

[Problem]
	type = FEProblem
[]

[Preconditioning]
	[./smp]
		type = SMP
		full = true
	[../]
[]

[Executioner]
	type = Transient
	end_time = 1E9

#	[./TimeIntegrator]
#		type = ImplicitEuler #AStableDirk4 #CrankNicolson #ImplicitMidpoint #AStableDirk4 #CrankNicolson #ImplicitEuler
#	[../]

	trans_ss_check = 1
	ss_check_tol = 1E-15
	ss_tmin = 1000

	petsc_options = '-snes_ksp_ew -superlu_dist' # -snes_converged_reason -snes_linesearch_monitor'
	solve_type = NEWTON

#	petsc_options_iname = '-pc_type -pc_factor_mat_solver_package -pc_factor_shift_type -pc_factor_shift_amount -ksp_type -snes_linesearch_minlambda -ksp_gmres_restart'
#	petsc_options_value = 'lu mumps NONZERO 1.e-10 preonly 1e-3 100'

	petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
	petsc_options_value = 'lu superlu_dist'

#	petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
#	petsc_options_value = 'asm lu'

	nl_rel_tol = 1E-3
	nl_abs_tol = 1E-11

	dtmin = 1E-7
	nl_max_its = 40
	[./TimeStepper]
		type = IterationAdaptiveDT
		dt = 0.004096
		cutback_factor = 0.8
		growth_factor = 1.2
		optimal_iterations = 25
	[../]
[]

[Debug]
	show_var_residual_norms = false
[]

[Outputs]
	print_perf_log = true
	print_linear_residuals = false
	console = true

	[./out]
		type = Exodus
		execute_on = 'final'
	[../]
	
	[./checkpoint]
		type = Checkpoint
		execute_on = 'final'
		num_files = 2
	[../]
[]

[Postprocessors]
	[./Full_EmissionCurrent]
		type = SchottkyEmissionPostProcessor
		boundary = left
		#r = 1
		variable = em
		potential = potential
		ip = Arp
	[../]
	[./Native_EmissionCurrent]
		type = SchottkyEmissionPostProcessor
		boundary = left
		#r = 1
		variable = em
		potential = native_potential
		ip = Arp
	[../]
[]

[Variables]
	[./potential]
	[../]
	[./native_potential]
	[../]
	[./em]
		block = 0
		initial_condition = -25
	[../]
	[./Arp]
		block = 0
		initial_condition = -25
	[../]
	[./mean_en]
		block = 0
		initial_condition = -25
	[../]
[]

[Kernels]
## Poisson's equation
	[./potential_diffusion_dom1]
		type = CoeffDiffusionLin
		variable = potential
		block = 0
	[../]

	[./Arp_charge_source]
		type = ChargeSourceMoles_KV
		variable = potential
		charged = Arp
		block = 0
	[../]
	[./em_charge_source]
		type = ChargeSourceMoles_KV
		variable = potential
		charged = em
		block = 0
	[../]

## Native Poisson's equation
	[./native_potential_diffusion_dom1]
		type = CoeffDiffusionLin
		variable = native_potential
		block = 0
	[../]
	[./Native_Poisson_RHS]
		type = SetRHS
		variable = native_potential
		value = 1E-15
		block = 0
	[../]

## Electron
	[./em_time_deriv]
		type = ElectronTimeDerivative
		variable = em
		block = 0
	[../]
	[./em_advection]
		type = EFieldAdvectionElectrons
		variable = em
		potential = potential
		mean_en = mean_en
		block = 0
	[../]
	[./em_diffusion]
		type = CoeffDiffusionElectrons
		variable = em
		mean_en = mean_en
		block = 0
	[../]
	[./em_ionization]
		type = ElectronsFromIonization
		variable = em
		em = em
		potential = potential
		mean_en = mean_en
		block = 0
	[../]

## Ion
	[./Arp_time_deriv]
		type = ElectronTimeDerivative
		variable = Arp
		block = 0
	[../]
	[./Arp_advection]
		type = EFieldAdvection
		variable = Arp
		potential = potential
		block = 0
	[../]
	[./Arp_diffusion]
		type = CoeffDiffusion
		variable = Arp
		block = 0
	[../]
	[./Arp_ionization]
		type = IonsFromIonization
		variable = Arp
		potential = potential
		em = em
		mean_en = mean_en
		block = 0
	[../]

## Mean energy
	[./mean_en_time_deriv]
		type = ElectronTimeDerivative
		variable = mean_en
		block = 0
	[../]
	[./mean_en_advection]
		type = EFieldAdvectionEnergy
		variable = mean_en
		potential = potential
		em = em
		block = 0
	[../]
	[./mean_en_diffusion]
		type = CoeffDiffusionEnergy
		variable = mean_en
		em = em
		block = 0
	[../]
	[./mean_en_joule_heating]
		type = JouleHeating
		variable = mean_en
		potential = potential
		em = em
		block = 0
	[../]
	[./mean_en_elastic]
		type = ElectronEnergyLossFromElastic
		variable = mean_en
		potential = potential
		em = em
		block = 0
	[../]
	[./mean_en_excitation]
		type = ElectronEnergyLossFromExcitation
		variable = mean_en
		potential = potential
		em = em
		block = 0
	[../]
	[./mean_en_ionization]
		type = ElectronEnergyLossFromIonization
		variable = mean_en
		potential = potential
		em = em
		block = 0
	[../]


## Stabilization
#	[./Arp_log_stabilization]
#		type = LogStabilizationMoles
#		variable = Arp
#		offset = 20
#		block = 0
#	[../]
#	[./em_log_stabilization]
#		type = LogStabilizationMoles
#		variable = em
#		offset = 20
#		block = 0
#	[../]
#	[./mean_en_log_stabilization]
#		type = LogStabilizationMoles
#		variable = mean_en
#		block = 0
#		offset = 35
#	[../]
#	[./mean_en_advection_stabilization]
#		type = EFieldArtDiff
#		variable = mean_en
#		potential = potential
#		block = 0
#	[../]
[]

[AuxVariables]
	[./x]
		order = CONSTANT
		family = MONOMIAL
	[../]
	[./Voltage]
		order = CONSTANT
		family = MONOMIAL
	[../]
	[./Efield]
		order = CONSTANT
		family = MONOMIAL
	[../]
	[./em_lin]
		order = CONSTANT
		family = MONOMIAL
		block = 0
	[../]
	[./Arp_lin]
		order = CONSTANT
		family = MONOMIAL
		block = 0
	[../]

	[./Current_em]
		order = CONSTANT
		family = MONOMIAL
		block = 0
	[../]
	[./Current_Arp]
		order = CONSTANT
		family = MONOMIAL
		block = 0
	[../]
	[./tot_gas_current]
		order = CONSTANT
		family = MONOMIAL
		block = 0
	[../]

	[./e_temp]
		block = 0
		order = CONSTANT
		family = MONOMIAL
	[../]
#	[./x_node]
#	[../]
	[./rho]
		order = CONSTANT
		family = MONOMIAL
		block = 0
	[../]
	[./EFieldAdvAux_em]
		order = CONSTANT
		family = MONOMIAL
		block = 0
	[../]
	[./DiffusiveFlux_em]
		order = CONSTANT
		family = MONOMIAL
		block = 0
	[../]
	[./PowerDep_em]
		order = CONSTANT
		family = MONOMIAL
		block = 0
	[../]
	[./PowerDep_Arp]
		order = CONSTANT
		family = MONOMIAL
		block = 0
	[../]

	[./PowerDepProvided_em]
		order = CONSTANT
		family = MONOMIAL
		block = 0
	[../]
	[./PowerDepProvided_Arp]
		order = CONSTANT
		family = MONOMIAL
		block = 0
	[../]

	[./ProcRate_el]
		order = CONSTANT
		family = MONOMIAL
		block = 0
	[../]
	[./ProcRate_ex]
		order = CONSTANT
		family = MONOMIAL
		block = 0
	[../]
	[./ProcRate_iz]
		order = CONSTANT
		family = MONOMIAL
		block = 0
	[../]
	
	[./Alpha_el]
		order = CONSTANT
		family = MONOMIAL
		block = 0
	[../]
	[./Alpha_ex]
		order = CONSTANT
		family = MONOMIAL
		block = 0
	[../]
	[./Alpha_iz]
		order = CONSTANT
		family = MONOMIAL
		block = 0
	[../]

	[./Emission_energy_flux]
		order = CONSTANT
		family = MONOMIAL
		block = 0
	[../]
[]

[AuxKernels]
## Cell AuxKernels
	[./x_g]
		type = Position
		variable = x
		block = 0
	[../]
	[./Voltage]
		type = Potential
		variable = Voltage
		potential = potential
		block = 0
	[../]

	[./Efield_g]
		type = Efield
		component = 0
		potential = potential
		variable = Efield
		block = 0
	[../]

	[./em_lin]
		type = DensityMoles
		variable = em_lin
		density_log = em
		convert_units = true
		block = 0
	[../]
	[./Arp_lin]
		type = DensityMoles
		variable = Arp_lin
		density_log = Arp
		convert_units = true
		block = 0
	[../]

	[./rho]
		type = ParsedAux
		variable = rho
		args = 'em_lin Arp_lin'
		function = 'Arp_lin - em_lin'
		execute_on = 'timestep_end'
		block = 0
	[../]

	[./Current_em]
		type = Current
		component = 0
		potential = potential
		density_log = em
		variable = Current_em
		art_diff = false
		block = 0
	[../]
	[./Current_Arp]
		type = Current
		component = 0
		potential = potential
		density_log = Arp
		variable = Current_Arp
		art_diff = false
		block = 0
	[../]
	[./tot_gas_current]
		type = ParsedAux
		variable = tot_gas_current
		args = 'Current_em Current_Arp'
		function = 'Current_em + Current_Arp'
		execute_on = 'timestep_end'
		block = 0
	[../]

	[./e_temp]
		type = ElectronTemperature
		variable = e_temp
		electron_density = em
		mean_en = mean_en
		block = 0
	[../]

	[./PowerDep_em]
		type = PowerDep
		density_log = em
		potential = potential
		art_diff = false
		variable = PowerDep_em
		block = 0
	[../]
	[./PowerDep_Arp]
		type = PowerDep
		density_log = Arp
		potential = potential
		art_diff = false
		variable = PowerDep_Arp
		block = 0
	[../]

	[./PowerDepProvided_em]
		type = PowerDep
		density_log = em
		potential = native_potential
		art_diff = false
		power_used = true
		variable = PowerDepProvided_em
		block = 0
	[../]
	[./PowerDepProvided_Arp]
		type = PowerDep
		density_log = Arp
		potential = native_potential
		art_diff = false
		power_used = true
		variable = PowerDepProvided_Arp
		block = 0
	[../]

	[./ProcRate_el]
		type = ProcRate
		em = em
		potential = potential
		proc = el
		variable = ProcRate_el
		block = 0
	[../]
	[./ProcRate_ex]
		type = ProcRate
		em = em
		potential = potential
		proc = ex
		variable = ProcRate_ex
		block = 0
	[../]
	[./ProcRate_iz]
		type = ProcRate
		em = em
		potential = potential
		proc = iz
		variable = ProcRate_iz
		block = 0
	[../]
	
	[./Alpha_el]
		type = Alpha
		proc = el
		variable = Alpha_el
		block = 0
	[../]
	[./Alpha_ex]
		type = Alpha
		proc = ex
		variable = Alpha_ex
		block = 0
	[../]
	[./Alpha_iz]
		type = Alpha
		proc = iz
		variable = Alpha_iz
		block = 0
	[../]
	
#	[./x_ng]
#		type = Position
#		variable = x_node
#		block = 0
#	[../]


	[./EFieldAdvAux_em]
		type = EFieldAdvAux
		potential = potential
		density_log = em
		variable = EFieldAdvAux_em
		block = 0
	[../]
	[./DiffusiveFlux_em]
		type = DiffusiveFlux
		density_log = em
		variable = DiffusiveFlux_em
		block = 0
	[../]
[]

[BCs]
## Potential boundary conditions ##
	[./potential_dirichlet_left]
		type = FunctionDirichletBC
		variable = potential
		boundary = left
		function = potential_bc_func
	[../]

	[./potential_dirichlet_right]
		type = DirichletBC
		variable = potential
		boundary = right
		value = 0
	[../]

### Native potential boundary conditions ###
	[./native_potential_dirichlet_left]
		type = FunctionDirichletBC
		variable = native_potential
		boundary = left
		function = potential_bc_func
	[../]

	[./native_potential_dirichlet_right]
		type = DirichletBC
		variable = native_potential
		boundary = right
		value = 0
	[../]

### Electron boundary conditions ##
	[./Emission_left]
		type = SchottkyEmissionNewBC
#		type = SchottkyEmissionBC
#		type = SecondaryElectronBC
		variable = em
		em = em
		boundary = left
		potential = potential
		mean_en = mean_en
		ip = Arp
		r = 0.9
		relax = false
		work_function = 3.5 # eV
		field_enhancement = 55
		Richardson_coefficient = 80E4
		temperature = 1273 # K
	[../]

	[./em_physical_left]
		type = HagelaarElectronBC # HagelaarElectronAdvectionBC
		variable = em
		boundary = left
		potential = potential
		mean_en = mean_en
		r = 0.9
	[../]
	
	[./em_physical_right]
		type = HagelaarElectronBC # HagelaarElectronAdvectionBC
		variable = em
		boundary = right
		potential = potential
		mean_en = mean_en
		r = 0.9
	[../]

## Argon boundary conditions ##
	[./Arp_physical_left_diffusion]
		type = HagelaarIonDiffusionBC
		variable = Arp
		boundary = 'left'
		r = 0.1
	[../]
	[./Arp_physical_left_advection]
		type = HagelaarIonAdvectionBC
		variable = Arp
		boundary = 'left'
		potential = potential
		r = 0.1
	[../]

	[./Arp_physical_right_diffusion]
		type = HagelaarIonDiffusionBC
		variable = Arp
		boundary = right
		r = 0
	[../]
	[./Arp_physical_right_advection]
		type = HagelaarIonAdvectionBC
		variable = Arp
		boundary = right
		potential = potential
		r = 0
	[../]

## Mean energy boundary conditions ##
	[./mean_en_emission_left]
#		type = SchottkyEmissionEnergyBC
		type = SchottkyEmissionEnergyNewBC
		variable = mean_en
		boundary = left
		em = em
		potential = potential
		ip = Arp
		mean_en = mean_en
		r = 0.9
		relax = false
		work_function = 3.5 # eV
		field_enhancement = 55
		Richardson_coefficient = 80E4
		temperature = 1273 # K
	[../]

	[./mean_en_physical_left]
		type = HagelaarEnergyBC # HagelaarEnergyAdvectionBC
		variable = mean_en
		boundary = left
		potential = potential
		em = em
		r = 0.9
	[../]
	
	[./mean_en_physical_right]
		type = HagelaarEnergyBC # HagelaarEnergyAdvectionBC
		variable = mean_en
		boundary = right
		potential = potential
		em = em
		r = 0.9
	[../]
[]

[ICs]
	[./potential_ic]
		type = FunctionIC
		variable = potential
		function = potential_ic_func
	[../]

	[./native_potential_ic]
		type = FunctionIC
		variable = native_potential
		function = potential_ic_func
	[../]
[]

[Functions]
	[./potential_bc_func]
		type = ParsedFunction
		vars = 'VHigh'
		vals = '20')
		value = '-VHigh'
	[../]

	[./potential_ic_func]
		type = ParsedFunction
		value = '-20 * (10 - x) / 10'
	[../]

[]

[Materials]
	[./argon_gas_block]
		type = Gas
		enable = true
		interp_trans_coeffs = true
		interp_elastic_coeff = true
		ramp_trans_coeffs = false
		em = em
		potential = potential
		ip = Arp
		mean_en = mean_en
		user_se_coeff = 0.02
		user_work_function = 3.5 # eV
		user_field_enhancement = 55
		user_Richardson_coefficient = 80E4
		user_cathode_temperature = 1273 # K
		property_tables_file = td_argon_mean_en.tsv
		block = 0
		user_T_gas = 500
	[../]
	
	[./xenon_gas_block]
		type = XenonGas
		enable = false
		interp_trans_coeffs = true
		interp_elastic_coeff = true
		ramp_trans_coeffs = false
		em = em
		potential = potential
		ip = Arp
		mean_en = mean_en
		user_se_coeff = 0.02
		user_work_function = 3.5 # eV
		user_field_enhancement = 55
		user_Richardson_coefficient = 80E4
		user_cathode_temperature = 1273 # K
		property_tables_file = td_xenon_mean_en.tsv
		block = 0
		user_T_gas = 500
	[../]

	[./electricConstant]
		type = GenericConstantMaterial
		block = 0
		prop_names  = 'diffnative_potential'
		prop_values = '8.85418782E-12'
	[../]
[]
