include_directories(${FAUST_SOURCE_DIR}/)

## TODO: needs to be replaced by saga detection!
include_directories(/work/oweidner/megajobs/include/)
link_directories(/work/oweidner/megajobs/lib/)

## Required boost libraries
set (Boost_COMPONENTS_NEEDED program_options)
find_package(Boost 1.38.0 COMPONENTS ${Boost_COMPONENTS_NEEDED})

#include_directories(${Boost_INCLUDE_DIR})
#include_directories(${Boost_LIBRARY_DIR})

## Create agent executable
set (LIBFAUST_FACADE_SOURCE_FILES 
    ${FAUST_SOURCE_DIR}/faust/faust/description.cpp
    ${FAUST_SOURCE_DIR}/faust/faust/job.cpp
    ${FAUST_SOURCE_DIR}/faust/faust/job_group.cpp
    ${FAUST_SOURCE_DIR}/faust/faust/object.cpp
    ${FAUST_SOURCE_DIR}/faust/faust/resource.cpp
    ${FAUST_SOURCE_DIR}/faust/faust/resource_description.cpp
    ${FAUST_SOURCE_DIR}/faust/faust/resource_monitor.cpp
    ${FAUST_SOURCE_DIR}/faust/faust/service.cpp
    ${FAUST_SOURCE_DIR}/faust/faust/version.cpp
)

set (LIBFAUST_IMPL_SOURCE_FILES 
    ${FAUST_SOURCE_DIR}/faust/impl/description_impl.cpp
    ${FAUST_SOURCE_DIR}/faust/impl/job_impl.cpp
    ${FAUST_SOURCE_DIR}/faust/impl/job_group_impl.cpp
    ${FAUST_SOURCE_DIR}/faust/impl/object_impl.cpp
    ${FAUST_SOURCE_DIR}/faust/impl/resource_impl.cpp
    ${FAUST_SOURCE_DIR}/faust/impl/resource_description_impl.cpp
    ${FAUST_SOURCE_DIR}/faust/impl/resource_monitor_impl.cpp
    ${FAUST_SOURCE_DIR}/faust/impl/service_impl.cpp
)


add_library(faust ${LIBFAUST_FACADE_SOURCE_FILES} ${LIBFAUST_IMPL_SOURCE_FILES})
#target_link_libraries(faust_agent ${Boost_LIBRARIES})
