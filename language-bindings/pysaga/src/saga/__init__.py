""" 

This is the API documentation of the SAGA Python language binding. Application
programmers can use this API to create Python applications using SAGA, the
Simple API for Grid Applications. Python applications can then be run on top 
of SAGA implementations using implementations of this language binding. At this
point, an implementation of this language binding is created to be used on top
of the SAGA Java reference implementation. This implementation is called JySaga.
Another project is currently implementing the language binding on top of the
SAGA C++ reference implementation. Using these implementations, it should be 
possible to create a Python application that can run on both SAGA reference 
implementations.

"""

__all__ = ["attributes", "buffer", "context", "error", "file", "job", \
           "logicalfile", "monitoring", "namespace", "object", "permissions", \
           "rpc", "session", "stream", "task", "url"]