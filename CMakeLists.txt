if(CONFIG_APP_EVENT_MANAGER)
    zephyr_interface_library_named(APPEVENTMANAGERLIB)
    
    zephyr_include_directories(include)
    zephyr_library_include_directories(src)
    zephyr_library()
    zephyr_library_sources(
        src/app_event_manager.c
    )
    
    zephyr_library_sources_ifdef(CONFIG_SHELL src/app_event_manager_shell.c)
    zephyr_linker_sources(SECTIONS src/aem.ld)
    zephyr_library_link_libraries(APPEVENTMANAGERLIB)
    target_link_libraries(APPEVENTMANAGERLIB INTERFACE zephyr_interface)
endif()