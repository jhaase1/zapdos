#### import the simple module from the paraview
from paraview.simple import *
import glob, re, os

def CreateCellSelection(ids):
    source = IDSelectionSource()
    source.FieldType = "CELL"
    sids = []
    for i in ids:
        sids.append(0) #proc-id
        sids.append(i) #cell-id
    source.IDs = sids
    return source

def ParaviewExportData(PathBase):
	reader = ExodusIIReader(FileName=PathBase + '.e')
	reader.GenerateObjectIdCellArray = 1
	reader.GenerateGlobalElementIdArray = 1
	reader.ElementVariables = reader.ElementVariables.Available
	reader.PointVariables = reader.PointVariables.Available
	reader.ElementBlocks = reader.ElementBlocks.Available
	reader.ApplyDisplacements = 1
	reader.DisplacementMagnitude = 1.0
	
	# create a new 'Integrate Variables'
	integrateVariables = IntegrateVariables(Input=reader)
	
	# create a new 'Plot Selection Over Time'
	selection = CreateCellSelection(ids=[0])
	integrateVariablesOverTime = PlotSelectionOverTime(Input=integrateVariables, Selection=selection)
	integrateVariablesOverTime.OnlyReportSelectionStatistics = 0
	
	# create a new 'Probe Location'
	cathodeProbe = ProbeLocation(Input=reader, ProbeType='Fixed Radius Point Source')
	cathodeProbe.PassFieldArrays = 1
	cathodeProbe.ComputeTolerance = 1
	cathodeProbe.Tolerance = 2.220446049250313e-16
	cathodeProbe.ProbeType.Center = [0.0, 0.0, 0.0]
	cathodeProbe.ProbeType.NumberOfPoints = 1
	cathodeProbe.ProbeType.Radius = 0.0
	
	# create a new 'Plot Selection Over Time'
	selection = SelectCells(query="id==0", proxy=cathodeProbe)
	cathodeProbeOverTime = PlotSelectionOverTime(Input=cathodeProbe, Selection=selection)
	cathodeProbeOverTime.OnlyReportSelectionStatistics = 0
	
	writer = CreateWriter(PathBase + '_integrated.csv', integrateVariablesOverTime, Precision=6, UseScientificNotation=1, WriteAllTimeSteps=1)
	writer.UpdatePipeline()
	del writer
	
	writer = CreateWriter(PathBase + '_cathode.csv', integrateVariablesOverTime, Precision=6, UseScientificNotation=1, WriteAllTimeSteps=1)
	writer.UpdatePipeline()
	del writer
	
	Delete(integrateVariablesOverTime)
	del integrateVariablesOverTime
	
	Delete(cathodeProbeOverTime)
	del cathodeProbeOverTime
	
	Delete(integrateVariables)
	del integrateVariables
	
	Delete(cathodeProbe)
	del cathodeProbe
	
	Delete(reader)
	del reader

filePath = os.getcwd() + "/download/"

for file in glob.glob("*.e"):
	fileBase = re.sub('\.e$', '', file)
	ParaviewExportData(PathBase=filePath+fileBase)