from Data import *

from diane.util import File
from diane import IApplicationWorker, SimpleApplicationManager

from diane import getLogger

logger = getLogger('G4Analysis')

from diane.analysis_tools.stats import Histogram1D

import StringIO
import os

class ApplicationManager(SimpleApplicationManager):
    def __init__(self):
        SimpleApplicationManager.__init__(self)
        self.done_counter = 0
        
    def initialize(self,run_data):

        # compute and store the number of tasks
        self.task_num = run_data.runParameters.eventNumber/run_data.eventChunk + 1

        # original C++ module used CLHEP for initial table of random seeds
        #HepRandom::setTheEngine(new HepJamesRandom);
        #HepRandom::setTheSeed(run_data.runParams.seed);

        import random
        random.seed(run_data.runParameters.seed)
        
        task_list = []

        # create tasks (they differ only by the seed)
        for i in range(self.task_num):
            t = self._task()
            t.task_input = TaskInputData()
            t.task_input.taskNo = i
            t.task_input.runParameters.macroTemplate = run_data.runParameters.macroTemplate
            t.task_input.runParameters.seed = int(random.uniform(0,run_data.runParameters.eventNumber)) # CLHEP: (long) RandFlat::shoot(10000.0);
            t.task_input.runParameters.eventNumber = run_data.eventChunk
            task_list.append(t)

        self.worker_init = run_data.workerInit

        self.run_data = run_data
        self.merged_hists = {}
        
        return task_list

    def tasks_done(self, tasks):
        import math
        
        for t in tasks:
            if t.status == t.IGNORED:
                logger.info('** ignored task %d',t.tid)
            else:
                if self.run_data.save_partial_output_files:
                    print 'writing the output file',t.task_output.outputFile.name
                    t.task_output.outputFile.write()
                hist_file = StringIO.StringIO(t.task_output.outputFile.text)
                if self.run_data.merge_histograms:
                    mfn = 'merged1D_%s'%t.task_output.outputFile.name
                    print 'merging histograms to file %s'%os.path.abspath(mfn)
                    self.merged_hists = Histogram1D.mergeHistograms(hist_file,self.merged_hists)
                    mf = file(mfn,'w')
                    Histogram1D.storeToAidaXML(mf,self.merged_hists)
                    mf.close()
                    
        self.done_counter += len(tasks)

    def has_more_work(self):
        return self.done_counter < self.task_num
    


def run_cmd(x):
    r = os.system(x)
    if r != 0:
        raise Exception('command failed (exit code = %d): %s'%(r,x))


class Worker(IApplicationWorker):
    def initialize(self, init_data):
        self.file_transfer = self._agent.ftc
        self.init_data = init_data

        appname = init_data.G4ApplicationName

        # download the application
        self.file_transfer.download('_%s.tar'%appname)
        run_cmd('cd _python; tar xfv ../_%s.tar'%appname)

        # compile the application
        run_cmd('cd _python/%s && source setup.sh && gmake all'%appname)

    def do_work(self, input_data):

        print "*** Starting G4 worker task..."
        print "random seed for this task:",input_data.runParameters.seed

        print "task number:", input_data.taskNo

        beamOnMacro = '/run/beamOn %d\n'%input_data.runParameters.eventNumber
        
        macro_file = file("G4-run.macro",'w')
        macro_file.write('\n'.join(self.init_data.initMacros.splitlines()+input_data.runParameters.macroTemplate.splitlines()+[beamOnMacro]))
        macro_file.close()

        print 'running the simulation...'

        params = { 'G4App': self.init_data.G4ApplicationName}
        params['exe'] = params['G4App'][2:] # remove leading G4 from the application name
        params['macro'] = macro_file.name
        params['seed'] = input_data.runParameters.seed
        
        run_cmd('pushd _python/%(G4App)s && source setup.sh && popd && ${G4WORKDIR}/bin/${G4SYSTEM}/%(exe)s %(macro)s %(seed)s'%params)

        outputFileName = self.init_data.outputfn

        taskOutputData = TaskOutputData()

        # make the unique output name to be written to the master (containing task id)
        destName = os.path.splitext(os.path.basename(outputFileName))
        destName = '%s.%04d%s'%(destName[0],input_data.taskNo,destName[1])
        
        taskOutputData.outputFile = File(outputFileName,dest=destName)
        taskOutputData.taskNo = input_data.taskNo
        
        return taskOutputData
        
    def finalize(self,cookie):
        pass
