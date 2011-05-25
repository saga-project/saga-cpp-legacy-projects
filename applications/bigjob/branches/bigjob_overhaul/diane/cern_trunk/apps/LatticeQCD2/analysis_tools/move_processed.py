import os,sys,shutil

for fn in file(sys.argv[1]).readlines():
    fn2 = os.path.dirname(os.path.dirname(fn))
    print fn2, os.path.basename(fn2)
    shutil.move(fn2,'processed/'+os.path.basename(fn2))
 
