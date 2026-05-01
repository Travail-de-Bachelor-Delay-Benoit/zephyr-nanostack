if(CONFIG_NANOSTACK)

  zephyr_library_named(zephyr_library_nanostack)
  
  zephyr_library_link_libraries(mbedTLS)

  set(PORT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/zephyr_port)

  zephyr_library_include_directories(
    src/sal-stack-nanostack/nanostack
    ${PORT_DIR}/include
    )

  zephyr_library_sources(
    ${PORT_DIR}/source/zephyr_port.c
  )

  target_compile_options(zephyr_library_nanostack PRIVATE
    -include ${PORT_DIR}/include/glue.h
    -fcommon 
    -Wno-error=inline
  )

  add_library(mbed-nanostack-sal_stack INTERFACE)
  add_library(mbed-nanostack INTERFACE)
  add_library(mbed-coap INTERFACE)
  add_library(mbed-core INTERFACE)
  add_library(mbed-nanostack-libservice INTERFACE)
  add_library(mbed-nanostack-coap_service INTERFACE)
  add_library(mbed-nanostack-sal_stack-event_loop INTERFACE)

  zephyr_include_directories(
    include
    src/nanostack-libservice/mbed-client-libservice
    src/sal-stack-nanostack-eventloop/nanostack-event-loop
    src/sal-stack-nanostack/nanostack
    src/mbed-trace/include/mbed-trace
    src/nanostack-libservice
    src/coap-service/coap-service
    src/sal-stack-nanostack-eventloop/nanostack-event-loop
  )

  add_subdirectory(src/sal-stack-nanostack)
  add_subdirectory(src/mbed-coap)
  add_subdirectory(src/mbed-trace)
  add_subdirectory(src/nanostack-libservice)
  add_subdirectory(src/coap-service)
  add_subdirectory(src/sal-stack-nanostack-eventloop)

  zephyr_library_sources(
    $<TARGET_PROPERTY:mbed-coap,INTERFACE_SOURCES>
    $<TARGET_PROPERTY:mbed-nanostack-sal_stack,INTERFACE_SOURCES> # On veut les SOURCES ici
    $<TARGET_PROPERTY:mbed-core,INTERFACE_SOURCES>
    $<TARGET_PROPERTY:mbed-nanostack-libservice,INTERFACE_SOURCES>
    $<TARGET_PROPERTY:mbed-nanostack-coap_service,INTERFACE_SOURCES>
    $<TARGET_PROPERTY:mbed-nanostack-sal_stack-event_loop,INTERFACE_SOURCES>
    src/sample.c
)
  zephyr_library_include_directories(
    $<TARGET_PROPERTY:mbed-coap,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:mbed-nanostack-sal_stack,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:mbed-core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:mbed-nanostack-libservice,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:mbed-nanostack-coap_service,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:mbed-nanostack-sal_stack-event_loop,INTERFACE_INCLUDE_DIRECTORIES>
    ${ZEPHYR_BASE}/../modules/crypto/mbedtls-3.6/include
    ${ZEPHYR_BASE}/../modules/mbedtls/include
    ${ZEPHYR_BASE}/modules/mbedtls/include
    )

  zephyr_library_compile_definitions(
    MBED_CONF_NANOSTACK_LIBSERVICE_PRESENT=1
    MBED_CONF_MBED_TRACE_FEA_IPV6=1
    )

endif()