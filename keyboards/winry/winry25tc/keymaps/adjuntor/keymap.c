/* Copyright 2021 Andrzej Ressel (andrzej.ressel@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H

enum my_layers {
    _FIRST_LAYER
};

enum custom_keycodes {
    CONFIRM = SAFE_RANGE
};

// clang-format on
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
    case CONFIRM:
        if (record->event.pressed) {
            //spam_confirm = true;
            SEND_STRING(SS_LCTL("8") SS_DELAY(500) SS_TAP(X_ENTER));
        }
    }
    return true;
}



const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_FIRST_LAYER] = LAYOUT(
        KC_UP, KC_7, KC_8, KC_9, CONFIRM,
        KC_DOWN, KC_4, KC_5, KC_6, _______,
        LCTL(KC_4), KC_1, KC_2, KC_3, LCTL(KC_9),
        _______, _______, KC_0, _______, _______,
        LCTL(KC_6), _______, _______, _______, LCTL(KC_7)
    ),
};
