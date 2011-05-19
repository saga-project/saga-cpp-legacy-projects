""" Bigjob API

This Module contains the API of the Bigjob framework.
All Bigjob implementation must be derived from this API.

"""

#
# Bigjob data structures
# 
class BigjobResource():
    """class to pass resource descriptions values around"""
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
    """class to pass resource capabilities around"""
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
        """ Create a Bigjob object. Takes a resource description as only argument. """
        # self.__uuid
        pass
         
    def get_state(self):        
        """ Return the state of the Bigjob. """
        pass

    def get_capabilities(self):
        """ Return the capabilities that this Bigjob provides. """
        pass
    
    def get_subjobs(self):        
        """ Return the list of Subjobs that are submitted to this Bigjob. """
        pass

    def resize(self):        
        """ (re-)Calibrate the BigJob to the given 'size'. """
        pass

    def submit_subjob(self, subjob):
        """ Execute a Subjob to this Bigjob. """
        pass

    def cancel(self):        
        """ Cancel the BigJob and its Subjobs. """
        pass






#
# Subjob data structures
# 
class SubjobRequirements():
    """ Class to pass Subjob requirements around. """
    def __init__(self):
        # self.number_of_cores
        # self.minimum_runtime
        pass

class SubjobInfo():
    """ Class to describe a task. """
    def __init__(self):
        # self.runtime
        # self.location
        pass

class SubjobTask():
    """ Class to describe a task. """
    def __init__(self):
        # self.state
        # self.program
        # self.inputs
        # self.outputs
        pass

#
# Subjob class
# 
class Subjob():
    """ This class represents the Subjob concept in the Bigjob framework.
        It is the representation of the schedulable entity.
    """

    def __init__(self, task_list, requirements):
        # self.__uuid
        pass

    def get_requirements(self):        
        """ Return the requirements that this Subjob has """
        pass

    def get_bigjob(self):        
        """ Return the Bigjob where this Subjob is running """
        pass

    def set_bigjob(self):        
        """ Set the Bigjob where this Subjob is running """
        pass
    
    def get_state(self):        
        """ Return the (simple) state of the Subjob """
        pass

    def get_info(self):        
        """ Return structured info about the Subjob """
        pass
    
    def get_task_info(self):        
        """ Return info about the individual tasks """
        pass

    def cancel(self):
        """ Cancel the Subjob and its tasks """
        pass
