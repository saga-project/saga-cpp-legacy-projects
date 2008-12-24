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




class UrlTest{

	int start(String[] argv){
		URL url = null;
		Session session;
		Buffer buf
		Directory dir = null;
		List<URL> lijst;
		File file = null;
		
		
		try {
			url = URLFactory.createURL("file:/tmp/bla");
			session = SessionFactory.createSession();  //session, 
			buf = BufferFactory.createBuffer(); 
			file = FileFactory.createFile


			
			
			
			
		} catch (NotImplementedException e) {
			e.printStackTrace();
		} catch (IncorrectURLException e) {
			e.printStackTrace();
		} catch (AuthenticationFailedException e) {
			e.printStackTrace();
		} catch (AuthorizationFailedException e) {
			e.printStackTrace();
		} catch (PermissionDeniedException e) {
			e.printStackTrace();
		} catch (BadParameterException e) {
			e.printStackTrace();
		} catch (AlreadyExistsException e) {
			e.printStackTrace();
		} catch (DoesNotExistException e) {
			e.printStackTrace();
		} catch (TimeoutException e) {
			e.printStackTrace();
		} catch (NoSuccessException e) {
			System.out.println(e);
		} catch (IncorrectStateException e) {
			e.printStackTrace();
		} 

		System.out.println("Tot Zover");  

		return 0;
	}

	public static void main(String[] argv){
		new UrlTest().start(argv);
	}
}
