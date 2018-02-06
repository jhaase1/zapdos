#### import the simple module from the paraview
from paraview.simple import *
import glob, re, os, numpy, csv

def CreatePointSelection(ids):
  source = IDSelectionSource()
  source.FieldType = "POINT"
  sids = []
  for i in ids:
    sids.append(0) #proc-id
    sids.append(i) #cell-id
  source.IDs = sids
  return source

def selectElectrodeData(data, voi):
  dataOverTime = PlotSelectionOverTime(Input=data, Selection=selection)
  dataOverTime.OnlyReportSelectionStatistics = 0
  
  dataOverTimeT = TransposeTable(Input=dataOverTime, VariablesofInterest=voi)
  dataOverTimeT.UpdatePipeline()
  dataOverTimeTT = TransposeTable(Input=dataOverTimeT)
  
  dataOverTimeTT.Usethecolumnwithoriginalcolumnsname = 1
  dataOverTimeTT.Addacolumnwithoriginalcolumnsname = 0
  dataOverTimeTT.UpdatePipeline()
  
  return dataOverTimeTT

def CreateCellSelection(ids):
    source = IDSelectionSource()
    source.FieldType = "CELL"
    sids = []
    for i in ids:
        sids.append(0) #proc-id
        sids.append(i) #cell-id
    source.IDs = sids
    return source

def IntegrateOverSpace(data):
    integrateVariables = IntegrateVariables(Input=data)
    
    selection = CreateCellSelection(ids=[0])
    integrateVariablesOverSpace = PlotSelectionOverTime(Input=integrateVariables, Selection=selection)
    integrateVariablesOverSpace.OnlyReportSelectionStatistics = 0
    integrateVariablesOverSpace.UpdatePipeline()
    
    return integrateVariablesOverSpace

path = os.getcwd() + "/"
#path = "C:\\Users\\John\\Desktop\\"
fileBase = "SteadyState_out"
#fileBase = "Ar_phi_2.5_eV_tOn_2_ns_tOff_100_ns_d_10_um_VHigh_45.6_V_VLow_1.0_V"

## Define selection ##
selection = CreatePointSelection(ids=[0])

## Get reader data ##
reader = ExodusIIReader(FileName=path+fileBase + '.e')
reader.GenerateObjectIdCellArray = 1
reader.GenerateGlobalElementIdArray = 1
reader.ElementVariables = reader.ElementVariables.Available
reader.PointVariables = reader.PointVariables.Available
reader.ElementBlocks = reader.ElementBlocks.Available
reader.ApplyDisplacements = 1
reader.DisplacementMagnitude = 1.0

## Get cathode and anode coordinates
calc = Calculator(Input=reader)
calc.ResultArrayName = 'coords'
calc.Function = 'coords'
calc.UpdatePipeline()

coordRange = calc.PointData['coords'].GetRange()
minX = coordRange[0]
maxX = coordRange[1]
L = maxX - minX

Delete(calc)
del calc

## Prepare to extract electrode data ##
electrodeData = []
VariablesofInterest = ['Time', 'Voltage', 'Current_Arp', 'Current_em', 'tot_gas_current', 'Emission_energy_flux']

## Extract cathode data ##
cathodeValues = ExtractLocation(Input=reader)
cathodeValues.Location = [minX,0,0]
cathodeValues.Mode = 'Interpolate At Location'

electrodeData.append(selectElectrodeData(cathodeValues, VariablesofInterest))

## Extract anode data ##
anodeValues = ExtractLocation(Input=reader)
anodeValues.Location = [maxX,0,0]
anodeValues.Mode = 'Interpolate At Location'

electrodeData.append(selectElectrodeData(anodeValues, VariablesofInterest))

electrodeData.append(reader)

electrodeData.append(IntegrateOverSpace(data=reader))

## Calculate average powers and efficiency ##
PowerAndEfficiency = ProgrammableFilter(Input=electrodeData)
PowerAndEfficiency.Script = """
from numpy import trapz
import paraview.simple as pv
import numpy as np

L = """ + str(L) + """
for c, a, r, integ, outTable in zip(inputs[0], inputs[1], inputs[2], inputs[3], output):

  voltageDelta = 1E-1 # (V)
  timeUnits = 1E-9 # (s/ns)
  
  potential = c.RowData['Voltage'] - a.RowData['Voltage'] # (V)
  
  loadVoltage = max( potential ) * np.ones(len(c.RowData['Voltage'])) # (V)
  workFunctionDelta = 1
  workFunctionDeltaVector = workFunctionDelta * np.ones(len(c.RowData['Voltage'])) # (eV)
  appliedVoltage = numpy.round( potential - loadVoltage , 4 ) # (V)
  
  ind = np.where( max( potential ) - voltageDelta < np.array(potential)) # (V)
  
  time = c.RowData['Time'] - min(c.RowData['Time']) # (ns)
  period = max(time) - min(time) # (ns)
  offTime = max(time[ind]) - min(time[ind]) # (ns)
  onTime = period - offTime # (ns)  
  
  # current density
  j = a.RowData['tot_gas_current'] 
  
  # The units stay the same because it is being integrated over ns, then divided by ns
  
  # Time (ns)
  outTable.RowData.append(time, 'time')
  
  # Total current density leaving at the boundaries (A/m^2)
  outTable.RowData.append(j, 'CurrentDensity')
  
  # Cathode anode potential difference (V)
  outTable.RowData.append(potential, 'CathodeAnodePotentialDifference')
  
  # Output voltage (V)
  outTable.RowData.append(workFunctionDeltaVector + potential, 'OutputVoltage')
  
  # Production voltage (V)
  outTable.RowData.append(workFunctionDeltaVector + loadVoltage, 'ProductionVoltage')
  
  # Applied voltage (V)
  outTable.RowData.append(appliedVoltage, 'AppliedVoltage')
  
  # Net power (W/m^2)
  outTable.RowData.append(j * (workFunctionDeltaVector + potential), 'NetPower')
  
  # Power produced (W/m^2)
  outTable.RowData.append(j * (workFunctionDeltaVector + loadVoltage), 'PowerProduced')
  
  # Power consumed (W/m^2)
  outTable.RowData.append(j * appliedVoltage, 'PowerConsumed')
   
  # ElectronCooling (W/m^2)
  outTable.RowData.append(c.RowData['Emission_energy_flux'], 'ElectronCooling')
  
  for integratedVariable in ['em_lin','Arp_lin']: #integ.RowData.keys():
    outTable.RowData.append(integ.RowData[integratedVariable]/L, 'Avg_' + integratedVariable)
  
  # Total current density leaving at the boundaries (A/m^2)
  outTable.RowData.append(r.FieldData['Full_EmissionCurrent'], 'EmittedCurrentDensity') # Emitted current density (A/m^2)
  outTable.RowData.append(r.FieldData['Thermionic_EmissionCurrent'], 'ThermionicEmissionCurrent') # Thermionic emitted current density (A/m^2)
  outTable.RowData.append(r.FieldData['Native_EmissionCurrent'], 'NativeCurrentDensity') # Native emitted current density (A/m^2)
"""

PowerAndEfficiency.UpdatePipeline()

fname = glob.glob(path + 'TimeDependentData*.csv')
for f in fname:
  os.remove(f)

writer = CreateWriter(path + 'TimeDependentData.csv', PowerAndEfficiency, Precision=13, UseScientificNotation=1)
writer.UpdatePipeline()

fname = glob.glob(path + 'TimeDependentData*.csv')
os.rename(fname[0] , path + 'TimeDependentData.csv')

for f in GetSources().values():
    Delete(f)
    del f

