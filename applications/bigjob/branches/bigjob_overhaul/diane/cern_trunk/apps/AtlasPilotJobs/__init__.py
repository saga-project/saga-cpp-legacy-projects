from pilots import *

def run(input,config):
     input.scheduler = AtlasPilotScheduler
     input.manager = AtlasPilotManager
     input.worker =  AtlasPilotWorker
     input.data = AtlasPilotRunData()


import os

# copy all input files into the run directory
def run_post(input,config):
     input.data._udir = os.path.dirname(input._runfile)
     input.data._idir = os.path.join(input._rundir,'input_files')
     input.data._odir = os.path.join(input._rundir,'output_files')
     input.data._rdir = input._rundir
     
