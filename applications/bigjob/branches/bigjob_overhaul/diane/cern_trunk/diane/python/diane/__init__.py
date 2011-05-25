""" Main DIANE package.
"""
from logger import getLogger
from config import getConfig

# convenience imports
from IApplicationWorker import IApplicationWorker
from SimpleApplicationManager import SimpleApplicationManager
from SimpleTaskScheduler import SimpleTaskScheduler
from ITaskScheduler import ITaskScheduler
from TaskInfo import TaskInfo

# import compatibility modules...
import util.compatibility

############################################################################################################
## @mainpage DIANE API and reference documentation
#
# @section overview Overview
# DIANE is a framework for efficient control and scheduling of computations on a set of distributed worker nodes.
# It allows to:
# - reduce the application execution time by using the resources more efficiently,
# - reduce the user work overhead by providing fully automatic execution and failure management,
# - efficiently integrate local and Grid resources.
#
# The framework is highly customizable and, under certain conditions, may be used with legacy %applications.
#
# @section GeneralTopics General Topics
# <ul>
# <li> @subpage QuickStart
# <li> @subpage ComputingModel
# <li> @subpage Architecture
# <li> @subpage ApplicationFramework
# <li> @subpage UserInterface
# <li> @subpage ResourceBackends
# <li> @subpage NetworkingLayer
# </ul>

############################################################################################################
## @page QuickStart Quick Start
#
# Let's run a simple application called crash. This application is used in the
# testing framework. It does not do much more than just testing if all works
# OK but it is sufficient to give you an idea how the system works.  The
# application code is defined here: crash.py - this is more or less as complex
# as the code can get even for more realistic %applications. The input
# parameters for our run are defined in testOK.py. Actually this
# file contains also extra code to connect it to the testing framework and to
# validate the output - you may safely ignore them.
#
# @section StartCommands Start the master and worker.
# Start master and worker in separate windows to see what happens.
# @verbatim
# $ diane-run python/diane/test/testOK.py &
# $ diane-worker-start &
# @endverbatim
# If everything goes well you should get output similar to the one below.
#
# Here is what happens:
#  - a run master service is started in a directory \c ~/diane/runs/n , where n is the current run number
#  - testOK.py is used as a run file, it defines
#     - \b application loaded by the framework (\c diane.applications.crash)
#     - \b run() function which defines the parameters and data to be processed: 10 commands (tasks) which just sleep for a moment
#  - when the worker agent is started, it looks into the current run directory and finds an address of the running master service
#  - the worker agent registers to the master service, \c wid=1 , and asks for workload
#  - the master sends the task and waits until the worker agent sends back the results
#  - when the first task is finished then the next tasks are processes in a similar way
#  - when no tasks are left the master service quits
#
# @section MasterOutput Master Output
# @verbinclude testOK.py_diane-run_0.out
# @section WorkerOutput WorkerOutput
# @verbinclude testOK.py_diane-worker-start_0.out


############################################################################################################
## @page ApplicationFramework Application Framework
#
# Application components are python classes which implement the following interfaces:
# - on the master node:
#    - task scheduler: diane.ITaskScheduler.ITaskScheduler
#    - application manager: diane.IApplicationManager.IApplicationManager
# - on the worker nodes:
#    - application worker: diane.IApplicationWorker.IApplicationWorker
#
# @section MasterComponents Scheduling and master application control
#
# Task scheduler is responsible for mapping tasks to workers and deciding what
# to do in case of failures in task execution.  The application manager
# defines the application-specific actions (e.g. merging of outputs) when the
# tasks are completed or failed. Separation between application manager and
# task scheduler enables to separate scheduling aspects from application
# callbacks. However for some application models this may not be possible and
# the application actions must be embedded in the specific implementation of
# the scheduler.  In such cases the application manager does not perform any
# function.
#
# @image html DIANE-master-components.png
# @image latex DIANE-master-components.eps
#
# Task scheduler is an active object (thread derived from
# diane.BaseThread.BaseThread) which implements the
# diane.ITaskScheduler.ITaskScheduler interface and which runs in the master
# process, alongside the diane.RunMaster.RunMaster core service. The RunMaster
# notifies the scheduler of the events using the ITaskScheduler interface.
# The RunMaster implements the diane.RunMaster.IMasterController. This
# interface is used by the scheduler to start and cancel the tasks.  The
# communication between RunMaster and task scheduler is fully asynchronous.
#
# The specific implementations of application manager may (but does not have
# to) be derived from the BaseThread. In this case the additional thread to
# control application actions is spawned.  Such a thread may be used, for
# example, to monitor the application progress or to add new tasks to the
# scheduler (refer to the specific scheduler implementation such as
# SimpleTaskScheduler).
#
#
# @section WorkerComponents Worker application control
#
# Application worker component is derived from diane.IApplicationWorker.IApplicationWorker. The worker agent process,
# represented by diane.WorkerAgent.WorkerAgent core object, passes the task parameters recieved from the run master
# and waits for output produced by the application worker to send it back to the run master.
#
# @image html DIANE-worker-components.png
# @image latex DIANE-worker-components.eps
#
# @section TriviallyParallelApplications Trivially parallel applications
#
# In the case of trivially parallel %applications all tasks may be executed independently.
# The diane.SimpleTaskScheduler.SimpleTaskScheduler may be used in this case. 
# The behaviour of this scheduler is controlled by the diane.SimpleTaskScheduler.SimplePolicy parameters.
# Application specific actions on the master side are defined by diane.SimpleApplicationManager.SimpleApplicationManager.
#
# In order to interface a trivially parallel application one needs to implement a simple application manager and
# an application worker which provide the appropriate links to the application itself.
#
# Examples:
#  - how to specify scheduler parameters in the run file: \ref testFail.py (and more files in \c python/diane/test directory)
#  - how to set the scheduler in the application package: \ref crash.py
#  - an example of simple application manager for testing: diane.applications.crash.CrashApplicationManager
#
# @section UserDefinedApplications User-defined applications
#
# User-defined application adapter packages are located in ~/diane/apps (or $DIANE_USER_WORKSPACE/apps).
# The master will automatically send the application code to the worker agent, including all parent packages.
# Example: application module imported by run file as "diane_apps.A.B" is located at ~/diane/apps/diane_apps/A/B
# The entire file tree with the root at ~/diane/apps/diane_apps will be shipped to the worker agent.


############################################################################################################
## @page UserInterface User Interface
#
#
# All commands support -h and --help options. Full list of shell commands and utilities: @ref CommandsIndex.
#
# @section master_start Starting and querying the master
#
#Starting the master without submitting workers:
#
#<tt> diane-run file.run </tt>
#
#<i>OUTDATED If you want your master to join the directory service, then you should specify an additional --ds option. Read more on DIANEDirectoryService</i>
#
# The runfile (<tt>file.run</tt>) is a small python script which defines the run
# configuration parameters and input data. For convenience this file may also be used to customize the
# behaviour of the worker agent submitter scripts.
#
# @subsection basic_run_parameters Basic run parameters
# The minimal run-file consists of specifying the application and the input parameters for the run:
#
#  - <tt>application</tt> defines the application package (python module or package)
#  - <tt>run(input,config)</tt> is a function called by diane-run which sets:
#     - <tt>input.data</tt> the application-specific input parameters
#     - <tt>input.worker, input.scheduler, input.manager</tt>: application
#     control components (these parameters are typically defined in the
#     application package and need not be specified on-per-run basis)
#
# @subsubsection additional_run_parameters Additional run parameters
# Parameters for configuration of the transport layer (omniorb) and customization of the submitters scripts:
#
#  - <tt>omniorb_config_type</tt> is a string indicating the network connectivity setup for the underlying transport omniorb layer (e.g. GSI-enabled versus unsecure)
#  - <tt>default_master,default_worker</tt> are strings to configure the omniORB transport layer for the master and worker processes
#  - <tt>gsi_master,gsi_worker</tt> configure omniORB if <tt>omniorb_config_type</tt> is set to <tt>gsi</tt>
#  - <tt>initialize_submitter()</tt> is a callback function used to customize the standard submitter scripts
#  - <tt>worker_submit(j)</tt> is a callback function used to customize standard submitter scripts
#
# @subsection evaluation_of_run_parameters Evaluation of run parameters
# Except for the application symbol in the run file, all other parameters have
# the cascading behaviour i.e. they may be defined in the application package itself
# and, if needed, overriden in the run file. For example: if both the run file
# and the application package define the omniorb_config_type, the value set in
# the run file is used.
#
# The cascading of functions works in the following way: first the function
# defined in the application package is called, then the function in the run
# file is called. The same arguments are passed to both calls (and functions
# may modify them by reference). The function return values are discarded.
#
# @section master_kill Killing the last running master
#
#diane-master-ping kill
#
# @section master_query Checking what the master is doing:
#
#diane-master-ping
#
# @section worker_commands Submitting workers
#
#Submitting more workers which will connect to the last started master (corresponding to the latest directory in $DIANE_WORKSPACE/runs):
#
#<tt>diane-env -d ganga LCG.py --diane-run-file input.run --diane-worker-number 5
#</tt>
#
#Submitting more workers which will connect to the master XXX (corresponding to =$DIANE_WORKSPACE/runs/XXX):
#
#<tt>diane-env -d ganga LCG.py --diane-run-file input.run --diane-worker-number 5 --diane-master=workspace:XXX
#</tt>
#
#Checking the output of the workers:
#
#  - start ganga and wait until the worker job status is completed
#  - then use j.peek() method or ls -l $j.outputdir.
#

############################################################################################################
## @page ResourceBackends Computing Resources, Backends and Job Submission
#
# diane-worker-start
#
# For convenience a set of preconfigured submitters based on Ganga Job Management
# Interface (http://cern.ch/ganga) are provided. Ganga supports many computing
# backends and offers a uniform interface to job submission.  The ganga
# submitter scripts must be run on the Worker Submitter Host i.e.  where the
# backend user interface commands are available. For example: in order to start worker processes
# on EGEE Grid the gLite user interface must be available on the Worker Submitter Host.
#
#
## @dot
#digraph example {
# subgraph MW {
#  node [shape="record", fontname=Helvetica, fontsize=10];
#  edge [ arrowhead="none", style="dotted" ];
#  m [ label="RunMaster" URL="\ref diane.RunMaster.RunMaster"];
#  w1 [ label="Grid|WorkerAgent1|WorkerAgent2|..." URL="\ref diane.WorkerAgent.WorkerAgent"];
#  w2 [ label="LSF|WorkerAgent3|..." URL="\ref diane.WorkerAgent.WorkerAgent"];
#  wN [ label="ssh|WorkerAgent4|..." URL="\ref diane.WorkerAgent.WorkerAgent"];
#  w1 -> m;
#  w2 -> m;
#  wN -> m;
# }
# 
#  MH [ shape=ellipse, label="master host"];
#  WSH [ shape=ellipse, label="worker submitter host"];
#  WSH -> w1 [label="start remote processes"]; WSH->w2; WSH->wN;
#  MH -> m [label = "start local process"];
#}
# @enddot

############################################################################################################
## @page NetworkingLayer Networking and Communication Layer
# The underlying communication layer is invisible to the application. The
# communication layer is responsible for reliable messaging between the master and
# the workers. . From the networking point of view the workers are clients of
# the master service and the communication is always unidirectional: from the
# workers to the master. Therefore the connectivity requirements are minimal
# and consist of: open outbound connectivity from the worker node and at least
# one open port for inbound connectivity (through a firewall) on the master
# node. On the other side the internal communication protocol allows the  
# control of the worker agents by the master as if the communication was bidirectional.
#

############################################################################################################
## @page ComputingModel Computing Model
#
# The computing model is based on the Master/Worker architecture.
#
# The computation consists of many worker processes which communicate with one
# master process (the worker processes do not need to share the filesystem nor
# memory). The ensemble of computation is called a <b>run</b> and it consists
# of many <b>tasks</b> which may be executed in parallel. A task is defined as
# a set of parameters which are produced by the RunMaster (running on a master
# node) and consumed by the WorkerAgent (running on a worker node).  The
# worker produces the output and sends it back to the master. The master keeps
# track of the tasks in order to react to failures in the task execution,
# spawns new tasks or synchronizes them. The mapping of tasks to workers
# (scheduling) in done dynamically at the run-time (by the TaskScheduler and
# ApplicationManager components). The application execution on the worker node
# is controlled by the ApplicationWorker component.
#
# @image html DIANE-Master-Worker-Model.png
# @image latex DIANE-Master-Worker-Model.eps
#
# The DIANE framework handles the communication and networking transparently
# so that an application is plugged in core the system via the higher-level
# software components implemented as python classes. See \ref ApplicationFramework
#
# DIANE comes with some predefined components which cover basic functionality
# and which are ready to use. An advanced user may implement their own
# scheduler and worker classes to support more complex scenarios such as DAGs,
# workflows or other application-specific scheduling and control.
#
# Many (but not all) parallel applications may be efficiently
# and easily mapped to this model. This is true in particular for the
# applications which may be run on the wide-area networks (e.g. on the Grid)
# and which are not communication intensive.
#
############################################################################################################
## @page Architecture Architecture
#
# The main subsystems of DIANE are the following:
#  - <b>Master/Worker Core Services + File Transfer Service</b> provide basic functionality,
#  - (optional) <b>Submitter Scripts</b> help to launch Worker Agents on various distributed infrastructures,
#  - (optional) <b>Directory Service</b> provides additional flexibility for collaborative masters.
#
# @image html DIANE-architecture-subsystems.png
# @image latex DIANE-architecture-subsystems.eps
#
# Various components of DIANE may be used independently in a way which
# provides maximal flexibility for a given use case.  Launching of worker
# agents is independent from starting a master: the worker may be started by
# any means on a remote worker node (even by logging to a remote host and
# starting the worker agent from a shell command line).  For convenience a set
# of submitter scripts based on Ganga (http://cern.ch/ganga) is provided. This
# allows a user to submit worker agents to a variety of batch systems and the
# Grid.  However Master/Worker subsystem does not depend in any way on any
# particualar job submission or process launching system. This allows the
# resources from mutliple infrastructures to be used simultaneously.
#
# @image html DIANE-architecture.png
# @image latex DIANE-architecture.eps
#
# The run master is started on local user machine as a transient service: it
# is created at the beginning of the run and it disappears when the run is
# finished. The master is started in the <tt>runs</tt> directory of the
# <tt>DIANE_USER_WORKSPACE</tt> (default <tt>~/diane</tt>).
# 
#
# The worker agents are started independently of the run master.
# 
# on a distributed infrastructure 
# 
#The worker
# processes are terminated as soon as the run is finished and the worker is not needed anymore. 
#

#
#
# Typically the master is started by the user as a local process on the master
# host. The worker processes are typically started remotely on the worker
# nodes (in \ref QuickStart section we started a worker as a local process).
# How the worker processes are started depends on the underlying computing
# infrastructure (computing backends): batch farms, Grid systems, interactive
# clusters.  DIANE does not depend on any particualar job submission or
# process launching system and resources from mutliple places may be used
# simultaneously.
#
#
# @section Scheduling Scheduling and workload balancing
#

#




## @page EnvironmentVariables EnvironmentVariables
#
#
# @par DIANE_OUTPUT_LEVEL
# Verbosity of the output. If not set then INFO logging level is used. Possible values:
#  - DEBUG
#  - INFO
#  - WARNING
#  - ERROR
#  - CRITICAL
#
# @par DIANE_USER_WORKSPACE
# Local directory where the runs and the user defined application components are stored. Default: \c ~/diane
#
# @par DIANE_CONFIG
# Configuration file. See \ref ConfigurationOptions.
#
# @par DIANE_NO_INTERNAL_SETUP
# For advanced debugging: disable internal environment setup and process
# reexecution.  Some debuggers (e.g. WingIDE) cannot work correctly when the
# process is restarted therefore internal setup should be disabled and instead
# the environment should be set in the system shell (see below).
#
# @par DIANE_PLATFORM
# For advanced debugging: force the platform string on diane.PACKAGE.getPlatform()
#
# <hr>
# @section OtherVariables Other Variables
# These variables are not directly defined by DIANE but may affect it.
#
# @par OMNIORB_CONFIG
# DIANE will ignore this variable because it always uses an internal configuration file for omniORB. User may however
# modify the internal configuration file. TODO: more about it...
# @par X509_CERT_DIR
# @par X509_USER_PROXY
# @par OPENSSL_ALLOW_GRIDPROXY_CERTS
#

## @page EmbedingScripts Embeding user scripts
#
# If you want to import diane packages in your own python script you must be sure to have full diane environment, which
# normally is generated by <tt>diane-env -d</tt> command. So you need to type: <tt>diane-env -d myscript</tt>.
# If you want to avoid this and call the exeutable directly you may put the following header in your executable script: see setup-template.py
#


# trash

## @dot
#digraph example {
#node [shape=record, fontname=Helvetica, fontsize=10];
#edge [ arrowhead="normal", style="solid" ];
#  m [ label="RunMaster" URL="\ref diane.RunMaster.RunMaster"];
#  w1 [ label="WorkerAgent1" URL="\ref diane.WorkerAgent.WorkerAgent"];
#  w2 [ label="WorkerAgent2" URL="\ref diane.WorkerAgent.WorkerAgent"];
#  wN [ label="WorkerAgentN" URL="\ref diane.WorkerAgent.WorkerAgent"];
#  m -> w1;
#  w1 -> m;
#  w2 -> m;
#  m -> w2;
#  m -> wN;
#  wN -> m;
#}
# @enddot
#
