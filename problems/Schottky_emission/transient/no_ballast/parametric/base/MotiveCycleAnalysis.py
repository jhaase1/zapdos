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

## Calculate average powers and efficiency ##
PowerAndEfficiency = ProgrammableFilter(Input=electrodeData)
PowerAndEfficiency.Script = """
from numpy import trapz
import numpy as np

for c, a, outTable in zip(inputs[0], inputs[1], output):
  voltageDelta = 1E-1 # (V)
  timeUnits = 1E-9 # (s/ns)
  
  potential = np.round(c.RowData['Voltage'] - a.RowData['Voltage'],4) # (V)
  
  loadVoltage = np.round(max( potential ) * np.ones(len(c.RowData['Voltage'])) , 4) # (V)
  workFunctionDelta = 1
  workFunctionDeltaVector = workFunctionDelta * np.ones(len(c.RowData['Voltage'])) # (eV)
  appliedVoltage = np.round( potential - loadVoltage , 4 ) # (V)
  
  ind = np.where( max( potential ) - voltageDelta < np.array(potential)) # (V)
  
  time = c.RowData['Time'] - min(c.RowData['Time']) # (ns)
  period = max(time) - min(time) # (ns)
  offTime = max(time[ind]) - min(time[ind]) # (ns)
  onTime = period - offTime # (ns)
  
  # current density
  j = a.RowData['tot_gas_current']
  
  PNet = j * (workFunctionDeltaVector + potential)
  PCons = j * appliedVoltage
  PProd = j * (workFunctionDeltaVector + loadVoltage)
  
  # The units stay the same because it is being integrated over ns, then divided by ns
  
  # Output voltage (V)
  outTable.RowData.append(np.round( workFunctionDelta + max( potential ) , 4 ), 'VOut')
  
  # Average current density (A/m^2)
  outTable.RowData.append(trapz(j, x=time) / period, 'CurrentDensity')
  
  # Net power (W/m^2)
  outTable.RowData.append(trapz(PNet, x=time) / period, 'NetPower')
  
  # Power produced (W/m^2)
  outTable.RowData.append(trapz(PProd, x=time) / period, 'PowerProduced')
  
  # Power consumed (W/m^2)
  outTable.RowData.append(trapz(PCons, x=time) / onTime, 'PowerConsumed')
  
  # Net energy (W/m^2)
  outTable.RowData.append(trapz(PNet, x=time) * timeUnits, 'NetEnergy')
  
  # Energy produced (W/m^2)
  outTable.RowData.append(trapz(PProd, x=time) * timeUnits, 'EnergyProduced')
  
  # Energy consumed (W/m^2)
  outTable.RowData.append(trapz(PCons, x=time) * timeUnits, 'EnergyConsumed')
  
  # Abs Net energy (W/m^2)
  outTable.RowData.append(trapz(abs(PNet), x=time) * timeUnits, 'AbsNetEnergy')
  
  # Abs Energy produced (W/m^2)
  outTable.RowData.append(trapz(abs(PProd), x=time) * timeUnits, 'AbsEnergyProduced')
  
  # Abs Energy consumed (W/m^2)
  outTable.RowData.append(trapz(abs(PCons), x=time) * timeUnits, 'AbsEnergyConsumed')
  
  # ElectronCooling (W/m^2)
  outTable.RowData.append(trapz(c.RowData['Emission_energy_flux'], x=time) / period, 'ElectronCooling')
  
#Individual net energy segments (W/m^2)
  zcNet = np.unique(np.append(np.append([0],np.where(np.diff(np.sign(PNet)))[0]), [len(time)-1]))
  
  NetEnergySegments = []
  for idx, (lowerBound, upperBound) in enumerate(zip(zcNet[0:-1], zcNet[1:])):
    NetEnergySegments.append(trapz(PNet[lowerBound:upperBound], x=time[lowerBound:upperBound]) * timeUnits)
  
  ind = []
  seg = []
  
  for a, b in zip(zcNet[0:-1], NetEnergySegments):
    if abs(b) > 1E-15:
      ind.append(a)
      seg.append(b)
  
  zcNet = ind
  NetEnergySegments = seg
    
#Individual consumed energy segments (W/m^2)
  zcCons = np.unique(np.append(np.append([0],np.where(np.diff(np.sign(PCons)))[0]), [len(time)-1]))

  ConsEnergySegments = []
  for idx, (lowerBound, upperBound) in enumerate(zip(zcCons[0:-1], zcCons[1:])):
    ConsEnergySegments.append(trapz(PCons[lowerBound:upperBound], x=time[lowerBound:upperBound]) * timeUnits)
  
  ind = []
  seg = []
  
  for a, b in zip(zcCons[0:-1], ConsEnergySegments):
    if abs(b) > 1E-15:
      ind.append(a)
      seg.append(b)
  
  zcCons = ind
  ConsEnergySegments = seg
	
#Individual produced energy segments (W/m^2)
  zcProd = np.unique(np.append(np.append([0],np.where(np.diff(np.sign(PProd)))[0]), [len(time)-1]))
  
  ProdEnergySegments = []
  for idx, (lowerBound, upperBound) in enumerate(zip(zcProd[0:-1], zcProd[1:])):
    ProdEnergySegments.append(trapz(PProd[lowerBound:upperBound], x=time[lowerBound:upperBound]) * timeUnits)
  
  ind = []
  seg = []
  
  for a, b in zip(zcProd[0:-1], ProdEnergySegments):
    if abs(b) > 1E-15:
      ind.append(a)
      seg.append(b)
  
  zcProd = ind
  ProdEnergySegments = seg

  outTable.RowData.append(len(zcNet), 'NetCrossings')
  outTable.RowData.append(len(zcCons), 'ConsumedCrossings')
  outTable.RowData.append(len(zcProd), 'ProducedCrossings')
  
  for idx, (ind, seg) in enumerate(zip(zcNet, NetEnergySegments)):
    outTable.RowData.append(time[ind] - min(time), 'NetEnergyCrossTime' + str(idx))
    outTable.RowData.append(seg, 'NetEnergySegment' + str(idx))
  
  for idx, (ind, seg) in enumerate(zip(zcCons, ConsEnergySegments)):
    outTable.RowData.append(time[ind] - min(time), 'ConsEnergyCrossTime' + str(idx))
    outTable.RowData.append(seg, 'ConsumedEnergySegment' + str(idx))
  
  for idx, (ind, seg) in enumerate(zip(zcProd, ProdEnergySegments)):
    outTable.RowData.append(time[ind] - min(time), 'ProdEnergyCrossTime' + str(idx))
    outTable.RowData.append(seg, 'ProducedEnergySegment' + str(idx))
"""

PowerAndEfficiency.UpdatePipeline()

fname = glob.glob(path + 'PowerAndEfficiency*.csv')
for f in fname:
  os.remove(f)

writer = CreateWriter(path + 'PowerAndEfficiency.csv', PowerAndEfficiency, Precision=12, UseScientificNotation=1)
writer.UpdatePipeline()

fname = glob.glob(path + 'FieldData*.csv')
for f in fname:
  os.remove(f)

writer = CreateWriter(path + 'FieldData.csv', reader, Precision=12, UseScientificNotation=1, FieldAssociation='Field Data')
writer.UpdatePipeline()

fname = glob.glob(path + 'FieldData*.csv')
FieldData = numpy.genfromtxt(fname[0], delimiter=',').transpose()
for f in fname:
  os.remove(f)

time = reader.TimestepValues

FullEmission = numpy.delete(FieldData[0], 0)
NativeEmission = numpy.delete(FieldData[1], 0)
relTime = [t - min(time) for t in time]

zipped = zip(relTime, FullEmission, NativeEmission)
numpy.savetxt(path + 'data.csv', zipped, delimiter = ',')


EmissionBenefit = numpy.trapz( FullEmission / NativeEmission , x=time) / (max(time) - min(time))
cycles = max(time) / (max(time) - min(time))

fname = glob.glob(path + 'PowerAndEfficiency*.csv')
os.rename(fname[0] , path + 'PowerAndEfficiency.csv')

with open(path + 'PowerAndEfficiency.csv', 'rb') as csvfile:
    reader = csv.reader(csvfile, delimiter=',', quotechar='|')
    header = next(reader)
    data = next(reader)

header = [h.replace('"','') for h in header]
header.extend(['emission_increase', 'cycles'])
data.extend([str(EmissionBenefit), str(int(max(time)/(max(time)-min(time))))])

with open(path + 'PowerAndEfficiency.csv', 'wb') as csvfile:
  writer = csv.writer(csvfile, delimiter=',', quoting=csv.QUOTE_MINIMAL)
  writer.writerow(header)
  writer.writerow(data)

with open(path + 'emission_increase.csv', 'wb') as csvfile:
  writer = csv.writer(csvfile, delimiter=',', quoting=csv.QUOTE_NONE)
  writer.writerow([EmissionBenefit])

for f in GetSources().values():
    Delete(f)
    del f

