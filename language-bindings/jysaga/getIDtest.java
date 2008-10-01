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

class getIDtest{

	int start(String[] argv){
		URL url = null;
		URL url2 = null;
		URL url3 = null;
		Session session;
		Directory dir = null;
		List<URL> lijst;
		File file = null;
		
		try {

			// By obtaining a list of all public methods, both declared and inherited.
			//Class cls = URLFactory.class;
			//java.lang.reflect.Method[] methods;			
			//methods = cls.getMethods();
		    	//for (int i=0; i<methods.length; i++) {
			//	System.out.println( methods[i].getName() );        			
				//Class returnType = methods[i].getReturnType();
        		//	System.out.println("\t" + methods[i].getReturnType().toString());
			//	//Class[] paramTypes = methods[i].getParameterTypes();
			//        System.out.println( "\t" + methods[i].getParameterTypes().toString() );
			//	//process(methods[i]);
    			//}

			url = URLFactory.createURL("file:/home/paul/Saga");
			
			//url2= URLFactory.createURL("file:/home/paul/Saga/PySaga/sagaText.txt");
			//url3= URLFactory.createURL("file:/home/paul/Saga/PySaga/sagaCopy.txt");


			//session = SessionFactory.createSession();  //session, 
			dir = FileFactory.createDirectory(url,Flags.READ.getValue());
			System.out.println(dir.getId());			

			//lijst = dir.list("");
			//for (int i=0; i<lijst.size(); i++){
			//	System.out.println(lijst.get(i).getURL());
			//	
			//}
			
			//file = FileFactory.createFile(url2, Flags.READ.getValue() );
			//file.copy(url3, Flags.OVERWRITE.getValue());
			
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
//		} catch (IncorrectStateException e) {
//			e.printStackTrace();
		} 

		System.out.println("Tot Zover");  

		return 0;
	}

	public static void main(String[] argv){
		new getIDtest().start(argv);
	}
}
