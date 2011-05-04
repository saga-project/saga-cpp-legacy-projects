from bigjob_diane_backend import *

# set transport layer parameters (buffer size, threading model etc)

default_master = """
giopMaxMsgSize = 19100200
threadPerConnectionPolicy = 0
maxServerThreadPoolSize = 150
#dumpConfiguration = 1
endPoint = giop:tcp:130.39.12.221:
"""

default_worker = default_master
