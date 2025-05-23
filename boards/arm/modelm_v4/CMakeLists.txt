target_sources_ifdef(CONFIG_LED_LOCK_STATUS app PRIVATE ../../common/src/leds.c)
target_sources_ifdef(CONFIG_PWMLED_LOCK_STATUS app PRIVATE ../../common/src/leds_pwm.c)
target_sources_ifdef(CONFIG_LED_INDICATOR_STATUS app PRIVATE ../../common/src/status.c)
target_sources(app PRIVATE ../../common/src/anti_ghost.c)
