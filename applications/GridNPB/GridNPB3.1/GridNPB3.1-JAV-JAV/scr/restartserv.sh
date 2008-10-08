#! /bin/sh
PATH=$PATH:/usr/java/bin/:/usr/java130/bin/:/usr/java1.1/bin/:/usr/local/bin/:
kill -9 `ps -ef | grep java | grep .rmi | awk '{ print $2 }' | tr '\012' ' '`
kill -9 `ps -ef | grep rmi | grep registry | awk '{ print $2 }'`
rmiregistry >&- <&- 2>/dev/null&
sleep 3
java -mx800000000 -Djava.rmi.server.codebase=http://${MACH_PORT}/${CLASSPATH}/  -Djava.security.policy=${CLASSPATH}/brmi/policy brmi.BenchServer&
