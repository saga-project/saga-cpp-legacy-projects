from executable import *

def run(input,config):
     input.scheduler = diane.SimpleTaskScheduler
     input.manager = ExecutableApplicationManager
     input.worker = ExecutableWorker
     input.data = ExecutableRunData()


import os

# copy all input files into the run directory
def run_post(input,config):
     input.data._udir = os.path.dirname(input._runfile)
     input.data._idir = os.path.join(input._rundir,'input_files')
     input.data._odir = os.path.join(input._rundir,'output_files')
     input.data._rdir = input._rundir
     
__all__ = ['run','run_post']
