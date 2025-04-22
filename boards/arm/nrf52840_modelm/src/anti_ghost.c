#include <zephyr/logging/log.h>
#include <string.h>
#include <zmk/event_manager.h>
#include <zmk/events/position_state_changed.h>
#include <zmk/physical_layouts.h>
#include <zmk/matrix_transform.h>

LOG_MODULE_REGISTER(antighost, LOG_LEVEL_INF);

// Declare struct to access the transform fields
typedef struct {
    const uint32_t *lookup_table;
    size_t len;
    uint8_t rows;
    uint8_t columns;
    uint8_t col_offset;
    uint8_t row_offset;
} zmk_matrix_transform_real_t;

static bool pressed_phys[256];  // state of the physical switches (max 256)
static size_t phys_len;
static bool initialized = false;

// Get the current matrix_transform
static const zmk_matrix_transform_real_t *get_mt(void) {
    const struct zmk_physical_layout *const *layouts;
    size_t count = zmk_physical_layouts_get_list(&layouts);
    int sel = zmk_physical_layouts_get_selected();
    if (sel < 0 || sel >= (int)count) {
        return NULL;
    }
    return (const void *)layouts[sel]->matrix_transform;
}

// Init
static void antighost_init(void) {
    const zmk_matrix_transform_real_t *mt = get_mt();
    if (!mt) {
        LOG_ERR("Antighost init: no transform");
        return;
    }
    phys_len = mt->len;
    if (phys_len > 256) {
        phys_len = 256;
    }
    memset(pressed_phys, 0, phys_len * sizeof(bool));
    initialized = true;
    LOG_INF("Antighost initialized: %u positions", (uint32_t)phys_len);
}

// Determines whether phys_idx is the third or forth corner in the ghost rectangle
// ref: https://www.microsoft.com/applied-sciences/projects/anti-ghosting
static bool is_ghost_corner(size_t phys_idx) {
    const zmk_matrix_transform_real_t *mt = get_mt();
    if (!mt) {
        return false;
    }
    size_t cols = mt->columns;
    size_t rows = mt->rows;
    // calculate r2,c2 for phys_idx
    size_t r2 = phys_idx / cols;
    size_t c2 = phys_idx % cols;

    // examine every possible r1 != r2 e c1 != c2
    for (size_t r1 = 0; r1 < rows; r1++) {
        if (r1 == r2) continue;
        for (size_t c1 = 0; c1 < cols; c1++) {
            if (c1 == c2) continue;
            // calculate linear index
            size_t pos00 = r1 * cols + c1;
            size_t pos01 = r1 * cols + c2;
            size_t pos10 = r2 * cols + c1;
            size_t pos11 = r2 * cols + c2;
            // is phys_idx one of the four corners
            if (phys_idx != pos00 && phys_idx != pos01 &&
                phys_idx != pos10 && phys_idx != pos11) {
                continue;
            }
            // count how many of the other 3 positions are pressed
            int count = 0;
            if (phys_idx != pos00 && pressed_phys[pos00]) count++;
            if (phys_idx != pos01 && pressed_phys[pos01]) count++;
            if (phys_idx != pos10 && pressed_phys[pos10]) count++;
            if (phys_idx != pos11 && pressed_phys[pos11]) count++;
            // if at least 2 of the others are pressed, it's ghost/3rd corner
            if (count >= 2) {
                return true;
            }
        }
    }
    return false;
}

static int antighost_listener(const zmk_event_t *eh) {
    struct zmk_position_state_changed *ev = as_zmk_position_state_changed(eh);
    if (!ev) {
        return ZMK_EV_EVENT_BUBBLE;
    }
    if (!initialized) {
        antighost_init();
        if (!initialized) {
            return ZMK_EV_EVENT_BUBBLE;
        }
    }

    const zmk_matrix_transform_real_t *mt = get_mt();
    if (!mt) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    // logical position map -> physical index
    size_t phys_idx = mt->len;
    for (size_t i = 0; i < mt->len; i++) {
        uint32_t v = mt->lookup_table[i];
        if (v && (v - 1) == (int32_t)ev->position) {
            phys_idx = i;
            break;
        }
    }
    if (phys_idx >= phys_len) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    if (ev->state) {
        // block the third and forth press on the rectangle
        if (is_ghost_corner(phys_idx)) {
            LOG_WRN("Antighost: corner discard idx %u", (uint32_t)phys_idx);
            return ZMK_EV_EVENT_HANDLED;
        }
        // otherwise accept the press
        pressed_phys[phys_idx] = true;
    } else {
        // release
        pressed_phys[phys_idx] = false;
    }

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(antighost, antighost_listener);
ZMK_SUBSCRIPTION(antighost, zmk_position_state_changed);