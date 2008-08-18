import java.io.*;
import java.util.*;
import org.ogf.saga.URL;
import org.ogf.saga.context.Context;
import org.ogf.saga.job.Job;
import org.ogf.saga.job.JobDescription;
import org.ogf.saga.job.JobFactory;
import org.ogf.saga.job.JobService;
import org.ogf.saga.monitoring.Callback;
import org.ogf.saga.monitoring.Metric;
import org.ogf.saga.monitoring.Monitorable;
import org.ogf.saga.buffer.*;
import org.ogf.saga.context.*;
import org.ogf.saga.file.Directory;
import org.ogf.saga.file.FileFactory;
import org.ogf.saga.logicalfile.LogicalDirectory;
import org.ogf.saga.namespace.NSEntry;



import org.ogf.saga.context.ContextFactory;
import org.ogf.saga.logicalfile.LogicalFile;
import org.ogf.saga.logicalfile.LogicalFileFactory;
import org.ogf.saga.namespace.Flags;
import org.ogf.saga.session.Session;
import org.ogf.saga.session.SessionFactory;

import org.ogf.saga.namespace.NSDirectory;
import org.ogf.saga.namespace.NSFactory;
import org.ogf.saga.file.IOVec;
import org.ogf.saga.file.SeekMode;
import org.ogf.saga.job.JobSelf;

import org.ogf.saga.task.*;

public class GlueingService
{
	public String runSAGA(String adapter,String app, String arg, String jobId) throws Exception
	{
	
		String line="";
		String s="Error";int result =0;
		Process proc = Runtime.getRuntime().exec("$SAGA_LOCATION/bin/run_saga_app glue.service.SAGAJobSubmit "+adapter+" "+app+" "+arg+" "+jobId);
		//proc.waitFor();
		result=proc.waitFor();
		BufferedReader br = new BufferedReader (new InputStreamReader(proc.getInputStream()));
		BufferedReader br_err = new BufferedReader (new InputStreamReader(proc.getErrorStream()));
		if (result!=0) 
        	{
			s = "Process  returned non-zero value:"+result;
        		System.out.println("Process  returned non-zero value:"+result);
        		
       		 }
   		else
       		 {
			s="";
        		System.out.println("Process  executed successfully");
			while((line=br.readLine()) != null) {
    			s += line+":";
  			}
        		System.out.println("Process output:\n"+s);
        	
        	}
		return s;
	}
	public JobService createJobService(URL url)
	{
		JobService js=null;
		try{
		js = JobFactory.createJobService(url);
		}catch(Exception e){}
		return js;
	}

	public String createJob(String adaptor, String app, String arg)
	{
		String jobId ="";
	try {
		System.setProperty("JobService.adaptor.name", adaptor);

            JobService js = JobFactory.createJobService(new URL("https://localhost:18443/gridsam/services/gridsam"));

            JobDescription jd = JobFactory.createJobDescription();
            jd.setAttribute(JobDescription.EXECUTABLE, app);
            jd.setVectorAttribute(JobDescription.ARGUMENTS,
                    new String[] { arg });
            jd.setAttribute(JobDescription.OUTPUT, "uname.out");
            String host = java.net.InetAddress.getLocalHost()
                    .getCanonicalHostName();
            jd.setVectorAttribute(JobDescription.FILETRANSFER,
                    new String[] { "ftp://" + host
                            + ":12346/uname.out < uname.out" });
            Job job = js.createJob(jd);
		jobId = job.getId();
		logIt(jobId,adaptor,app,arg);
		}catch(Exception e){}
		
		return jobId;
	}

	private void logIt(String id, String adaptor, String app, String arg)
	{
		try {
			BufferedWriter out = new BufferedWriter(new FileWriter("GlueLogs.txt",true));
			out.write(id+","+adaptor+","+app+","+arg);
			out.newLine();
			out.close();
		}
		catch (IOException e)
		{
			System.out.println("Exception ");		
		}

	}
	
	public String run(String jobId)
	{
		String monitor = "";
		String adaptor = null,app=null, arg = null;
		try{
		BufferedReader input =  new BufferedReader(new FileReader(new File("GlueLogs.txt")));
		String line=null;
		while (( line = input.readLine()) != null){
          			String str[] = line.split(",");
				if(str[0].equals(jobId))
				{
					adaptor=str[1];
					app = str[2];
					arg = str[3];
					break;
				}
        		}
			System.out.println(adaptor+" "+app+" "+arg+" hellooo");
			monitor = runSAGA(adaptor,app,arg,jobId);
		}catch(Exception e){}
		try {
			BufferedWriter out = new BufferedWriter(new FileWriter("GlueLogs.txt",true));
			out.write(jobId+"monitor_status,"+monitor);
			out.newLine();
			out.close();
		}
		catch (IOException e)
		{
			System.out.println("Exception ");		
		}
		return "The Job with Id = "+jobId+" is submitted to gridSAM";

	}

	public String monitor(String jobId)
	{
		String monitor = "";
		
		try{
		BufferedReader input =  new BufferedReader(new FileReader(new File("GlueLogs.txt")));
		String line=null;
		while (( line = input.readLine()) != null){
          			String str[] = line.split(",");
				if(str[0].equals(jobId+"monitor_status"))
				{
					monitor = str[2];
					break;
				}
        		}

		}catch(Exception e){}
		monitor = monitor.replaceAll(":","\n");
		int index = monitor.indexOf("ftp\n");
		String monitor1 = monitor.substring(0,index);
		String monitor2 = monitor.substring(index);
		monitor2 = monitor2.replaceAll("\n",":");
		return monitor1 + monitor2;
	}
	
	public String getMetric(String jobId, String metricName)
	{
		String value =null;
			String jobidt = jobId+"monitor";
		try{
		BufferedReader input =  new BufferedReader(new FileReader(new File("GlueLogs.txt")));
		String line=null;
		while (( line = input.readLine()) != null){
          			String str[] = line.split(",");
				if(str[0].equals(jobidt))
				{
					if(metricName.equals("MEMORYUSE"))
					{
						value=str[2];
						break;
					}
					else if(metricName.equals("CPUTIME"))
					{
						value = str[4];
						break;
					}
					else if(metricName.equals("PERFORMANCE"))
					{
						value = str[6];
						break;
					}
					else{ value = "Unknown Metric Name"; break;}
				}
        		}
			
			
		}catch(Exception e){}
			
		return value;

	}

	public String replicate(String url)
	{
		String status = "hello";
		try {
            Session session = SessionFactory.createSession(true);
            
            Context ftpContext = ContextFactory.createContext("ftp");
            session.addContext(ftpContext);
            
            LogicalFile f = LogicalFileFactory.createLogicalFile(session,
                    new URL("ftp://localhost:12345/incoming/mail.txt"), Flags.CREATE.or(Flags.READWRITE));
            //f.addLocation(new URL("ftp://localhost:12345/incoming/mail.txt"));
            f.replicate(new URL("file://localhost/home/yasir/Desktop/abc.txt"));
            f.close();
        } catch (Throwable t) {
            System.out.println("ouch..." + t);
            t.printStackTrace();
        }

		/*try {
            Session session = SessionFactory.createSession(true);
            
            Context ftpContext = ContextFactory.createContext("ftp");
            session.addContext(ftpContext);
            // Possibly add other contexts ...
            
            NSDirectory entry = NSFactory.createNSDirectory(session, new URL("."),
                    Flags.NONE.getValue());
            entry.copy(new URL("ftp://localhost:12346/mail.txt"), new URL("ftp://localhost:12345/incoming/smail.txt"));
            System.out.println("copied!");
        } catch (Throwable t) {
            System.out.println("ouch..." + t);
            t.printStackTrace();
        }*/
		return url;
		
	}
	
	//org.ogf.saga.buffer.Buffer	All not implemented
	void close(Buffer b)
	{}
	void close(Buffer b, float timeInSeconds){}
	byte[] getData(Buffer b){ return new byte[]{};}
	int getSize(Buffer b){ return 0;}
	void setData(Buffer b){}
	void setSize(Buffer b){}
	void setSize(Buffer b, int size){}

	//org.ogf.saga.buffer.BufferFactory          All functions are dummy and are not implemented for use
	public Buffer createBuffer(BufferFactory bf)
	{ 
		Buffer b = null;
		try{
			b = BufferFactory.createBuffer();
		}catch(Exception e){}
		return b;
	}
	public Buffer createBuffer(BufferFactory bf, int size)
	{ 
		Buffer b = null;
		try{
			b = BufferFactory.createBuffer();
		}catch(Exception e){}
		return b;
	}
	public Buffer createBuffer(BufferFactory bf, byte[] data)
	{ 
		Buffer b = null;
		try{
			b = BufferFactory.createBuffer();
		}catch(Exception e){}
		return b;
	}
	

	//org.ogf.saga.context.Context          All the functions are dummy and not implemented for use
	public void setDefaults(Context c){}

	//org.ogf.saga.context.ContextFactory

	public Context createContext(ContextFactory cf)
	{ 
		Context c = null;
		try{
			c = ContextFactory.createContext();
		}catch(Exception e){}
		return c;
	}
	public Context createContext(ContextFactory cf, String type)
	{ 
		Context c = null;
		try{
			c = ContextFactory.createContext();
		}catch(Exception e){}
		return c;
	}

	//import org.ogf.saga.file.Directory                 All the functions are dummy and not implemented for use
	
	public Task getSize(Directory d, TaskMode mode, URL name) 
	{ 
		Task t = null;
		try{
		JobService js = JobFactory.createJobService(new URL(""));

            	JobDescription jd = JobFactory.createJobDescription();
		t = (Task)js.createJob(jd);
		}catch(Exception e){}
		return t; 
	}
	public Task getSize(Directory d, TaskMode mode, URL name, int flags) 
	{ 
		Task t = null;
		try{
		JobService js = JobFactory.createJobService(new URL(""));

            	JobDescription jd = JobFactory.createJobDescription();
		t = (Task)js.createJob(jd);
		}catch(Exception e){}
		return t; 
	}
	public long getSize(Directory d, URL name){ return 0;}
	public long getSize(Directory d, URL name, int flags){ return 0;}
	public Task isFile(Directory d, TaskMode mode, URL name) 
	{ 
		Task t = null;
		try{
		JobService js = JobFactory.createJobService(new URL(""));

            	JobDescription jd = JobFactory.createJobDescription();
		t = (Task)js.createJob(jd);
		}catch(Exception e){}
		return t; 
	}
	public boolean isFile(URL name){ return false;}
	public Task openDirectory(Directory d, TaskMode mode, URL name) 
	{ 
		Task t = null;
		try{
		JobService js = JobFactory.createJobService(new URL(""));

            	JobDescription jd = JobFactory.createJobDescription();
		t = (Task)js.createJob(jd);
		}catch(Exception e){}
		return t; 
	}
	public Task openDirectory(Directory d, TaskMode mode, URL name, int flags) 
	{ 
		Task t = null;
		try{
		JobService js = JobFactory.createJobService(new URL(""));

            	JobDescription jd = JobFactory.createJobDescription();
		t = (Task)js.createJob(jd);
		}catch(Exception e){}
		return t; 
	}
	public Directory openDirectory (Directory d, URL name)
	{
		try{  
		d = FileFactory.createDirectory(name);}catch(Exception e){}
		return d;
	}
	public Directory openDirectory (Directory d, URL name, int flags)
	{
		try{  
		d = FileFactory.createDirectory(name);}catch(Exception e){}
		return d;
	}
	public Task openFile(Directory d, TaskMode mode, URL name) 
	{ 
		Task t = null;
		try{
		JobService js = JobFactory.createJobService(new URL(""));

            	JobDescription jd = JobFactory.createJobDescription();
		t = (Task)js.createJob(jd);
		}catch(Exception e){}
		return t; 
	}
	public Task openFile(Directory d, TaskMode mode, URL name, int flags) 
	{ 
		Task t = null;
		try{
		JobService js = JobFactory.createJobService(new URL(""));

            	JobDescription jd = JobFactory.createJobDescription();
		t = (Task)js.createJob(jd);
		}catch(Exception e){}
		return t; 
	}
	public org.ogf.saga.file.File openFile(Directory d, URL name)
	{
		org.ogf.saga.file.File f = null;
		try{  
		f = FileFactory.createFile(name);}catch(Exception e){}
		return f;
	}
	public org.ogf.saga.file.File openFile(Directory d, URL name, int flags)
	{
		org.ogf.saga.file.File f = null;
		try{  
		f = FileFactory.createFile(name);}catch(Exception e){}
		return f;
	}
	public Task openFileInputStream(Directory d, TaskMode mode, URL name) 
	{ 
		Task t = null;
		try{
		JobService js = JobFactory.createJobService(new URL(""));

            	JobDescription jd = JobFactory.createJobDescription();
		t = (Task)js.createJob(jd);
		}catch(Exception e){}
		return t; 
	}
	public org.ogf.saga.file.FileInputStream openFileInputStream(URL name)
	{
		org.ogf.saga.file.FileInputStream f = null;
		try{  
		f = FileFactory.createFileInputStream(name);}catch(Exception e){}
		return f;
	}
	public Task openFileOutputStream(Directory d, TaskMode mode, URL name) 
	{ 
		Task t = null;
		try{
		JobService js = JobFactory.createJobService(new URL(""));

            	JobDescription jd = JobFactory.createJobDescription();
		t = (Task)js.createJob(jd);
		}catch(Exception e){}
		return t; 
	}
	public org.ogf.saga.file.FileOutputStream openFileOutputStream(URL name)
	{
		org.ogf.saga.file.FileOutputStream f = null;
		try{  
		f = FileFactory.createFileOutputStream(name);}catch(Exception e){}
		return f;
	}
	public org.ogf.saga.file.FileOutputStream openFileOutputStream(URL name, boolean append)
	{
		org.ogf.saga.file.FileOutputStream f = null;
		try{  
		f = FileFactory.createFileOutputStream(name);}catch(Exception e){}
		return f;
	}

	
                
	//import org.ogf.saga.file.File                 All the functions are dummy and not implemented for use
	
	//implemented
	public long getSize(org.ogf.saga.file.File f)
	{
		long length=0;
		try{
		length =  f.getSize();
		}catch (Exception e){}
		return length;
	}
	
	//implemented
	public Task getSize(org.ogf.saga.file.File f,TaskMode mode)
	{
		Task t = null;
		try{
			t = f.getSize(TaskMode.TASK);
		 
		}catch(Exception e){}
		return t;
	}
	//implemented
	public List modesE(org.ogf.saga.file.File f)
	{
		List l  =null;
		try{
		l = f.modesE();}catch(Exception e){}
		return l;
	}
	//implemented
	public Task modesE(org.ogf.saga.file.File f,int mode)
	{
		Task t = null;
		try{
			t = f.modesE(TaskMode.TASK);
		 
		}catch(Exception e){}
		return t;
	}
	//implemented
	public int read(org.ogf.saga.file.File f, Buffer b)
	{
		int a = 0;
		try {
		a = f.read(b);}catch(Exception e){}
		return a;
	}
	
	//implemented
	public int read(org.ogf.saga.file.File f, Buffer b, int len)
	{
		int a = 0;
		try {
		a = f.read(b,len);}catch(Exception e){}
		return a;
	}

	//implemented
	public int read(org.ogf.saga.file.File f, Buffer b,int offset, int len)
	{
		int a = 0;
		try {
		a = f.read(b,offset,len);}catch(Exception e){}
		return a;
	}
	//implemented
	public Task read(org.ogf.saga.file.File f,int mode, Buffer b)
	{
		Task t = null;
		try {
		t = f.read(TaskMode.TASK,b);}catch(Exception e){}
		return t;
	}
	//implemented
	public Task read(org.ogf.saga.file.File f,int mode, Buffer b, int len)
	{
		Task t = null;
		try {
		t = f.read(TaskMode.TASK,b, len);}catch(Exception e){}
		return t;
	}
	//implemented
	public Task read(org.ogf.saga.file.File f,int mode, Buffer b,int offset, int len)
	{
		Task t = null;
		try {
		t = f.read(TaskMode.TASK,b,offset, len);}catch(Exception e){}
		return t;
	}
	//implemented
	public int readE(org.ogf.saga.file.File f,String emode, String spec, Buffer b)
	{
		int a = 0;
		try{
		f.readE(emode,spec,b);}catch(Exception e){}
		return a;
	}
	//implemented
	public Task readE(org.ogf.saga.file.File f,int mode,String emode, String spec, Buffer b)
	{
		Task t = null ;
		try{
		t  = f.readE(TaskMode.TASK,emode,spec,b);}catch(Exception e){}
		return t;
	}
	//implemented 
	public int readP(org.ogf.saga.file.File f, String pattern, Buffer b)
	{
		int a = 0;
		try{
		f.readP(pattern,b);}catch(Exception e){}
		return a;
	}
	//implemented 
	public int readP(org.ogf.saga.file.File f,int mode, String pattern, Buffer b)
	{
		int a = 0;
		try{
		f.readP(TaskMode.TASK,pattern,b);}catch(Exception e){}
		return a;
	}
	
	public void readV(org.ogf.saga.file.File f, IOVec[] iovecs){}
	public long seek(org.ogf.saga.file.File f, long offset, SeekMode whence)
	{
		long a = 0;
		try{
			a = f.seek(offset, whence);}catch(Exception e){}
		return a;
	}
	public Task seek(org.ogf.saga.file.File f,TaskMode mode, long offset, SeekMode whence)
	{
		Task t = null;
		try{
			t = f.seek(mode, offset, whence);}catch(Exception e){}
		return t;
	}
	public int sizeE(org.ogf.saga.file.File f, String emode, String spec)
	{
		int a = 0;
		try{
			a = f.sizeE(emode, spec);}catch(Exception e){}
		return a;
	}
	public Task sizeE(org.ogf.saga.file.File f,TaskMode mode, String emode, String spec)
	{
		Task t = null;
		try{
			t = f.sizeE(mode, emode, spec);}catch(Exception e){}
		return t;
	}
	public int sizeP(org.ogf.saga.file.File f,String pattern)
	{
		int a = 0;
		try{
			a = f.sizeP(pattern);}catch(Exception e){}
		return a;
	}
	public Task sizeP(org.ogf.saga.file.File f,TaskMode mode, String pattern)
	{
		Task t = null;
		try{
			t = f.sizeP(mode, pattern);}catch(Exception e){}
		return t;
	}
	public int write(org.ogf.saga.file.File f, Buffer b)
	{
		int a = 0;
		try {
		a = f.write(b);}catch(Exception e){}
		return a;
	}
	public int write(org.ogf.saga.file.File f, Buffer b, int len)
	{
		int a = 0;
		try {
		a = f.write(b,len);}catch(Exception e){}
		return a;
	}
	public int write(org.ogf.saga.file.File f, Buffer b,int offset, int len)
	{
		int a = 0;
		try {
		a = f.write(b,offset,len);}catch(Exception e){}
		return a;
	}
	//---------------------
	public Task write(org.ogf.saga.file.File f,int mode, Buffer b)
	{
		Task t = null;
		try {
		t = f.write(TaskMode.TASK,b);}catch(Exception e){}
		return t;
	}
	//implemented
	public Task write(org.ogf.saga.file.File f,int mode, Buffer b, int len)
	{
		Task t = null;
		try {
		t = f.write(TaskMode.TASK,b, len);}catch(Exception e){}
		return t;
	}
	//implemented
	public Task write(org.ogf.saga.file.File f,int mode, Buffer b,int offset, int len)
	{
		Task t = null;
		try {
		t = f.write(TaskMode.TASK,b,offset, len);}catch(Exception e){}
		return t;
	}
	//implemented
	public int writeE(org.ogf.saga.file.File f,String emode, String spec, Buffer b)
	{
		int a = 0;
		try{
		f.writeE(emode,spec,b);}catch(Exception e){}
		return a;
	}
	//implemented
	public Task writeE(org.ogf.saga.file.File f,int mode,String emode, String spec, Buffer b)
	{
		Task t = null ;
		try{
		t  = f.writeE(TaskMode.TASK,emode,spec,b);}catch(Exception e){}
		return t;
	}
	//implemented 
	public int writeP(org.ogf.saga.file.File f, String pattern, Buffer b)
	{
		int a = 0;
		try{
		 a = f.writeP(pattern,b);}catch(Exception e){}
		return a;
	}
	//implemented 
	public int writeP(org.ogf.saga.file.File f,int mode, String pattern, Buffer b)
	{
		int a = 0;
		try{
		f.writeP(TaskMode.TASK,pattern,b);}catch(Exception e){}
		return a;
	}
	
	public void writeV(org.ogf.saga.file.File f, IOVec[] iovecs){}
	public Task writeV(org.ogf.saga.file.File f,int mode, IOVec[] iovecs)
	{
		Task a = null;
		try{
		a = f.writeV(TaskMode.TASK,iovecs);}catch(Exception e){}
		return a;
	}

	//org.ogf.saga.job.Job

	public void checkpoint(Job j)
	{
		try{
		j.checkpoint();}catch(Exception e){}
	}
	public Task checkpoint(Job j, TaskMode mode)
	{
		Task a = null;
		try{
		a = j.checkpoint(mode);}catch(Exception e){}
		return a;
	}
	public JobDescription getJobDescription(Job j)
	{
		JobDescription a = null;
		try{
		a = j.getJobDescription();}catch(Exception e){}
		return a;
		
	}
	public Task getDescription(Job j,TaskMode mode)
	{
		Task a = null;
		try{
		a = j.getJobDescription(mode);}catch(Exception e){}
		return a;
	}
	public InputStream getStderr(Job j)
	{
		InputStream a = null;
		try{
		a = j.getStderr();}catch(Exception e){}
		return a;
	}
	public Task getStderr(Job j,TaskMode mode)
	{
		Task a = null;
		try{
		a = j.getStderr(mode);}catch(Exception e){}
		return a;
	}
	public OutputStream getStdin(Job j)
	{
		OutputStream a = null;
		try{
		a = j.getStdin();}catch(Exception e){}
		return a;
	}
	public Task getStdin(Job j,TaskMode mode)
	{
		Task a = null;
		try{
		a = j.getStdin(mode);}catch(Exception e){}
		return a;
	}
	public InputStream getStdout(Job j)
	{
		InputStream a = null;
		try{
		a = j.getStdout();}catch(Exception e){}
		return a;
	}
	public Task getStdout(Job j,TaskMode mode)
	{
		Task a = null;
		try{
		a = j.getStdout(mode);}catch(Exception e){}
		return a;
	}
	public void migrate(Job j, JobDescription jd)
	{
		try{
		j.migrate(jd);}catch(Exception e){}
	}
	public Task migrate(Job j,TaskMode mode, JobDescription jd)
	{
		Task a = null;
		try{
		a = j.migrate(mode,jd);}catch(Exception e){}
		return a;
	}
	public void resume(Job j)
	{
		try{
		j.resume();}catch(Exception e){}
	}
	public Task resume(Job j,TaskMode mode)
	{
		Task a = null;
		try{
		a = j.resume(mode);}catch(Exception e){}
		return a;
	}
	public void signal(Job j,int signum)
	{
		try{
		j.signal(signum);}catch(Exception e){}
	}
	public Task resume(Job j,TaskMode mode, int signum)
	{
		Task a = null;
		try{
		a = j.signal(mode, signum);}catch(Exception e){}
		return a;
	}
	public void suspend(Job j)
	{
		try{
		j.suspend();}catch(Exception e){}
	}
	public Task suspend(Job j,TaskMode mode)
	{
		Task a = null;
		try{
		a = j.suspend(mode);}catch(Exception e){}
		return a;
	}

	//org.ogf.saga.job.JobService

	public Task createJob(JobService js, TaskMode mode, JobDescription jd)
	{
		Task a = null;
		try{
		a = js.createJob(mode,jd);}catch(Exception e){}
		return a;
	}
	public Job getJob(JobService js,String jobId)
	{
		Job j =null;
		try{
			j = js.getJob(jobId);}catch(Exception e){}
		return j;
	}
	public Task getJob(JobService js, TaskMode mode, String jobId)
	{
		Task a = null;
		try{
		a = js.getJob(mode,jobId);}catch(Exception e){}
		return a;
	}
	public JobSelf getSelf(JobService js)
	{
		JobSelf j =null;
		try{
			j = js.getSelf();}catch(Exception e){}
		return j;
	}
	public Task getSelf(JobService js, TaskMode mode)
	{
		Task a = null;
		try{
		a = js.getSelf(mode);}catch(Exception e){}
		return a;
	}
	
	public List list(JobService js)
	{
		List l = null;
		try{
		l = js.list();}catch(Exception e){};
		return l;
	}
	public Task list(JobService js, TaskMode mode)
	{
		Task a = null;
		try{
		a = js.list(mode);}catch(Exception e){}
		return a;
	}
	public Job runJob(JobService js, String command)
	{
		Job j =null;
		try{
			j = js.runJob(command);}catch(Exception e){}
		return j;
	}
	public Job runJob(JobService js, String command, boolean interactive)
	{
		Job j =null;
		try{
			j = js.runJob(command, interactive);}catch(Exception e){}
		return j;
	}
	public Job runJob(JobService js, String command, String host)
	{
		Job j =null;
		try{
			j = js.runJob(command,host);}catch(Exception e){}
		return j;
	}	
	public Task runJob(JobService js, TaskMode mode, String command)
	{
		Task t =null;
		try{
			t = js.runJob(mode,command);}catch(Exception e){}
		return t;
	}
	public Task runJob(JobService js, TaskMode mode, String command, boolean interactive)
	{
		Task t =null;
		try{
			t = js.runJob(mode,command,interactive);}catch(Exception e){}
		return t;
	}
	public Task runJob(JobService js, TaskMode mode, String command, String host)
	{
		Task t =null;
		try{
			t = js.runJob(mode,command, host);}catch(Exception e){}
		return t;
	}
	public Task runJob(JobService js, TaskMode mode, String command, String host, boolean interactive)
	{
		Task t =null;
		try{
			t = js.runJob(mode,command, host, interactive);}catch(Exception e){}
		return t;
	}
	//org.ogf.saga.file.LogicalDirectory
	public List find(LogicalDirectory ld, String namePattern, String[] attrPattern)
	{
		List l = null;
		try{
		l = ld.find(namePattern, attrPattern);}catch(Exception e){};
		return l;
	}
	public List find(LogicalDirectory ld, String namePattern, String[] attrPattern, int flag)
	{
		List l = null;
		try{
		l = ld.find(namePattern, attrPattern,flag);}catch(Exception e){};
		return l;
	}
	public Task find(LogicalDirectory ld,TaskMode mode, String namePattern, String[] attrPattern)
	{
		Task l = null;
		try{
		l = ld.find(mode,namePattern, attrPattern);}catch(Exception e){};
		return l;
	}
	public Task isFile(LogicalDirectory ld,TaskMode mode, URL name)
	{
		Task l = null;
		try{
		l = ld.isFile(mode,name);}catch(Exception e){};
		return l;
	}
	public boolean isFile(LogicalDirectory ld, URL name)
	{
		boolean a = false;
		try{
		a = ld.isFile(name);}catch(Exception e){}
		return a;
	}
	// impllll
	public Task opernLogicalDir(LogicalDirectory ld,TaskMode mode, URL name)
    {
        Task l = null;
        try{
        l = ld.openLogicalDir(mode,name);}catch(Exception e){};
        return l;
    }
    public Task opernLogicalDir(LogicalDirectory ld,TaskMode mode, URL name, int flags)
    {
        Task l = null;
        try{
        l = ld.openLogicalDir(mode,name,flags);}catch(Exception e){};
        return l;
    }
    public LogicalDirectory openLogicalDir(LogicalDirectory ld, URL name)
    {
        LogicalDirectory a = null;
        try{
        a = ld.openLogicalDir(name);}catch(Exception e){}
        return a;
    }
    public LogicalDirectory openLogicalDir(LogicalDirectory ld, URL name, int flags)
    {
        LogicalDirectory a = null;
        try{
        a = ld.openLogicalDir(name,flags);}catch(Exception e){}
        return a;
    }
    public Task opernLogicalFile(LogicalDirectory ld,TaskMode mode, URL name, int flags)
    {
        Task l = null;
        try{
        l = ld.openLogicalFile(mode,name,flags);}catch(Exception e){};
        return l;
    }
    public Task opernLogicalFile(LogicalDirectory ld,TaskMode mode, URL name)
    {
        Task l = null;
        try{
        l = ld.openLogicalFile(mode,name);}catch(Exception e){};
        return l;
    }
    public LogicalFile openLogicalFile(LogicalDirectory ld, URL name)
    {
        LogicalFile a = null;
        try{
        a = ld.openLogicalFile(name);}catch(Exception e){}
        return a;
    }
    public LogicalFile openLogicalFile(LogicalDirectory ld, URL name, int flags)
    {
        LogicalFile  a = null;
        try{
        a = ld.openLogicalFile(name, flags);}catch(Exception e){}
        return a;
    }
   
    //org.ogf.saga.monitoring
    public int addCallback(Monitorable m, String name, Callback cb)
    {
        int a = 0;
        try{
        a = m.addCallback(name,cb);}catch(Exception e){}
        return a;
    }
    public Metric getMetric(Monitorable m, String name)
    {
        Metric a = null;
        try{
        a = m.getMetric(name);}catch(Exception e){}
        return a;
    }
    public String[] listMetrics (Monitorable m)
    {
        String[] a = {};
        try{
        a = m.listMetrics();}catch(Exception e){}
        return a;
    }
    public void removeCallback(Monitorable m, String name, int cookie)
    {
        try{
        m.removeCallback(name,cookie);}catch(Exception e){}
    }
   
    //import org.ogf.saga.namespace.NSDirectory
   
    public Task changeDir(NSDirectory nsdir, TaskMode mode, URL dir)
    {
        Task l = null;
        try{
        l = nsdir.changeDir(mode,dir);}catch(Exception e){};
        return l;
    }
    public void changeDir(NSDirectory nsdir, URL dir)
    {
        try{
        nsdir.changeDir(dir);}catch(Exception e){};
    }
    public void copy(NSDirectory nsdir, String source, URL target)
    {
        try{
        nsdir.copy(source, target);}catch(Exception e){};
    }
    public void copy(NSDirectory nsdir, String source, URL target, int flags)
    {
        try{
        nsdir.copy(source, target, flags);}catch(Exception e){};
    }
    public Task copy(NSDirectory nsdir, TaskMode mode, String source, URL target)
    {
        Task l = null;
        try{
        l = nsdir.copy(mode,source, target);}catch(Exception e){};
        return l;
    }
    public Task copy(NSDirectory nsdir, TaskMode mode, String source, URL target, int flags)
    {
        Task l = null;
        try{
        l = nsdir.copy(mode,source, target, flags);}catch(Exception e){};
        return l;
    }
    public Task copy(NSDirectory nsdir, TaskMode mode, URL source, URL target)
    {
        Task l = null;
        try{
        l = nsdir.copy(mode,source, target);}catch(Exception e){};
        return l;
    }
    public Task copy(NSDirectory nsdir, TaskMode mode, URL source, URL target, int flags)
    {
        Task l = null;
        try{
        l = nsdir.copy(mode,source, target, flags);}catch(Exception e){};
        return l;
    }
    public void copy(NSDirectory nsdir, URL source, URL target)
    {
        try{
        nsdir.copy(source, target);}catch(Exception e){};
    }
    public void copy(NSDirectory nsdir, URL source, URL target, int flags)
    {
        try{
        nsdir.copy(source, target, flags);}catch(Exception e){};
    }
    public Task exists(NSDirectory nsdir, TaskMode mode, URL name)
    {
        Task l = null;
        try{
        l = nsdir.exists(mode,name);}catch(Exception e){};
        return l;
    }
    public boolean exists(NSDirectory nsdir, URL name)
    {
        boolean l = false;
        try{
        l = nsdir.exists(name);}catch(Exception e){};
        return l;
    }
    public List find(NSDirectory nsdir, String pattern)
    {
        List l = null;
        try{
        l = nsdir.find(pattern);}catch(Exception e){};
        return l;
    }
	public List find(NSDirectory nsdir, String pattern, int flags)
    {
        List l = null;
        try{
        l = nsdir.find(pattern, flags);}catch(Exception e){};
        return l;
    }
	public Task find(NSDirectory nsdir, TaskMode mode, String pattern)
    {
        Task l = null;
        try{
        l = nsdir.find(mode,pattern);}catch(Exception e){};
        return l;
    }
	public Task find(NSDirectory nsdir, TaskMode mode, String pattern, int flags)
    {
        Task l = null;
        try{
        l = nsdir.find(mode,pattern, flags);}catch(Exception e){};
        return l;
    }
	public URL getEntry(NSDirectory nsdir, int entry)
    {
        URL l = null;
        try{
        l = nsdir.getEntry(entry);}catch(Exception e){};
        return l;
    }
	public Task getEntry(NSDirectory nsdir,TaskMode mode, int entry)
    {
        Task l = null;
        try{
        l = nsdir.getEntry(mode, entry);}catch(Exception e){};
        return l;
    }
	public int getNumEntries(NSDirectory nsdir)
    {
        int a  = 0;
        try{
        a = nsdir.getNumEntries();}catch(Exception e){};
        return a;
    }
	public Task getNumEntries(NSDirectory nsdir, TaskMode mode)
    {
        Task a  = null;
        try{
        a = nsdir.getNumEntries(mode);}catch(Exception e){};
        return a;
    }
	public Task isDir(NSDirectory nsdir, TaskMode mode, URL name)
    {
        Task a  = null;
        try{
        a = nsdir.isDir(mode, name);}catch(Exception e){};
        return a;
    }
	public boolean isDir(NSDirectory nsdir, URL name)
    {
        boolean a  = false;
        try{
        a = nsdir.isDir(name);}catch(Exception e){};
        return a;
    }
	
	public Task isEntry(NSDirectory nsdir, TaskMode mode, URL name)
    {
        Task a  = null;
        try{
        a = nsdir.isEntry(mode, name);}catch(Exception e){};
        return a;
    }
	public boolean isEntry(NSDirectory nsdir, URL name)
    {
        boolean a  = false;
        try{
        a = nsdir.isEntry(name);}catch(Exception e){};
        return a;
    }
	public Task isLink(NSDirectory nsdir, TaskMode mode, URL name)
    {
        Task a  = null;
        try{
        a = nsdir.isLink(mode, name);}catch(Exception e){};
        return a;
    }
	public boolean isLink(NSDirectory nsdir, URL name)
    {
        boolean a  = false;
        try{
        a = nsdir.isLink(name);}catch(Exception e){};
        return a;
    }
	public void link(NSDirectory nsdir, String source, URL target)
	{
		try{
        	 nsdir.link(source, target);}catch(Exception e){};
	}
	public void link(NSDirectory nsdir, String source, URL target, int flags)
	{
		try{
        	 nsdir.link(source, target, flags);}catch(Exception e){};
	}
	public Task link(NSDirectory nsdir,TaskMode mode, String source, URL target)
	{
		Task t = null;
		try{
        	 t  = nsdir.link(mode, source, target);}catch(Exception e){};
		return t;
	}
	public Task link(NSDirectory nsdir,TaskMode mode, String source, URL target, int flags)
	{
		Task t = null;
		try{
        	 t  = nsdir.link(mode, source, target, flags);}catch(Exception e){};
		return t;
	}
	public Task link(NSDirectory nsdir,TaskMode mode, URL source, URL target)
	{
		Task t = null;
		try{
        	 t  = nsdir.link(mode, source, target);}catch(Exception e){};
		return t;
	}
	public Task link(NSDirectory nsdir,TaskMode mode, URL source, URL target, int flags)
	{
		Task t = null;
		try{
        	 t  = nsdir.link(mode, source, target, flags);}catch(Exception e){};
		return t;
	}
	public void link(NSDirectory nsdir, URL source, URL target)
	{
		try{
        	 nsdir.link(source, target);}catch(Exception e){};
	}
	public void link(NSDirectory nsdir, URL source, URL target, int flags)
	{
		try{
        	 nsdir.link(source, target);}catch(Exception e){};
	}
	public List list(NSDirectory nsdir)
	{
		List t = null;
		try{
        	 t  = nsdir.list();}catch(Exception e){};
		return t;
	}
	public List list(NSDirectory nsdir, int flags)
	{
		List t = null;
		try{
        	 t  = nsdir.list(flags);}catch(Exception e){};
		return t;
	}
	public List list(NSDirectory nsdir, String pattern)
	{
		List t = null;
		try{
        	 t  = nsdir.list(pattern);}catch(Exception e){};
		return t;
	}
	public List list(NSDirectory nsdir,String pattern, int flags)
	{
		List t = null;
		try{
        	 t  = nsdir.list(pattern,flags);}catch(Exception e){};
		return t;
	}

	public Task list(NSDirectory nsdir, TaskMode mode)
	{
		Task t = null;
		try{
        	 t  = nsdir.list(mode);}catch(Exception e){};
		return t;
	}
	public Task list(NSDirectory nsdir,TaskMode mode, int flags)
	{
		Task t = null;
		try{
        	 t  = nsdir.list(mode, flags);}catch(Exception e){};
		return t;
	}
	public Task list(NSDirectory nsdir,TaskMode mode, String pattern)
	{
		Task t = null;
		try{
        	 t  = nsdir.list(mode,pattern);}catch(Exception e){};
		return t;
	}
	public Task list(NSDirectory nsdir,TaskMode mode, String pattern, int flags)
	{
		Task t = null;
		try{
        	 t  = nsdir.list(mode,pattern,flags);}catch(Exception e){};
		return t;
	}
	public Task makeDir(NSDirectory nsdir, TaskMode mode, URL target)
	{
		Task t = null;
		try{
        	 t  = nsdir.makeDir(mode, target);}catch(Exception e){};
		return t;
	}
	public Task makeDir(NSDirectory nsdir, TaskMode mode, URL target, int flags)
	{
		Task t = null;
		try{
        	 t  = nsdir.makeDir(mode, target, flags);}catch(Exception e){};
		return t;
	}
	public void makeDir(NSDirectory nsdir, URL target, int flags)
	{
		
		try{
        	 nsdir.makeDir( target, flags);}catch(Exception e){};
		
	}
	public void makeDir( NSDirectory nsdir,URL target)
	{
		
		try{
        	 nsdir.makeDir( target);}catch(Exception e){};
		
	}
	
	
	public void move(NSDirectory nsdir, String source, URL target)
	{
		try{
        	 nsdir.move(source, target);}catch(Exception e){};
	}
	public void move(NSDirectory nsdir, String source, URL target, int flags)
	{
		try{
        	 nsdir.move(source, target, flags);}catch(Exception e){};
	}
	public Task move(NSDirectory nsdir,TaskMode mode, String source, URL target)
	{
		Task t = null;
		try{
        	 t  = nsdir.move(mode, source, target);}catch(Exception e){};
		return t;
	}
	public Task move(NSDirectory nsdir,TaskMode mode, String source, URL target, int flags)
	{
		Task t = null;
		try{
        	 t  = nsdir.move(mode, source, target, flags);}catch(Exception e){};
		return t;
	}
	public Task move(NSDirectory nsdir,TaskMode mode, URL source, URL target)
	{
		Task t = null;
		try{
        	 t  = nsdir.move(mode, source, target);}catch(Exception e){};
		return t;
	}
	public Task move(NSDirectory nsdir,TaskMode mode, URL source, URL target, int flags)
	{
		Task t = null;
		try{
        	 t  = nsdir.move(mode, source, target, flags);}catch(Exception e){};
		return t;
	}
	public void move(NSDirectory nsdir, URL source, URL target)
	{
		try{
        	 nsdir.move(source, target);}catch(Exception e){};
	}
	public void move(NSDirectory nsdir, URL source, URL target, int flags)
	{
		try{
        	 nsdir.move(source, target);}catch(Exception e){};
	}
	public Task oepn(NSDirectory nsdir, TaskMode mode, URL name)
    {
        Task a  = null;
        try{
        a = nsdir.open(mode, name);}catch(Exception e){};
        return a;
    }	
	public Task open(NSDirectory nsdir, TaskMode mode, URL name, int flags)
    {
        Task a  = null;
        try{
        a = nsdir.open(mode, name, flags);}catch(Exception e){};
        return a;
    }
	public NSEntry open(NSDirectory nsdir, URL name)
    {
        NSEntry a  = null;
        try{
        a = nsdir.open(name);}catch(Exception e){};
        return a;
    }
	public NSEntry open(NSDirectory nsdir, URL name, int flags)
    {
        NSEntry a  = null;
        try{
        a = nsdir.open(name, flags);}catch(Exception e){};
        return a;
    }
		public Task oepnDir(NSDirectory nsdir, TaskMode mode, URL name)
    {
        Task a  = null;
        try{
        a = nsdir.openDir(mode, name);}catch(Exception e){};
        return a;
    }	
	public Task openDir(NSDirectory nsdir, TaskMode mode, URL name, int flags)
    {
        Task a  = null;
        try{
        a = nsdir.openDir(mode, name, flags);}catch(Exception e){};
        return a;
    }
	public NSDirectory openDir(NSDirectory nsdir, URL name)
    {
        NSDirectory a  = null;
        try{
        a = nsdir.openDir(name);}catch(Exception e){};
        return a;
    }
	public NSDirectory openDir(NSDirectory nsdir, URL name, int flags)
    {
        NSDirectory a  = null;
        try{
        a = nsdir.openDir(name, flags);}catch(Exception e){};
        return a;
    }

		// 2nd day
	
	public void permissionsAllow (NSDirectory nsdir, String target, String id, int permissions)
	{
		try 
		{
			nsdir.permissionsAllow(target,id,permissions);
			}catch(Exception e){}
	}
	public void permissionsAllow (NSDirectory nsdir, String target, String id, int permissions, int flags)
	{
		try 
		{
			nsdir.permissionsAllow(target,id,permissions,flags);
			}catch(Exception e){}
	}
	public Task permissionsAllow (NSDirectory nsdir, TaskMode mode, String target, String id, int permissions)
	{
			Task t = null;
		try 
		{
			t = nsdir.permissionsAllow(mode, target,id,permissions);
			}catch(Exception e){}
			return t;
	}
	public Task permissionsAllow (NSDirectory nsdir, TaskMode mode, String target, String id, int permissions, int flags)
	{
			Task t = null;
		try 
		{
			t = nsdir.permissionsAllow(mode,target,id,permissions, flags);
			}catch(Exception e){}
			return t;
	}
	public Task permissionsAllow (NSDirectory nsdir, TaskMode mode, URL target, String id, int permissions, int flags)
	{
			Task t = null;
		try 
		{
			t = nsdir.permissionsAllow(mode,target,id,permissions, flags);
			}catch(Exception e){}
			return t;
	}
	/*public Task permissionsAllow (NSDirectory nsdir, TaskMode mode, URL target, String id, int permissions, int flags)
	{
			Task t = null;
		try 
		{
			t = nsdir.permissionsAllow(mode,target,id,permissions, flags);
			}catch(Exception e){}
			return t;
	}*/
	
	public void permissionsAllow (NSDirectory nsdir, URL target, String id, int permissions)
	{
		try 
		{
			nsdir.permissionsAllow(target,id,permissions);
			}catch(Exception e){}
	}
	public void permissionsAllow (NSDirectory nsdir, URL target, String id, int permissions, int flags)
	{
		try 
		{
			nsdir.permissionsAllow(target,id,permissions, flags);
			}catch(Exception e){}
	}
	
	public void permissionsDeny(NSDirectory nsdir, String target, String id, int permissions)
	{
		try 
		{
			nsdir.permissionsDeny(target,id,permissions);
			}catch(Exception e){}
	}
	public void permissionsDeny (NSDirectory nsdir, String target, String id, int permissions, int flags)
	{
		try 
		{
			nsdir.permissionsDeny(target,id,permissions,flags);
			}catch(Exception e){}
	}
	public Task permissionsDeny(NSDirectory nsdir, TaskMode mode, String target, String id, int permissions)
	{
			Task t = null;
		try 
		{
			t = nsdir.permissionsDeny(mode, target,id,permissions);
			}catch(Exception e){}
			return t;
	}
	public Task permissionsDeny(NSDirectory nsdir, TaskMode mode, String target, String id, int permissions, int flags)
	{
			Task t = null;
		try 
		{
			t = nsdir.permissionsDeny(mode,target,id,permissions, flags);
			}catch(Exception e){}
			return t;
	}
	public Task permissionsDeny(NSDirectory nsdir, TaskMode mode, URL target, String id, int permissions, int flags)
	{
			Task t = null;
		try 
		{
			t = nsdir.permissionsDeny(mode,target,id,permissions, flags);
			}catch(Exception e){}
			return t;
	}
	/*public Task permissionsDeny(NSDirectory nsdir, TaskMode mode, URL target, String id, int permissions, int flags)
	{
			Task t = null;
		try 
		{
			t = nsdir.permissionsDeny(mode,target,id,permissions, flags);
			}catch(Exception e){}
			return t;
	}*/
	
	public void permissionsDeny(NSDirectory nsdir, URL target, String id, int permissions)
	{
		try 
		{
			nsdir.permissionsDeny(target,id,permissions);
			}catch(Exception e){}
	}
	public void permissionsDeny(NSDirectory nsdir, URL target, String id, int permissions, int flags)
	{
		try 
		{
			nsdir.permissionsDeny(target,id,permissions, flags);
			}catch(Exception e){}
	}
	
	public URL readLink(NSDirectory nsdir, URL name)
	{
		URL url = null;
		try{
			url = nsdir.readLink(name);
		}catch(Exception e){}
		return url;
	}
	public Task readLink(NSDirectory nsdir, TaskMode mode, URL name)
	{
		Task t = null;
		try{
			t = nsdir.readLink(mode, name);
		}catch(Exception e){}
		return t;
	}
	
	public void remove(NSDirectory nsdir, String target,int flags)
	{
		try 
		{
			nsdir.remove(target, flags);
			}catch(Exception e){}
	}
	public void remove(NSDirectory nsdir, String target)
	{
		try 
		{
			nsdir.remove(target);
			}catch(Exception e){}
	}
	public Task remove(NSDirectory nsdir, TaskMode mode, String target)
	{
			Task t = null;
		try 
		{
			t = nsdir.remove(mode, target);
			}catch(Exception e){}
			return t;
	}
	public Task remove(NSDirectory nsdir, TaskMode mode, String target, int flags)
	{
			Task t = null;
		try 
		{
			t = nsdir.remove(mode,target,flags);
			}catch(Exception e){}
			return t;
	}
	public Task remove(NSDirectory nsdir, TaskMode mode, URL target, int flags)
	{
			Task t = null;
		try 
		{
			t = nsdir.remove(mode,target,flags);
			}catch(Exception e){}
			return t;
	}
	public Task remove(NSDirectory nsdir, TaskMode mode, URL target)
	{
			Task t = null;
		try 
		{
			t = nsdir.remove(mode,target);
			}catch(Exception e){}
			return t;
	}
	
	public void remove(NSDirectory nsdir, URL target)
	{
		try 
		{
			nsdir.remove(target);
			}catch(Exception e){}
	}
	public void remove(NSDirectory nsdir, URL target, int flags)
	{
		try 
		{
			nsdir.remove(target,flags);
			}catch(Exception e){}
	}
	
	//org.ogf.saga.namespace.NSEntry
	
	public void close(NSEntry nse)
	{
		try{
			nse.close();
		}catch(Exception e){}
	}
	public void close(NSEntry nse, float timeoutInSeconds)
	{
		try{
			nse.close(timeoutInSeconds);
		}catch(Exception e){}
		
	}
	public Task close(NSEntry nse, TaskMode mode)
	{
		Task t = null;
		try{
			t = nse.close(mode);
		}catch(Exception e){}
		return t;
	}
	public Task close(NSEntry nse,TaskMode mode, float timeoutInSeconds)
	{
		Task t = null;
		try{
			 t = nse.close( mode, timeoutInSeconds);
		}catch(Exception e){}
		return t;
		
	}
	
	public void copy(NSEntry nse, URL target)
	{
		try{
		nse.copy(target);}catch(Exception e){};
	}
	public void copy(NSEntry nse, URL target, int flags)
	{
		try{
		nse.copy( target, flags);}catch(Exception e){};
	}
	public Task copy(NSEntry nse, TaskMode mode, URL target)
	{
		Task l = null;
		try{
		l = nse.copy(mode, target);}catch(Exception e){};
		return l;
	}
	public Task copy(NSEntry nse, TaskMode mode, URL target, int flags)
	{
		Task l = null;
		try{
		l = nse.copy(mode, target, flags);}catch(Exception e){};
		return l;
	}
	public URL getCWD(NSEntry nse)
	{
		URL url = null;
		try{
			url = nse.getCWD();
		}catch(Exception e){}
		return url;
	}
	public Task getCWD(NSEntry nse, TaskMode mode)
	{
		Task t = null;
		try{
			t = nse.getCWD(mode);
		}catch(Exception e){}
		return t;
	}
	
	public URL getName(NSEntry nse)
	{
		URL url = null;
		try{
			url  = nse.getName();
		}catch(Exception e){}
		return url;
	}
	public Task getName(NSEntry nse, TaskMode mode)
	{
		Task t = null;
		try{
			t = nse.getName(mode);
		}catch(Exception e){}
		return t;
	}
	
	public URL getURL(NSEntry nse)
	{
		URL url = null;
		try{
			url  = nse.getURL();
		}catch(Exception e){}
		return url;
	}
	public Task getURL(NSEntry nse, TaskMode mode)
	{
		Task t = null;
		try{
			t = nse.getURL(mode);
		}catch(Exception e){}
		return t;
	}
	
	public boolean isDir(NSEntry nse)
	{
		boolean flag = false;
		try{
			flag  = nse.isDir();
		}catch(Exception e){}
		return flag;
	}
	public Task isDir(NSEntry nse, TaskMode mode)
	{
		Task t = null;
		try{
			t = nse.isDir(mode);
		}catch(Exception e){}
		return t;
	}
	
	public boolean isEntry(NSEntry nse)
	{
		boolean flag = false;
		try{
			flag  = nse.isEntry();
		}catch(Exception e){}
		return flag;
	}
	public Task isEntry(NSEntry nse, TaskMode mode)
	{
		Task t = null;
		try{
			t = nse.isEntry(mode);
		}catch(Exception e){}
		return t;
	}
	
	public boolean isLink(NSEntry nse)
	{
		boolean flag = false;
		try{
			flag  = nse.isLink();
		}catch(Exception e){}
		return flag;
	}
	public Task isLink(NSEntry nse, TaskMode mode)
	{
		Task t = null;
		try{
			t = nse.isLink(mode);
		}catch(Exception e){}
		return t;
	}
	
	public void link(NSEntry nse, URL target)
	{
		try{
		nse.link(target);}catch(Exception e){};
	}
	public void link(NSEntry nse, URL target, int flags)
	{
		try{
		nse.link( target, flags);}catch(Exception e){};
	}
	public Task link(NSEntry nse, TaskMode mode, URL target)
	{
		Task l = null;
		try{
		l = nse.link(mode, target);}catch(Exception e){};
		return l;
	}
	public Task link(NSEntry nse, TaskMode mode, URL target, int flags)
	{
		Task l = null;
		try{
		l = nse.link(mode, target, flags);}catch(Exception e){};
		return l;
	}
	
	public void move(NSEntry nse, URL target)
	{
		try{
		nse.move(target);}catch(Exception e){};
	}
	public void move(NSEntry nse, URL target, int flags)
	{
		try{
		nse.move( target, flags);}catch(Exception e){};
	}
	public Task move(NSEntry nse, TaskMode mode, URL target)
	{
		Task l = null;
		try{
		l = nse.move(mode, target);}catch(Exception e){};
		return l;
	}
	public Task move(NSEntry nse, TaskMode mode, URL target, int flags)
	{
		Task l = null;
		try{
		l = nse.move(mode, target, flags);}catch(Exception e){};
		return l;
	}
	
	public Task permissionsAllow(NSEntry nse, TaskMode mode, String id, int permissions, int flags)
	{
			Task t = null;
		try 
		{
			t = nse.permissionsAllow(mode,id,permissions, flags);
			}catch(Exception e){}
			return t;
	}
	
	public void permissionsAllow(NSEntry nse, String id, int permissions, int flags)
	{
		try 
		{
			nse.permissionsAllow(id,permissions, flags);
			}catch(Exception e){}
	}
	
	public Task permissionsDeny(NSEntry nse, TaskMode mode, String id, int permissions, int flags)
	{
			Task t = null;
		try 
		{
			t = nse.permissionsDeny(mode,id,permissions, flags);
			}catch(Exception e){}
			return t;
	}
	
	public void permissionsDeny(NSEntry nse, String id, int permissions, int flags)
	{
		try 
		{
			nse.permissionsDeny(id,permissions, flags);
			}catch(Exception e){}
	}
	
	public URL readLink(NSEntry nse)
	{
		URL url = null;
		try{
			url  = nse.readLink();
		}catch(Exception e){}
		return url;
	}
	public Task readLink(NSEntry nse, TaskMode mode)
	{
		Task t = null;
		try{
			t = nse.readLink(mode);
		}catch(Exception e){}
		return t;
	}
	
	public void remove(NSEntry nse)
	{
		try{
			nse.remove();
		}catch(Exception e){}
	}
	public void remove(NSEntry nse, int flags)
	{
		try{
			nse.remove(flags);
		}catch(Exception e){}
		
	}
	public Task remove(NSEntry nse, TaskMode mode)
	{
		Task t = null;
		try{
			t = nse.remove(mode);
		}catch(Exception e){}
		return t;
	}
	public Task remove(NSEntry nse,TaskMode mode, int flags)
	{
		Task t = null;
		try{
			 t = nse.remove( mode, flags);
		}catch(Exception e){}
		return t;
		
	}

	

}
