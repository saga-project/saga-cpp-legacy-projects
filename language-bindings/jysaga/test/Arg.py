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

import sys

def main():
    #URL url = null;
	#URL url2 = null;
	#URL url3 = null;
	#Session session;
	#Directory dir = null;
	#List<URL> lijst;
	#File file = null;
	#try:
    try:
        url2= org.ogf.saga.url.URLFactory.createURL("file:/home/paul/Saga/PySaga/sagaText.txt")
        url3= org.ogf.saga.url.URLFactory.createURL("file:/home/paul/Saga/PySaga/sagaCopy.txt")	
        file = org.ogf.saga.file.FileFactory.createFile(url2, org.ogf.saga.namespace.Flags.READ.getValue() )
        file.copy(url3, org.ogf.saga.namespace.Flags.OVERWRITE.getValue())
    except Exception, e:
        System.out.println(e.getMessage())
    print "Tot Zover"  
		
if __name__ == "__main__":
    main()
