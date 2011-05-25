from dianetime import ctime

from File import File, chmod_executable

# cookbook
def importName(modulename, name):
    """ Import a named object from a module in the context of this function,
        which means you should use fully qualified module paths.

        Return None on failure.
    """
    try:
        module = __import__(modulename, globals(), locals(), [name])
    except ImportError:
        return None

    #it is necessary to check for KeyError exception in case the module exists but doesn't contain specified object 
    try:
        return vars(module)[name]
    except KeyError:
        return None


def funcToMethod(func,clas,method_name=None):
    setattr(clas,method_name or func.__name__, func)
