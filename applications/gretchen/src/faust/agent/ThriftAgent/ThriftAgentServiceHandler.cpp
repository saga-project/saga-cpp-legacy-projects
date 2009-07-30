class ThriftAgentServiceHandler : virtual public ThriftAgentServiceIf {
public:
  ThriftAgentServiceHandler() {
    // Your initialization goes here
  }
  
  bool ping() {
    // Your implementation goes here
    printf("ping\n");
    return true;
  }
  
};
