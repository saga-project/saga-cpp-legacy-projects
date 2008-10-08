#! /bin/csh
setenv CLASSPATH `pwd`
setenv JROOT /usr/java1.1
setenv JAVAFLAGS -O 
setenv HOST `hostname`
#The port to listen for http connections
setenv MACH_PORT ${HOST}:1102
#Location of the Java security policy
setenv SEC_POLICY ${CLASSPATH}/brmi/policy
#Heapsize in bytes for server's JVM 
setenv SERV_HEAP 400000000
