package saga.monitoring
{
  // callbacks are used for asynchronous notification of
  // metric changes (events)
  interface callback
  {
    cb               (in monitorable       mt,
                      in metric            metric,
                      in context           ctx,
                      out bool             keep);
  }
  // a metric represents an entity / value to be monitored.
  class metric : implements    saga::object
                 implements   saga::attributes
              // from object saga::error_handler
  {
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
                                                              115
saga-core-wg@ogf.org
GFD-R-P.90            SAGA Monitoring Model         January 15, 2008
     //    mode:  depending on the mode attribute above
     //    type:  String
     //    value: -
     //    notes: see description of value formating below
   }
   // SAGA objects which provide metrics and can thus be
   // monitored implement the monitorable interface
   interface monitorable
   {
     // introspection
     list_metrics        (out array<string>   names);
     get_metric          (in string           name,
                          out metric          metric);
     // callback handling
     add_callback        (in  string          name,
                          in  callback        cb,
                          out int             cookie);
     remove_callback     (in  int             cookie);
   }
   // SAGA objects which can be steered by changing their
   // metrics implement the steerable interface
   interface steerable : implements monitorable
   {
     // metric handling
     add_metric          (in metric           metric,
                          out bool            success);
     remove_metric       (in string           name);
     fire_metric         (in string           name);
   }
 }
