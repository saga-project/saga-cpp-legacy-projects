""" Bigjob API

This Module contains the API of the Bigjob framework.
All Bigjob implementation must be derived from this API.

"""

#
# Bigjob data structures
# 
class BigjobResource():
    """ Class to pass resource descriptions values around. """
    def __init__(self):
        # self.name 
        # self.type 
        # self.url
        # self.queue
        # self.project
        # self.working_directory
        # self.walltime
        # self.userproxy
        # self.agent_executable
        pass

class BigjobCapabilities():
    """ Class to pass resource capabilities around. """

    def __init__(self):
        # self.number_of_cores
        # self.number_of_nodes
        # self.time_to_live
        pass

#
# Bigjob class
#
class Bigjob():
    """ This class represents the Bigjob. """

    def __init__(self, resource):
        """ Create a Bigjob object.
            Takes a resource description as only argument.
        """
        # self.__uuid
        pass
         
    def get_state(self):        
        """ Return the state of the Bigjob. """
        pass

    def get_capabilities(self):
        """ Return the capabilities that this Bigjob provides. """
        pass
    
    def get_subjobs(self):        
        """ Return the list of Subjobs that are running in this Bigjob. """
        pass

    def get_tasks(self):        
        """ Return the list of Tasks that are running in this Bigjob. """
        pass

    def resize(self):        
        """ (re-)Calibrate the BigJob to the given 'size'. """
        pass

    def assign_task(self, task):
        """ Assign a Task to this Bigjob. """
        pass

    def cancel(self):        
        """ Cancel the BigJob and its Subjobs/Tasks. """
        pass


#
# Subjob data structures
#
class SubjobInfo():
    """ Read-only class to describe a SubJob. 
        It is the internal representation of the schedulable entity inside
        a Bigjob, but its information is exposed through the Bigjob API.
    """

    def __init__(self):
        # self.runtime
        # self.location
        # self.size
        pass


#
# Task data structures
# 
class TaskRequirements():
    """ Class to pass Task requirements around. """

    def __init__(self):
        # self.number_of_cores
        # self.minimum_runtime
        # self.color_of_the_walls_at_the_datacenter
        pass

#
# Task class
# 
class Task():
    """ This class represents the Task concept in the Bigjob framework. """

    def __init__(self, requirements):
        # self.__uuid
        # self.state
        # self.action
        # self.inputs
        # self.outputs
        pass

    def get_requirements(self):        
        """ Return the requirements that this Task has. """
        pass

    def get_bigjob(self):        
        """ Return the Bigjob where this Task is running. """
        pass

    def get_state(self):        
        """ Return the state of the Task. """
        pass

    def get_subjob(self):        
        """ Return the Subjob that this Tasks runs in. """
        pass
    
    def cancel(self):
        """ Cancel the Task. """
        pass
