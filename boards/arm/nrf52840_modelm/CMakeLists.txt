target_sources(app PRIVATE leds.c)
target_sources_ifdef(CONFIG_CUSTOM_OUTPUT_STATUS app PRIVATE status.c)
