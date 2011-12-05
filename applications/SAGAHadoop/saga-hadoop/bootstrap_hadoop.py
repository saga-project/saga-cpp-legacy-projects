#!/usr/bin/env python
""" Hadoop Bootstrap Script (based on hadoop 0.20.203 release) """
import os, sys
import urllib
import subprocess
import logging
import uuid
import shutil
logging.basicConfig(level=logging.DEBUG)

# For automatic Download and Installation
HADOOP_DOWNLOAD_URL="http://www.apache.org/dist//hadoop/common/hadoop-0.20.203.0/hadoop-0.20.203.0rc1.tar.gz"
WORKING_DIRECTORY=os.path.join(os.getcwd(), "work")

# For using an existing installation
HADOOP_HOME="/Users/luckow/workspace-saga/hadoop/hadoop-0.20.203.0"
JAVA_HOME="/System/Library/Frameworks/JavaVM.framework/Home/"

class HadoopBootstrap(object):

    def __init__(self, working_directory):
        self.working_directory=working_directory
        self.jobid = str(uuid.uuid1())
        self.job_working_directory = os.path.join(WORKING_DIRECTORY, self.jobid)
        self.job_conf_dir = os.path.join(self.job_working_directory, "conf")
        self.job_name_dir = os.path.join(self.job_working_directory, "name")
        self.job_log_dir = os.path.join(self.job_working_directory, "logs")
        try:
            os.makedirs(job_dir)       
            os.makedirs(log_dir)
        except:
            pass
    
    def get_core_site_xml(self, hostname):
        return """<?xml version="1.0"?>
    <?xml-stylesheet type="text/xsl" href="configuration.xsl"?>
    <configuration>
         <property>
             <name>fs.default.name</name>
             <value>hdfs://%s:9000</value>
         </property>
    </configuration>"""%(hostname)
    
    def get_hdfs_site_xml(self, hostname, name_dir):
        return """<?xml version="1.0"?>
    <?xml-stylesheet type="text/xsl" href="configuration.xsl"?>
    <configuration>
         <property>
             <name>dfs.replication</name>
             <value>1</value>
         </property>
         <property>
             <name>dfs.name.dir</name>
             <value>%s</value>
         </property>     
    </configuration>"""%(name_dir)
    
    
    def get_mapred_site_xml(self,hostname):
        return """<?xml version="1.0"?>
    <configuration>
         <property>
             <name>mapred.job.tracker</name>
             <value>%s:9001</value>
         </property>
    </configuration>"""%(hostname)
    
    
    def get_pbs_allocated_nodes(self):
        pass
    
    def configure_hadoop(self):
        logging.debug("Configure Hadoop")
        shutil.copytree(os.path.join(HADOOP_HOME, "conf"), self.job_conf_dir)
        
        core_site_file = open(os.path.join(self.job_working_directory, "conf/core-site.xml"), "w")
        core_site_file.write(self.get_core_site_xml("localhost"))
        core_site_file.close() 
        
        hdfs_site_file = open(os.path.join(self.job_working_directory,"conf/hdfs-site.xml"), "w")
        hdfs_site_file.write(self.get_hdfs_site_xml("localhost", self.job_name_dir))
        hdfs_site_file.close() 
        
        mapred_site_file = open(os.path.join(self.job_working_directory,"conf/mapred-site.xml"), "w")
        mapred_site_file.write(self.get_mapred_site_xml("localhost"))
        mapred_site_file.close() 
        
    def start_hadoop(self):
        logging.debug("Start Hadoop")    
        self.set_env()    
        format_command = os.path.join(HADOOP_HOME, "bin/hadoop") + " --config " + self.job_conf_dir + " namenode -format"
        logging.debug("Execute: %s"%format_command)
        os.system(format_command)        
        start_command = os.path.join(HADOOP_HOME, "bin/start-all.sh")
        logging.debug("Execute: %s"%start_command)
        os.system(start_command)
        
        
    def stop_hadoop(self, job_configuration_directory):
        logging.debug("Stop Hadoop")    
        self.set_env() 
        stop_command = os.path.join(HADOOP_HOME, "bin/stop-all.sh")
        logging.debug("Execute: %s"%stop_command)
        os.system(stop_command)
    
    
    def start(self):
        self.configure_hadoop()
        self.start_hadoop()
    
    def set_env(self):
        logging.debug("Export HADOOP_CONF_DIR to %s"%self.job_conf_dir)
        os.environ["HADOOP_CONF_DIR"]=self.job_conf_dir  
        logging.debug("Export HADOOP_LOG_DIR to %s"%self.job_log_dir)
        os.environ["HADOOP_LOG_DIR"]=self.job_log_dir
    
#########################################################
#  main                                                 #
#########################################################
if __name__ == "__main__" :
    
   
    if not os.path.exists(HADOOP_HOME):
        logging.debug("Download Hadoop")
        opener = urllib.FancyURLopener({})
        opener.retrieve(HADOOP_DOWNLOAD_URL, WORKING_DIRECTORY+"hadoop.tar.gz");
    
        logging.debug("Install Hadoop")
        os.chdir(WORKING_DIRECTORY)
        os.system("tar -xzf hadoop.tar.gz")
    
   
    hadoop = HadoopBootstrap(WORKING_DIRECTORY)
    hadoop.start()
    
    
    
    
    
     