""" Tools and utilities to handle journals.
"""

import time

class Journal:
    """ Create journal file and add entries to it.

    If you want to read a journal have a look in the analysis_tools package.

    Journal is an easily parsable log of events, so the run may be
    reconstructed from the journal rather than parsing the standard output
    messages.

    Each line of the journal file looks like this: [timestamp, 'label',
    {'k1':val1, ... }] so it may be easily read from a python script using
    eval.

    All lines with the same label correspond to the same event in the system
    and it is recommended that they data dictionary always has the same keys
    for the same label.
    """
    
    def __init__(self,fn,uuid): #need run id for MSG Monitoring
        self.f = file(fn,'a')
        self.uuid = uuid
        self.addEntry('journal_open')

    def addEntry(self,label,**kwds):
        t = time.time()
        kwds['_master_uuid'] = self.uuid
        msg = repr([t,label,kwds])
        
        import MSGWrap
        MSGWrap.journalEntry([t,label,kwds])

        self.f.write(msg+'\n')
        self.f.flush()

    def __del__(self):
        self.addEntry('journal_close')

if __name__=="__main__":
    j = Journal('test.journal.txt')
    j.addEntry('a')
    j.addEntry('b',c=1,d=2)
    j.addEntry('c',x='xxx')
    j.addEntry('x',blah='whatever goes =\t')
    print 'journal test OK'
    
