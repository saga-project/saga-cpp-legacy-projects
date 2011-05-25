
# configuration subsystem

from diane.diane_exceptions import DianeException

class ConfigError(DianeException):
    pass

def getConfig(name):
    """Return the configuration section object with the specified name or, if
    name is None, return the object which is the container of all
    configuration sections.

    All calls to this function with a given name return the same configuration
    object instance. This means that configuration instances never need to be
    passed between different parts of an application.
    """
    
    global __the_config
    
    if not name:
        return __the_config
        
    try:
        return __all_configs[name]
    except KeyError:
        __all_configs[name] = ConfigSection(name)
        return __all_configs[name]

def log_configuration(title=None,loglevel=None):
    """ Print all configuration options using the logger at the specified
    loglevel (default: INFO).
    """
    _make_logger()
    if loglevel is None:
        import logging
        loglevel = logging.INFO

    if title is None:
        title = 'configuration'

    logger.log(loglevel,'='*30)
    logger.log(loglevel,title)
    logger.log(loglevel,'-'*30)
    for c in __all_configs.values():
        c.log_config(loglevel)
    logger.log(loglevel,'='*30)        

def load_config_file():
     """ Initialize all config sections and load the configuration file -
     specified by DIANE_CONFIG environment varialble.
     """

     # all modules which define config sections should be imported at this point
     import diane.WorkerAgent
     import diane.RunMaster
     import diane.FileTransfer.config
     import diane.MSGWrap
     import os

     _make_logger()
        
     config_file_name = None
     try:
         config_file_name = os.environ['DIANE_CONFIG']
         logger.info('DIANE_CONFIG file: %s',config_file_name)
     except KeyError:
         logger.debug('DIANE_CONFIG variable not set')

     if config_file_name:
         if not os.path.exists(config_file_name):
             raise ConfigError('configuration file not found: %s'%config_file_name)
         try:
             execfile(config_file_name,{'config':__the_config})
         except Exception,x:
             logger.exception('problem reading $DIANE_CONFIG file (%s): %s',config_file_name,x)
             raise
         
def initialize():
    getConfig(None)
    load_config_file()

def capture_config():
    _make_logger()
    logger.debug('capturing current configuration')
    import copy
    return copy.deepcopy(__all_configs)

def restore_config(capture):
    _make_logger()
    logger.debug('restoring configuration')    
    for c_src in capture.values():
        c_dest = getConfig(c_src.getName())
        for opt in c_src:
            try:
                setattr(c_dest,opt.name,opt.value)
            except AttributeError:
                c_dest.addOption(opt.name,opt.value,opt.doc)
    
######################################################################
# internal implementation

class ConfigSectionContainer:
    """ Collection of user-defined configuration parameters.
    This class is just syntactic sugar for the standard getConfig(name) interface.
    """

    def __init__(self):
        pass

    def __getattr__(self,name):
        if name[:2] == "__" and name [-2:] == "__":
            raise AttributeError,name
        
        if self.__dict__.has_key(name):
            return name
        else:
            if hasConfig(name):
                c = getConfig(name)
                self.__dict__[name] = c
                return c
            else:
                raise AttributeError, "config section '%s' not defined. this may be wrong usage (check the name!) or incomplete implementation of load_config_file() in config.py module."%name

    def __iter__(self):
        import diane.config
        return getattr(diane.config,'__all_configs').values().__iter__()

class ConfigOption:
    def __init__(self,name,value,doc):
        self.name = name
        self.value = value
        self.doc = doc
        
class ConfigSection:
    def __init__(self,name):
        self.__dict__['_ConfigSection__name'] = name
        self.__dict__['_ConfigSection__options'] = {}

    def getName(self):
        return self.__name
    
    def addOption(self,name,value,doc):
        self.__options[name] = ConfigOption(name,value,doc)

    def __getattr__(self,name):
        if name[:2] == "__" and name [-2:] == "__":
            raise AttributeError,name
        
        return self.__options[name].value

    def __iter__(self):
        return self.__options.values().__iter__()
    
    def __setattr__(self,name,value):
        try:
            opt = self.__options[name]
            opt.value = value
        except KeyError,x:
            raise AttributeError,name

    def log_config(self,loglevel=None):
        _make_logger()
        if loglevel is None:
            import logging
            loglevel = logging.INFO

        for o in self.__options:
            opt = self.__options[o]
            logger.log(loglevel,"config.%s.%s = %s",self.__name,o,opt.value)

def hasConfig(name):
    return __all_configs.has_key(name)

def _make_logger():
    global logger
    if logger is None:
        from diane import getLogger
        logger = getLogger('config')
            
__all_configs = {}

__the_config = ConfigSectionContainer()

logger = None


if __name__ == "__main__":
    #import sys
    #sys.setrecursionlimit(10)
    log_configuration('initial')
    c = getConfig('Test')
    c.addOption('x',10,'some option')
    log_configuration('option added')
    assert c.x == 10
    print 'option = ',c.x

    print c
    
    c.x = 20
    log_configuration('option manually set in the config object')
    print 'option = ',c.x    
    assert c.x == 20    

    cfg = capture_config()
    
    import tempfile
    f = tempfile.NamedTemporaryFile()
    f.write('config.Test.x=30')
    f.flush()
    import os
    os.environ['DIANE_CONFIG'] = f.name
    load_config_file()
    log_configuration('config file read')
    print 'option = ',c.x
    assert c.x == 30

    restore_config(cfg)
    log_configuration('config restored')
    print 'option = ',c.x
    assert c.x == 20

    
    
    
        
