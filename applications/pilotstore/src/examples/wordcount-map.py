#!/usr/bin/env python

# count lines, sentences, and words of a text file
# set all the counters to zero
#
# Arguments:
# #1 Pilot Data URL
# #2 Pilot Store Name
# #3 Chunk ID
# #4 URL to application root dir in advert service

import sys
import saga
import uuid
sys.path.append("../../../BigJob/")
sys.path.append("../store/")

from pilotstore import *

# for logging
import logging
logging.basicConfig(level=logging.DEBUG)

lines, blanklines, sentences, words = 0, 0, 0, 0

if len(sys.argv) !=5:
    print "Usage: " + sys.argv[0] + " <pilot data> <pilot store> <chunk id> <application advert url>"
    sys.exit(-1)

logging.debug("Pilot Data URL: " + sys.argv[1] + " Pilot Store Name: " 
              + sys.argv[2] + "\n Chunk id: " + sys.argv[3]  
              + " App URL: " + sys.argv[4])

pd = pilot_data.from_advert(saga.url(sys.argv[1]))
ps_name = sys.argv[2]
chunk_id=sys.argv[3]

uuid = str(uuid.uuid1())
app_url = saga.url(sys.argv[4] + "/map-" + uuid)

print '-' * 50

for ps in pd.list_pilot_store():
    if ps.name == ps_name:
        logging.debug("open pilot store: " + ps.name)
        for file in ps.list_files_for_chunk(int(chunk_id)):
            try:
                # use a text file you have, or google for this one ...
                saga_url = saga.url(file.get_string())
                filename = saga_url.path
                logging.debug("open file: " + filename)      
                textf = open(filename, 'r')
                   
                # reads one line at a time
                for line in textf:
                    print line,   # test
                    lines += 1
              
                    if line.startswith('\n'):
                        blanklines += 1
                    else:
                        # assume that each sentence ends with . or ! or ?
                        # so simply count these characters
                        sentences += line.count('.') + line.count('!') + line.count('?')
                        
                        # create a list of words
                        # use None to split at any whitespace regardless of length
                        # so for instance double space counts as one space
                        tempwords = line.split(None)
                        print tempwords  # test
                        
                        # word total count
                        words += len(tempwords)
                textf.close()
            except IOError:
              print 'Cannot open file %s for reading' % filename
              
          
logging.debug("Finished counting words")
          
filename = "data/result-map-"+uuid+".txt"
result_file = open(filename, 'w')
result_file.write("Lines      : " + str(lines)+ '\n')
result_file.write("Blank lines: " + str(blanklines)+ '\n')
result_file.write("Sentences  : " + str(sentences)+ '\n')
result_file.write("Words      : " + str(words)+ '\n')
result_file.close()

print "Creating pilot store for result file"
ps_name = "map-result-"+uuid
base_dir = saga.url("file://localhost" + os.getcwd()+"/data")
ps_results = pilot_store(ps_name, base_dir, pd)
ps_results.add_file(saga.url("file://localhost" + os.getcwd()+"/" + filename))
pd.add_pilot_store(ps_results)

print "Writing to advert directory: " + app_url.get_string()
app_dir = saga.advert.directory(app_url, saga.advert.Create | 
                                         saga.advert.CreateParents | 
                                         saga.advert.ReadWrite)
app_dir.set_attribute("ps", ps_name)
        
for p in pd.list_pilot_store():
    print p
