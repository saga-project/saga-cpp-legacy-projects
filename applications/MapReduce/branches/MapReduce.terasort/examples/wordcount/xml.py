import time
import fileinput
from socket import gethostname; print gethostname()
hnm= gethostname()
file = open("/N/u/smaddi2/ndfile.txt","rb")
sseq = "test"
list = ""
r=0
k=0
lstr=""
while (sseq != ''):
      sseq = file.readline()
      k=k+1
      kl= str(sseq)
      lst= kl.rstrip()
      if not (sseq == ''):
       if (k>8):
           if (lstr == lst):
             if (r < 9):
               list = list + '<Host arch="x86_64" OS="redhat"> ssh://%s/ </Host>\n' %lst
               r=r+1
               lstr=lst
             else:
                  r=r+1
             if(r==8):
               r=0

           else:
             list = list + '<Host arch="x86_64" OS="redhat"> ssh://%s/ </Host>\n' %lst
             lstr = lst
             r=r+1


file.close()
print list
nfile = open("/N/u/smaddi2/fix_mapreduce/examples/terasort/pbs_use.xml","rb")
sseq = "test"
newxml = ""
while (sseq != ''):
      sseq = nfile.readline()
      if ("replaceme" in sseq):
           
          newxml = newxml + list 
      elif ("replace1me" in sseq):
          newxml = newxml + " <MasterAddress>tcp://%s:80011/</MasterAddress>"%hnm
      else:   
          newxml = newxml + sseq 
nfile.close()
FILE = open("/N/u/smaddi2/fix_mapreduce/examples/terasort/pbs.xml", "w", 0) 
FILE.writelines(newxml)
