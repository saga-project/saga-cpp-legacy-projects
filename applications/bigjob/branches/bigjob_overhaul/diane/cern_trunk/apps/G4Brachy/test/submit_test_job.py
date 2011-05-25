import tarfile

#create the test script
script_file = file('test.job','w').write(""" #test script
hostname
uname -a
gcc -v
ls -l $VO_GEANT4_SW_DIR
ls -l $VO_GEANT4_SW_DIR/*
tar xfv G4Brachy.tar
cd G4Brachy
# wget http://cern.ch/diane/AFS/Geant4/G4DemoNSS2008/external/iAIDA.tgz
# tar xzvf iAIDA.tgz
source setup.sh
gmake G4DIANE_USE=1
ls -l
cd test
python test.py
echo 'test completed successfully'
""")

# tar up all G4Brachy directory
tf = tarfile.TarFile('G4Brachy.tar','w')
tf.add('../..','G4Brachy') 
tf.close()

#submit the job using Ganga

assert config.LCG.VirtualOrganisation == 'geant4'

j = Job()
j.application = Executable(exe='test.job')
j.inputsandbox = ["G4Brachy.tar","test.job"]
j.name='testlocalaidaversion'
j.backend=LCG()
j.backend.CE='ce105.cern.ch:2119/jobmanager-lcglsf-grid_geant4'
j.submit()

print '*** G4Brachy test job submitted OK ***'
