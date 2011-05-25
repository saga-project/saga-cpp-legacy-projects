#!/usr/bin/env ganga
#-*-python-*-

if __name__ == '__main__':

    from diane.submitters import Submitter

    prog = Submitter()
    prog.download=False
    prog.parser.description = "Submit worker agents to LSF batch system. "+prog.parser.description
    prog.parser.add_option("--delay",dest="delay",type="int",default=0,help="delay in seconds in between the worker submission (to avoid spikes in the worker agent registration)")
    prog.parser.add_option("--queue",dest="queue",type="string",default='',help="set LSF queue")
    prog.parser.add_option("--extraopts",dest="extraopts",type="string",default='',help="Pass extra options to batch system Actual options depends on batch system. See GANGA Batch interface for more information.")
    prog.initialize()
    import time

    for i in range(prog.options.N_WORKERS):
        j = Job()
        j.backend=LSF(queue=prog.options.queue)
        if prog.options.extraopts:
            j.backend.extraopts = prog.options.extraopts
        prog.submit_worker(j)
        if prog.options.delay:
            print "now sleeping for %d seconds "%prog.options.delay
            time.sleep(prog.options.delay)


