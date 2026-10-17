#include QMK_KEYBOARD_H
#include <stdlib.h>

enum layers{
  MAC_BASE,
  MAC_FN,
  WIN_BASE,
  MACRO,
  WIN_FN,
};

#define KC_TASK LGUI(KC_TAB)
#define KC_FLXP LGUI(KC_E)

enum custom_keycodes {
    SNIPERMACRO = SAFE_RANGE,
    SNIPERCOOP,
    LMOUSECLICK,
    RMOUSECLICK,
    GOBACK,
};

// Returns a random int in [min, max], inclusive
int randy(int min, int max) {
    return min + rand() % (max - min + 1);
}

void keyboard_post_init_user(void) {
    srand(timer_read32());
    rgb_matrix_mode(RGB_MATRIX_CUSTOM_default_off);
}

// ---------------------------------------------------------------------------
// Non-blocking click macros
// Each macro presses and releases its button across separate matrix scans,
// so the rest of the keyboard keeps working while a macro runs.
// ---------------------------------------------------------------------------
enum { SP_SNIPER, SP_COOP, SP_LCLICK, SP_RCLICK, SP_COUNT };

typedef struct {
    bool     enabled;
    bool     pressed;
    uint16_t keycode;
    uint16_t hold_min, hold_max;   // how long the button stays down (ms)
    uint16_t gap_min,  gap_max;    // pause between clicks (ms)
    uint32_t timer;
    uint32_t wait;
} spam_t;

static spam_t spam[SP_COUNT] = {
    [SP_SNIPER] = { .keycode = MS_BTN1, .hold_min = 210, .hold_max = 260, .gap_min = 30, .gap_max = 60  },
    [SP_COOP]   = { .keycode = MS_BTN1, .hold_min = 210, .hold_max = 260, .gap_min = 60, .gap_max = 120 },
    
    [SP_LCLICK] = { .keycode = MS_BTN1, .hold_min = 20,  .hold_max = 50,  .gap_min = 80, .gap_max = 100 },
    [SP_RCLICK] = { .keycode = MS_BTN2, .hold_min = 20,  .hold_max = 50,  .gap_min = 80, .gap_max = 100 },
};

static void spam_set(uint8_t i, bool on) {
    spam_t *s = &spam[i];
    if (s->pressed) {
        unregister_code(s->keycode);   // never leave a button stuck down
        s->pressed = false;
    }
    s->enabled = on;
    s->timer   = timer_read32();
    s->wait    = randy(s->gap_min, s->gap_max);
}

static void spam_all_off(void) {
    for (uint8_t i = 0; i < SP_COUNT; i++) {
        spam_set(i, false);
    }
}

// MACRO
void matrix_scan_user(void) {
    for (uint8_t i = 0; i < SP_COUNT; i++) {
        spam_t *s = &spam[i];
        if (!s->enabled) continue;
        if (timer_elapsed32(s->timer) < s->wait) continue;

        s->timer = timer_read32();
        if (s->pressed) {
            unregister_code(s->keycode);
            s->pressed = false;
            s->wait    = randy(s->gap_min, s->gap_max);
        } else {
            register_code(s->keycode);
            s->pressed = true;
            s->wait    = randy(s->hold_min, s->hold_max);
        }
    }
}

// Change LED
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    uint8_t layer = get_highest_layer(layer_state | default_layer_state);

    uint8_t r = 0, g = 0, b = 0;
    bool    colorize = false;

    switch (layer) {
        case MACRO:
            r = 0xDC; g = 0x14; b = 0x3C;  // RGB Crimson
            colorize = true;
            break;
        case WIN_FN:
        case MAC_FN:
            r = 0x15; g = 0x80; b = 0x15;  // RGB Green
            colorize = true;
            break;
        default:
            break;
    }

    if (colorize) {
        for (uint8_t row = 0; row < MATRIX_ROWS; ++row) {
            for (uint8_t col = 0; col < MATRIX_COLS; ++col) {
                uint8_t index = g_led_config.matrix_co[row][col];
                if (index >= led_min && index < led_max && index != NO_LED &&
                    keymap_key_to_keycode(layer, (keypos_t){col, row}) > KC_TRNS) {
                    rgb_matrix_set_color(index, r, g, b);
                }
            }
        }
    }

    if (spam[SP_LCLICK].enabled) {
        rgb_matrix_set_color(32, RGB_BLUE);
    }
    if (spam[SP_RCLICK].enabled) {
        rgb_matrix_set_color(53, RGB_BLUE);
    }
    return true;   // keep keyboard-level indicators (Caps Lock, etc.)
}


// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [MAC_BASE] = LAYOUT_104_ansi(
        KC_ESC,             KC_BRID,  KC_BRIU,  _______,  _______,  RM_VALD,  RM_VALU,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,    KC_VOLU,  KC_NO,    KC_NO,    RM_NEXT,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,     KC_BSPC,  KC_INS,   KC_HOME,  KC_PGUP,  KC_NUM,   KC_PSLS,  KC_PAST,  KC_PMNS,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,    KC_BSLS,  KC_DEL,   KC_END,   KC_PGDN,  KC_P7,    KC_P8,    KC_P9,    KC_PPLS,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,              KC_ENT,                                 KC_P4,    KC_P5,    KC_P6,
        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,              KC_RSFT,            KC_UP,              KC_P1,    KC_P2,    KC_P3,    KC_PENT,
        KC_LCTL,  KC_LOPT,  KC_LCMD,                                KC_SPC,                                 KC_RCMD,  KC_ROPT,  MO(MAC_FN), KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT,  KC_P0,              KC_PDOT         ),

    [MAC_FN] = LAYOUT_104_ansi(
        _______,            KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,     KC_F12,   _______,  _______,  RM_NEXT,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        RM_TOGG,  RM_NEXT,  RM_VALU,  RM_HUEU,  RM_SATU,  RM_SPDU,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  RM_PREV,  RM_VALD,  RM_HUED,  RM_SATD,  RM_SPDD,  _______,  _______,  _______,  _______,  _______,  _______,              _______,                                _______,  _______,  _______,
        _______,            _______,  _______,  _______,  _______,  _______,  NK_TOGG,  _______,  _______,  _______,  _______,              _______,            _______,            _______,  _______,  _______,  _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,            _______         ),

    [WIN_BASE] = LAYOUT_104_ansi(
        KC_ESC,             KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,     KC_F12,   KC_PSCR,  KC_NO,    TG(MACRO),
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,     KC_BSPC,  KC_INS,   KC_HOME,  KC_PGUP,  KC_NUM,   KC_PSLS,  KC_PAST,  KC_PMNS,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,    KC_BSLS,  KC_DEL,   KC_END,   KC_PGDN,  KC_P7,    KC_P8,    KC_P9,    KC_PPLS,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,              KC_ENT,                                 KC_P4,    KC_P5,    KC_P6,
        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,              KC_RSFT,            KC_UP,              KC_P1,    KC_P2,    KC_P3,    KC_PENT,
        KC_LCTL,  KC_LWIN,  KC_LALT,                                KC_SPC,                                 KC_RALT,  KC_RWIN,  MO(WIN_FN), KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT,  KC_P0,              KC_PDOT         ),

    [MACRO] = LAYOUT_104_ansi(
        _______,            _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  GOBACK,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  LMOUSECLICK,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  SNIPERMACRO,  SNIPERCOOP,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  RMOUSECLICK,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,              _______,                                _______,  _______,  _______,
        _______,            _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,              _______,            _______,            _______,  _______,  _______,  _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,            _______         ),

    [WIN_FN] = LAYOUT_104_ansi(
        _______,            KC_BRID,  KC_BRIU,  KC_TASK,  KC_FLXP,  RM_VALD,  RM_VALU,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,    KC_VOLU,  _______,  _______,  RM_TOGG,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        RM_TOGG,  RM_NEXT,  RM_VALU,  RM_HUEU,  RM_SATU,  RM_SPDU,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  RM_PREV,  RM_VALD,  RM_HUED,  RM_SATD,  RM_SPDD,  _______,  _______,  _______,  _______,  _______,  _______,              _______,                                _______,  _______,  _______,
        _______,            _______,  _______,  _______,  _______,  _______,  NK_TOGG,  _______,  _______,  _______,  _______,              _______,            _______,            _______,  _______,  _______,  _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,            _______         ),
};

// clang-format on
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
    case SNIPERMACRO:
        spam_set(SP_SNIPER, record->event.pressed);   // hold to fire
        break;

    case SNIPERCOOP:
        spam_set(SP_COOP, record->event.pressed);     // hold to fire
        break;

    case LMOUSECLICK:
        if (record->event.pressed) {
            spam_set(SP_LCLICK, !spam[SP_LCLICK].enabled);   // toggle
        }
        break;

    case RMOUSECLICK:
        if (record->event.pressed) {
            spam_set(SP_RCLICK, !spam[SP_RCLICK].enabled);   // toggle
        }
        break;

    // DISABLE ALL MACROS AND RETURN TO BASE LAYER
    case GOBACK:
        if (record->event.pressed) {
            spam_all_off();
        } else {
            layer_off(MACRO);
        }
        break;
    }
    return true;
}
