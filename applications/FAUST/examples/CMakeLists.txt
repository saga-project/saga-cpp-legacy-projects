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
set (EX01_SOURCE_FILES 
	${FAUST_SOURCE_DIR}/examples/ex01.cpp
)

add_executable(faust_ex01 ${EX01_SOURCE_FILES})
target_link_libraries(faust_ex01 ${Boost_LIBRARIES})
target_link_libraries(faust_ex01 faust)
