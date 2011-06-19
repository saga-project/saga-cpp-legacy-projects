""" Bigjob Frontend

This Module contains the frontend of the Bigjob framework.

"""

import saga
import uuid

#
# BigJob/Diane implementation
#
from diane.bigjob_diane_frontend import BigjobDiane

class uow_description(saga.job.description):
    pass

class bj_description(saga.job.description):
    pass

#
# Supported list of backends. Static for now.
#
class bigjob_type(object):
    Unknown = 0
    Advert = 1
    DIANE = 2

#
# Isoldate uuid calls
# 
def getuuid():
    return uuid.uuid4()

#
# Bigjob class
#
#class Bigjob(api.base.Bigjob): # make it dependend on api again
class Bigjob(object):

    """ This class represents the Bigjob. """

    def __init__(self, type=None, rm=None, job_desc=None, context=None):
        """ Create a Bigjob object.

            Keyword arguments:
            type -- Backend type
            rm -- URL pointing to resource management backend
            job_desc -- SAGA job description
            context -- security context

            TODO: name/label?

            Return value:
            If resource_url and job_desc are supplied, return a job service.
            Otherwise return None

        """

        self.__uuid = getuuid()
        if type != None and rm != None and job_desc != None:
            self.add_resource(type, rm, job_desc, context)

    def add_resource(self, type, rm, job_desc, context=None):
        """ Add a (list of) resource(s) to the Bigjob

            Keyword arguments:
            type -- Backend type
            resource_url -- URL pointing to resource management backend
            job_desc -- SAGA job description
            context -- a security context
        """

        if type == bigjob_type.Advert:
            raise saga.not_implemented("Advert Backend not yet implemented")
            
        elif type == bigjob_type.DIANE:
            self.bj = BigjobDiane()
            resource_url = rm
            number_nodes = 1 # number nodes for agent
            queue = None
            project = None
            #workingdirectory = "gsiftp://oliver1.loni.org/work/marksant/diane"
            workingdirectory = job_desc.get_attribute('WorkingDirectory')
            userproxy = None # userproxy (not supported yet due to context issue w/ SAGA)
            walltime = None
            processes_per_node = 1
            bigjob_agent = workingdirectory

            self.bj.start_pilot_job(resource_url,
                bigjob_agent,
                number_nodes,
                queue,
                project,
                workingdirectory, 
                userproxy,
                walltime,
                processes_per_node)

        else:
            raise saga.exception.no_success("Unknown Backend type")


    def list_resources(self):
        """ Return the resources that are managed by this BigJob.  

            Keyword arguments:
            None

            Return value:
            A list of resources of type job_service
        """

        pass

    def remove_resource(self, rm):
        """ Remove the resource from the Bigjob

            Keyword arguments:
            resource -- a resource description
        """
        pass
         
    def get_capabilities(self, rm=None):
        """ Return the capabilities that this Bigjob or resource provides.
       
            Keyword arguments:

            resource -- The (optional) resource to act upon.
        """
        pass
    
    def list_uows(self, rm):        
        """ Return the list of UoWs that are assigned to this Bigjob. """
        pass

    def resize_resource(self, rm):        
        """ (re-)Calibrate the resouce in the Bigjob to the given 'size'.
        
            Keyword arguments:
            resource -- The resource to act upon.
        """
        pass

    def assign_uow(self, uow):
        """ Assign a UoW to this Bigjob.

            Keyword argument:
            uow -- The Unit of Work Dfrom the application
            XXX

        """
        return UoW(self.bj, uow)

    def cancel(self, rm=None):        
        """ Cancel the a resource in the BigJob.

            Keyword arguments:
            rm -- The optional resource(s) to act upon.
        """
        pass


#
# Unit of Work class
# 
class UoW(object):
    """ This class represents the Unit of Work concept in the Bigjob framework. """

    def __init__(self, bj, uowd, requirements=None):
        """ Create a new Unit of Work.

            Keyword arguments:
            bj -- the bigjob this corresponds to
            uowd -- the description of this UoW
            requirements -- the (optional) application requirements for this UoW

        """

        self.bj = bj
        self.uuid = bj.submit_job(uowd)

    def get_description(self):        
        """ Return the description of this UoW. """
        pass

    def get_requirements(self):        
        """ Return the requirements that this UoW has. """
        pass

    def get_state(self):        
        """ Return the state of the UoW. """
        return self.bj.get_job_state(self.uuid)

    def get_input(self):        
        """ Return the input(s) of this UoW. """
        pass

    def get_output(self):        
        """ Return the output(s) of this UoW. """
        pass

    def get_error(self):        
        """ Return the error(s) of this UoW. """
        pass

    def cancel(self):
        """ Cancel the Task. """
        pass
