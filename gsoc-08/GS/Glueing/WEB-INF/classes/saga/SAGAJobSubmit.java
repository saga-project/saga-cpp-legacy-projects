package saga;

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



public class SAGAJobSubmit implements Callback {

    public static void main(String[] args) {
        try {
            // Make sure the gridsam adaptor is selected.
		//args[0] = gridsam or any other adaptor
            System.setProperty("JobService.adaptor.name", "gridsam");

            
            JobService js = JobFactory.createJobService(new URL("https://164.11.38.162:18443/gridsam/services/gridsam"));

            JobDescription jd = JobFactory.createJobDescription();
            //args[1]=/bin/echo or any other exe
		//if(args[1].equals("add"))
			//args[1]="/home/yasir/OMII/saga/runApp";
            jd.setAttribute(JobDescription.EXECUTABLE, "/bin/echo");
		
            jd.setVectorAttribute(JobDescription.ARGUMENTS,
                    new String[] { "yasir" });
            jd.setAttribute(JobDescription.OUTPUT, "uname.out");
            
            // Get hostname for poststage target.
            String host = java.net.InetAddress.getLocalHost()
                    .getCanonicalHostName();
			//jd.setVectorAttribute(JobDescription.FILETRANSFER,new String[] { "ftp://" + host + ":12345/incoming/"+args[3]+".out < "+ args[3]+".out" });
            	//jd.setVectorAttribute(JobDescription.FILETRANSFER,new String[] { "ftp://164.11.38.162:12346/uname.out < uname.out" });
		//jd.setVectorAttribute(JobDescription.FILETRANSFER,new String[] { "ftp://164.11.138.166:21/uname.out < "+ args[3]+".out" });

            // Create the job, run it, and wait for it.
            Job job = js.createJob(jd);
            job.addCallback(Job.JOB_STATE, new SAGAJobSubmit());
            job.addCallback(Job.JOB_STATEDETAIL, new SAGAJobSubmit());
            job.run();
            job.waitFor();
            System.out.println("Exit status = " + job.getAttribute(Job.EXITCODE));
		//System.out.println("The staged out results can be viewed at: "+ "ftp://" + host + ":12345/incoming/"+args[3]+".out");
		/*String monitorId = args[3]+"monitor";
		Metric m = job.getMetric(Job.JOB_MEMORYUSE);
		String memoryUseName = m.getAttribute(Metric.NAME);
		String memoryUseValue = m.getAttribute(Metric.VALUE);
		m = job.getMetric(Job.JOB_CPUTIME);
		String cpuName = m.getAttribute(Metric.NAME);
		String cpuValue = m.getAttribute(Metric.VALUE);
		m = job.getMetric(Job.JOB_PERFORMANCE);
		String perName = m.getAttribute(Metric.NAME);
		String perValue = m.getAttribute(Metric.VALUE);*/
		
		/*BufferedWriter out = new BufferedWriter(new FileWriter("/home/yasir/Download/apache-tomcat-6.0.16/bin/GlueLogs.txt",true));
			out.write(monitorId+","+memoryUseName+","+memoryUseValue+","+cpuName+","+cpuValue+","+perName+","+perValue);
			out.newLine();
			out.close();*/	
        } catch (Throwable e) {
            System.out.println("Got exception " + e);
            e.printStackTrace();
            e.getCause().printStackTrace();
        }
    }

    // Callback monitors job.
    public boolean cb(Monitorable m, Metric metric, Context ctxt) {
        try {
		
            String value = metric.getAttribute(Metric.VALUE);
            String name = metric.getAttribute(Metric.NAME);
            System.out.println("METRIC_NAME = "
                    + name + " METRIC_VALUE = " + value);
        } catch (Throwable e) {
            System.err.println("error" + e);
            e.printStackTrace(System.err);
        }
        // Keep the callback.
        return true;
    }
}
