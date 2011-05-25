## class OutputDuplicator:
##     def __init__(self,dest1,dest2,auto_close2=None):
##         self.dest1 = dest1
##         self.dest2 = dest2
##         self.auto_close2 = auto_close2
##         self.closed = False

##     def write(self,text):
##         self.f.write(text)
##         self.dest1.write(text)
##     def close(self):
##         if self.auto_close2 and self.auto_close2():
##             self.dest2.close()
##         self.dest1.close()
##         self.closed = True

## def duplicate(out_dest,err_dest=None):
##     assert err_dest is None, "duplicating stderr to a separate file not implemented yet"

##     def auto_close(dummy):
##         if sys.stdout.closed
        
##     import sys
##     sys.stdout = OutputDuplicator(sys.stdout,out_dest,

## >>> f = open('log.test','w')
## >>> sys.stdout = Writer()
