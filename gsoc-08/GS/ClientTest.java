import org.apache.axis.client.Service;
import org.apache.axis.client.Call;
import javax.xml.namespace.QName;
import java.io.*;
import java.util.*;


public class ClientTest{
public static void main(String[] args) {
	try {
	String endpoint = "http://localhost:8080/axis/GlueingService.jws";
	Service service = new Service();
	Call call = (Call) service.createCall();
		//call.setOperationName(new QName(endpoint, "runSAGA"));
	call.setOperationName(new QName(endpoint, "createJob"));
	call.setTargetEndpointAddress(new java.net.URL(endpoint));

	//Integer ret = (Integer) call.invoke(new Object[] { new Integer(15), new 
	//Integer(6) });
	//Integer ret = (Integer) call.invoke(new Object (void));
	String temp = "14:14";	
	//JobService js = (JobService)call.invoke(new Object[] {new URL("https://localhost:18443/gridsam/services/gridsam")});
	System.out.println(temp);
	String id = (String) call.invoke(new Object[] {new String("gridsam"), new String("add"), new String(temp)});
	//System.out.println(id+ " hmm" );
	call.setOperationName(new QName(endpoint, "run"));
	String ret = (String) call.invoke(new Object[] {new String(id)});
	System.out.println(ret);
	call.setOperationName(new QName(endpoint, "monitor"));
	String mon = (String) call.invoke(new Object[] {new String(id)});
	System.out.println(mon); 
	/*call.setOperationName(new QName(endpoint, "replicate"));
	
	call.setTargetEndpointAddress(new java.net.URL(endpoint));
	String ret = (String) call.invoke(new Object[] {new String("hello")});
	System.out.println(ret);*/
		//String ret = (String) call.invoke(new Object[] {new String("gridsam"), new String("/bin/echo"), new String(temp)});
		//System.out.println("The Service results are \n = " + ret);
	/*
	int result;
	//prepare buffers for process output and error streams
    	String line="";
	String s="";
	try
    		{
			Runtime.getRuntime().exec("sh /home/ahsan/yasir/test/test.sh");
    			Process proc=Runtime.getRuntime().exec("echo $ABC");
    			//create thread for reading inputStream (process' stdout)
			BufferedReader br = new BufferedReader (new InputStreamReader(proc.getInputStream()));
			BufferedReader br_err = new BufferedReader (new InputStreamReader(proc.getErrorStream()));
    			result=proc.waitFor();
    
    			if (result!=0) 
        		{
        			System.out.println("Process  returned non-zero value:"+result);
        			//System.out.println("Process output:\n"+out.toString());
        			//System.out.println("Process error:\n"+err.toString());
        		}
    			else
        		{
        			System.out.println("Process  executed successfully");
				while((line=br.readLine()) != null) {
    				s += line;
  				}
        			System.out.println("Process output:\n"+s);
        			//System.out.println("Process error:\n"+err.toString());
        		}
    		}
		catch (Exception e)
    		{
    			System.out.println("Error executing ");
    			e.printStackTrace();
    			throw e;
    		}*/

	} catch (Exception e) {
		System.err.println("Execution failed. Exception: " + e);
	}
}
}
