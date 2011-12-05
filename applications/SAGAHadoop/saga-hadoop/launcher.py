#!/usr/bin/env python

import time
import bliss.saga as saga
import os, sys

import logging
logging.basicConfig(level=logging.DEBUG)

def main():
    
    try:
        # create a job service for Futuregrid's 'india' PBS cluster
        js = saga.job.Service("fork://localhost")

        # describe our job
        jd = saga.job.Description()
        # resource requirements
        jd.wall_time_limit  = "0:05:00"
        jd.total_cpu_count = 1     
        # environment, executable & arguments
        #jd.environment = {'SLEEP_TIME':'10'}
        executable = os.path.join(os.getcwd(), "bootstrap_hadoop.py")
        logging.debug("Run %s"%executable)
        jd.executable  = executable
        jd.arguments   = []
        # output options
        jd.output = "hadoop_job.stdout"
        jd.error  = "hadoop_job.stderr"
        jd.working_directory=os.getcwd()
        # create the job (state: New)
        myjob = js.create_job(jd)

        print "Job ID    : %s" % (myjob.jobid)
        print "Job State : %s" % (myjob.get_state())

        print "\n...starting job...\n"
        # run the job (submit the job to PBS)
        myjob.run()

        print "Job ID    : %s" % (myjob.jobid)
        print "Job State : %s" % (myjob.get_state())

        print "\n...waiting for job...\n"
        # wait for the job to either finish or fail
        myjob.wait()

        print "Job State : %s" % (myjob.get_state())
        print "Exitcode  : %s" % (myjob.exitcode)

    except saga.Exception, ex:
        print "Oh, snap! An error occured: %s" % (str(ex))

if __name__ == "__main__":
    main()
