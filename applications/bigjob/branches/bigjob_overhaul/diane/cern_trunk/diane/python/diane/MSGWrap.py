import diane, sys, time, os

config = diane.getConfig('MSGMonitoring')

# MSG specific options
config.addOption('MSG_MONITORING_ENABLED', True, 'enable/disable MSG monitoring')
config.addOption('MSG_SERVER', 'ganga.msg.cern.ch:6163', 'MSG server')
config.addOption('MSG_EXIT_TIMEOUT',5, 'Maximum seconds to clear queued monitoring messages on exit.')

# username and password was requested by the CERN MSG team for accounting purposes
# this is not a security measure, this change goes along with the stomputil version 2.4 which
# fixes the disconnect frames of the STOMP protocol
config.addOption('MSG_USERNAME','ganga','This is used for accounting purposes and NOT for security')
config.addOption('MSG_PASSWORD','analysis','This is used for accounting purposes and NOT for security')

if os.environ.has_key('DIANE_MSG_TEST'):
    config.log_config()
    config.MSG_SERVER = 'gridmsg001.cern.ch:6163' # USE TEST SERVER INSTEAD
    

from diane.logger import getLogger
log = getLogger('MSG-Logger')

publisher = None

def create_publisher():
    global publisher
    import stomputil
    server, port = config.MSG_SERVER.split(':')
    port = int(port)
    #MSGUtil.SERVER, MSGUtil.PORT = server, int(port)
    log.debug('Creating publisher...')
    publisher = stomputil.createPublisher(diane.BaseThread.BaseThread, server, port, config.MSG_USERNAME, config.MSG_PASSWORD, logger=log)
    publisher.start()
    publisher.addExitHandler(config.MSG_EXIT_TIMEOUT)

def send(dst, msg): # entopic the msg in msg_q for the connection thread to consume and send
    if not publisher:
        create_publisher()
    if config.MSG_MONITORING_ENABLED:
        if not msg.has_key('_publisher_t'): # journalEntry has own publisher timestamp which is identical to the one in the journal
            msg['_publisher_t'] = time.time() # add publisher timestamp
        publisher.send(dst, repr(msg)) # need to repr() for stomputil2.0

def sendStatus(label, _data):
    data = _data.copy()
    data['event'] = label
    log.debug('Creating message %s' % data)
    send('/queue/diane.status', data)
                
def journalEntry(msg):
    [t,label,_data] = msg
    data = _data.copy()
    data['event'] = label
    data['_publisher_t'] = t
    log.debug('Creating message (Master) %s' % data)
    send('/queue/diane.journal', data)
