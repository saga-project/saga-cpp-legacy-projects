Last update: dec 14 2008

PySAGA is the project concerning the specification of the Python-SAGA 
language binding. The result will be a Python specification which is 
independent of the SAGA reference implementation (Cpp or Java). Later, 
a Java specific backend will be added with the help of Jython. This part 
of the project is called JySAGA

Python program -> Python-Saga binding -> SAGA implementation

Pysaga is a specification which has some api documentation available in
the apidoc directory. It is also available at http://www.few.vu.nl/~pzn400/apidoc/
I try to keep them up to date with the changes made.
The specification is specified for Python 2.x and is not expected to change anymore.

Jysaga is beginning to take a form. The ../Jysaga directory has the source files
to run a python program which uses the method calls from Pysaga. This python 
program must be able run on Jython to use the Java SAGA reference implementation.

The ../Jysaga/test directory has the startJysagaApp script to run a python program
and has some test files (<digits>_<name>Test.py) to test specific Jysaga source files

If you downloaded the trunk version from gforge.cs.vu.nl (in the ibis project) you
can use the ./trunkStartJysagaApp program. Don't forget to set the environment 
variables.

The source code, documentation, tests and pretty much everything is 
somewhere near beta phase, so use at own risk. It should not hurt your 
system, but not everything will work.

For more information contact Paul van Zoolingen, pzn400 at few dot vu 
dot nl, or use the saga-rg mailinglist (http://www.ogf.org/pipermail/saga-rg/) 

Supervision is done by Thilo Kielmann, Faculty of Exact Sciences, Vrije 
Universiteit, Amsterdam.




