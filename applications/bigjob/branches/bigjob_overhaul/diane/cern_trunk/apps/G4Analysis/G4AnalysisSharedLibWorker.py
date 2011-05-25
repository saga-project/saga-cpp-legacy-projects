from Data import *

from diane.util import File
from diane import IApplicationWorker

from diane import getLogger

logger = getLogger('G4Analysis')

import os

def load(name):
    simname = "%sSimulation"%name[2:] # strip leading G4 from the name
    module = __import__("%s.python"%name,globals(),locals(),[simname])
    simclass = vars(module)[simname]
    return simclass(0)

class G4SimulationFailure(Exception):
    pass

# this function is called before the Worker object is created: we get the application software to the worker node
# and set the environment, the Worker object should be created in a different process using shell
def setup_application(init_data,agent):
    import os
    file_transfer = agent.ftc
    appname = init_data.G4ApplicationName
    file_transfer.download('_%s.tar'%appname)
    os.system('cd _python; tar xfv ../_%s.tar'%appname)
    return ('source _python/%s/setup.sh'%appname,'')
    
class Worker(IApplicationWorker):
    def initialize(self, init_data):
        self.file_transfer = self._agent.ftc

        import os
        os.system('cd _python/%s && gmake G4DIANE_USE=1 all'%init_data.G4ApplicationName)

        # load library and create simulation object
        self.sim = load(init_data.G4ApplicationName)

        argv = ["distributed-Geant4-simulation"]

        if not self.sim.initialize(len(argv),argv):
            raise G4SimulationFailure('failed to initialize simulation')
        
        initMacrosFileName = "G4-init.macro"
        file(initMacrosFileName, 'w').write(init_data.initMacros)

        print 'executing initial macros...'

        self.sim.executeMacro(initMacrosFileName)

        print 'initialization done'
    
    def do_work(self, input_data):

        print "*** Starting G4 worker task..."
        print "random seed for this task:",input_data.runParameters.seed

        self.sim.setSeed(input_data.runParameters.seed)

        print "task number:", input_data.taskNo

        runMacrosFileName = "G4-run.macro"

        beamOnMacro = '/run/beamOn %d\n'%input_data.runParameters.eventNumber
        
        file(runMacrosFileName,'w').write('\n'.join(input_data.runParameters.macroTemplate.splitlines()+[beamOnMacro]))

        print 'running the simulation...'
        self.sim.executeMacro(runMacrosFileName)

        outputFileName = self.sim.getOutputFilename()

        taskOutputData = TaskOutputData()

        # make the unique output name to be written to the master (containing task id)
        import os
        destName = os.path.splitext(os.path.basename(outputFileName))
        destName = '%s.%04d%s'%(destName[0],input_data.taskNo,destName[1])
        
        taskOutputData.outputFile = File(outputFileName,dest=destName)
        taskOutputData.taskNo = input_data.taskNo
        
        return taskOutputData
        
    def finalize(self,cookie):
        self.sim.finish()


