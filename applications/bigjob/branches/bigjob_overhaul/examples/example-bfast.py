import troy

# Initiate the PilotJobService
ps = troy.PilotJobService()

# Create a PilotJob on Ranger, let the "system" decide which type
p1_desc = troy.PilotJobDescription()
p1_desc.NumberOfCores = 128
p1 = ps.create_pilotjob(p1_desc, 'gram://ranger')

# Create a PilotJob on Kraken, specify that we want a DIANE backend
p2_desc = troy.PilotJobDescription()
p2_desc.NumberOfCores = 1024
p2 = ps.create_pilotjob(p2_desc, 'gram://kraken', troy.PILOTJOB_TYPE.DIANE)

# Instantiate a WorkUnitService, linked to the PilotJobService we created before
ws = troy.WorkUnitService(ps)

all_input_files = { '/data/file1', '/data/file2' }

# Create bfast WorkUnits and submit them to the WorkUnitService
for input in all_input_files:
  wu_desc = troy.WorkUnitDescription()
  wu_desc.Executable = "/bin/bfast"
  wu_desc.NumberOfCores = 4
  wu_desc.Arguments = ["match", "-t4", input]
  wu = ws.submit(wu_desc)
