#application.py

import sys
#import saga
from org.ogf.saga.buffer import BufferFactory, Buffer
from org.ogf.saga.url import URLFactory, URL
from org.ogf.saga.file import FileFactory, File
import array

def main():
    #session = saga.Session()
    #urlOrgineel  = saga.URL("file:/home/paul/Saga/PySaga/sagaText.txt")
    #urlCopy      = saga.URL("file:/home/paul/Saga/PySaga/sagaCopy.txt")
    #urlCopy      = saga.URL("sagaCopy.txt")
    #
    #file = saga.File(session, urlOrgineel, flags=saga.Flags.read)
    #file.copy(urlCopy, flags=saga.Flags.overwrite);

    urlOrgineel  = URLFactory.createURL("testBestand")
    file = FileFactory.createFile(urlOrgineel) 
    
    b = BufferFactory.createBuffer(255)
    print "type b:" + str(type(b))
    
    file.read(b)
    data =  b.getData()
    print "=============================================================="
    print "type data:" + str(type(data))
    print data
    print data.tostring()
    uData = array.array('c')
    for i in range(255):
        if data[i] < 0:
            uData.append(chr(data[i]+256))
        else:
            uData.append(chr(data[i]))
    print "=============================================================="            
    print "type uData:" + str(type(uData))
    print uData   
    print uData.tostring()

if __name__ == "__main__":
    main()


#################################################################
 #import sagaContext
#import sagaSession
   #for x in cont:
   #     print x
   
 
   #s = SessionFactory.createSession()
   # print "session:", s
   #sp = s.clone()
   # print "sessionClone:", sp
   #rij = s.listContexts()
   # print "rij:", rij, " len:", len(rij)
   #s.addContext(ContextFactory.createContext("ftp"))
    
   #rij = s.listContexts()
   # print "rij:", rij, " len:", len(rij)
   # print rij[0]
   #s.__class__
   #b = Blar()
   #b.session = s
   #bp = b.cloner()
   #print b.session, b
   #print bp.session, bp
   #f= FileFactory.createFile(session, url, flags)

#from org.ogf.saga.url import URLFactory, URL
#from org.ogf.saga.error import BadParameterException
#from org.ogf.saga.error import NoSuccessException
#from org.ogf.saga.error import NotImplementedException

#   try:
#           url = URLFactory.createURL("file:/c:/sagatest/sagaText.txt")
#   except (BadParameterException, NoSuccessException, NotImplementedException), e:
#       print e
#
#   print "done\n"

def cloner(obj):
    return obj.clone() 

class Blar:
    session = None
    
    def cloner(self):
        if not self.session==None:
            b = Blar()
            b.session = self.session.clone()
            return b    

