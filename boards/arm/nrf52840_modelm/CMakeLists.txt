target_sources_ifdef(CONFIG_LED_LOCK_STATUS app PRIVATE src/leds.c)
target_sources_ifdef(CONFIG_PWMLED_LOCK_STATUS app PRIVATE src/leds_pwm.c)
target_sources_ifdef(CONFIG_LED_INDICATOR_STATUS app PRIVATE src/status.c)
target_sources(app PRIVATE src/anti_ghost.c)
