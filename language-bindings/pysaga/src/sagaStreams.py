# Page 281
  package saga.stream
  {
    enum state
    {
      New          =  1
      Open         =  2,
      Closed       =  3,
      Dropped      =  4,
      Error        =  5
    }
    enum activity
    {
      Read         =  1,
      Write        =  2,
      Exception    =  4
    }
    class stream_service : implements saga::object
                           implements saga::async
                           implements saga::monitorable
                           implements saga::permissions
                                                        282
saga-core-wg@ogf.org
GFD-R-P.90                  SAGA Streams                January 15, 2008
                         // from object     saga::error_handler
    {
      CONSTRUCTOR        (in      session          s,
                          in      saga::url        url,
                          out     stream_service   obj);
      DESTRUCTOR         (in      stream_service   obj);
      get_url            (out     saga::url        url);
      serve              (in      float            timeout = -1.0,
                          out     stream           stream);
      close              (in      float            timeout = 0.0);
      // Metrics:
      //   name: stream_server.client_connect
      //   desc: fires if a client connects
      //   mode: ReadOnly
      //   unit: 1
      //   type: Trigger
      //   value: 1
    }
    class stream : extends         saga::object
                    implements     saga::async
                    implements     saga::attributes
                    implements     saga::monitorable
                 // from object saga::error_handler
    {
      // constructor / destructor
      CONSTRUCTOR (in       session            s,
                     in    saga::url           url = "",
                     out   stream              obj);
      DESTRUCTOR    (in     stream             obj);
      // inspection methods
      get_url       (out    saga::url          url);
      get_context (out      context            ctx);
      // management methods
      connect       (void);
      wait          (in     int                what,
                     in    float               timeout = -1.0,
                     out   int                 cause);
      close         (in     float              timeout = 0.0);
                                                                     283
saga-core-wg@ogf.org
GFD-R-P.90                 SAGA Streams             January 15, 2008
      // I/O methods
      read         (inout  buffer          buf,
                    in    int              len_in = -1,
                    out   int              len_out);
      write        (in     buffer          buf,
                    in    int              len_in = -1,
                    out   int              len_out);
      // Attributes:
      //   name: Bufsize
      //   desc: determines the size of the send buffer,
      //          in bytes
      //   mode: ReadWrite, optional
      //   type: Int
      //   value: system dependend
      //   notes: - the implementation MUST document the
      //            default value, and its meaning (e.g. on what
      //            layer that buffer is maintained, or if it
      //            disables zero copy).
      //
      //   name: Timeout
      //   desc: determines the amount of idle time
      //          before dropping the line, in seconds
      //   mode: ReadWrite, optional
      //   type: Int
      //   value: system dependend
      //   notes: - the implementation MUST document the
      //            default value
      //          - if this attribute is supported, the
      //            connection MUST be closed by the
      //            implementation if for that many seconds
      //            nothing has been read from or written to
      //            the stream.
      //
      //   name: Blocking
      //   desc: determines if read/writes are blocking
      //          or not
      //   mode: ReadWrite, optional
      //   type: Bool
      //   value: True
      //   notes: - if the attribute is not supported, the
      //            implementation MUST be blocking
      //          - if the attribute is set to ’True’, a read or
      //            write operation MAY return immediately if
      //            no data can be read or written - that does
                                                                 284
saga-core-wg@ogf.org
GFD-R-P.90                 SAGA Streams             January 15, 2008
      //            not constitute an error (see EAGAIN in
      //            POSIX).
      //
      //   name:  Compression
      //   desc:  determines if data are compressed
      //          before/after transfer
      //   mode: ReadWrite, optional
      //   type: Bool
      //   value: schema dependent
      //   notes: - the implementation MUST document the
      //            default values for the available schemas
      //
      //   name:  Nodelay
      //   desc:  determines if packets are sent
      //          immediately, i.e. without delay
      //   mode: ReadWrite, optional
      //   type: Bool
      //   value: True
      //   notes: - similar to the TCP_NODELAY option
      //
      //   name:  Reliable
      //   desc:  determines if all sent data MUST arrive
      //   mode:  ReadWrite, optional
      //   type:  Bool
      //   value: True
      //   notes: - if the attribute is not supported, the
      //            implementation MUST be reliable
      // Metrics:
      //   name: stream.state
      //   desc: fires if the state of the stream changes,
      //          and has the value of the new state
      //          enum
      //   mode: ReadOnly
      //   unit: 1
      //   type: Enum
      //   value: New
      //
      //   name: stream.read
      //   desc: fires if a stream gets readable
      //   mode: ReadOnly
      //   unit: 1
      //   type: Trigger
      //   value: 1
      //   notes: - a stream is considered readable if a
                                                                 285
saga-core-wg@ogf.org
GFD-R-P.90                 SAGA Streams             January 15, 2008
     //             subsequent read() can sucessfully read
     //             1 or more bytes of data.
     //
     //    name:  stream.write
     //    desc:  fires if a stream gets writable
     //    mode:  ReadOnly
     //    unit:  1
     //    type:  Trigger
     //    value: 1
     //    notes: - a stream is considered writable if a
     //             subsequent write() can sucessfully write
     //             1 or more bytes of data.
     //
     //    name:  stream.exception
     //    desc:  fires if a stream has an error condition
     //    mode:  ReadOnly
     //    unit:  1
     //    type:  Trigger
     //    value: 1
     //    notes: -
     //
     //    name:  stream.dropped
     //    desc:  fires if the stream gets dropped by the
     //           remote party
     //    mode: ReadOnly
     //    unit: 1
     //    type: Trigger
     //    value: 1
   }
 }

