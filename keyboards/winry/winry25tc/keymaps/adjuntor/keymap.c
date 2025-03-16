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
    _FIRST_LAYER,
};

enum custom_keycodes {
    ASSASSINATE,
    STAB,
};

bool spam_assassinate;
bool spam_stab;

int randy(int max, int min){
   return (rand() % (max + 1 - min)) + min;
}
//tap_code_delay(KC_A, randy(80,50));

bool process_record_user(uint16_t keycode, keyrecord_t* record) {
    switch (keycode) {
    case ASSASSINATE:
        if (record->event.pressed) {
            spam_assassinate = true;
        } else {
            spam_assassinate = false;
        }
        break;
    case STAB:
        if (record->event.pressed) {
            spam_stab = true;
        } else {
            spam_stab = false;
        }
        break;
    }
    return true;
};

void matrix_scan_user(void) {
  if (spam_assassinate) {
    SEND_STRING( SS_TAP(X_LEFT_CTRL) SS_TAP(X_A));
  }
  if (spam_stab) {
    SEND_STRING( SS_TAP(X_LEFT_SHIFT) SS_TAP(X_A) SS_TAP(X_T));
  }

};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_FIRST_LAYER] = LAYOUT(
        KC_D,  KC_F, KC_J, KC_EQUAL, KC_O,
        KC_7,   KC_8,   KC_9,   KC_0, KC_MINUS,
        KC_6,   KC_Y,   KC_5,   KC_U, KC_3,
        KC_INSERT, KC_HOME, KC_PGUP, KC_PGDN, KC_4,
        KC_DEL,   KC_END, ASSASSINATE, KC_LEFT_ALT, STAB
    ),
};
