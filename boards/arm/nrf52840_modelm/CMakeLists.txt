target_sources_ifdef(CONFIG_LED_LOCK_STATUS app PRIVATE leds.c)
target_sources_ifdef(CONFIG_LED_INDICATOR_STATUS app PRIVATE status.c)
