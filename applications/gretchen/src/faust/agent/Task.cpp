#include <saga/saga.hpp>
#include <faust/agent/Task.hpp>

using namespace faust::agent;

Task::Task(std::string command)
: command(command), state(TaskNew)
{
  UUID = saga::uuid().string();
  executionTime = 0;
}


Task::Task(std::string command, std::vector<std::string> args)
: command(command), args(args), state(TaskNew)
{
  UUID = saga::uuid().string();
  executionTime = 0;
  
}


/*Task::Task(const Task& task)
{
  UUID = saga::uuid().string();
  executionTime = 0;
  
  command = task.command;
  args = task.args;
  
  result.clear();
  state = TaskNew;
}*/


void Task::setCommand(std::string command)
{
  this->command = command;
}


std::string Task::getCommand()
{
  return command;
}


void Task::setArguments(std::vector<std::string> args)
{
  this->args = args;
}


std::vector<std::string> Task::getArguments()
{
  return args;
}


time_t Task::getExecutionTime()
{
  return executionTime;
}


std::string Task::getResult()
{
  return result;
}

std::string Task::getUUID()
{
  return UUID;
}

faust::agent::TaskState Task::getState()
{
  return state;
}
