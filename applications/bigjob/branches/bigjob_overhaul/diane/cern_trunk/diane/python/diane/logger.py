
import os,sys
import logging

# we use "USER" level to track progress of processing, the "INFO" level is more verbose
# we also add a user() method to the Logger class, so that we can easily use this new level from the code 
logging.addLevelName(25,'USER')

logging.USER = 25

def user(self,msg, *args, **kwargs):
    self.log(logging.USER, msg, *args, **kwargs)

import diane.util
diane.util.funcToMethod(user,logging.Logger)

# level is a public, read-only attribute of this module (used to determine the current log level of the console)
# use setLevel() to modify the current level
level = logging.USER

try:
    level = getattr(logging,os.environ['DIANE_OUTPUT_LEVEL'])
except KeyError:
    pass
except AttributeError,x:
    print >> sys.stderr, 'ERROR: $DIANE_OUTPUT_LEVEL: cannot set the logging level:',x

FORMAT = '%(asctime)s: %(message)s'

try:
    logging.basicConfig(level=logging.NOTSET,format=FORMAT)
except TypeError: # < 2.5
    logging.basicConfig()

root_logger = logging.getLogger()
root_logger.setLevel(logging.NOTSET)
root_handler = root_logger.handlers[0]
root_handler.setLevel(level)

class ColourFormatter(logging.Formatter):
    def __init__(self,stream,*args,**kwds):
        logging.Formatter.__init__(self,*args,**kwds)
        from diane.util.TerminalController import TerminalController
        self.term = TerminalController(stream)

        self.colours = { logging.INFO : self.term.NORMAL,
                         logging.WARNING : self.term.MAGENTA,
                         logging.ERROR: self.term.RED,
                         logging.CRITICAL: self.term.RED+self.term.BOLD,
                         logging.DEBUG: self.term.BLACK }
        
    def markup(self,s,level):
        c = self.colours.get(level,self.term.NORMAL)
        return c + s + self.term.NORMAL
        
    def format(self,record):
        s = logging.Formatter.format(self,record)
        s = self.markup(s,record.levelno)
        return s

root_handler.setFormatter(ColourFormatter(root_handler.stream,FORMAT))


def addFileLog(name):
    file_handler = logging.FileHandler(name)
    file_handler.setLevel(logging.DEBUG)
    file_handler.setFormatter(logging.Formatter('%(asctime)s %(name)s %(levelname)s: %(message)s'))
    addHandler(file_handler)

def addHandler(h):
    logger.addHandler(h)

def setLevel(name,override_user_setting=True):
    try:
        if not override_user_setting and os.environ.has_key('DIANE_OUTPUT_LEVEL'):
            root_logger.debug('logging level set by the user %s will not be overriden by %s',os.environ['DIANE_OUTPUT_LEVEL'],name)
            return
        root_logger.debug('setting root logger level to "%s"',name)
        level = getattr(logging,name)
        root_handler.setLevel(level)
        os.environ['DIANE_OUTPUT_LEVEL'] = name
    except AttributeError,x:
        root_logger.error('cannot set the logging level:',x)
    
def getLogger(name):
    return logging.getLogger('DIANE.'+name)

# this is the default logger
logger = logging.getLogger('DIANE')

# function decorator: log function arguments and return value
def logit(f,logger=logger):
    def log_f(*args,**kwds):
        fc = "%s(%s)"%(f.__name__, ','.join([str(a) for a in args ]+["%s=%s"%x for x in kwds.items()]))
        logger.debug(fc + '...')
        r = f(*args,**kwds)
        logger.debug("%s(...) -> %s",f.__name__,str(r))
    return log_f

#formatter = logging.Formatter('%(name)-12s: %(levelname)-8s %(message)s')
#console = logging.StreamHandler()
#console.setLevel(logging.INFO)
#console.setFormatter(formatter)
#logger.addHandler(console)
