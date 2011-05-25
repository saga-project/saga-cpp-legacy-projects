from diane.submitters import Submitter

import sys, time, Ganga.GPI

class LCGSubmitter(Submitter):

        def __init__(self):
                Submitter.__init__(self)
                self.parser.description = 'Submit worker agents to LCG. '+self.parser.description
                self.parser.add_option('--delay',dest='delay',type='int',default=0,help='delay in seconds in between the worker submission (to avoid spikes in the worker agent registration)')
                self.parser.add_option('--CE',dest='CE',type='string',default='',help='set LCG.CE attribute, example: ce.cyf-kr.edu.pl:2119/jobmanager-pbs-gear, tbn20.nikhef.nl:2119/jobmanager-pbs-qlong')
                self.parser.add_option('--CE-list',dest='CE_list',type='string',default='',help='a file containing the list of CEs to be used; for each job the LCG.CE attribute will be assigned using round robin algorithm')

                self.download=True

                #the number of the current CE to be used
                self.current = 0
                #Computing Element list
                self.CEs = []

        def initialize(self):
                Submitter.initialize(self)

                self.CEs = []
                if self.options.CE_list != '' and self.options.CE != '':
                        print 'WARNING: both --CE and --CE-list options are specified. Use one or the other. The script will now exit...'
                        sys.exit(0)

                if self.options.CE_list != '':
                        f = open(self.options.CE_list)
                        try:
                                for line in f:
                                        self.CEs.append(line.strip())
                        finally:
                                f.close()

                if self.options.CE != '':
                        self.CEs = [self.options.CE]

                
                if self.options.delay<0:
                        self.options.delay = 0

        def excludeCE(self, ce):
                #escape . to make ce string compatible with regular expression format in config.LCG
                ce = ce.replace('.', '\\.')
                excluded = Ganga.GPI.config.LCG.ExcludedCEs.split()
                if ce in excluded:
                        return
                # QUICKFIX: surround by try-except to the bug in ganga #
        try:
                Ganga.GPI.config.LCG.ExcludedCEs += ' ' + ce2
        except: 
                pass
            ########################################################

        def restoreCE(self, ce):
                ce = ce.replace('.', '\\.')
                excluded = Ganga.GPI.config.LCG.ExcludedCEs.split()
                if ce in excluded:
                        excluded.remove(ce)
                try:
                        all = ''
                        for ce in excluded:
                                all += ' ' + ce
                        Ganga.GPI.config.LCG.ExcludedCEs = all
                except:
                        pass

        def submit_worker(self,j,ce=None):
                j.backend = Ganga.GPI.LCG()
                #j.backend = Ganga.GPI.Local()
                if self.CEs != []:
                        #set the CE for a given job
                        CE = self.CEs[self.current]
                        print 'Select CE:', CE
                        j.backend.CE = CE
                        #update and normalize last CE number
                        self.current = (self.current + 1) % len(self.CEs)
                elif ce is not None:
                        j.backend.CE = ce #only if the CEs list is empty

                #call the superclass' method to set other diane-specific job settings
                Submitter.submit_worker(self,j)

                if self.options.delay:
                        print 'now sleeping for %d seconds '%self.options.delay
                        time.sleep(self.options.delay)

if __name__ == '__main__':
        submitter = LCGSubmitter()
        submitter.initialize()
        for i in range(submitter.options.N_WORKERS):
                j=Ganga.GPI.Job()
                submitter.submit_worker(j)
