from saga.error import SagaException
from saga.context import Context

s =  SagaException("message", Context())
print s.get_object()
print s.saga_object
print s.get_message()
print s.message