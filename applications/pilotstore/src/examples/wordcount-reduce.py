#!/usr/bin/env python

# Aggregate subtotals of mapping phase
import saga
import sys
sys.path.append("../../../BigJob/")
sys.path.append("../store/")

from pilotstore import *

# for logging
import logging
logging.basicConfig(level=logging.DEBUG)

APPLICATION_NAME="wordcount"

lines, blanklines, sentences, words = 0, 0, 0, 0

if len(sys.argv) !=5:
    print "Usage: " + sys.argv[0] + " <pilot data> <pilot store> <chunk id> <application advert url>"
    sys.exit(-1)


logging.debug("Pilot Data URL: " + sys.argv[1] + " Pilot Store Name: " 
              + sys.argv[2] + " Chunk id: " + sys.argv[3] 
              + " App URL: " + sys.argv[4])

pd = pilot_data.from_advert(saga.url(sys.argv[1]))
ps_name = sys.argv[2]
chunk_id=sys.argv[3]
uuid = str(uuid.uuid1())
app_url = saga.url(sys.argv[4] + "/reduce-" + uuid)

print '-' * 50
total_lines = 0
total_blanklines = 0
total_sentences = 0
total_words = 0
for ps in pd.list_pilot_store():
    if ps.name == sys.argv[2]:
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
                    comp = line.partition(":")
                    if comp[0].strip()=="Lines":
                        total_lines = total_lines + int(comp[2])            
                
                textf.close()
            except IOError:
              print 'Cannot open file %s for reading' % filename
              import sys
              sys.exit(0)
          
logging.debug("Finished counting words")
          
filename = "data/result-reduce-"+uuid+".txt"
result_file = open(filename, 'w')
result_file.write("Lines      : " + str(total_lines)+ '\n')
result_file.write("Blank lines: " + str(total_blanklines)+ '\n')
result_file.write("Sentences  : " + str(total_sentences)+ '\n')
result_file.write("Words      : " + str(total_words)+ '\n')
result_file.close()

print "Creating pilot store for result file"
base_dir = saga.url("file://localhost" + os.getcwd()+"/data")
ps_results = pilot_store("reduce-results", base_dir, pd)
pd.add_pilot_store(ps_results)

for p in pd.list_pilot_store():
    print p
