import DIANE_CORBA 
import DIANE_CORBA__POA

class Peer(DIANE_CORBA__POA.Peer):
    def __init__(self,server,logger):
        self.server = server
        self.logger = logger
        
    def kill(self):
        self.logger.info('I am being killed')
        self.server.stop()
        return True

    def verbosity(self,level, subsystem):
        self.logger.info('verbosity(level=%d,subsystem="%s")',level,subsystem)
        if not subsystem or subsystem == 'diane':
            try:
                if level < 0:
                    return self.logger.level,''
                else:
                    self.logger.setLevel(level)
                    return level,''
            except Exception,x:
                self.logger.error('verbosity("%s"): failed with exception: %s'%(level,str(x)))
                return -1,str(x)
        try:
            tag,param = subsystem.split('.')
        except ValueError:
            msg = 'verbosity(): malformed logging subsystem specification (%s) - ignored'%subsystem
            self.logger.error(msg)
            return -1,msg
        
        if tag == 'omniORB':
            import omniORB
            try:
                p = getattr(omniORB,param)
            except AttributeError:
                msg = 'verbosity(): unknown parameter of omniORB logging (%s) - ignored'%param
                self.logger.error(msg)
                return -1,msg

            try:
                if level < 0:
                    return p(),''
                else:
                    p(level)
                    return p(),''
            except Exception,x:
                self.logger.error(x)
                return -1,"verbosity():"+str(x)

        msg = 'verbosity(): unknown logging subsystem specified (%s) - ignored'%subsystem
        self.logger.error(msg)
        return -1,msg
        
