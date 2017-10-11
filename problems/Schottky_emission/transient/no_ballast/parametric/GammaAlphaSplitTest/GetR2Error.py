#### import the simple module from the paraview
from paraview.simple import *
import glob, re, os, csv

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


path = os.getcwd() + '/'
files = ['SteadyState_out.e', 'PreviousCycle_out.e' ]
reader = []

for idx, fl in enumerate(files):
    reader.append(ExodusIIReader(FileName=path+fl))
    reader[idx].GenerateObjectIdCellArray = 1
    reader[idx].GenerateGlobalElementIdArray = 1
    ElementVariables = reader[idx].ElementVariables.Available
    ElementVariables.remove('x')
    reader[idx].ElementVariables = ElementVariables
    reader[idx].PointVariables = reader[idx].PointVariables.Available
    reader[idx].ElementBlocks = reader[idx].ElementBlocks.Available

reader[-1].ElementVariables = reader[-1].ElementVariables.Available

temporalShift = []
tMax = [];
tMin = []

for idx, r in enumerate(reader):
    temporalShift.append(TemporalShiftScale(Input=r))
    temporalShift[idx].PreShift = -r.TimestepValues[0]
    tMin.append(min(r.TimestepValues))
    tMax.append(max(r.TimestepValues))

period = abs(tMin[0] - tMin[1])
EndTime = max(tMax)
nCycles = EndTime / period

squareVar = ProgrammableFilter(Input=temporalShift)
script = "output.CellData.append(inputs[-1].CellData['x'], 'x')\n"

for var in reader[0].ElementVariables:
    script=script + "output.CellData.append((inputs[1].CellData['" + var + "'])**2 , '" + var + "')\n"

squareVar.Script=script
squareVar.UpdatePipeline()

squareDiff = ProgrammableFilter(Input=temporalShift)
script = "output.CellData.append(inputs[-1].CellData['x'], 'x')\n"

for var in reader[0].ElementVariables:
    script=script + "output.CellData.append((inputs[0].CellData['" + var + "'] - inputs[1].CellData['" + var + "'])**2 , '" + var + "')\n"

squareDiff.Script=script
squareDiff.UpdatePipeline()

RelNormOverTimePart = []

RelNormOverTimePart.append(IntegrateOverSpace(data=squareDiff))
RelNormOverTimePart.append(IntegrateOverSpace(data=squareVar))

RelNorm = ProgrammableFilter(Input=RelNormOverTimePart)
RelNorm.Script = """
from numpy import trapz
for t0, t1, outTable in zip(inputs[0], inputs[1], output):
    for aname in t0.RowData.keys():
        outTable.RowData.append(trapz(t0.RowData[aname]/t1.RowData[aname], x=t0.RowData['Time']), aname)
"""

RelNorm.UpdatePipeline()

RelNormTrans = TransposeTable(Input=RelNorm, VariablesofInterest=[a for a in reader[0].ElementVariables if not a.startswith('ProcRate')])
TotalRelNorm = ProgrammableFilter(Input=RelNormTrans)
TotalRelNorm.Script = """
from numpy import sum
for t0, outTable in zip(inputs[0], output):
    outTable.RowData.append(sum(t0.RowData['0']), 'sum')
"""

TotalRelNorm.UpdatePipeline()

writer = CreateWriter(path + 'TotalRelNorm.csv', TotalRelNorm, Precision=12, UseScientificNotation=1)
writer.UpdatePipeline()

for f in GetSources().values():
    Delete(f)
    del f

with open(path + 'TotalRelNorm0.csv', 'rb') as csvfile:
    reader = csv.reader(csvfile, delimiter=',', quotechar='|')
    row = next(reader)
    row = next(reader)

for data in row:
    data = float(data)

with open(path + 'TotalRelNorm.csv', 'wb') as csvfile:
    writer = csv.writer(csvfile, delimiter=',', quoting=csv.QUOTE_MINIMAL)
    writer.writerow([nCycles] + row)

with open(path + 'sum.csv', 'wb') as csvfile:
    writer = csv.writer(csvfile, delimiter=',', quoting=csv.QUOTE_MINIMAL)
    writer.writerow(row)
	
os.remove(path + 'TotalRelNorm0.csv')

print(', '.join(map(str, [nCycles] + row)))