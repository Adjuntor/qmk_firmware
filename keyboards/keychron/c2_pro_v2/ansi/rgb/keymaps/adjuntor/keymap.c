#include QMK_KEYBOARD_H
#include <stdlib.h>

enum layers{
  MAC_BASE,
  MAC_FN,
  WIN_BASE,
  MACRO,
  WIN_FN,
  NIKKE,
  MAPLE,
  TURBO_LAYER,   // keep last so it sits on top of everything
};

#define KC_TASK LGUI(KC_TAB)
#define KC_FLXP LGUI(KC_E)

// Older QMK versions call it IS_MOUSEKEY
#ifndef IS_MOUSE_KEYCODE
#    define IS_MOUSE_KEYCODE(kc) IS_MOUSEKEY(kc)
#endif

enum custom_keycodes {
    SNIPERMACRO = SAFE_RANGE,
    SNIPERCOOP,
    RUSH_THREATEN,
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

// ---------------------------------------------------------------------------
// Generic turbo layer (toggle)
// While TURBO_LAYER is on, tap any eligible key to start auto-repeating it,
// tap it again to stop. Active keys light up blue.
// Slots are tied to the physical key position, so two keys that share a
// keycode can't cancel each other.
// ---------------------------------------------------------------------------
#define TURBO_SLOTS 6

// Tweak to taste (ms)
#define TURBO_HOLD_MIN 20
#define TURBO_HOLD_MAX 50
#define TURBO_GAP_MIN  30
#define TURBO_GAP_MAX  60

typedef struct {
    bool     active;
    bool     pressed;
    keypos_t key;       // physical key that owns this slot
    uint16_t keycode;
    uint32_t timer;
    uint32_t wait;
} turbo_t;

static turbo_t turbo[TURBO_SLOTS];

// Physical keys whose press was captured by turbo, so their release gets swallowed too
static bool turbo_swallowed[MATRIX_ROWS][MATRIX_COLS];

static void turbo_stop(turbo_t *t) {
    if (t->pressed) unregister_code16(t->keycode);   // never leave a key stuck
    t->pressed = false;
    t->active  = false;
}

static void turbo_all_off(void) {
    for (uint8_t i = 0; i < TURBO_SLOTS; i++) turbo_stop(&turbo[i]);
}

static turbo_t *turbo_find(keypos_t key) {
    for (uint8_t i = 0; i < TURBO_SLOTS; i++) {
        if (turbo[i].active && turbo[i].key.row == key.row && turbo[i].key.col == key.col) {
            return &turbo[i];
        }
    }
    return NULL;
}

static bool turbo_eligible(uint16_t kc) {
    if (kc <= KC_TRNS) return false;
    if (kc == KC_LGUI || kc == KC_RGUI) return false;     // leave the GUI/Win/Cmd keys alone
    if (kc == KC_CAPS || kc == KC_NUM) return false;      // lock keys make no sense here
    return kc <= QK_MODS_MAX || IS_MOUSE_KEYCODE(kc);     // basic (+ modded) keys, Shift/Ctrl/Alt, mouse buttons
}

// MACRO + TURBO
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

    for (uint8_t i = 0; i < TURBO_SLOTS; i++) {
        turbo_t *t = &turbo[i];
        if (!t->active) continue;
        if (timer_elapsed32(t->timer) < t->wait) continue;

        t->timer = timer_read32();
        if (t->pressed) {
            unregister_code16(t->keycode);
            t->pressed = false;
            t->wait    = randy(TURBO_GAP_MIN, TURBO_GAP_MAX);
        } else {
            register_code16(t->keycode);
            t->pressed = true;
            t->wait    = randy(TURBO_HOLD_MIN, TURBO_HOLD_MAX);
        }
    }
}

// Layers that must not stay active while turbo is on
#define TURBO_EXCLUDED_LAYERS ((1UL << MACRO) | (1UL << NIKKE) | (1UL << MAPLE))

layer_state_t layer_state_set_user(layer_state_t state) {
    if (layer_state_cmp(state, TURBO_LAYER)) {
        state &= ~TURBO_EXCLUDED_LAYERS;   // turbo is exclusive
    } else {
        turbo_all_off();                   // leaving turbo releases every slot
    }
    return state;
}

// Change LED
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    uint8_t layer = get_highest_layer(layer_state | default_layer_state);

    uint8_t r = 0, g = 0, b = 0;
    bool    colorize = false;
    bool    fill_all = false;   // light transparent keys too

    switch (layer) {
        case TURBO_LAYER:
            r = 0xDF; g = 0xFF; b = 0x00;  // RGB Chartreuse
            colorize = true;
            fill_all = true;
            break;
        case MACRO:
            r = 0x9D; g = 0x00; b = 0xFF;  // RGB Purple
            colorize = true;
            break;
        case NIKKE:
            r = 0xDC; g = 0x14; b = 0x3C;  // RGB Crimson
            colorize = true;
            break;
        case MAPLE:
            r = 0xFF; g = 0xBF; b = 0x00;  // RGB Amber
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
                    (fill_all || keymap_key_to_keycode(layer, (keypos_t){col, row}) > KC_TRNS)) {
                    rgb_matrix_set_color(index, r, g, b);
                }
            }
        }
    }

    // Keys currently being turbo'd light up blue
    for (uint8_t i = 0; i < TURBO_SLOTS; i++) {
        if (!turbo[i].active) continue;
        uint8_t index = g_led_config.matrix_co[turbo[i].key.row][turbo[i].key.col];
        if (index != NO_LED && index >= led_min && index < led_max) {
            rgb_matrix_set_color(index, RGB_BLUE);
        }
    }

    if (spam[SP_LCLICK].enabled) {
        rgb_matrix_set_color(13, RGB_BLUE);
    }
    if (spam[SP_RCLICK].enabled) {
        rgb_matrix_set_color(14, RGB_BLUE);
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
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  TG(TURBO_LAYER),  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,              _______,                                _______,  _______,  _______,
        _______,            _______,  _______,  _______,  _______,  _______,  TG(NIKKE),  TG(MAPLE),  _______,  _______,  _______,              _______,            _______,            _______,  _______,  _______,  _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,            _______         ),

    [WIN_FN] = LAYOUT_104_ansi(
        _______,            KC_BRID,  KC_BRIU,  KC_TASK,  KC_FLXP,  RM_VALD,  RM_VALU,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,    KC_VOLU,  _______,  _______,  RM_TOGG,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        RM_TOGG,  RM_NEXT,  RM_VALU,  RM_HUEU,  RM_SATU,  RM_SPDU,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  RM_PREV,  RM_VALD,  RM_HUED,  RM_SATD,  RM_SPDD,  _______,  _______,  _______,  _______,  _______,  _______,              _______,                                _______,  _______,  _______,
        _______,            _______,  _______,  _______,  _______,  _______,  NK_TOGG,  _______,  _______,  _______,  _______,              _______,            _______,            _______,  _______,  _______,  _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,            _______         ),

    [NIKKE] = LAYOUT_104_ansi(
        _______,            _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  GOBACK,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  SNIPERMACRO,  SNIPERCOOP,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,              _______,                                _______,  _______,  _______,
        _______,            _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,              _______,            _______,            _______,  _______,  _______,  _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,            _______         ),

    [MAPLE] = LAYOUT_104_ansi(
        _______,            _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  GOBACK,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        RUSH_THREATEN,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,              _______,                                _______,  _______,  _______,
        _______,            _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,              _______,            _______,            _______,  _______,  _______,  _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,            _______         ),

    // Everything transparent: keys fall through to the base layer and get turbo'd.
    // LMOUSECLICK/RMOUSECLICK/GOBACK stay available here.
    [TURBO_LAYER] = LAYOUT_104_ansi(
        _______,            _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  LMOUSECLICK,  RMOUSECLICK,  GOBACK,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,              _______,                                _______,  _______,  _______,
        _______,            _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,              _______,            _______,            _______,  _______,  _______,  _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,            _______         ),
};

// clang-format on
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    keypos_t key   = record->event.key;
    bool     valid = key.row < MATRIX_ROWS && key.col < MATRIX_COLS;   // ignore combo/virtual events

    // The press was captured by turbo, so swallow the matching release
    if (valid && !record->event.pressed && turbo_swallowed[key.row][key.col]) {
        turbo_swallowed[key.row][key.col] = false;
        return false;
    }

    // Tap an eligible key on the turbo layer to toggle turbo on it
    if (valid && record->event.pressed && layer_state_is(TURBO_LAYER) && turbo_eligible(keycode)) {
        turbo_t *existing = turbo_find(key);
        if (existing) {                                  // already turbo'ing: stop it
            turbo_stop(existing);
            turbo_swallowed[key.row][key.col] = true;
            return false;
        }
        for (uint8_t i = 0; i < TURBO_SLOTS; i++) {
            if (!turbo[i].active) {
                turbo[i] = (turbo_t){
                    .active  = true,
                    .pressed = false,
                    .key     = key,
                    .keycode = keycode,
                    .timer   = timer_read32(),
                    .wait    = 0,                        // fire immediately
                };
                turbo_swallowed[key.row][key.col] = true;
                return false;
            }
        }
        // All slots busy: fall through and let the key act normally
    }

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
    
    case RUSH_THREATEN:
        if (record->event.pressed) {
            tap_code(KC_C);
            wait_ms(650);
            tap_code(KC_DEL);
        }
        break;

    // DISABLE ALL MACROS (INCLUDING TURBO) AND RETURN TO BASE LAYER
    case GOBACK:
        if (record->event.pressed) {
            spam_all_off();
            turbo_all_off();
        } else {
            layer_off(MACRO);
            layer_off(NIKKE);
            layer_off(MAPLE);
            layer_off(TURBO_LAYER);
        }
        break;
    }
    return true;
}
