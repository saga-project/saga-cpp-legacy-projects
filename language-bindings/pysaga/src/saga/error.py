# Package: saga
# Module: error 
# Description: The module which specifies all the error classes
# Specification and documentation can be found in section 3.1, page 35-46 of the GFD-R-P.90 document
# Author: P.F.A. van Zoolingen, Computer Systems Section, Faculty of Exact Science (FEW), Vrije Universiteit, Amsterdam, The Netherlands.
class SagaException(Exception):
	"""
	This is the exception base class inherited by all exceptions raised by a SAGA object implementation.
	@version: 1.0
	"""
	message = ""
	sagaObject = None
	

	def __init__(self, message, sagaObject=None):
		"""
		Initialize an Exception object.
		@summary: initialize an Exception object.
		@param message: the message to be associated with the new exception
		@type message: string
		@param sagaObject: a sagaObject associated with this exception I{(optional)}.	
		@type sagaObject: L{Object}
		"""
		pass

	def getMessage():
		"""
		Returns the message associated with the exception.
		@summary: Returns the message associated with the exception.
		@return: the message
		@rtype: string
		"""
		pass

	def getObject():
		"""
		Returns the L{Object} associated with the exception.
		@summary: Returns the sagaObject associated with the exception.
		@return: the saga Object
		@rtype: L{Object}
		"""
		pass

class NotImplemented(SagaException):
	"""
	If a method is specified in the SAGA API, but cannot be provided by a specific SAGA implementation, 
	this exception is raised.
	"""
	pass

class IncorrectURL(SagaException):
	"""
	This exception is raised if a method is invoked with a URL argument that could not be handled.
	"""
	pass

class BadParameter(SagaException):
	"""
	This exception indicates that at least one of the parameters of the method call is ill-formed, 
	invalid, out of bounds or otherwise not usable.
	"""
	pass

class AlreadyExists(SagaException):
	"""
	This exception indicates that an operation cannot succeed because an entity to
	be created or registered already exists or is already registered, and cannot be
	overwritten.
	"""
	pass

class DoesNotExist(SagaException):
	"""
	This exception indicates that an operation cannot succeed because a required entity is missing.
	"""
	pass

class IncorrectState(SagaException):
	"""
	This exception indicates that the object a method was called on is in a state
	where that method cannot possibly succeed.
	"""
	pass

class PermissionDenied(SagaException):
	"""
	An operation failed because the identity used for the operation did not have
	sufficient permissions to perform the operation successfully. The authentication
	and authorization steps have been completed successfully.
	"""
	pass

class AuthorizationFailed(SagaException):
	"""
	An operation failed because none of the available contexts of the used session
	could be used for successful authorization. 
	"""
	pass

class AuthenticationFailed(SagaException):
	"""
	An operation failed because none of the available session contexts could successfully
	be used for authentication.
	"""
	pass

class Timeout(SagaException):
	"""
	This exception indicates that a remote operation did not complete successfully
	because the network communication or the remote service timed out.
	"""
	pass

class NoSuccess(SagaException):
	"""
	This exception indicates that an operation failed semantically, e.g. the operation
	was not successfully performed. This exception is the least specific exception
	defined in SAGA, and CAN be used for all error conditions which do not indicate
	a more specific exception specified above.
	"""
	pass

