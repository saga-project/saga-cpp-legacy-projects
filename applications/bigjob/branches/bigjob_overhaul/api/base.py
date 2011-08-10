""" 

This Module contains the API for the TROY framework.

"""

#
# TROY Resource Description
#
class ResourceDescription():
    """ TROY Resource Description. Describes a resource that is available to the user. """

    # Properties:
    resource_manager_url = None
    scheduling_constraints = None # Performance, Cost, Priority, etc.

    pass


#
# TROY Resource
#
class Resource():
    """ TROY Resource. A by the TROY framework instantiated handle. """

    def __init__(self):
        """ Create a Resource object.

            Keyword arguments:
            None

            Return value:
            Handle to a Resource

        """
        pass

    def cancel(self):        
        """ Remove the resource from the Pilot Service.

            Keyword arguments:
        """
        pass

    def resize_resource(self ):        
        """ (re-)Calibrate the Resource to the given 'size'.
        
            Keyword arguments:
        """
        pass


#
# TROY Pilot Service
#
class PilotService():
    """ TROY PilotService. Dealing with the Pilot Resources. Is used by the WorkUnitService. """

    def __init__(self):
        """ Create a PilotService object.

            Keyword arguments:
            None

            Return value:
            Handle to PilotService

        """
        pass

    def add_resource(self, resource_desc, context=None):
        """ Add a resource to the PilotService

            Keyword arguments:
            resource_desc -- Resource Description
            context -- Security context

            Return value:
            A Resource handle
        """
        pass

    def list(self):
        """ Return the Resources that are managed by this PilotService.  

            Keyword arguments:
            None

            Return value:
            A list of Resources
        """
        pass



#
# TROY WorkUnitService
# 
class WorkUnitService():
    """ The WorkUnitService is the application's access to the Pilot resources. """

    def __init__(self, pilot_service):
        """ Create a Work Service object.

            Keyword arguments:
            pilot_service -- The Pilot Service to which this Work Unit Service will connect

            Return value:
            Handle to Work Service

        """
        pass

    def submit(self, wu):
        """ Submit a WU to this Pilot Service.

            Keyword argument:
            wu -- The Work Unit from the application

            Return:
            WorkUnit object
             

        """
        pass

    def list(self):        
        """ Return the list of WUs that are assigned to the specified Pilot Job.

            Keyword argument:

            Return:
            List of WorkUnit objects

        """
        pass


#
# TROY Work Unit Description
# 
class WorkUnitDescription():
    """ The Work Unit Description is a job/task/call description. """
    pass



#
# TROY Work Unit (WU)
# 
class WorkUnit():
    """ The WorkUnit is the instantiated task that is now handled by the TROY framework. """

    def __init__(self):
        """ Create a new WorkUnit

            Keyword arguments:

            Return value: Handle to WorkUnit

        """
        pass

    def get_description(self):        
        """ Return the description of this WU. """
        pass

    def get_state(self):        
        """ Return the state of the WU. """
        pass

    def get_state_detail(self):        
        """ Return the detailed (application specific) state of the WU. """
        pass

    def cancel(self):
        """ Cancel the WU. """
        pass
