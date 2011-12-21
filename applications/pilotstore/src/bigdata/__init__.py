import os
import logging
import uuid

logging.basicConfig(level=logging.DEBUG, datefmt='%m/%d/%Y %I:%M:%S %p',
                   format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
logger = logging.getLogger(name='bigdata')

# read and log the version of the used BigData
version = "latest"

try:
    fn = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'VERSION')
    version = open(fn).read().strip()
    logger.debug("Loading BigData version: " + version)
except IOError:
    pass


# generate global application id for this instance
#application_id = str(uuid.uuid1())
application_id = "bigdata"


from bigdata.manager.pilotdata_manager import PilotData as MyPilotData
from bigdata.manager.pilotdata_manager import DataUnit as MyDataUnit
from bigdata.manager.pilotjob_manager import PilotJobService as MyPilotJobService
from bigdata.manager.pilotstore_manager import PilotStoreService as MyPilotStoreService
from bigdata.manager.pstar_manager import WorkDataService as MyWorkDataService
from bigdata.troy.compute.api import State as MyState

# define external-facing API

class PilotJobService(MyPilotJobService):
    pass


class PilotStoreService(MyPilotStoreService):
    pass


class WorkDataService(MyWorkDataService):
    pass


class State(MyState):
    pass


class PilotData(MyPilotData):
    pass


class DataUnit(MyDataUnit):
    pass