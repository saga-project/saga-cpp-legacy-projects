# NOTE: this module was renamed from exceptions.py due to side effects of
# python relative import and potential class with a builtin exceptions module

class DianeException(Exception):
    def __init__(self,msg,exc=False):
        Exception.__init__(self,msg)
        if exc:
            from diane.util.compatibility import format_exc
            self.exc = format_exc()
        else:
            self.exc = ''
        self.msg = msg

    def log(self,logger):
        logger.error(self.exc+self.msg)

    def warning(self,logger):
        logger.warning(self.exc+self.msg)
        
