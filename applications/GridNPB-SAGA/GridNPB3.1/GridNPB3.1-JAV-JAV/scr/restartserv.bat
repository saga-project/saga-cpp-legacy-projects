set CLASSPATH=d:\users\frumkin\java\GridNPB3.0
start /B d:\jdk1.1.8\bin\rmiregistry
start /B java -mx500MB -Djava.rmi.server.codebase=http://pc178:1102/ -Djava.security.policy=d:\users\frumkin\java\GridNPB3.0\brmi\policy -classpath d:\users\frumkin\java\GridNPB3.0 brmi.BenchServer&
