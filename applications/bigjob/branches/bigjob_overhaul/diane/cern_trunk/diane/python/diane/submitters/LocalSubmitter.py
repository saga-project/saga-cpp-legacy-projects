#!/usr/bin/env ganga
#-*-python-*-

if __name__ == '__main__':

    from diane.submitters import Submitter

    prog = Submitter()
    prog.parser.description = "Submit worker agents locally. "+prog.parser.description
    prog.initialize()

    for i in range(prog.options.N_WORKERS):
        j = Job()
        j.backend=Local()
        prog.submit_worker(j)

