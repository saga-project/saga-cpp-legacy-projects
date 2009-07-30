#include <boost/thread/mutex.hpp>
#include <faust/agent/Task.hpp>

#ifndef FAUST_AGENT_AGENT
#define FAUST_AGENT_AGENT


namespace faust { namespace agent {
  
  /*! \brief Possible states for an %Agent instance. It represents the state
   *         of the associated %Agent executable running on a computing element.
   *
   */      
  enum AgentState {
    AgentNew        = 0x1001, /**< %Agent executable has not been started yet.*/  
    
    AgentStarting   = 0x1002, /**< %Agent executable has been started but is 
                                   not yet running (e.g. still queued).       */  
    AgentRunning    = 0x1003, /**< %Agent executable is running.              */
    
    AgentFailed     = 0x1004, /**< %Agent executable submission or execution 
                                   has failed.                                */  
    AgentTerminated = 0x1005  /**< %Agent executable was terminated by the 
                                   application.                               */  
  };
  
  /*! \brief Returns the %Agent state as a string.
   *
   */
  static std::string agentStateToStr(AgentState state) {
    std::string ss("");
    
    switch (state) {
      case AgentNew:
        ss =  "AgentNew";
        break;
      case AgentStarting:
        ss =  "AgentStarting";
        break;
      case AgentRunning:
        ss =  "AgentRunning";
        break;
      case AgentFailed:
        ss =  "AgentFailed";
        break;
      case AgentTerminated:
        ss =  "AgentTerminated";
        break;
    }
    return ss;
  };
  
  
  /*! \brief Abstract base class (Interface) for all %Agent implementations.
   *
   */   
  class Agent
    {
    protected:
      
      AgentState state;
      std::string UUID;
      
      std::vector<Task> taskQueue;
      std::string taskQueueUUID;
      
      boost::mutex m_mutex;
      
    protected:
      
      /*! \brief Thread-safe method to set the state internally.
       *
       */ 
      void set_state_ts(AgentState state);
      
      /*! \brief Thread-safe method to retrieve the state internally.
       *
       */ 
      AgentState get_state_ts();
      
    public: 
      
      /// @cond - hide from Doxygen
      virtual ~Agent() {} 
      /// @endcond 
      
      /*! \brief Starts the %agent.
       *
       */      
      virtual void start() = 0;
      
      /*! \brief Terminates the %agent.
       *
       */ 
      virtual void terminate() = 0;

      /*! \brief Adds a command to the command queue.
       *
       */       
      virtual void taskQueuePush(Task);

      /*! \brief Removes a %Task from the command queue. Only %Tasks in 
       *         'TaskNew' state can be removed. 
       *
       */   
      virtual void taskQueuePop();

      /*! \brief Lists the elements in the command queue.
       *
       */   
      virtual std::vector<Task> taskQueueList();
      
      
      /*! \brief Returns the current state of the %agent.
       *
       */ 
      AgentState getState();
      
      
      /*! \brief Returns the unique identifier (UUID) for this %Task.
       *
       */
      std::string getUUID();
      
    };
  
} }

#endif