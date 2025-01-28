#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/led.h>
#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zmk/hid_indicators.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

// Only use LEDs with aliases in the DT
#if DT_NODE_EXISTS(DT_ALIAS(led_caps))
#define HAS_LED_CAPS_LOCK
#endif
#if DT_NODE_EXISTS(DT_ALIAS(led_num))
#define HAS_LED_NUM_LOCK
#endif
#if DT_NODE_EXISTS(DT_ALIAS(led_scroll))
#define HAS_LED_SCROLL_LOCK
#endif

#define LED_GPIO_NODE_ID DT_COMPAT_GET_ANY_STATUS_OKAY(gpio_leds)

// GPIO-based LED device
static const struct device *led_dev = DEVICE_DT_GET(LED_GPIO_NODE_ID);

static int led_capslock_listener_cb(const zmk_event_t *eh) {
    zmk_hid_indicators_t flags = zmk_hid_indicators_get_current_profile();

#define CAPS_BIT    (1 << (HID_USAGE_LED_CAPS_LOCK - 1))
#define NUM_BIT     (1 << (HID_USAGE_LED_NUM_LOCK - 1))
#define SCROLL_BIT  (1 << (HID_USAGE_LED_SCROLL_LOCK - 1)0

#ifdef HAS_LED_CAPS_LOCK
    if (flags & CAPS_BIT) {
        LOG_INF("Caps Lock is ON");
        led_on(led_dev, DT_NODE_CHILD_IDX(DT_ALIAS(led_caps)));
    } else {
        LOG_INF("Caps Lock is OFF");
        led_off(led_dev, DT_NODE_CHILD_IDX(DT_ALIAS(led_caps)));
    }
#endif

#ifdef HAS_LED_NUM_LOCK
    if (flags & NUM_BIT) {
        LOG_INF("Num Lock is ON");
        led_on(led_dev, DT_NODE_CHILD_IDX(DT_ALIAS(led_num)));
    } else {
        LOG_INF("Num Lock is OFF");
        led_off(led_dev, DT_NODE_CHILD_IDX(DT_ALIAS(led_num)));
    }
#endif

#ifdef HAS_LED_SCROLL_LOCK
    if (flags & SCROLL_BIT) {
        LOG_INF("Scroll Lock is ON");
        led_on(led_dev, DT_NODE_CHILD_IDX(DT_ALIAS(led_scroll)));
    } else {
        LOG_INF("Scroll Lock is OFF");
        led_off(led_dev, DT_NODE_CHILD_IDX(DT_ALIAS(led_scroll)));
    }
#endif

    return 0;
}

ZMK_LISTENER(led_indicators_listener, led_capslock_listener_cb);
ZMK_SUBSCRIPTION(led_indicators_listener, zmk_hid_indicators_changed);

static int leds_init(void) {
    if (!device_is_ready(led_dev)) {
        LOG_ERR("Device %s is not ready", led_dev->name);
        return -ENODEV;
    }

    return 0;
}

// Run leds_init on boot
SYS_INIT(leds_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
