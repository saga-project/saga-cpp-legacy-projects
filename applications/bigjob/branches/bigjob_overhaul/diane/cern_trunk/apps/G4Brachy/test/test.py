import sys,os

# just make sure that G4Brachy directory is in the python path
sys.path.insert(0,os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
print 'DEBUG: sys.path=',sys.path

# here is the macro file with simulation commands
mf = file("test.macro",'w')
print >>mf, """
/control/verbose 1
/run/verbose 0
/event/verbose 0
/source/switch Iodium
/run/energy Iodium
/run/initialize
/run/beamOn 10
"""
mf.close()

## Voila, let's import the simulation module and give it some instructions
import python.BrachySimulation
s = python.BrachySimulation(0)

print "output will be saved in ",s.getOutputFilename()

s.initialize(1,['blah'])
s.setSeed(0)
s.executeMacro(mf.name)
s.finish()

print "output file:"
print file(s.getOutputFilename()).read()

print
print "*** test.py OK ***"
