""" 

This Module contains the frontend of the TROY framework.

"""

#
# Supported list of Pilots. Static for now.
#
class TROY_TYPE():
    UNKNOWN = 0
    BIGJOB = 1
    DIANE = 2

#
# Pilot Description (for now just a saga job description)
#
class PilotDescription():
    pass

#
# Pilot (for now just a saga job description)
#
class Pilot():
    def cancel_pilot(self, rm=None):        
        """ Cancel the a resource in the Pilot Service.

            Keyword arguments:
            rm -- The optional resource(s) to act upon.
        """
        pass

    def resize_pilot(self, pilot):        
        """ (re-)Calibrate the Pilot to the given 'size'.
        
            Keyword arguments:
            rm -- The resource to act upon.
        """
        pass

#
# TROY Pilot Service class
#
class PilotService():
    """ This class represents the TROY PilotService. """

    def __init__(self):
        """ Create a PilotService object.

            Keyword arguments:
            None

            Return value:
            Handle to PilotService

        """
        pass


    def create_pilot(self, type, rm, pilot_desc, context=None):
        """ Add a (list of) resource(s) to the PilotService

            Keyword arguments:
            type -- TROY Backend type
            rm -- URL pointing to resource management backend
            pilot_desc -- Pilot Description
            context -- a security context

            Return value:
            A Pilot handle
        """
        pass


    def list(self):
        """ Return the pilots that are managed by this PilotService.  

            Keyword arguments:
            None

            Return value:
            A list of pilots
        """
        pass

    def get_capabilities(self, rm=None):
        """ Return the capabilities that this Pilot Job Service or resource provides.
       
            Keyword arguments:

            rm -- The (optional) resource to act upon.
        """
        pass
    


#
# WorkService class
# 
class WorkUnitService():
    """ This class manages WorkUnit's """

    def __init__(self):
        """ Create a Work Service object.

            Keyword arguments:
            None

            Return value:
            Handle to Work Service

        """
        pass

    def submit(self, wu):
        """ Submit a WU to this Pilot Service.

            Keyword argument:
            wu -- The Work Unit from the application

            Return: WorkUnit
             

        """
        pass

    def get_workunits(self):        
        """ Return the list of WUs that are assigned to the specified Pilot Job. """
        pass

#
# Work Unit Description (for now just a saga job description)
# 
class WorkUnitDescription():
    pass


#
# Work Unit (WU) class
# 
class WorkUnit():
    """ This class represents the Work Unit concept in the P* model. """

    def __init__(self, wu):
        """ Create a new WorkUnit

            Keyword arguments:
            wu -- the description of this WU

        """
        pass

    def get_description(self):        
        """ Return the description of this WU. """
        pass

    def get_state(self):        
        """ Return the state of the WU. """
        pass

    def cancel(self):
        """ Cancel the WU. """
        pass
