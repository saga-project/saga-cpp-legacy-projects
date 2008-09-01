#Page 114 of the GFD-R-P.90 document, Chapter 3.9, package saga.monitoring

from error import NotImplemented
from attributes import Attributes
from object import Object, ObjectType

class Callback(object):
    """Callbacks are used for asynchronous notification of metric changes (events) """
    
    def cb(self, mt, metric, ctx):
        #in monitorable mt, in metric metric, in context ctx, out bool keep
        """
        Asynchronous handler for metric changes
        mt: the saga monitorable object which causes the callback invocation
        metric: the metric causing the callback invocation
        ctx: the context associated with the callback causing entity
        keep: indicates if callback stays registered
        Pre Condition: the passed context is authenticated.
        Post Condition: - if keep is returned as True, the callback stays registered, and will be invoked again on the next metric update.
                        - if keep is returned as False, the callback gets unregistered, and will not be invoked again on metric updates, unless it gets re-added by the user.
        Raises: NotImplemented
                AuthorizationFailed
        Notes: - "metric" is the metric the callback is invoked on - that means that this metric
               recently changed. Note that this change is semantically defined by the metric, e.g. the
               string of the "value" attribute of the metric might have the same value in two subsequent
               invocations of the callback.
             - "mt" is the monitorable object the metric "metric" belonges to.
             - the context "ctx" is the context which allows the callback to authorize the metric change.
               If the cb method decides not to authorize this particular invocation, it MUST throw an "AuthorizationFailed" exception.
             - if no context is available, a context of type "Unknown" is passed, with no attributes
               attached. Note that this can also indicate that a non-authenticated party connected.
             - a callback can be added to a metric multiple times. A "false" return value (no keep) will
               remove only one registration, and keep the others.
             - a callback can be added to multiple metrics at the same time. A false return (no keep) will
               only remove the registration on the metric the callback was invoked on.
             - the application must ensure appropriate locking of callback instances which are used with multiple metrics.
             - a callback added to exactly one metric exactly once is guaranteed to be active at most once at
               any given time. That implies that the SAGA implementation MUST queue pending requests
               until a callback invocation is finished.

        """
        raise NotImplemented, "cb() is not implemented in this object"
  
class Metric(Object, Attributes):
    """A metric represents an entity / value to be monitored."""

    def __init__(self, name, desc, mode, unit, type, value):
    #in string name, in string desc, in string mode, in string unit, in string type, in string value, out metric          obj);
        """
        Create the object
        name: name of the metric
        desc: description of the metric
        mode: mode of the metric
        unit: unit of the metric value
        type: type of the metric
        value: initial value of the metric
        Post Condition: callbacks can be registered on the metric.
        Raises:  NotImplemented
                 BadParameter
                 Timeout
                 NoSuccess
Notes: - a metric is not attached to a session, but can be used in different sessions.
       - the string arguments given are used to initialize the attributes of the metric.
       - the constructor ensures that metrics are always initialized completely. All changes to
         attributes later will always result in an equally valid metric.
       - incorrectly formatted "value" parameter, invalid "mode" and "type" parameter, and empty
         required parameter (all but "unit") will cause a BadParameter exception.
       - a "Timeout" or "NoSuccess" exception indicates that the backend could not create that specific metric.
        """
        super(Metric, self).__init__()
    
      
    #callback handling
    def add_callback(self, cb): 
        #in callback cb, out int cookie);
        """
        Add asynchronous notifier callback to watch metric changes
        cb: Callback class instance
        cookie:  handle for this callback, to be used for removal
        Pre Condition: the metric is not "Final".
        Post Condition: the callback is invoked on metric changes.
        Permission: Read
        Raises: NotImplemented
                IncorrectState
                PermissionDenied
                AuthorizationFailed
                AuthenticationFailed
                Timeout
                NoSuccess
        Notes: - "IncorrectState" is thrown if the metric is "Final".
               - the "callback" method on cb will be invoked on any change of the metric (not only when its value changes)
               - if the "callback" method returns true, the callback is kept registered; if it returns
                 false, the callback is called, and is un-registered after completion. If the
                 callback throws an exception, it stays registered.
               - the cb is passed by reference.
               - the returned cookie uniquely identifies the callback, and can be used to remove it.
               - A "Timeout" or "NoSuccess" exception is thrown if the implementation cannot invoke the
                 callback on metric changes.
               - a backend MAY limit the ability to add callbacks - the method may hence cause an
                 "AuthenticationFailed", "AuthorizationFailed" or "PermissionDenied" exception to be thrown.

        """
        cookie = 0
        return cookie
     
      
      
    def remove_callback(self, cookie):
        #in int cookie
        """
        Remove a callback from a metric
        cookie: handle identifying the callback to be removed
        Pre Condition: the callback identified by "cookie" is registered for that metric.
        Post Condition: the callback identified by "cookie" is not active, nor invoked ever again.
        Permission: Read
        Raises: NotImplemented
                BadParameter
                PermissionDenied
                AuthorizationFailed
                AuthenticationFailed
                Timeout
                NoSuccess
     Notes:  - if a callback is active at the time of removal, the call MUST block until
               that callback returns. The callback is not activated anew during or after that block.
             - if the callback was removed earlier, or was unregistered by returning false, this call does nothing.
             - the removal only affects the cb identified by "cookie", even if the same callback was
               registered multiple times.
             - if the cookie was not created by adding a callback to this object instance, a BadParameter is thrown.
             - a "Timeout" or "NoSuccess" exception is thrown if the backend cannot guarantee that the callback gets successfully removed.
             - note that the backend MUST allow the removal of the callback, if it did allow its addition -
               hence, no authentication, autorization or permission faults are to be expected.

        """
              
              
    #actively signal an event
    def fire(self):
        """
        Push a new metric value to the backend
        Pre Condition: - the metric is not "Final".
                       - the metric is "ReadWrite"
        Post Condition: callbacks registered on the metric are invoked.
        Permission: Write
        Raises: NotImplemented
                IncorrectState
                PermissionDenied
                AuthorizationFailed
                AuthenticationFailed
                Timeout
                NoSuccess
        Notes: - "IncorrectState" is thrown if the metric is "Final".
               - "PermissionDenied" is thrown if the metric is not "ReadWrite" -- That also holds for a once
                 writable metric which was flagged "Final". To catch race conditions on this exceptions,
                 the application should try/except the fire().
            - it is not necessary to change the value of a metric in order to fire it.
            - "set_attribute ("value", "...") on a metric does NOT imply a fire. Hence the value can be
              changed multiple times, but unless fire() is explicitly called, no consumer will notice.
            - if the application invoking fire() has callbacks registered on the metric, these callbacks are invoked.
            - "AuthenticationFailed", "AuthorizationFailed" or "PermissionDenied" may get thrown if the
              current session is not allowed to fire this metric.
            - a "Timeout" or "NoSuccess" exception signals that the implementation could not communicate
              the new metric state to the backend.

        """
        
class Monitorable(object):
    """SAGA objects which provide metrics and can thus be monitored implement the monitorable interface"""

    def list_metrics(self):
        #return array<string> names
        """
        List all metrics associated with the object
        names: tuple of names identifying the metrics associated with the object instance
        Permission: Query
        Raises: NotImplemented
                PermissionDenied
                AuthorizationFailed
                AuthenticationFailed
                Timeout
                NoSuccess
        Notes: - several SAGA objects are required to expose certain metrics (e.g. "task.state"). However,
                 in general that assumption cannot be made, as implementations might be unable to provide
                 metrics. In particular, listed metrics might actually be unavailable.
               - no order is implied on the returned tuple
               - the returned array is guaranteed to have no double entries (names are unique)
               - an "AuthenticationFailed", "AuthorizationFailed" or "PermissionDenied"
                 exception indicates that the current session is not allowed to list the available metrics.
               - a "Timeout" or "NoSuccess" exception indicates that the backend was not able to list the available metrics.
        """
        raise NotImplemented, "list_metrics() is not implemented in this object"
     
    def get_metric(self, name):
         #in string name, return metric metric
        """
        Returns a metric instance, identified by name
        name: name of the metric to be returned
        metric: metric instance identified by name
        Permission: Query
        Raises: NotImplemented
                DoesNotExist
                PermissionDenied
                AuthorizationFailed
                AuthenticationFailed
                Timeout
                NoSuccess
        Notes: - multiple calls of this method with the same value for name return multiple identical
              instances (copies) of the metric.
            - a "DoesNotExist" exception indicates that the backend does not know the metric with the given name.
            - an "AuthenticationFailed", "AuthorizationFailed" or "PermissionDenied"
              exception indicates that the current session is not allowed to obtain the named metric.
            - a "Timeout" or "NoSuccess" exception indicates that the backend was not able to return the named metric.

        """
        raise NotImplemented, "get_metric() is not implemented in this object"
     
    def add_callback(self, name, cb):
        #in string name, in callback cb, out int cookie
        """
        Add a callback to the specified metric
        name: identifies the metric to which cb is to be added
        cb: reference to Callback class instance to be registered
        cookie: handle for callback removal
        Post Condition: the callback is registered on the metric.
        Permission: Read on the metric.
        Raises: NotImplemented
                DoesNotExist
                IncorrectState
                PermissionDenied
                AuthorizationFailed
                AuthenticationFailed
                Timeout
                NoSuccess
        Note: - notes to the add_callback method of the metric class apply.

        """
        raise NotImplemented, "add_callback() is not implemented in this object"
     
    def remove_callback(self, name):
        #in string name, in int cookie
        """
        Remove a callback from the specified metric
        name: identifies the metric for which cb is to be removed
        cookie: identifies the cb to be removed
        Pre Condition: the callback was registered on the metric.
        Permission: read on the metric.
        Raises: NotImplemented
                BadParameter
                DoesNotExist
                PermissionDenied
                AuthorizationFailed
                AuthenticationFailed
                Timeout
                NoSuccess
        Note: notes to the remove_callback method of the metric class apply
        """
        raise NotImplemented, "remove_callback() is not implemented in this object"
   
class Steerable(Monitorable):
    """SAGA objects which can be steered by changing their metrics implement the steerable interface"""

    def add_metric(self, metric):
        #in metric metric, out bool success
        """
        Add a metric instance to the application instance
        metric: metric to be added
        success: indicates success
        Post Condition: the metric can be accessed from this application, and possibly from other applications.
        Permission: Write
        Raises: NotImplemented
                AlreadyExists
                IncorrectState
                PermissionDenied
                AuthorizationFailed
                AuthenticationFailed
                Timeout
                NoSuccess
        Notes: - a metric is uniquely identified by its name attribute - no two metrics with the same name can be added.
               - any callbacks already registered on the metric stay registered (the state of metric is not changed)
               - an object being steerable does not guarantee that a metric can in fact be added -- the
                 returned boolean indicates if that particular metric could be added.
               - an "AuthenticationFailed", "AuthorizationFailed" or "PermissionDenied"
                 exception indicates that the current session is not allowed to add metrics to the steerable.
               - a "Timeout" or "NoSuccess" exception indicates that the backend was not able to add the  metric.
               - if a metric with the same name is already known for the object, an "AlreadyExists" exception is thrown.
               - if the steerable instance does not support the addition of new metrics, i.e. if only the
                 default metrics can be steered, an "IncorrectState" exception is thrown.
        """
        raise NotImplemented, "add_metric() is not implemented in this object"

    def remove_metric(self, name):
        #in string name
        """
        Remove a metric instance
        name: identifies the metric to be removed
        Post Condition: - all callbacks registered on that metric are unregistered.
                        - the metric is not available anymore.
        Permission:    Write
        Raises: NotImplemented
                DoesNotExist
                IncorrectState
                PermissionDenied
                AuthorizationFailed
                AuthenticationFailed
                Timeout
                NoSuccess
     Notes:    - only previously added metrics can be removed; default metrics (saga defined or implementation
                 specific) cannot be removed; attempts to do so raise a BadParameter exception.
               - an "AuthenticationFailed", "AuthorizationFailed" or "PermissionDenied"
                 exception indicates that the current session is not allowed to remove the metrics from the steerable.
               - a "Timeout" or "NoSuccess" exception indicates that the backend was not able to remove the metric.
               - if a metric with that name is not known for the object, a "DoesNotExist" exception is thrown.
               - if a steerable instance does not support the removal of some metric, e.g. if a metric
                 needs to be always present, an "IncorrectState" exception is thrown.
                 For example, the "state" metric on a steerable job cannot be removed.

        """
        raise NotImplemented, "remove_metric() is not implemented in this object"

    def fire_metric(self, name):
        #in string name
        """
        Push a new metric value to the backend
        name: identifies the metric to be fired
        Permission: Write
        Raises: NotImplemented
                DoesNotExist
                IncorrectState
                PermissionDenied
                AuthorizationFailed
                AuthenticationFailed
                Timeout
                NoSuccess
     Notes:    - notes to the fire method of the metric class apply
               - fire can be called for metrics which have been added with add_metric(), and for predefined metrics
               - an "AuthenticationFailed", "AuthorizationFailed" or "PermissionDenied" exception indicates that the current session
                 is not allowed to fire the metric.
               - a "Timeout" or "NoSuccess" exception indicates that the backend was not able to fire the metric.
               - if a metric with that name is not known for the object, a "DoesNotExist" exception is thrown.
               - an attempt to fire a metric which is "ReadOnly" results in an "IncorrectState" exception.
               - an attempt to fire a "Final" metric results in an "IncorrectState" exception.

        """
        raise NotImplemented, "fire_metric() is not implemented in this object"
