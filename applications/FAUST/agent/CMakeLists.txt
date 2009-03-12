include_directories(${faust_SOURCE_DIR}/)

## TODO: needs to be replaced by saga detection!
include_directories(/usr/local/saga-1.1/include/)
link_directories(/usr/local/saga-1.1/lib/)

## Required boost libraries
set (Boost_COMPONENTS_NEEDED program_options)
find_package(Boost 1.38.0 COMPONENTS ${Boost_COMPONENTS_NEEDED})

include_directories(${Boost_INCLUDE_DIR})
include_directories(${Boost_LIBRARY_DIR})

## Create agent executable
set (AGENT_SOURCE_FILES ${faust_SOURCE_DIR}/agent/main.cpp
                        ${faust_SOURCE_DIR}/agent/agent.cpp
                        ${faust_SOURCE_DIR}/faust/impl/logwriter.cpp )

add_executable(faust_agent ${AGENT_SOURCE_FILES})
target_link_libraries(faust_agent ${Boost_LIBRARIES})
