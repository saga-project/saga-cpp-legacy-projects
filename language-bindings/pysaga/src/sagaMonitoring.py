#Page 114 of the GFD-R-P.90 document, Chapter 3.9, package saga.monitoring

from sagaErrors import NotImplemented
from sagaAttributes import Attributes

class Callback(object):
    """Callbacks are used for asynchronous notification of metric changes (events) """
    
    def cb(self, mt, metric, ctx):
        #in monitorable mt, in metric metric, in context ctx, out bool keep
        raise NotImplemented, "cb() is not implemented in this object"
  
class Metric(Object, Attributes):
    """A metric represents an entity / value to be monitored."""

    CONSTRUCTOR        (in string            name,
                        in string            desc,
                        in string            mode,
                        in string            unit,
                        in string            type,
                          in string           value,
                          out metric          obj);
      DESTRUCTOR         (in metric           obj);
      // callback handling
      add_callback       (in callback         cb,
                          out int             cookie);
      remove_callback    (in int              cookie);
      // actively signal an event
      fire               (void);
      // Attributes:
      //   name: Name
      //   desc: name of the metric
      //   mode: ReadOnly
      //   type: String
      //   value: -
      //   notes: naming conventions as described below apply
      //
      //   name: Description
      //   desc: description of the metric
      //   mode: ReadOnly
      //   type: String
      //
      //   name: Mode
      //   desc: access mode of the metric
      //   mode: ReadOnly
      //   type: String
      //   value: ’ReadOnly’, ’ReadWrite’ or ’Final’
      //
      //   name: Unit
      //   desc: unit of the metric
      //   mode: ReadOnly
      //   type: String
      //
      //   name: Type
      //   desc: value type of the metric
      //   mode: ReadOnly
      //   type: String
      //   value: ’String’, ’Int’, ’Enum’, ’Float’, ’Bool’,
      //          ’Time’ or ’Trigger’
      //
      //   name: Value
      //   desc: value of the metric
     //    mode:  depending on the mode attribute above
     //    type:  String
     //    value: -
     //    notes: see description of value formating below
   }

class Monitorable(object):
    """SAGA objects which provide metrics and can thus be monitored implement the monitorable interface"""

    def list_metrics(self):
        #return array<string> names
        raise NotImplemented, "list_metrics() is not implemented in this object"
     
     def get_metric(self, name):
         #in string name, return metric metric
         raise NotImplemented, "get_metric() is not implemented in this object"
     
     def add_callback(self, name, cb):
         #in string name, in callback cb, out int cookie
         raise NotImplemented, "add_callback() is not implemented in this object"
     
     def remove_callback(self, cookie)
         #in int cookie
         raise NotImplemented, "remove_callback() is not implemented in this object"
   
class Steerable(Monitorable):
    """SAGA objects which can be steered by changing their metrics implement the steerable interface"""

    def add_metric(self, metric):
        #in metric metric, out bool success
        raise NotImplemented, "add_metric() is not implemented in this object"

    def remove_metric(self, name):
        #in string name
        raise NotImplemented, "remove_metric() is not implemented in this object"

    def fire_metric(self, name):
        #in string name
        raise NotImplemented, "fire_metric() is not implemented in this object"
