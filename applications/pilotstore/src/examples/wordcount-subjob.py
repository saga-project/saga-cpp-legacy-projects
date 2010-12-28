#!/usr/bin/python

# count lines, sentences, and words of a text file
# set all the counters to zero
import sys
sys.path.append("../../../BigJob/")
sys.path.append("../store/")

from pilotstore import *

# for logging
import logging
logging.basicConfig(level=logging.DEBUG)

lines, blanklines, sentences, words = 0, 0, 0, 0

if len(sys.argv) !=3:
    print "Usage: " + sys.argv[0] + " <pilot data> <pilot store>"
    sys.exit(-1)

logging.debug("Pilot Data URL: " + sys.argv[1] + " Pilot Store Name: " + sys.argv[2])
pd = pilot_data.from_advert(saga.url(sys.argv[1]))

print '-' * 50

for ps in pd.list_pilot_store():
    logging.debug("open pilot store: " + ps.name)
    for file in ps.list_files():
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
          import sys
          sys.exit(0)
          
logging.debug("Finished counting words")
          
filename = "result.txt"
result_file = open(filename, 'w')
result_file.write("Lines      : " + str(lines)+ '\n')
result_file.write("Blank lines: " + str(blanklines)+ '\n')
result_file.write("Sentences  : " + str(sentences)+ '\n')
result_file.write("Words      : " + str(words)+ '\n')
result_file.close()

print "Creating pilot store for result file"

base_dir = saga.url("file://localhost" + os.getcwd()+"/data")
ps_results = pilot_store("affinity2", base_dir, pd)
pd.add_pilot_store(ps_results)

for p in pd.list_pilot_store():
    print p