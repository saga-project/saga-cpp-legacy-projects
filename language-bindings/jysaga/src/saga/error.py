# Package: saga
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
	@version: 1.0
	"""

	def __init__(self, message, saga_object=None):
		"""
		Initialize an Exception object.
		@summary: Initialize an Exception object.
		@param message: the message to be associated with the new exception
		@type message: string
		@param saga_object: a SAGA base object associated with this exception.	
		@type saga_object: L{Object}
		"""
		#TODO: wade through erroneous code in here and Pysaga
		
		if message is not None:
			self._message = message
		else:
			raise BadParameter, "parameter message is None"
			
		if saga_object is not None:
			from saga.object import Object
			if not issubclass(saga_object.__class__, Object):
				raise BadParameter, "parameter saga_object is not an Object"
			else:
				self._saga_object = saga_object
		else:
			self._saga_object = None

	def __str__(self):
		return repr(self._message)

	def get_message(self):
		"""
		Returns the message associated with the exception.
		@summary: Returns the message associated with the exception.
		@return: the message
		@rtype: string
		"""
		return self._message

	def get_object(self):
		"""
		Returns the SAGA base L{Object} associated with the exception.
		@summary: Returns the SAGA base L{Object} associated with the exception.
		@return: the saga Object
		@rtype: L{Object}
		"""
		return self._saga_object

	message = property(get_message, 
		doc="""The message associated with the exception.\n@type: string""")
	saga_object = property(get_object, 
		doc="""The object associated with the exception.\n@type: L{Object}""")

class NotImplemented(SagaException):
	"""
	If a method is specified in the SAGA API, but cannot be provided by a 
	specific SAGA implementation, this exception is raised.
	"""
	pass

class IncorrectURL(SagaException):
	"""
	This exception is raised if a method is invoked with a URL argument that 
	could not be handled.
	"""
	pass

class BadParameter(SagaException):
	"""
	This exception indicates that at least one of the parameters of the method 
	call is ill-formed, invalid, out of bounds or otherwise not usable.
	"""
	pass

class AlreadyExists(SagaException):
	"""
	This exception indicates that an operation cannot succeed because an entity 
	to be created or registered already exists or is already registered, and 
	cannot be overwritten.
	"""
	pass

class DoesNotExist(SagaException):
	"""
	This exception indicates that an operation cannot succeed because a required 
	entity is missing.
	"""
	pass

class IncorrectState(SagaException):
	"""
	This exception indicates that the object a method was called on is in a 
	state where that method cannot possibly succeed.
	"""
	pass

class PermissionDenied(SagaException):
	"""
	An operation failed because the identity used for the operation did not have
	sufficient permissions to perform the operation successfully. The 
	authentication and authorization steps have been completed successfully.
	"""
	pass

class AuthorizationFailed(SagaException):
	"""
	An operation failed because none of the available contexts of the used 
	session could be used for successful authorization. 
	"""
	pass

class AuthenticationFailed(SagaException):
	"""
	An operation failed because none of the available session contexts could 
	successfully be used for authentication.
	"""
	pass

class Timeout(SagaException):
	"""
	This exception indicates that a remote operation did not complete 
	successfully because the network communication or the remote service timed 
	out.
	"""
	pass

class NoSuccess(SagaException):
	"""
	This exception indicates that an operation failed semantically, e.g. the 
	operation was not successfully performed. This exception is the least 
	specific exception defined in SAGA, and CAN be used for all error conditions 
	which do not indicate a more specific exception specified above.
	"""
	pass

class sagaIO(SagaException, IOError):
	"""
	This exception indicates that something went wrong in a IO related method,
	and raises this exception instead of returning POSIX error code. 
	"""

#TODO: make this a better subtype
