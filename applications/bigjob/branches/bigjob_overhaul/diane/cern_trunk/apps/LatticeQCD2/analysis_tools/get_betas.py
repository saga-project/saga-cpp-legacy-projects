import utils;
d={};
import sys
for b in utils.get_betas(sys.argv[1]):
    d[b[0]]=1;
betas = d.keys()
betas.sort()
print betas
print len(betas)


 
