# Package: saga
# Module: monitoring
# Description: The module which specifies the classes concerning Callbacks, Metrics, Monitorable and Steerable in saga
# Specification and documentation can be found in section 3.9, page 114-139 of the GFD-R-P.90 document
# Author: P.F.A. van Zoolingen, Computer Systems Section, Faculty of Exact Science (FEW), Vrije Universiteit, Amsterdam, The Netherlands.

from error import NotImplemented
from attributes import Attributes
from object import Object, ObjectType

class Callback(object):
    """
    Callbacks are used for asynchronous notification of metric changes 
    (events) 
    """
    
    def cb(self, monitorable, metric, context):
        """
        Asynchronous handler for metric changes.
        @summary: Asynchronous handler for metric changes.
        @param monitorable: the saga monitorable object which causes the 
            callback invocation
        @type monitorable: L{Monitorable<saga.monitoring.Monitorable>}
        @param metric: the metric causing the callback invocation
        @type metric: L{Metric<saga.monitoring.Metric>}
        @param context: the context associated with the entity causing the 
            callback
        @type context: L{Context<saga.context.Context>}
        @return: indicator if callback stays registered
        @rtype: bool
        @PreCondition: the passed context is authenticated.
        @PostCondition: if cb returns True, the callback stays registered, and 
            will be invoked again on the next metric update.
        @PostCondition: if cb returns False, the callback gets unregistered, and 
            will not be invoked again on metric updates, unless it gets re-added 
            by the user.
        @Raise NotImplemented:
        @raise AuthorizationFailed:
        @Note: "metric" is the Metric the callback is invoked on, that means 
            that this metric recently changed. Note that this change is 
            semantically defined by the metric, e.g. the string of the "value" 
            attribute of the metric might have the same value in two subsequent
            invocations of the callback.
        @note: "monitorable" is the Monitorable object the metric "metric" 
            belonges to.
        @note: "context" is the Context which allows the callback to authorize 
            the metric change. If the cb method decides not to authorize this 
            particular invocation, it raises an "AuthorizationFailed" exception.
        @note: if no context is available, a context of type "Unknown" is 
            passed, with no attributes attached. Note that this can also 
            indicate that a non-authenticated party connected.
        @note: a callback can be added to a metric multiple times. A "False" 
            return value (no keep) will remove only one registration, and keep 
            the others.
        @note: a callback can be added to multiple metrics at the same time. A 
            "False" return (no keep) will only remove the registration on the 
            metric the callback was invoked on.
        @note: the application must ensure appropriate locking of callback 
            instances which are used with multiple metrics.
        @note: a callback added to exactly one metric exactly once is guaranteed 
            to be active at most once at any given time. That implies that the 
            SAGA implementation queues pending requests until a callback 
            invocation is finished.

        """
  
class Metric(Object, Attributes):
    """
    A metric represents an entity or value to be monitored.
    
        - Attributes:
            - B{Name}
                -   name: Name
                -   desc: name of the metric
                -   mode: ReadOnly
                -   type: String
                -   value: -
                -   notes: naming conventions as described below apply
            - B{Description}
                -   name: Description
                -   desc: description of the metric
                -   mode: ReadOnly
                -   type: String
            - B{Mode}
                -   name: Mode
                -   desc: access mode of the metric
                -   mode: ReadOnly
                -   type: String
                -   value: 'ReadOnly', 'ReadWrite' or 'Final'
            - B{Unit}
                -   name: Unit
                -   desc: unit of the metric
                -   mode: ReadOnly
                -   type: String
            - B{Type}
                -   name: Type
                -   desc: value type of the metric
                -   mode: ReadOnly
                -   type: String
                -   value: 'String', 'Int', 'Float', 'Bool', 'Time' or 'Trigger'
            -B{Value}
                -   name: Value
                -   desc: value of the metric

    
    """

    def __init__(self, name, desc, mode, unit, mtype, value):
        """
        Initializes the Metric object.
        @summary: Initializes the Metric object.
        @param name: name of the metric
        @type name: string
        @param desc: description of the metric
        @type desc: string
        @param mode: mode of the metric
        @type mode: string
        @param unit: unit of the metric value
        @type unit: string
        @param mtype: type of the metric
        @type mtype: string
        @param value: initial value of the metric
        @type value: string
        @PostCondition: callbacks can be registered on the metric.
        @Raise NotImplemented:
        @raise BadParameter:
        @raise Timeout:
        @raise NoSuccess:
        @note: a metric is not attached to a session, but can be used in different sessions.
        @note: the string arguments given are used to initialize the attributes of the metric.
        @note: the constructor ensures that metrics are always initialized completely. All changes to
         attributes later will always result in an equally valid metric.
        @note: incorrectly formatted "value" parameter, invalid "mode" and "type" parameter, and empty
         required parameter (all but "unit") will cause a BadParameter exception.
        @note: a "Timeout" or "NoSuccess" exception indicates that the backend could not create that specific metric.
        """
      
    def add_callback(self, cb): 
        """
        Add asynchronous notifier callback to watch metric changes.
        @summary: Add asynchronous notifier callback to watch metric changes.
        @param cb: Callback to be added
        @type cb: L{Callback<saga.monitoring.Callback>}
        @return: handle for this callback, called a cookie, to be used for removal
        @rtype: int
        @PreCondition: the metric is not "Final".
        @PostCondition: the callback is invoked on metric changes.
        @Permission: Read
        @Raise NotImplemented:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: "IncorrectState" is raised if the metric is "Final".
        @note: the "callback" method on cb will be invoked on any change of the metric (not only when its value changes)
        @note: if the "callback" method returns true, the callback is kept registered; if it returns
                 false, the callback is called, and is un-registered after completion. If the
                 callback raises an exception, it stays registered.
        @note: the returned cookie uniquely identifies the callback, and can be used to remove it.
        @note: A "Timeout" or "NoSuccess" exception is raised if the implementation cannot invoke the
                 callback on metric changes.
        @note: a backend MAY limit the ability to add callbacks - the method may hence cause an
                 "AuthenticationFailed", "AuthorizationFailed" or "PermissionDenied" exception to be raised.

        """
      
    def remove_callback(self, cookie):
        """
        Remove a callback from a metric.
        @summary: Remove a callback from a metric.
        @param cookie: handle identifying the callback to be removed
        @type cookie: int
        @PreCondition: the callback identified by "cookie" is registered for that metric.
        @PostCondition: the callback identified by "cookie" is not active, nor invoked ever again.
        @Permission: Read
        @Raise NotImplemented:
        @Raise BadParameter:
        @Raise PermissionDenied:
        @Raise AuthorizationFailed:
        @Raise AuthenticationFailed:
        @Raise Timeout:
        @Raise NoSuccess:
        @Note: if a callback is active at the time of removal, the call blocks until
               that callback returns. The callback is not activated anew during or after that block.
        @note: if the callback was removed earlier, or was unregistered by returning false, this call does nothing.
        @note: the removal only affects the cb identified by "cookie", even if the same callback was
               registered multiple times.
        @note: if the cookie was not created by adding a callback to this object instance, a BadParameter is raised.
        @note: a "Timeout" or "NoSuccess" exception is raised if the backend cannot guarantee that the callback gets successfully removed.
        @note: note that the backend allows the removal of the callback, if it did allow its addition -
               hence, no authentication, autorization or permission faults are to be expected.

        """
              
    def fire(self):
        """
        Push a new metric value to the backend.
        @summary: Push a new metric value to the backend.
        @PreCondition: the metric is not "Final"
        @PreCondition: the metric is "ReadWrite"
        @PostCondition: callbacks registered on the metric are invoked.
        @Permission: Write
        @Raise NotImplemented:
        @Raise IncorrectState:
        @Raise PermissionDenied:
        @Raise AuthorizationFailed:
        @Raise AuthenticationFailed:
        @Raise Timeout:
        @Raise NoSuccess:
        @Note: "IncorrectState" is raised if the metric is "Final".
        @note: "PermissionDenied" is raised if the metric is not "ReadWrite". That also holds for a once
                 writable metric which was flagged "Final". To catch race conditions on this exceptions,
                 the application should try/raise the fire().
        @note: it is not necessary to change the value of a metric in order to fire it.
        @note: "set_attribute ("value", "...") on a metric does NOT imply a fire. Hence the value can be
              changed multiple times, but unless fire() is explicitly called, no consumer will notice.
        @note: if the application invoking fire() has callbacks registered on the metric, these callbacks are invoked.
        @note: "AuthenticationFailed", "AuthorizationFailed" or "PermissionDenied" may get raised if the
              current session is not allowed to fire this metric.
        @note: a "Timeout" or "NoSuccess" exception signals that the implementation could not communicate
              the new metric state to the backend.

        """
        
class Monitorable(object):
    """SAGA objects which provide metrics and can thus be monitored extend the Monitorable class"""

    def list_metrics(self):
        """
        List all metrics associated with the object.
        @summary: List all metrics associated with the object.
        @return: names identifying the metrics associated with the object instance
        @rtype: tuple
        @Permission: Query
        @Raise NotImplemented:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: several SAGA objects are required to expose certain metrics (e.g. "task.state"). However,
                 in general that assumption cannot be made, as implementations might be unable to provide
                 metrics. In particular, listed metrics might actually be unavailable.
        @note: no order is implied on the returned tuple
        @note: the returned tuple is guaranteed to have no double entries (names are unique)
        @note: an "AuthenticationFailed", "AuthorizationFailed" or "PermissionDenied"
                 exception indicates that the current session is not allowed to list the available metrics.
        @note: a "Timeout" or "NoSuccess" exception indicates that the backend was not able to list the available metrics.
        """
     
    def get_metric(self, name):
        """
        Returns a metric instance, identified by name.
        @summary: Returns a metric instance, identified by name.
        @param name: name of the metric to be returned
        @type name: string
        @return: metric instance identified by name
        @rtype: L{Metric<Metric>}
        @Permission: Query
        @Raise NotImplemented:
        @raise DoesNotExist:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: multiple calls of this method with the same value for name return multiple identical
              instances (copies) of the metric.
        @note: a "DoesNotExist" exception indicates that the backend does not know the metric with the given name.
        @note: an "AuthenticationFailed", "AuthorizationFailed" or "PermissionDenied"
              exception indicates that the current session is not allowed to obtain the named metric.
        @note: a "Timeout" or "NoSuccess" exception indicates that the backend was not able to return the named metric.

        """
     
    def add_callback(self, name, cb):
        """
        Add a callback to the specified metric.
        @summary: Add a callback to the specified metric.
        @param name: identifies the metric to which cb is to be added
        @type name: string
        @param cb: the Callback object to be registered
        @return: handle for callback removal called a cookie
        @rtype: int
        @PostCondition: the callback is registered on the metric.
        @Permission: Read on the metric.
        @Raise NotImplemented:
        @raise DoesNotExist:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: notes to the add_callback method of the metric class apply.

        """
     
    def remove_callback(self, name, cookie):
        """
        Remove a callback from the specified metric
        @summary: Remove a callback from the specified metric.
        @param name: identifies the metric for which cb is to be removed
        @type name: string
        @param cookie: identifies the callback to be removed
        @type cookie: int
        @PreCondition: the callback was registered on the metric.
        @Permission: read on the metric.
        @Raise NotImplemented:
        @raise BadParameter:
        @raise DoesNotExist:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: notes to the remove_callback method of the metric class apply
        """

   
class Steerable(Monitorable):
    """SAGA objects which can be steered by changing their metrics implement the steerable interface"""

    def add_metric(self, metric):
        """
        Add a metric instance to the application instance.
        @summary: Add a metric instance to the application instance.
        @param metric: metric to be added
        @type metric: L{Metric}
        @return: indicator if the metric was added successfully 
        @rtype: bool
        @PostCondition: the metric can be accessed from this application, and possibly from other applications.
        @Permission: Write
        @Raise NotImplemented:
        @Raise AlreadyExists:
        @Raise IncorrectState:
        @Raise PermissionDenied:
        @Raise AuthorizationFailed:
        @Raise AuthenticationFailed:
        @Raise Timeout:
        @Raise NoSuccess:
        @Note: a metric is uniquely identified by its name attribute - no two metrics with the same name can be added.
        @Note: any callbacks already registered on the metric stay registered (the state of metric is not changed)
        @Note: an object being steerable does not guarantee that a metric can in fact be added -- the
                 returned bool indicates if that particular metric could be added.
        @Note: an "AuthenticationFailed", "AuthorizationFailed" or "PermissionDenied"
                 exception indicates that the current session is not allowed to add metrics to the steerable.
        @Note: a "Timeout" or "NoSuccess" exception indicates that the backend was not able to add the  metric.
        @Note: if a metric with the same name is already known for the object, an "AlreadyExists" exception is raised.
        @Note: if the steerable instance does not support the addition of new metrics, i.e. if only the
                 default metrics can be steered, an "IncorrectState" exception is raised.
        """

    def remove_metric(self, name):
        """
        Remove a metric instance.
        @summary: Remove a metric instance.
        @param name: identifies the metric to be removed
        @type name: string
        @PostCondition: all callbacks registered on that metric are unregistered.
        @PostCondition: the metric is not available anymore.
        @Permission: Write
        @Raise NotImplemented:
        @Raise DoesNotExist:
        @Raise IncorrectState:
        @Raise PermissionDenied:
        @Raise AuthorizationFailed:
        @Raise AuthenticationFailed:
        @Raise Timeout:
        @Raise NoSuccess:
        @Note: only previously added metrics can be removed; default metrics (saga defined or implementation
                 specific) cannot be removed; attempts to do so raise a BadParameter exception.
        @Note: an "AuthenticationFailed", "AuthorizationFailed" or "PermissionDenied"
                 exception indicates that the current session is not allowed to remove the metrics from the steerable.
        @Note: a "Timeout" or "NoSuccess" exception indicates that the backend was not able to remove the metric.
        @Note: if a metric with that name is not known for the object, a "DoesNotExist" exception is raised.
        @Note: if a steerable instance does not support the removal of some metric, e.g. if a metric
                 needs to be always present, an "IncorrectState" exception is raised.
                 For example, the "state" metric on a steerable job cannot be removed.

        """

    def fire_metric(self, name):
        """
        Push a new metric value to the backend.
        @summary: Push a new metric value to the backend.
        @param name: identifies the metric to be fired
        @type name: string
        @Permission: Write
        @Raise NotImplemented:
        @Raise DoesNotExist:
        @Raise IncorrectState:
        @Raise PermissionDenied:
        @Raise AuthorizationFailed:
        @Raise AuthenticationFailed:
        @Raise Timeout:
        @Raise NoSuccess:
        @Note: notes to the fire method of the metric class apply
        @Note: fire can be called for metrics which have been added with add_metric(), and for predefined metrics
        @Note: an "AuthenticationFailed", "AuthorizationFailed" or "PermissionDenied" exception indicates that the current session
                 is not allowed to fire the metric.
        @Note: a "Timeout" or "NoSuccess" exception indicates that the backend was not able to fire the metric.
        @Note: if a metric with that name is not known for the object, a "DoesNotExist" exception is raised.
        @Note: an attempt to fire a metric which is "ReadOnly" results in an "IncorrectState" exception.
        @Note: an attempt to fire a "Final" metric results in an "IncorrectState" exception.

        """
