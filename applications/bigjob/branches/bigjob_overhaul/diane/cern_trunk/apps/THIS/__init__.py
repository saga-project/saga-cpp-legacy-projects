from THIS import *


# set transport layer parameters (buffer size, threading model etc)

default_master = """
giopMaxMsgSize = 19100200
threadPerConnectionPolicy = 0
maxServerThreadPoolSize = 150
#dumpConfiguration = 1
"""

default_worker = default_master

