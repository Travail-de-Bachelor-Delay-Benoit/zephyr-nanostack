if(CONFIG_NANOSTACK)

  zephyr_library()

  zephyr_library_sources(src/sample.c)

  zephyr_include_directories(include)

endif()