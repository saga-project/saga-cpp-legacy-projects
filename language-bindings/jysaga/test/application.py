import java.util.List;

import org.ogf.saga.url.URLFactory;
import org.ogf.saga.url.URL;
import org.ogf.saga.error.AlreadyExistsException;
import org.ogf.saga.error.AuthenticationFailedException;
import org.ogf.saga.error.AuthorizationFailedException;
import org.ogf.saga.error.BadParameterException;
import org.ogf.saga.error.DoesNotExistException;
import org.ogf.saga.error.IncorrectStateException;
import org.ogf.saga.error.IncorrectURLException;
import org.ogf.saga.error.NoSuccessException;
import org.ogf.saga.error.NotImplementedException;
import org.ogf.saga.error.PermissionDeniedException;
import org.ogf.saga.error.TimeoutException;
import org.ogf.saga.buffer.Buffer;
import org.ogf.saga.buffer.BufferFactory;
import org.ogf.saga.file.Directory;
import org.ogf.saga.file.File;
import org.ogf.saga.file.FileFactory;
import org.ogf.saga.namespace.Flags;
import org.ogf.saga.session.Session;
import org.ogf.saga.session.SessionFactory;

#application.py
import sagaContext
import sagaSession
     
def main():
   c = sagaContext.Context("ftp")
   s = sagaSession.Session()
   s.add_context(c)
   
   cont = s.list_contexts()
   print cont

if __name__ == "__main__":
    main()


#################################################################
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

def cloner(obj):
    return obj.clone() 

class Blar:
    session = None
    
    def cloner(self):
        if not self.session==None:
            b = Blar()
            b.session = self.session.clone()
            return b    

