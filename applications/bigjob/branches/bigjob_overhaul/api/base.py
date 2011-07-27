""" 

This Module contains the frontend of the TROY framework.

"""

#
# Supported list of PilotJobs. Static for now.
#
class TROY_TYPE():
    UNKNOWN = 0
    BIGJOB = 1
    DIANE = 2


#
# TROY PilotJobService class
#
class PilotJobService():
    """ This class represents the TROY PilotJobService. """

    def __init__(self, troy_type=None, rm=None, job_desc=None, context=None):
        """ Create a PilotJobService object.

            Keyword arguments:
            troy_type -- Pilot type
            rm -- URL pointing to resource management backend
            job_desc -- SAGA job description
            context -- security context

            TODO: name/label?

            Return value:
            If resource_url and job_desc are supplied, return a job service.
            Otherwise return None

        """
        pass


    def add_resource(self, troy_type, rm, job_desc, context=None):
        """ Add a (list of) resource(s) to the PilotJobService

            Keyword arguments:
            troy_type -- Backend type
            rm -- URL pointing to resource management backend
            job_desc -- SAGA job description
            context -- a security context
        """
        pass


    def list_resources(self):
        """ Return the resources that are managed by this PilotJobService.  

            Keyword arguments:
            None

            Return value:
            A list of resources of type job_service
        """
        pass

    def remove_resource(self, rm):
        """ Remove the resource from the Pilot Job Service

            Keyword arguments:
            rm -- a resource description
        """
        pass
         
    def get_capabilities(self, rm=None):
        """ Return the capabilities that this Pilot Job Service or resource provides.
       
            Keyword arguments:

            rm -- The (optional) resource to act upon.
        """
        pass
    
    def list_workunits(self, rm=None):        
        """ Return the list of WUs that are assigned to the specified Pilot Job. """
        pass

    def resize_resource(self, rm):        
        """ (re-)Calibrate the Pilot Job to the given 'size'.
        
            Keyword arguments:
            rm -- The resource to act upon.
        """
        pass

    def submit(self, wu):
        """ Submit a WU to this Pilot Job Service.

            Keyword argument:
            wu -- The Work Unit from the application

        """
        pass

    def cancel(self, rm=None):        
        """ Cancel the a resource in the Pilot Job Service.

            Keyword arguments:
            rm -- The optional resource(s) to act upon.
        """
        pass

#
# Work Unit Description (for now just a saga job description)
# 
class WorkUnit_Description():
    pass


#
# Work Unit (WU) class
# 
class WorkUnit():
    """ This class represents the Work Unit concept in the P* model. """

    def __init__(self, pjs, wu, requirements=None):
        """ Create a new WorkUnit

            Keyword arguments:
            pjs -- the troy pilotservice this corresponds to
            wu -- the description of this WU
            requirements -- the (optional) application requirements for this WU

        """
        pass

    def run(self, rm):
        """ run the WU """
        pass

    def get_description(self):        
        """ Return the description of this WU. """
        pass

    def get_requirements(self):        
        """ Return the requirements that this WU has. """
        pass

    def get_state(self):        
        """ Return the state of the WU. """
        pass

    def get_input(self):        
        """ Return the input(s) of this WU. (convenience function) """
        pass

    def get_output(self):        
        """ Return the output(s) of this WU. (convenience function) """
        pass

    def get_error(self):        
        """ Return the error(s) of this WU. """
        pass

    def cancel(self):
        """ Cancel the WU. """
        pass
