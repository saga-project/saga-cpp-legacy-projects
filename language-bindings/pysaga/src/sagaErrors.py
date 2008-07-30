#Error Handling, specified from page 35 to 46 of the GFD.90 document

class SagaException(Exception):
	"optional documentation string"

	def __init__(self, message, sagaObject=None):
		self.sagaObject = sageObject
		self.message = message

	def __del__(self):
		Exception.__del__(self);

	def getMessage():
		return self.message

	def getObject():
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

