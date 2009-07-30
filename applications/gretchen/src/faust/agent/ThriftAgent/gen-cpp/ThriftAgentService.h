/**
 * Autogenerated by Thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 */
#ifndef ThriftAgentService_H
#define ThriftAgentService_H

#include <TProcessor.h>
#include "Interface_types.h"



class ThriftAgentServiceIf {
 public:
  virtual ~ThriftAgentServiceIf() {}
  virtual bool ping() = 0;
};

class ThriftAgentServiceNull : virtual public ThriftAgentServiceIf {
 public:
  virtual ~ThriftAgentServiceNull() {}
  bool ping() {
    bool _return = false;
    return _return;
  }
};

class ThriftAgentService_ping_args {
 public:

  ThriftAgentService_ping_args() {
  }

  virtual ~ThriftAgentService_ping_args() throw() {}


  bool operator == (const ThriftAgentService_ping_args & /* rhs */) const
  {
    return true;
  }
  bool operator != (const ThriftAgentService_ping_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const ThriftAgentService_ping_args & ) const;

  uint32_t read(apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(apache::thrift::protocol::TProtocol* oprot) const;

};

class ThriftAgentService_ping_pargs {
 public:


  virtual ~ThriftAgentService_ping_pargs() throw() {}


  uint32_t write(apache::thrift::protocol::TProtocol* oprot) const;

};

class ThriftAgentService_ping_result {
 public:

  ThriftAgentService_ping_result() : success(0) {
  }

  virtual ~ThriftAgentService_ping_result() throw() {}

  bool success;

  struct __isset {
    __isset() : success(false) {}
    bool success;
  } __isset;

  bool operator == (const ThriftAgentService_ping_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const ThriftAgentService_ping_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const ThriftAgentService_ping_result & ) const;

  uint32_t read(apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(apache::thrift::protocol::TProtocol* oprot) const;

};

class ThriftAgentService_ping_presult {
 public:


  virtual ~ThriftAgentService_ping_presult() throw() {}

  bool* success;

  struct __isset {
    __isset() : success(false) {}
    bool success;
  } __isset;

  uint32_t read(apache::thrift::protocol::TProtocol* iprot);

};

class ThriftAgentServiceClient : virtual public ThriftAgentServiceIf {
 public:
  ThriftAgentServiceClient(boost::shared_ptr<apache::thrift::protocol::TProtocol> prot) :
    piprot_(prot),
    poprot_(prot) {
    iprot_ = prot.get();
    oprot_ = prot.get();
  }
  ThriftAgentServiceClient(boost::shared_ptr<apache::thrift::protocol::TProtocol> iprot, boost::shared_ptr<apache::thrift::protocol::TProtocol> oprot) :
    piprot_(iprot),
    poprot_(oprot) {
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }
  boost::shared_ptr<apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  boost::shared_ptr<apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  bool ping();
  void send_ping();
  bool recv_ping();
 protected:
  boost::shared_ptr<apache::thrift::protocol::TProtocol> piprot_;
  boost::shared_ptr<apache::thrift::protocol::TProtocol> poprot_;
  apache::thrift::protocol::TProtocol* iprot_;
  apache::thrift::protocol::TProtocol* oprot_;
};

class ThriftAgentServiceProcessor : virtual public apache::thrift::TProcessor {
 protected:
  boost::shared_ptr<ThriftAgentServiceIf> iface_;
  virtual bool process_fn(apache::thrift::protocol::TProtocol* iprot, apache::thrift::protocol::TProtocol* oprot, std::string& fname, int32_t seqid);
 private:
  std::map<std::string, void (ThriftAgentServiceProcessor::*)(int32_t, apache::thrift::protocol::TProtocol*, apache::thrift::protocol::TProtocol*)> processMap_;
  void process_ping(int32_t seqid, apache::thrift::protocol::TProtocol* iprot, apache::thrift::protocol::TProtocol* oprot);
 public:
  ThriftAgentServiceProcessor(boost::shared_ptr<ThriftAgentServiceIf> iface) :
    iface_(iface) {
    processMap_["ping"] = &ThriftAgentServiceProcessor::process_ping;
  }

  virtual bool process(boost::shared_ptr<apache::thrift::protocol::TProtocol> piprot, boost::shared_ptr<apache::thrift::protocol::TProtocol> poprot);
  virtual ~ThriftAgentServiceProcessor() {}
};

class ThriftAgentServiceMultiface : virtual public ThriftAgentServiceIf {
 public:
  ThriftAgentServiceMultiface(std::vector<boost::shared_ptr<ThriftAgentServiceIf> >& ifaces) : ifaces_(ifaces) {
  }
  virtual ~ThriftAgentServiceMultiface() {}
 protected:
  std::vector<boost::shared_ptr<ThriftAgentServiceIf> > ifaces_;
  ThriftAgentServiceMultiface() {}
  void add(boost::shared_ptr<ThriftAgentServiceIf> iface) {
    ifaces_.push_back(iface);
  }
 public:
  bool ping() {
    uint32_t sz = ifaces_.size();
    for (uint32_t i = 0; i < sz; ++i) {
      if (i == sz - 1) {
        return ifaces_[i]->ping();
      } else {
        ifaces_[i]->ping();
      }
    }
  }

};



#endif
