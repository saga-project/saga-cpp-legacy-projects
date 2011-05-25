# user helper classes (to be used in the application plugins)

# File and Base64BinaryFile are helpers to stream Files inside the data messages (so as task parameters and results)
# for example:
# a master may do File.read('somefile') /or File('somefile') and put the result into the task data (say variable x)
# the worker then can do x.write() and this will create 'somefile' in the CWD in the worker filesystem
# the contents of the file is shipped as string: make sure that ORBgiopMaxMsgSize is big enough to hold it
#
# File reads and writes filesin default mode (text)
# Base64BinaryFile additionally does encoding and writes binary files (sometimes useful)

import os

class File:
    def __init__(self,name,dest=None):
        self.name = name
        self.dest = dest
        if self.dest is None:
            self.dest = os.path.basename(self.name)
        self.text = file(name).read()

    def write(self):
        import os
        file(self.dest,'w').write(self.text)
        
    def read(name):
        return File(name)
	
    read = staticmethod(read)

class Base64BinaryFile:
    def __init__(self,name,dest=None):
	import base64
        self.name = str(name)
        self.dest = dest
        if self.dest is None:
            self.dest = os.path.basename(self.name)        
	self.etext = base64.encodestring(file(self.name, 'rb').read())

    def write(self):
        import os
	import base64
        file(self.dest,'wb').write(base64.decodestring(self.etext))
        
    def read(name):
        return Base64BinaryFile(name)

    read = staticmethod(read)


def chmod_executable(path):
    "make a file executable by the current user (u+x)"
    import stat
    os.chmod(path,stat.S_IXUSR|os.stat(path).st_mode)

def is_executable(path):
    "check if the file is executable by the current user (u+x)"
    import stat
    return os.stat(path)[0] & stat.S_IXUSR

