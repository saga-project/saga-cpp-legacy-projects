# Package: pysaga
# Module: error 
# Description: The module which specifies all the error classes
# Specification and documentation can be found in section 3.1, page 35-46 of 
#	the GFD-R-P.90 document
# Author: P.F.A. van Zoolingen, Computer Systems Section, Faculty of 
#	Exact Science (FEW), Vrije Universiteit, Amsterdam, The Netherlands.
class SagaException(Exception):
	"""
	This is the exception base class inherited by all exceptions raised by a 
	SAGA object implementation.
	
	@version: 1.0, designed for Python 2.x
	
	"""

	def __init__(self, message, saga_object=None):
		"""Initialize an Exception object.
		
		@summary: Initialize an Exception object.
		@param message: the message to be associated with the new exception
		@type message: string
		@param saga_object: a SAGA base object associated with this exception.	
		@type saga_object: L{Object}
		
		"""


	def get_message(self):
		"""Returns the message associated with the exception.
		
		@summary: Returns the message associated with the exception.
		@return: the message
		@rtype: string
		
		"""

       

	def get_object(self):
		"""Returns the SAGA base L{Object} associated with the exception.
		
		@summary: Returns the SAGA base L{Object} associated with the exception.
		@return: the saga Object
		@rtype: L{Object}
		
		"""
		
	
	message = property(get_message, 
		doc="""The message associated with the exception.
			@type: string""")
	
	saga_object = property(get_object, 
		doc="""The object associated with the exception.
			@type: L{Object}""")
	
	
class NotImplemented(SagaException):
	"""
	If a method is specified in the SAGA API, but cannot be provided by a 
	specific SAGA implementation, this exception is raised.
	"""


class IncorrectURL(SagaException):
	"""
	This exception is raised if a method is invoked with a URL argument that 
	could not be handled.
	"""


class BadParameter(SagaException):
	"""
	This exception indicates that at least one of the parameters of the method 
	call is ill-formed, invalid, out of bounds or otherwise not usable.
	"""


class AlreadyExists(SagaException):
	"""
	This exception indicates that an operation cannot succeed because an entity 
	to be created or registered already exists or is already registered, and 
	cannot be overwritten.
	"""


class DoesNotExist(SagaException):
	"""
	This exception indicates that an operation cannot succeed because a required 
	entity is missing.
	"""


class IncorrectState(SagaException):
	"""
	This exception indicates that the object a method was called on is in a 
	state where that method cannot possibly succeed.
	"""


class PermissionDenied(SagaException):
	"""
	An operation failed because the identity used for the operation did not have
	sufficient permissions to perform the operation successfully. The 
	authentication and authorization steps have been completed successfully.
	"""


class AuthorizationFailed(SagaException):
	"""
	An operation failed because none of the available contexts of the used 
	session could be used for successful authorization. 
	"""


class AuthenticationFailed(SagaException):
	"""
	An operation failed because none of the available session contexts could 
	successfully be used for authentication.
	"""


class Timeout(SagaException):
	"""
	This exception indicates that a remote operation did not complete 
	successfully because the network communication or the remote service timed 
	out.
	"""


class NoSuccess(SagaException):
	"""
	This exception indicates that an operation failed semantically, e.g. the 
	operation was not successfully performed. This exception is the least 
	specific exception defined in SAGA, and can be used for all error conditions 
	which do not indicate a more specific exception specified above.
	"""


