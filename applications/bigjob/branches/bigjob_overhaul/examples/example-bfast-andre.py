import troy

# Initiate the PilotJobServices
ps1 = troy.PilotJobService('diane://diane-producer.ranger')   # Dianre PJ
ps2 = troy.PilotJobService('bigjob://bigjob-producer.kraken') # BigJob PJ
ps3 = troy.PilotJobService('any://bigjob-producer.ranger')    # random PJ

# Create a Diane PilotJob on Ranger, 
p1_desc = troy.PilotJobDescription()
p1_desc.NumberOfCores = 128
pj1 = ps.create_pilotjob(p1_desc, 'gram://ranger')

# Create a BigJob PilotJob on Kraken
p2_desc = troy.PilotJobDescription()
p2_desc.NumberOfCores = 1024
pj2 = ps.create_pilotjob(p2_desc, 'gram://kraken')

# Create a PilotJob on Ranger, let backend decide on type
p3_desc = troy.PilotJobDescription()
p3_desc.NumberOfCores = 512
pj3 = ps.create_pilotjob(p3_desc, 'gram://ranger')

# Instantiate a WorkUnitService, wich is a container for pilot jobs
ws = troy.WorkUnitService()
ws.add_pj (pj1);
ws.add_pj (pj2);
ws.add_pj (pj3);

all_input_files = { '/data/file1', '/data/file2' }

# Create bfast WorkUnits and submit them to the WorkUnitService
for input in all_input_files:
  wu_desc = troy.WorkUnitDescription()
  wu_desc.Executable = "/bin/bfast"
  wu_desc.NumberOfCores = 4
  wu_desc.Arguments = ["match", "-t4", input]
  wu = ws.submit(wu_desc)

