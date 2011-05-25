class RunParameters:
    def __init__(self):
        self.seed = 0
        self.eventNumber = 0
        self.macroTemplate = "" #should NOT contain /run/beamOn as it is added automatically
        
class WorkerInitData:
    def __init__(self):
        self.initMacros = ""
        self.G4ApplicationName = None # e.g. G4Brachy
        self.outputfn = "" # output file name produced by the simulation

class TaskInputData:
    def __init__(self):
        self.runParameters = RunParameters()
        self.taskNo = 0
    
class TaskOutputData:
    def __init__(self):
        self.outputFile = None
        self.taskNo = 0

class G4RunData:
    def __init__(self):
        self.workerInit = WorkerInitData()
        self.runParameters = RunParameters()
        self.eventChunk = 1000

        self.merge_histograms = True   # merge histograms on-the-fly
        self.save_partial_output_files = False # preserve all partial output files (e.g. for offline merging)

        
