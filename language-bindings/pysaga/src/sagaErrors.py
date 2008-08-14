#Error Handling, specified from page 35 to 46 of the GFD.90 document

class SagaException(Exception):
	message = ""
	sagaObject = None
	"""
	This is the exception base class inherited by all exceptions thrown by a SAGA object implementation.
	"""

	def __init__(self, message, sagaObject=None):
		"""
		Init an Exception
		
		message: the exception message
		sagaObject: a sagaObject associated with this exception.	
		"""
		super(SagaException,self).__init__()
		self.sagaObject = sageObject
		self.message = message

	def getMessage():
		"""Returns the message associated with the exception """
		return self.message

	def getObject():
		"""Returns the sagaObject associated with the exception """
		return self.sagaObject

class NotImplemented(SagaException):
	pass

class IncorrectURL(SagaException):
	pass

class BadParameter(SagaException):
	pass

class AlreadyExists(SagaException):
	pass

class DoesNotExist(SagaException):
	pass

class IncorrectState(SagaException):
	pass

class PermissionDenied(SagaException):
	pass

class AuthorizationFailed(SagaException):
	pass

class AuthenticationFailed(SagaException):
	pass

class Timeout(SagaException):
	pass

class NoSuccess(SagaException):
	pass

