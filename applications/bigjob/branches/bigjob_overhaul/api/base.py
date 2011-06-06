""" Bigjob API

This Module contains the API of the Bigjob framework.
All Bigjob implementation must be derived from this API.

"""


# Dict to pass Resource descriptions values around. 
#Resource = {
#        'name' : None,
#        'type' : None,
#        'url' : None,
#        'queue' : None,
#        'project' : None,
#        'working_directory' : None,
#        'walltime': 0,
#        'userproxy' : None,
#        'agent_executable' : None
#        }

# Dict to pass Resource capabilities around.
#Capabilities = {
#        'number_of_cores' : 0,
#        'number_of_nodes' : 0,
#        'time_to_live' : 0,
#        'local_diskspace_available' : 0,
#        'shared_filesystem' : False
#        }

# Dict to pass UoW requirements around.
#Requirements = {
#        'number_of_cores' : 0,
#        'minimum_runtime' : 0,
#        }

# Dict to describe action to be passed to UoW
#Action = {
#        'application_kernel' : None,
#        'input' : None,
#        'output' : None
#        }

#
# Bigjob class
#
class Bigjob(object):
    """ This class represents the Bigjob. """

    def __init__(self, resource_url, job_desc):
        """ Create a Bigjob object.

            Keyword arguments:
            resource_url -- URL pointing to resource
            job_desc -- SAGA job description
        """
        # self.__uuid = getuuid()
        pass

    def add_resource(self, resource_url, job_desc):
        """ Add a (list of) resource(s) to the Bigjob

            Keyword arguments:
            resource_url -- URL pointing to resource
            job_desc -- SAGA job description
        """
        pass

    def get_resource(self):
        """ Return the resource(s) that is/are under this BigJob.  """
        pass

    def remove_resource(self, resource):
        """ Remove the resource(s) from the Bigjob

            Keyword arguments:

            resource -- a (list of) resource description(s)
        """
        pass
         
    def get_capabilities(self, resource=None):
        """ Return the capabilities that this Bigjob provides.
       
            Keyword arguments:

            resource -- The (optional) resource to act upon.
        """
        pass
    
    def get_uows(self):        
        """ Return the list of UoWs that are assigned to this Bigjob. """
        pass

    def resize_resource(self, resource):        
        """ (re-)Calibrate the resouce in the Bigjob to the given 'size'.
        
            Keyword arguments:
            resource -- The resource to act upon.
        """
        pass

    def assign_uow(self, uow):
        """ Assign a UoW to this Bigjob.

            Keyword argument:
            uow -- The Unit of Work from the application

        """
        pass

    def cancel(self, resource=None):        
        """ Cancel the a resource in the BigJob.

            Keyword arguments:
            resource -- The optional resource(s) to act upon.
        """
        pass


#
# Unit of Work class
# 
class UoW(object):
    """ This class represents the Unit of Work concept in the Bigjob framework. """

    def __init__(self, action, requirements=None):
        """ Create a new Unit of Work.

            Keyword arguments:
            action -- the description of the action for this UoW
            requirements -- the (optional) application requirements for this UoW

        """

        # self.__uuid = getuuid()
        # self.state = New
        # self.action = action.application_kernel
        # self.input = action.input
        # self.output = action.output
        # self.requirements = requirements
        pass

    def get_requirements(self):        
        """ Return the requirements that this UoW has. """
        pass

    def get_state(self):        
        """ Return the state of the UoW. """
        pass

    def get_input(self):        
        """ Return the input(s) of this UoW. """
        pass

    def get_output(self):        
        """ Return the output(s) of this UoW. """
        pass

    def get_action(self):        
        """ Return the action of this UoW. """
        pass

    def cancel(self):
        """ Cancel the Task. """
        pass
