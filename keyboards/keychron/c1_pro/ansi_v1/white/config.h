/* Copyright 2022 @ Keychron (https://www.keychron.com)
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

#pragma once

/* If PH3 used with a stronger pull resistor then the following definition needs be included */
// #define MATRIX_UNSELECT_DRIVE_HIGH

#ifdef LED_MATRIX_ENABLE
/* RGB Matrix Driver Configuration */
#    define DRIVER_COUNT 1
#    define DRIVER_ADDR_1 0b1110100

/* RGB Matrix Configuration */
#    define LED_MATRIX_LED_COUNT 90

/* Scan phase of led driver set as MSKPHASE_9CHANNEL(defined as 0x03 in CKLED2001.h) */
#    define PHASE_CHANNEL MSKPHASE_9CHANNEL
#    define CKLED2001_CURRENT_TUNE \
        { 0x9D, 0x9D, 0x44, 0x9D, 0x9D, 0x44, 0x9D, 0x9D, 0x44, 0x9D, 0x9D, 0x44 }

/* turn off effects when suspended */
#    define LED_DISABLE_WHEN_USB_SUSPENDED

#    define CAPS_LOCK_LED_INDEX 87
#    define MAC_OS_LED_INDEX 88
#    define WIN_OS_LED_INDEX 89

// LED Matrix Animation modes. Explicitly enabled
// For full list of effects, see:
// https://docs.qmk.fm/#/feature_led_matrix?id=led-matrix-effects
// #if defined(LED_MATRIX_KEYPRESSES) || defined(LED_MATRIX_KEYRELEASES)
#    define LED_MATRIX_KEYPRESSES
#    define LED_MATRIX_KEYRELEASES
#endif
