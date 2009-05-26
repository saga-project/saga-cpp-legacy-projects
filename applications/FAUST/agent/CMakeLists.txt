include_directories(${FAUST_SOURCE_DIR}/)

## TODO: needs to be replaced by saga detection!
include_directories(${SAGA_ROOT}/include/ ${SAGA_ROOT}/include/saga/)
link_directories(${SAGA_ROOT}/lib/)

## Required boost libraries
set (Boost_COMPONENTS_NEEDED program_options)
find_package(Boost 1.38.0 COMPONENTS ${Boost_COMPONENTS_NEEDED})

include_directories(${Boost_INCLUDE_DIR})
include_directories(${Boost_LIBRARY_DIR})

## Create agent executable
set (AGENT_SOURCE_FILES 
	${FAUST_SOURCE_DIR}/agent/main.cpp
	${FAUST_SOURCE_DIR}/agent/agent.cpp
	${FAUST_SOURCE_DIR}/agent/monitor/monitor.cpp
	${FAUST_SOURCE_DIR}/agent/monitor/monitor_group.cpp
)

set (FAUST_SOURCE_FILES
	${FAUST_SOURCE_DIR}/faust/faust/version.cpp
	${FAUST_SOURCE_DIR}/faust/faust/object.cpp
	${FAUST_SOURCE_DIR}/faust/faust/resource_description.cpp
	${FAUST_SOURCE_DIR}/faust/faust/resource_monitor.cpp
	
	${FAUST_SOURCE_DIR}/faust/impl/object_impl.cpp
	${FAUST_SOURCE_DIR}/faust/impl/resource_description_impl.cpp
	${FAUST_SOURCE_DIR}/faust/impl/resource_monitor_impl.cpp
	${FAUST_SOURCE_DIR}/faust/impl/logwriter.cpp 
)

add_executable(faust_agent ${AGENT_SOURCE_FILES} ${FAUST_SOURCE_FILES})
target_link_libraries(faust_agent ${Boost_LIBRARIES})
target_link_libraries(faust_agent saga_engine saga_package_namespace saga_package_advert)
