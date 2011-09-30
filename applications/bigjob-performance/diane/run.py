# tell DIANE that we are just running executables
# the ExecutableApplication module is a standard DIANE test application

from diane_test_applications import ExecutableApplication as application
import datetime

# the run function is called when the master is started
# input.data stands for run parameters
def run(input,config):
	config.MSGMonitoring.MSG_MONITORING_ENABLED = False
	d = input.data.task_defaults # this is just a convenience shortcut

	# all tasks will share the default parameters (unless set otherwise in individual task)
	#d.output_files = ['message.out']
	d.input=['/bin/date']
	d.executable = 'date'
	#d.commands = ['time.sleep(0.3)']*10

	# here are tasks differing by arguments to the executable
	for i in range(20):
		t = input.data.newTask()
		t.executable = '/bin/date'
		t.args = [str(i)]
