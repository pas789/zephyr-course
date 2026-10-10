if (CONFIG_PAS_DRIVER)
    target_sources(app PRIVATE pas_driver.c)
    target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endif ()
