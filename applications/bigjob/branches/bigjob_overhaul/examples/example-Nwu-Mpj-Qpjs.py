#
# This example demonstrates:
# N WorkUnits, to M PilotJob, using Q PilotJobService
#
import troy

# Create our callback class and method
class MyCallback(Callback):
    def cb(self, wu, member, value):
        print 'Called by', wu, 'because ', member, \
                ' changed, and its new value is:', value
        
        return True # Stay registered


# Initiate a PilotJobService
pjs1 = troy.PilotJobService()

# Create a PilotJob on Ranger, let the "system" decide which type
pj1_desc = troy.PilotJobDescription()
pj1_desc.total_cpu_count = 16
pj1 = pjs1.create('gram://ranger', pj1_desc)

# Initiate another PilotJobService
pjs2 = troy.PilotJobService()

# Create a PilotJob on Kraken, specify that we want a DIANE backend
pj2_desc = troy.PilotJobDescription()
pj2_desc.total_cpu_count = 16
pj2 = pjs2.create('gram://kraken', pj2_desc, pj_type=troy.PILOTJOB_TYPE.DIANE)

# Instantiate a WorkUnitService, 
wus = troy.WorkUnitService()

# Link the PilotJobServices we created before
wus.add(pjs1)
wus.add(pjs2)

# Some input
all_input_files = { '/data/file1', '/data/file2' }

# Create bfast WorkUnits and submit them to the WorkUnitService
for input in all_input_files:
  wu_desc = troy.WorkUnitDescription()
  wu_desc.executable = "/bin/bfast"
  wu_desc.total_cpu_count = 4
  wu_desc.arguments = ["match", "-t4", input]
  wu = wus.submit(wu_desc)
  cb = MyCallback()
  wu.add_callback(state, cb)


#
# Some work, sleep, etc.
#
