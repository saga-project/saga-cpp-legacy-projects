#include <vector>
#include <string>
#include <ctime>

#ifndef FAUST_AGENT_TASK
#define FAUST_AGENT_TASK

namespace faust { namespace agent {
  
  /*! \brief Possible states for an %Agent %Task.
   *
   */ 
  enum TaskState {
    TaskNew       = 0x3001, /**< %Task has not been executed yet. */ 
    TaskExecuting = 0x3002, /**< %Task is currently executing.    */ 
    TaskDone      = 0x3003, /**< %Task has finished execution.    */ 
    TaskFailed    = 0x3004  /**< %Task execution has failed.      */ 
  };
  
  
  /*! \brief Returns the %Task state as a string.
   *
   */
  static std::string taskStateToStr(TaskState state) {
    std::string ss("");
    
    switch (state) {
      case TaskNew:
        ss =  "TaskNew";
        break;
      case TaskExecuting:
        ss =  "TaskExecuting";
        break;
      case TaskDone:
        ss =  "TaskDone";
        break;
      case TaskFailed:
        ss =  "TaskFailed";
        break;
    }
    return ss;
  };
  
  
  /*! \brief A %Task represents an execution unit that can be executed
   *         by an %Agent. More specific special-purpose %Task classes 
   *         can be derived from this class. 
   *
   */ 
  class Task{
    
  private:
    
    std::string UUID;
    time_t executionTime;
    
  protected:
    
    std::string command;
    std::vector<std::string> args;
    std::string result;
    
    faust::agent::TaskState state;
    
  public:
    
    /*! \brief Creates a new %Task that executes a command. 
     *
     */
    Task(std::string command);
    
    /*! \brief Creates a new %Task that executes a command with a list of 
     *         arguments.
     *
     */
    Task(std::string command, std::vector<std::string> args);
    
    /*! \brief Copy constructor deep-copies this %Tasks data and sets is state
     *         to 'TaskNew' and its executionTime to 0.
     */
    //Task(const Task& task);
    
    /*! \brief Sets the command to be executed by this %Task. This method can
     *         only be called for %Tasks in 'TaskNew' state. 
     *
     */
    void setCommand(std::string command);

    /*! \brief Returns the command associated with this %Task. 
     *
     */
    std::string getCommand();
    
    
    /*! \brief Sets the arguments to be used by this %Task. This method can
     *         only be called for %Tasks in 'TaskNew' state. 
     *
     */
    void setArguments(std::vector<std::string> args);
    
    /*! \brief Returns the arguments associated with this %Task.
     *
     */
    std::vector<std::string> getArguments();
    
    /*! \brief Returns the time this %Task has been executed by the %Agent or
     *         0 if it hasn't been executed yet.
     *
     */
    time_t getExecutionTime();

    /*! \brief Returns the result (if any) of this %Task.
     *
     */
    std::string getResult();

    /*! \brief Returns the unique identifier (UUID) for this %Task.
     *
     */
    std::string getUUID();
    
    /*! \brief Returns the current state of this %Task.
     *
     */
    faust::agent::TaskState getState();
    
  };
  
}}

#endif
