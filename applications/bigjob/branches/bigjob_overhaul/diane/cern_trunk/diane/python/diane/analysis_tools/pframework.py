import logging
import logging.handlers

logger = logging.getLogger('diane.pframework')
loghandler = logging.handlers.RotatingFileHandler('pframework.log',maxBytes=100000)
logger.addHandler(loghandler)
loghandler.setFormatter(logging.Formatter('%(message)s')) #"%(asctime)s: %(levelname)s: %(message)s"))
loghandler.setLevel(logging.INFO)

#logger.setLevel(logging.ERROR) #by default with Console input stream don't show parameters in addition to what is already shown by console

import datetime, os

logger.info('*** pframework imported ***')

class Parameter(object):
    """ Parameter represents a variable which is read from the
    (default) input stream.  Depending on a type of the stream, the
    value may be input interactively by the user (ConsoleInputStream),
    read from a file (FileInputStream) or hardcoded (NoInputStream).
    """
    def __init__(self,name,value,doc=None,validator=None,env=None): #,required=None):
        """
        Parameter has a name which should have be a valid python
        identifier (e.g. like a variable name).  The two underscores
        are used to separate the namespaces.  Optional validator is a
        function taking a Parameter object as a single argument and
        returns True if the input is correct.
        If env option is True, then the value will be read from the environment variable DPF_NAME (if it exists)
        where NAME is a capitalized value of self.name. In this case the value defined in the environment takes
        precedence and the value is not read from the input stream.
        """
        #if required is None:
        #    required = True
        #self.required = required

        self.name = parameter_namespace.scope(name)
        self.value = value
        self.doc = doc
        self.validator = validator
        if env is None:
            env = False
        self.env = env


        input_stream.read(self)
        
#else:
#            input_stream.read(self)

        logger.info('%s = %s',self.name,repr(self.value))


def choice_validator(p):
    try:
        p.index = int(p.value)
        try:
            p.value = p.choices[p.index]
            return True
        except IndexError:
            return False
    except ValueError:
        for i,c in enumerate(p.choices):
            if c == p.value:
                p.index = i
                return True
    return False

class ChoiceParameter(Parameter):
    def __init__(self,name,value,choices,docs=None,env=None):
        self.choices = choices
        self.index = None
        doc = ['The choice of %s'%name]
        for i,c in enumerate(choices):
            try:
                if docs[i]:
                    inline_doc = ': ' + docs[i]
                else:
                    inline_doc = ''
            except TypeError:
                inline_doc = ''
            doc.append('%d : %s %s'%(i,str(c),inline_doc))
        doc = '\n'.join(doc)
        super(ChoiceParameter,self).__init__(name,value,doc=doc,validator=choice_validator,env=env)

def choose_and_call(name,f_default,callables,args=[],kwds={},env=None):
    """Call one of the callables using provided *args and **kwds.
    This choice is represented by Parameter name and has a default
    f_default.  Once a choice is made the new namespace is created for
    parameters defined by the execution of a chosen callable.

    Items in callables may be tuples (c,args,kwds) to set the
    *args and **kwds passed to a specific callable.
    """

    # x may be a tuple (c,args,kwds) or just c
    def first_of_tuple(x):
        try:
            return x[0]
        except TypeError:
            return x

    p = ChoiceParameter(name,f_default.__name__,[first_of_tuple(x).__name__ for x in callables],env=env)
        
    print repr(p.value), repr(p.index), repr(p.choices)

    x = callables[p.index]

    try:
        f,fargs,fkwds = x
    except TypeError:
        f = x
        fargs = args
        fkwds = kwds
    parameter_namespace.open(f.__name__)
    f(*fargs,**fkwds)
    parameter_namespace.close()



class ParameterError(Exception):
    def __init__(self,p,x=None):
        self.p = p
        self.x = x
    def __str__(self):
        return "%s: %s %s (%s)"%(self.__class__.__name__,self.p.name,repr(self.p.value),self.x)

class ValidationError(ParameterError):
    pass

class ParameterValueNotFoundError(ParameterError):
    pass

class ParameterDefinedInEnvironment(Exception):
    pass

# --------------------- Streams ----------------------

class InputStream(object):
    def __init__(self):
        self.params = []
        self.pdict = {}

    def read(self,p):
        if self.pdict.has_key(p.name):
            raise ValueError('Parameter %s already defined'%p.name)
        self.pdict[p.name] = p
        self.params.append(p)
        if p.env:
            try:
                #print 'trying env', "DPF_%s"%self.name.upper()
                p.value = os.environ["DPF_%s"%p.name.upper()]
                if p.validator and not p.validator(p):
                    raise ValidationError(p)
                else:
                    raise ParameterDefinedInEnvironment()
            except KeyError:
                pass
                
    def dump(self):
        for p in self.params:
            inline_doc = ''
            if p.doc:
                lines = p.doc.splitlines()
                if len(lines)>1:
                    for l in lines:
                        print '#',l
                else:
                    inline_doc = '# '+ p.doc
                        
            print '%s = %s %s'%(p.name,repr(p.value),inline_doc)


class ConsoleInputStream(InputStream):
    def read(self,p):
        try:
            super(ConsoleInputStream,self).read(p)
        except ParameterDefinedInEnvironment:
            return

        self.input_ok = False

        def validate(p):
            #print 'validating input',p.value,p.validator,p.validator(p)
            if p.validator and p.validator(p):
                self.input_ok = True
            if not p.validator:
                self.input_ok = True

        while not self.input_ok:
            if p.doc:
                print p.doc
            s = raw_input('Enter %(name)s (default=%(value)s): '%p.__dict__)
            if s:
                # for convenience we don't need to quote strings
                if type(p.value) is type(''):
                    p.value = s
                    validate(p)
                else:
                    try:
                        p.value = eval(s)
                        validate(p)
                    except Exception,x:
                        print repr(x)
            else:
                validate(p)

class NoInputStream(InputStream):
    def read(self,p):
        try:
            super(NoInputStream,self).read(p)
        except ParameterDefinedInEnvironment:
            return

        if p.validator and not p.validator(p):
            raise ValidationError(p)

class FileInputStream(InputStream):
    def __init__(self,fn):
        super(FileInputStream,self).__init__()
        self.fn = fn
        self.input_values = {}
        execfile(fn,globals(),self.input_values)

    def read(self,p):
        try:
            super(FileInputStream,self).read(p)
        except ParameterDefinedInEnvironment:
            return

        try:
            p.value = self.input_values[p.name]
        except KeyError:
            raise ParameterValueNotFoundError(p)

        if p.validator and not p.validator(p):
            raise ValidationError(p)


# --------------------- Namespaces ----------------------


class NestedParameterNamespace(object):
    def __init__(self):
        self.namestack = []

    def scope(self,name=''):
        return '__'.join(self.namestack+[name])

    def open(self,n):
        self.namestack.append(n)
        
    def close(self):
        return self.namestack.pop()

class FlatParameterNamespace(object):
    def scope(self,name=''):
        return name
    def open(self,n):
        pass
    def close(self):
        pass


def setInputStream(s):
    global input_stream
    #logger.setLevel(logging.INFO) #show parameters as they are defined
    input_stream = s

parameter_namespace = NestedParameterNamespace()
#parameter_namespace = FlatParameterNamespace()

# --------------------- Configuration ----------------------


input_stream = ConsoleInputStream()
#input_stream = NoInputStream()
#input_stream = FileInputStream('test.params')




if __name__ == '__main__':
    def f(aaa):
        print 'calling f(%s)'%aaa
        c = Parameter('string','some string').value

    def g(bbb,xxx,k=2):
        print 'calling g(%s)'%bbb,xxx,k
        a = Parameter('a',1.0,doc='some parameter').value
        b = Parameter('b',1.0).value

        choose_and_call('sub_function',f,[f,g],[1])

    choose_and_call('main_function',g,[f,(g,[1,2,3],{})],[1])

    print 

    input_stream.dump()
