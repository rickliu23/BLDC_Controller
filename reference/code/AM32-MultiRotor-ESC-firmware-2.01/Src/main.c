/* AM32- multi-purpose brushless controller firmware for the stm32f051 */

//===========================================================================
//=============================== Changelog =================================
//===========================================================================
/*
 * 1.54 Changelog;
 * --Added firmware name to targets and firmware version to main
 * --added two more dshot to beacons 1-3 currently working
 * --added KV option to firmware, low rpm power protection is based on KV
 * --start power now controls minimum idle power as well as startup strength.
 * --change default timing to 22.5
 * --Lowered default minimum idle setting to 1.5 percent duty cycle, slider range from 1-2.
 * --Added dshot commands to save settings and reset ESC.
 *
 *1.56 Changelog.
 * -- added check to stall protection to wait until after 40 zero crosses to fix high startup throttle hiccup.
 * -- added TIMER 1 update interrupt and PWM changes are done once per pwm period
 * -- reduce commutation interval averaging length
 * -- reduce false positive filter level to 2 and eliminate threshold where filter is stopped.
 * -- disable interrupt before sounds
 * -- disable TIM1 interrupt during stepper sinusoidal mode
 * -- add 28us delay for dshot300
 * -- report 0 rpm until the first 10 successful steps.
 * -- move serial ADC telemetry calculations and desync check to 10Khz interrupt.
 *
 * 1.57
 * -- remove spurious commutations and rpm data at startup by polling for longer interval on startup
 *
 * 1.58
 * -- move signal timeout to 10khz routine and set armed timeout to one quarter second 2500 / 10000
 * 1.59
 * -- moved comp order definitions to target.h
 * -- fixed update version number if older than new version
 * -- cleanup, moved all input and output to IO.c
 * -- moved comparator functions to comparator.c
 * -- removed ALOT of useless variables
 * -- added siskin target
 * -- moved pwm changes to 10khz routine
 * -- moved basic functions to functions.c
 * -- moved peripherals setup to periherals.c
 * -- added crawler mode settings
 *
 * 1.60
 * -- added sine mode hysteresis
 * -- increased power in stall protection and lowered start rpm for crawlers
 * -- removed onehot125 from crawler mode
 * -- reduced maximum startup power from 400 to 350
 * -- change minimum duty cycle to DEAD_TIME
 * -- version and name moved to permanent spot in FLASH memory, thanks mikeller
 *
 * 1.61
 * -- moved duty cycle calculation to 10khz and added max change option.
 * -- decreased maximum interval change to 25%
 * -- reduce wait time on fast acceleration (fast_accel)
 * -- added check in interrupt for early zero cross
 *
 * 1.62
 * --moved control to 10khz loop
 * --changed condition for low rpm filter for duty cycle from || to &&
 * --introduced max deceleration and set it to 20ms to go from 100 to 0
 * --added configurable servo throttle ranges
 *
 *
 *1.63
 *-- increase time for zero cross error detection below 250us commutation interval
 *-- increase max change a low rpm x10
 *-- set low limit of throttle ramp to a lower point and increase upper range
 *-- change desync event from full restart to just lower throttle.

 *1.64
 * --added startup check for continuous high signal, reboot to enter bootloader.
 *-- added brake on stop from eeprom
 *-- added stall protection from eeprom
 *-- added motor pole divider for sinusoidal and low rpm power protection
 *-- fixed dshot commands, added confirmation beeps and removed blocking behavior
 *--
 *1.65
 *-- Added 32 millisecond telemetry output
 *-- added low voltage cutoff , divider value and cutoff voltage needs to be added to eeprom
 *-- added beep to indicate cell count if low voltage active
 *-- added current reading on pa3 , conversion factor needs to be added to eeprom
 *-- fixed servo input capture to only read positive pulse to handle higher refresh rates.
 *-- disabled oneshot 125.
 *-- extended servo range to match full output range of receivers
 *-- added RC CAR style reverse, proportional brake on first reverse , double tap to change direction
 *-- added brushed motor control mode
 *-- added settings to EEPROM version 1
 *-- add gimbal control option.
 *--
 *1.66
 *-- move idwg init to after input tune
 *-- remove reset after save command -- dshot
 *-- added wraith32 target
 *-- added average pulse check for signal detection
 *--
 *1.67
 *-- Rework file structure for multiple MCU support
 *-- Add g071 mcu
 *--
 *1.68
 *--increased allowed average pulse length to avoid double startup
 *1.69
 *--removed line re-enabling comparator after disabling.
 *1.70 fix dshot for Kiss FC
 *1.71 fix dshot for Ardupilot / Px4 FC
 *1.72 Fix telemetry output and add 1 second arming.
 *1.73 Fix false arming if no signal. Remove low rpm throttle protection below 300kv
 *1.74 Add Sine Mode range and drake brake strength adjustment
 *1.75 Disable brake on stop for PWM_ENABLE_BRIDGE
       Removed automatic brake on stop on neutral for RC car proportional brake.
       Adjust sine speed and stall protection speed to more closely match
       makefile fixes from Cruwaller
       Removed gd32 build, until firmware is functional
 *1.76 Adjust g071 PWM frequency, and startup power to be same frequency as f051.
       Reduce number of polling back emf checks for g071
 *1.77 increase PWM frequency range to 8-48khz
 *1.78 Fix bluejay tunes frequency and speed.
       Fix g071 Dead time
       Increment eeprom version
 *1.79 Add stick throttle calibration routine
       Add variable for telemetry interval
 *1.80 -Enable Comparator blanking for g071 on timer 1 channel 4
       -add hardware group F for Iflight Blitz
       -adjust parameters for pwm frequency
       -add sine mode power variable and eeprom setting
       -fix telemetry rpm during sine mode
       -fix sounds for extended pwm range
       -Add adjustable braking strength when driving
 *1.81 -Add current limiting PID loop
       -fix current sense scale
       -Increase brake power on maximum reverse ( car mode only)
       -Add HK and Blpwr targets
       -Change low kv motor throttle limit
       -add reverse speed threshold changeover based on motor kv
       -doubled filter length for motors under 900kv
*1.82  -Add speed control pid loop.
*1.83  -Add stall protection pid loop.
       -Improve sine mode transition.
       -decrease speed step re-entering sine mode
       -added fixed duty cycle and speed mode build option
       -added rpm_controlled by input signal ( to be added to config tool )
*1.84  -Change PID value to int for faster calculations
       -Enable two channel brushed motor control for dual motors
       -Add current limit max duty cycle
*1.85  -fix current limit not allowing full rpm on g071 or low pwm frequency
        -remove unused brake on stop conditional
*1.86  - create do-once in sine mode instead of setting pwm mode each time.
*1.87  - fix fixed mode max rpm limits
*1.88  - Fix stutter on sine mode re-entry due to position reset
*1.89  - Fix drive by rpm mode scaling.
       - Fix dshot px4 timings
*1.90  - Disable comp interrupts for brushed mode
       - Re-enter polling mode after prop strike or desync
       - add G071 "N" variant
       - add preliminary Extended Dshot
*1.91  - Reset average interval time on desync only after 100 zero crosses
*1.92  - Move g071 comparator blanking to TIM1 OC5
       - Increase ADC read frequency and current sense filtering
       - Add addressable LED strip for G071 targets
*1.93  - Optimization for build process
       - Add firmware file name to each target hex file
       -fix extended telemetry not activating dshot600
       -fix low voltage cuttoff timeout
*1.94  - Add selectable input types
*1.95  - reduce timeout to 0.5 seconds when armed
*1.96  - Improved erpm accuracy dshot and serial telemetry, thanks Dj-Uran
         - Fix PID loop integral.
         - add overcurrent low voltage cuttoff to brushed mode.
*1.97    - enable input pullup
*1.98    - Dshot erpm rounding compensation.
*1.99    - Add max duty cycle change to individual targets ( will later become an settings option)
         - Fix dshot telemetry delay f4 and e230 mcu

*/

#include <stdint.h>
#include "main.h"
#include "targets.h"
#include "signal.h"
#include "dshot.h"
#include "phaseouts.h"
#include "eeprom.h"
#include "sounds.h"
#include "ADC.h"
#include "serial_telemetry.h"
#include "IO.h"
#include "comparator.h"
#include "functions.h"
#include "peripherals.h"
#include "common.h"

#ifdef USE_LED_STRIP
#include "WS2812.h"
#endif

#ifdef USE_CRSF_INPUT
#include "crsf.h"

#endif

#define VERSION_MAJOR 2
#define VERSION_MINOR 00

// firmware build options !! fixed speed and duty cycle modes are not to be used with sinusoidal startup !!

// #define FIXED_DUTY_MODE  // bypasses signal input and arming, uses a set duty cycle. For pumps, slot cars etc
// #define FIXED_DUTY_MODE_POWER 100     // 0-100 percent not used in fixed speed mode

// #define FIXED_SPEED_MODE  // bypasses input signal and runs at a fixed rpm using the speed control loop PID
// #define FIXED_SPEED_MODE_RPM  1000  // intended final rpm , ensure pole pair numbers are entered correctly in config tool.

// #define BRUSHED_MODE         // overrides all brushless config settings, enables two channels for brushed control
// #define GIMBAL_MODE     // also sinusoidal_startup needs to be on, maps input to sinusoidal angle.

//===========================================================================
//=============================  Defaults =============================
//===========================================================================

uint8_t drive_by_rpm = 0;
uint32_t MAXIMUM_RPM_SPEED_CONTROL = 5000;
uint32_t MINIMUM_RPM_SPEED_CONTROL = 500;

// assign speed control PID values values are x10000
fastPID speedPid = { // commutation speed loop time
    .Kp = 10,
    .Ki = 0,
    .Kd = 100,
    .integral_limit = 10000,
    .output_limit = 50000};

fastPID currentPid = { // 1khz loop time
    .Kp = 800,
    .Ki = 0,
    .Kd = 1000,
    .integral_limit = 20000,
    .output_limit = 100000};

fastPID stallPid = { // 1khz loop time
    .Kp = 2,
    .Ki = 0,
    .Kd = 50,
    .integral_limit = 10000,
    .output_limit = 50000};

enum inputType
{
    AUTO_IN,
    DSHOT_IN,
    SERVO_IN,
    SERIAL_IN,
    EDTARM,
};

uint16_t target_e_com_time_high;
uint16_t target_e_com_time_low;

uint8_t crsf_input_channel = 1;
char eeprom_layout_version = 2;
char dir_reversed = 0;
char comp_pwm = 1;
char VARIABLE_PWM = 1;
char bi_direction = 0;
char stuck_rotor_protection = 1; // Turn off for Crawlers
char brake_on_stop = 0;
char stall_protection = 0;
char use_sin_start = 0;
char TLM_ON_INTERVAL = 0;
uint8_t telemetry_interval_ms = 30;
uint8_t TEMPERATURE_LIMIT = 255; // degrees 255 to disable
char advance_level = 2;          // 7.5 degree increments 0 , 7.5, 15, 22.5)
uint16_t motor_kv = 2000;
char motor_poles = 14;
uint16_t CURRENT_LIMIT = 202;
uint8_t sine_mode_power = 5;
char drag_brake_strength = 10; // Drag Brake Power when brake on stop is enabled
uint8_t driving_brake_strength = 10;
uint8_t dead_time_override = DEAD_TIME;
char sine_mode_changeover_thottle_level = 5; // Sine Startup Range
uint16_t stall_protect_target_interval = TARGET_STALL_PROTECTION_INTERVAL;
char USE_HALL_SENSOR = 0;
uint16_t enter_sine_angle = 180;
char do_once_sinemode = 0;
//============================= Servo Settings ==============================
uint16_t servo_low_threshold = 1100;  // anything below this point considered 0
uint16_t servo_high_threshold = 1900; // anything above this point considered 2000 (max)
uint16_t servo_neutral = 1500;
uint8_t servo_dead_band = 100;

//========================= Battery Cuttoff Settings ========================
char LOW_VOLTAGE_CUTOFF = 0;         // Turn Low Voltage CUTOFF on or off
uint16_t low_cell_volt_cutoff = 330; // 3.3volts per cell

//=========================== END EEPROM Defaults ===========================

typedef struct __attribute__((packed))
{
    uint8_t version_major;
    uint8_t version_minor;
    char device_name[12];
} firmware_info_s;

firmware_info_s __attribute__((section(".firmware_info"))) firmware_info = {
    version_major : VERSION_MAJOR,
    version_minor : VERSION_MINOR,
    device_name : FIRMWARE_NAME
};

const char filename[30] __attribute__((section(".file_name"))) = FILE_NAME;

uint8_t EEPROM_VERSION;
// move these to targets folder or peripherals for each mcu
char RC_CAR_REVERSE = 0; // have to set bidirectional, comp_pwm off and stall protection off, no sinusoidal startup
uint16_t ADC_CCR = 30;
uint16_t current_angle = 90;
uint16_t desired_angle = 90;

uint16_t target_e_com_time = 0;
int16_t Speed_pid_output;
char use_speed_control_loop = 0;
float input_override = 0;
int16_t use_current_limit_adjust = 2000;
char use_current_limit = 0;
float stall_protection_adjust = 0;

uint32_t MCU_Id = 0;
uint32_t REV_Id = 0;

uint16_t armed_timeout_count;
uint16_t reverse_speed_threshold = 1500;
uint8_t desync_happened = 0;
char maximum_throttle_change_ramp = 1;

char crawler_mode = 0; // no longer used //
uint16_t velocity_count = 0;
uint16_t velocity_count_threshold = 75;

char low_rpm_throttle_limit = 1;

uint16_t low_voltage_count = 0;
uint16_t telem_ms_count;

char VOLTAGE_DIVIDER = TARGET_VOLTAGE_DIVIDER; // 100k upper and 10k lower resistor in divider
uint16_t battery_voltage;                      // scale in volts * 10.  1260 is a battery voltage of 12.60
char cell_count = 0;
char brushed_direction_set = 0;

uint16_t tenkhzcounter = 0;
float consumed_current = 0;
int32_t smoothed_raw_current = 0;
int16_t actual_current = 0;

char lowkv = 0;

uint16_t min_startup_duty = 120;
uint16_t sin_mode_min_s_d = 120;
char bemf_timeout = 10;

char startup_boost = 50;
char reversing_dead_band = 1;

uint16_t low_pin_count = 0;

uint8_t max_duty_cycle_change = 2;
char fast_accel = 1;
uint16_t last_duty_cycle = 0;
char play_tone_flag = 0;

typedef enum
{
    GPIO_PIN_RESET = 0U,
    GPIO_PIN_SET
} GPIO_PinState;

uint16_t startup_max_duty_cycle = 300 + DEAD_TIME;
uint16_t minimum_duty_cycle = DEAD_TIME;
uint16_t stall_protect_minimum_duty = DEAD_TIME;
char desync_check = 0;
char low_kv_filter_level = 20;

uint16_t tim1_arr = TIM1_AUTORELOAD;                 // current auto reset value
uint16_t TIMER1_MAX_ARR = TIM1_AUTORELOAD;           // maximum auto reset register value
uint16_t duty_cycle_maximum = TIM1_AUTORELOAD;       // restricted by temperature or low rpm throttle protect
uint16_t low_rpm_level = 20;                         // thousand erpm used to set range for throttle resrictions
uint16_t high_rpm_level = 70;                        //
uint16_t throttle_max_at_low_rpm = 400;              // 低转速时允许的最大占空比
uint16_t throttle_max_at_high_rpm = TIM1_AUTORELOAD; // 高转速时允许的最大占空比

uint16_t commutation_intervals[6] = {0};
uint32_t average_interval = 0;
uint32_t last_average_interval;
int e_com_time;

uint16_t ADC_smoothed_input = 0;
uint8_t degrees_celsius;
uint16_t converted_degrees;
uint8_t temperature_offset;
uint16_t ADC_raw_temp;
uint16_t ADC_raw_volts;
uint16_t ADC_raw_current;
uint16_t ADC_raw_input;
uint8_t adc_counter = 0;
char send_telemetry = 0;
char telemetry_done = 0;
char prop_brake_active = 0;

uint8_t eepromBuffer[176] = {0};

char dshot_telemetry = 0;

uint8_t last_dshot_command = 0;
char old_routine = 0;
uint16_t adjusted_input = 0;

#define TEMP30_CAL_VALUE ((uint16_t *)((uint32_t)0x1FFFF7B8))
#define TEMP110_CAL_VALUE ((uint16_t *)((uint32_t)0x1FFFF7C2))

uint16_t smoothedinput = 0;
const uint8_t numReadings = 30; // the readings from the analog input
uint8_t readIndex = 0;          // the index of the current reading
int total = 0;
uint16_t readings[30];

uint8_t bemf_timeout_happened = 0;
uint8_t changeover_step = 5;
uint8_t filter_level = 5;
uint8_t running = 0;
uint16_t advance = 0;
uint8_t advancedivisor = 6;
char rising = 1;

////Space Vector PWM ////////////////
// const int pwmSin[] ={128, 132, 136, 140, 143, 147, 151, 155, 159, 162, 166, 170, 174, 178, 181, 185, 189, 192, 196, 200, 203, 207, 211, 214, 218, 221, 225, 228, 232, 235, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 248, 249, 250, 250, 251, 252, 252, 253, 253, 253, 254, 254, 254, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 254, 254, 254, 253, 253, 253, 252, 252, 251, 250, 250, 249, 248, 248, 247, 246, 245, 244, 243, 242, 241, 240, 239, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 248, 249, 250, 250, 251, 252, 252, 253, 253, 253, 254, 254, 254, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 254, 254, 254, 253, 253, 253, 252, 252, 251, 250, 250, 249, 248, 248, 247, 246, 245, 244, 243, 242, 241, 240, 239, 238, 235, 232, 228, 225, 221, 218, 214, 211, 207, 203, 200, 196, 192, 189, 185, 181, 178, 174, 170, 166, 162, 159, 155, 151, 147, 143, 140, 136, 132, 128, 124, 120, 116, 113, 109, 105, 101, 97, 94, 90, 86, 82, 78, 75, 71, 67, 64, 60, 56, 53, 49, 45, 42, 38, 35, 31, 28, 24, 21, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 8, 7, 6, 6, 5, 4, 4, 3, 3, 3, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 5, 6, 6, 7, 8, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 8, 7, 6, 6, 5, 4, 4, 3, 3, 3, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 5, 6, 6, 7, 8, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 21, 24, 28, 31, 35, 38, 42, 45, 49, 53, 56, 60, 64, 67, 71, 75, 78, 82, 86, 90, 94, 97, 101, 105, 109, 113, 116, 120, 124};

////Sine Wave PWM ///////////////////
int16_t pwmSin[] = {180, 183, 186, 189, 193, 196, 199, 202,
                    205, 208, 211, 214, 217, 220, 224, 227,
                    230, 233, 236, 239, 242, 245, 247, 250,
                    253, 256, 259, 262, 265, 267, 270, 273,
                    275, 278, 281, 283, 286, 288, 291, 293,
                    296, 298, 300, 303, 305, 307, 309, 312,
                    314, 316, 318, 320, 322, 324, 326, 327,
                    329, 331, 333, 334, 336, 337, 339, 340,
                    342, 343, 344, 346, 347, 348, 349, 350,
                    351, 352, 353, 354, 355, 355, 356, 357,
                    357, 358, 358, 359, 359, 359, 360, 360,
                    360, 360, 360, 360, 360, 360, 360, 359,
                    359, 359, 358, 358, 357, 357, 356, 355,
                    355, 354, 353, 352, 351, 350, 349, 348,
                    347, 346, 344, 343, 342, 340, 339, 337,
                    336, 334, 333, 331, 329, 327, 326, 324,
                    322, 320, 318, 316, 314, 312, 309, 307,
                    305, 303, 300, 298, 296, 293, 291, 288,
                    286, 283, 281, 278, 275, 273, 270, 267,
                    265, 262, 259, 256, 253, 250, 247, 245,
                    242, 239, 236, 233, 230, 227, 224, 220,
                    217, 214, 211, 208, 205, 202, 199, 196,
                    193, 189, 186, 183, 180, 177, 174, 171,
                    167, 164, 161, 158, 155, 152, 149, 146,
                    143, 140, 136, 133, 130, 127, 124, 121,
                    118, 115, 113, 110, 107, 104, 101, 98,
                    95, 93, 90, 87, 85, 82, 79, 77,
                    74, 72, 69, 67, 64, 62, 60, 57,
                    55, 53, 51, 48, 46, 44, 42, 40,
                    38, 36, 34, 33, 31, 29, 27, 26,
                    24, 23, 21, 20, 18, 17, 16, 14,
                    13, 12, 11, 10, 9, 8, 7, 6,
                    5, 5, 4, 3, 3, 2, 2, 1,
                    1, 1, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 1, 1, 1, 2, 2,
                    3, 3, 4, 5, 5, 6, 7, 8,
                    9, 10, 11, 12, 13, 14, 16, 17,
                    18, 20, 21, 23, 24, 26, 27, 29,
                    31, 33, 34, 36, 38, 40, 42, 44,
                    46, 48, 51, 53, 55, 57, 60, 62,
                    64, 67, 69, 72, 74, 77, 79, 82,
                    85, 87, 90, 93, 95, 98, 101, 104,
                    107, 110, 113, 115, 118, 121, 124, 127,
                    130, 133, 136, 140, 143, 146, 149, 152,
                    155, 158, 161, 164, 167, 171, 174, 177};

// int sin_divider = 2;
int16_t phase_A_position;
int16_t phase_B_position;
int16_t phase_C_position;
uint16_t step_delay = 100;
char stepper_sine = 0;
char forward = 1;
uint16_t gate_drive_offset = DEAD_TIME;

uint8_t stuckcounter = 0;
uint16_t k_erpm;
uint16_t e_rpm; // electrical revolution /100 so,  123 is 12300 erpm

uint16_t adjusted_duty_cycle;

uint8_t bad_count = 0;
uint8_t bad_count_threshold = CPU_FREQUENCY_MHZ / 24;
uint8_t dshotcommand;
uint16_t armed_count_threshold = 1000;

char armed = 0;

// 只有在油门最低并保持一段时间后，才允许电机启动。防止一上电或一连接信号就突然转动
uint16_t zero_input_count = 0; // 是连续检测到"油门为 0"的计数器，主要用于 ESC 的解锁安全机制。

uint16_t input = 0;
uint16_t newinput = 0;
char inputSet = 0;
char dshot = 0;
char servoPwm = 0;
uint32_t zero_crosses;

uint8_t zcfound = 0;

uint8_t bemfcounter;
uint8_t min_bemf_counts_up = TARGET_MIN_BEMF_COUNTS;
uint8_t min_bemf_counts_down = TARGET_MIN_BEMF_COUNTS;

uint16_t lastzctime;
uint16_t thiszctime;

uint16_t duty_cycle = 0;
char step = 1;
uint16_t commutation_interval = 12500;
uint16_t waitTime = 0;
uint16_t signaltimeout = 0;
uint8_t ubAnalogWatchdogStatus = RESET;

void checkForHighSignal()
{
    changeToInput();
    LL_GPIO_SetPinPull(INPUT_PIN_PORT, INPUT_PIN, LL_GPIO_PULL_DOWN);
    delayMicros(1000);
    for (int i = 0; i < 1000; i++)
    {
        if (!(INPUT_PIN_PORT->IDR & INPUT_PIN))
        { // if the pin is low for 5 checks out of 100 in  100ms or more its either no signal or signal. jump to application
            low_pin_count++;
        }
        delayMicros(10);
    }
    LL_GPIO_SetPinPull(INPUT_PIN_PORT, INPUT_PIN, LL_GPIO_PULL_UP);
    if (low_pin_count > 5)
    {
        return; // its either a signal or a disconnected pin
    }
    else
    {
        allOff();
        NVIC_SystemReset();
    }
}

float doPidCalculations(struct fastPID *pidnow, int actual, int target)
{

    pidnow->error = actual - target;
    pidnow->integral = pidnow->integral + pidnow->error * pidnow->Ki;
    if (pidnow->integral > pidnow->integral_limit)
    {
        pidnow->integral = pidnow->integral_limit;
    }
    if (pidnow->integral < -pidnow->integral_limit)
    {
        pidnow->integral = -pidnow->integral_limit;
    }

    pidnow->derivative = pidnow->Kd * (pidnow->error - pidnow->last_error);
    pidnow->last_error = pidnow->error;

    pidnow->pid_output = pidnow->error * pidnow->Kp + pidnow->integral + pidnow->derivative;

    if (pidnow->pid_output > pidnow->output_limit)
    {
        pidnow->pid_output = pidnow->output_limit;
    }
    if (pidnow->pid_output < -pidnow->output_limit)
    {
        pidnow->pid_output = -pidnow->output_limit;
    }
    return pidnow->pid_output;
}

void loadEEpromSettings()
{
    read_flash_bin(eepromBuffer, EEPROM_START_ADD, 176);

    if (eepromBuffer[17] == 0x01)
    {
        dir_reversed = 1;
    }
    else
    {
        dir_reversed = 0;
    }
    if (eepromBuffer[18] == 0x01)
    {
        bi_direction = 1;
    }
    else
    {
        bi_direction = 0;
    }
    if (eepromBuffer[19] == 0x01)
    {
        use_sin_start = 1;
        //	 min_startup_duty = sin_mode_min_s_d;
    }
    if (eepromBuffer[20] == 0x01)
    {
        comp_pwm = 1;
    }
    else
    {
        comp_pwm = 0;
    }
    if (eepromBuffer[21] == 0x01)
    {
        VARIABLE_PWM = 1;
    }
    else
    {
        VARIABLE_PWM = 0;
    }
    if (eepromBuffer[22] == 0x01)
    {
        stuck_rotor_protection = 1;
    }
    else
    {
        stuck_rotor_protection = 0;
    }
    if (eepromBuffer[23] < 4)
    {
        advance_level = eepromBuffer[23];
    }
    else
    {
        advance_level = 2; // * 7.5 increments
    }

    if (eepromBuffer[24] < 49 && eepromBuffer[24] > 7)
    {
        if (eepromBuffer[24] < 49 && eepromBuffer[24] > 23)
        {
            TIMER1_MAX_ARR = map(eepromBuffer[24], 24, 48, TIM1_AUTORELOAD, TIM1_AUTORELOAD / 2);
        }
        if (eepromBuffer[24] < 24 && eepromBuffer[24] > 11)
        {
            TIMER1_MAX_ARR = map(eepromBuffer[24], 12, 24, TIM1_AUTORELOAD * 2, TIM1_AUTORELOAD);
        }
        if (eepromBuffer[24] < 12 && eepromBuffer[24] > 7)
        {
            TIMER1_MAX_ARR = map(eepromBuffer[24], 7, 16, TIM1_AUTORELOAD * 3, TIM1_AUTORELOAD / 2 * 3);
        }
        TIM1->ARR = TIMER1_MAX_ARR;
        throttle_max_at_high_rpm = TIMER1_MAX_ARR;
        duty_cycle_maximum = TIMER1_MAX_ARR;
    }
    else
    {
        tim1_arr = TIM1_AUTORELOAD;
        TIM1->ARR = tim1_arr;
    }

    if (eepromBuffer[25] < 151 && eepromBuffer[25] > 49)
    {
        min_startup_duty = (eepromBuffer[25] + DEAD_TIME) * TIMER1_MAX_ARR / 2000;
        minimum_duty_cycle = (eepromBuffer[25] / 2 + DEAD_TIME / 3) * TIMER1_MAX_ARR / 2000;
        stall_protect_minimum_duty = minimum_duty_cycle + 10;
    }
    else
    {
        min_startup_duty = 150;
        minimum_duty_cycle = (min_startup_duty / 2) + 10;
    }
    motor_kv = (eepromBuffer[26] * 40) + 20;
    motor_poles = eepromBuffer[27];
    if (eepromBuffer[28] == 0x01)
    {
        brake_on_stop = 1;
    }
    else
    {
        brake_on_stop = 0;
    }
    if (eepromBuffer[29] == 0x01)
    {
        stall_protection = 1;
    }
    else
    {
        stall_protection = 0;
    }
    setVolume(5);
    if (eepromBuffer[1] > 0)
    { // these commands weren't introduced until eeprom version 1.

        if (eepromBuffer[30] > 11)
        {
            setVolume(5);
        }
        else
        {
            setVolume(eepromBuffer[30]);
        }
        if (eepromBuffer[31] == 0x01)
        {
            TLM_ON_INTERVAL = 1;
        }
        else
        {
            TLM_ON_INTERVAL = 0;
        }
        servo_low_threshold = (eepromBuffer[32] * 2) + 750; // anything below this point considered 0
        servo_high_threshold = (eepromBuffer[33] * 2) + 1750;
        ; // anything above this point considered 2000 (max)
        servo_neutral = (eepromBuffer[34]) + 1374;
        servo_dead_band = eepromBuffer[35];

        if (eepromBuffer[36] == 0x01)
        {
            LOW_VOLTAGE_CUTOFF = 1;
        }
        else
        {
            LOW_VOLTAGE_CUTOFF = 0;
        }

        low_cell_volt_cutoff = eepromBuffer[37] + 250; // 2.5 to 3.5 volts per cell range
        if (eepromBuffer[38] == 0x01)
        {
            RC_CAR_REVERSE = 1;
        }
        else
        {
            RC_CAR_REVERSE = 0;
        }
        if (eepromBuffer[39] == 0x01)
        {
#ifdef HAS_HALL_SENSORS
            USE_HALL_SENSOR = 1;
#else
            USE_HALL_SENSOR = 0;
#endif
        }
        else
        {
            USE_HALL_SENSOR = 0;
        }
        if (eepromBuffer[40] > 4 && eepromBuffer[40] < 26)
        { // sine mode changeover 5-25 percent throttle
            sine_mode_changeover_thottle_level = eepromBuffer[40];
        }
        if (eepromBuffer[41] > 0 && eepromBuffer[41] < 11)
        { // drag brake 1-10
            drag_brake_strength = eepromBuffer[41];
        }

        if (eepromBuffer[42] > 0 && eepromBuffer[42] < 10)
        { // motor brake 1-9
            driving_brake_strength = eepromBuffer[42];
            dead_time_override = DEAD_TIME + (150 - (driving_brake_strength * 10));
            if (dead_time_override > 200)
            {
                dead_time_override = 200;
            }
            min_startup_duty = eepromBuffer[25] + dead_time_override;
            minimum_duty_cycle = eepromBuffer[25] / 2 + dead_time_override;
            throttle_max_at_low_rpm = throttle_max_at_low_rpm + dead_time_override;
            startup_max_duty_cycle = startup_max_duty_cycle + dead_time_override;
            TIM1->BDTR |= dead_time_override;
        }

        if (eepromBuffer[43] >= 70 && eepromBuffer[43] <= 140)
        {
            TEMPERATURE_LIMIT = eepromBuffer[43];
        }

        if (eepromBuffer[44] > 0 && eepromBuffer[44] < 100)
        {
            CURRENT_LIMIT = eepromBuffer[44] * 2;
            use_current_limit = 1;
        }
        if (eepromBuffer[45] > 0 && eepromBuffer[45] < 11)
        {
            sine_mode_power = eepromBuffer[45];
        }

        if (eepromBuffer[46] >= 0 && eepromBuffer[46] < 10)
        {
            switch (eepromBuffer[46])
            {
            case AUTO_IN:
                dshot = 0;
                servoPwm = 0;
                EDT_ARMED = 1;
                break;
            case DSHOT_IN:
                dshot = 1;
                EDT_ARMED = 1;
                break;
            case SERVO_IN:
                servoPwm = 1;
                break;
            case SERIAL_IN:
                break;
            case EDTARM:
                EDT_ARM_ENABLE = 1;
                EDT_ARMED = 0;
                dshot = 1;
                break;
            };
        }
        else
        {
            dshot = 0;
            servoPwm = 0;
            EDT_ARMED = 1;
        }

        if (motor_kv < 300)
        {
            low_rpm_throttle_limit = 0;
        }
        low_rpm_level = motor_kv / 100 / (32 / motor_poles);
        high_rpm_level = motor_kv / 17 / (32 / motor_poles);
    }
    //	   reverse_speed_threshold =  map(motor_kv, 300, 3000, 2500 , 1250);
    reverse_speed_threshold = 200;
    if (!comp_pwm)
    {
        bi_direction = 0;
    }
}

void saveEEpromSettings()
{

    eepromBuffer[1] = eeprom_layout_version;
    if (dir_reversed == 1)
    {
        eepromBuffer[17] = 0x01;
    }
    else
    {
        eepromBuffer[17] = 0x00;
    }
    if (bi_direction == 1)
    {
        eepromBuffer[18] = 0x01;
    }
    else
    {
        eepromBuffer[18] = 0x00;
    }
    if (use_sin_start == 1)
    {
        eepromBuffer[19] = 0x01;
    }
    else
    {
        eepromBuffer[19] = 0x00;
    }

    if (comp_pwm == 1)
    {
        eepromBuffer[20] = 0x01;
    }
    else
    {
        eepromBuffer[20] = 0x00;
    }
    if (VARIABLE_PWM == 1)
    {
        eepromBuffer[21] = 0x01;
    }
    else
    {
        eepromBuffer[21] = 0x00;
    }
    if (stuck_rotor_protection == 1)
    {
        eepromBuffer[22] = 0x01;
    }
    else
    {
        eepromBuffer[22] = 0x00;
    }
    eepromBuffer[23] = advance_level;
    save_flash_nolib(eepromBuffer, 176, EEPROM_START_ADD);
}

void getSmoothedInput()
{

    total = total - readings[readIndex];
    readings[readIndex] = commutation_interval;
    total = total + readings[readIndex];
    readIndex = readIndex + 1;
    if (readIndex >= numReadings)
    {
        readIndex = 0;
    }
    smoothedinput = total / numReadings;
}

void getBemfState()
{
    uint8_t current_state = 0;
#ifdef MCU_F031
    if (step == 1 || step == 4)
    {
        current_state = PHASE_C_EXTI_PORT->IDR & PHASE_C_EXTI_PIN;
    }
    if (step == 2 || step == 5)
    { //        in phase two or 5 read from phase A Pf1
        current_state = PHASE_A_EXTI_PORT->IDR & PHASE_A_EXTI_PIN;
    }
    if (step == 3 || step == 6)
    { // phase B pf0
        current_state = PHASE_B_EXTI_PORT->IDR & PHASE_B_EXTI_PIN;
    }
#else
    current_state = !LL_COMP_ReadOutputLevel(active_COMP); // polarity reversed
#endif
    if (rising)
    {
        if (current_state)
        {
            bemfcounter++;
        }
        else
        {
            bad_count++;
            if (bad_count > bad_count_threshold)
            {
                bemfcounter = 0;
            }
        }
    }
    else
    {
        if (!current_state)
        {
            bemfcounter++;
        }
        else
        {
            bad_count++;
            if (bad_count > bad_count_threshold)
            {
                bemfcounter = 0;
            }
        }
    }
}

void commutate()
{
    commutation_intervals[step - 1] = thiszctime;
    e_com_time = ((commutation_intervals[0] + commutation_intervals[1] + commutation_intervals[2] + commutation_intervals[3] + commutation_intervals[4] + commutation_intervals[5]) + 4) >> 1; // COMMUTATION INTERVAL IS 0.5US INCREMENTS

    //	COM_TIMER->CNT = 0;
    if (forward == 1)
    {
        step++;
        if (step > 6)
        {
            step = 1;
            desync_check = 1;
        }
        rising = step % 2;
    }
    else
    {
        step--;
        if (step < 1)
        {
            step = 6;
            desync_check = 1;
        }
        rising = !(step % 2);
    }

    if (!prop_brake_active)
    {
        comStep(step);
    }

    changeCompInput();

    if (average_interval > 2000 && (stall_protection || RC_CAR_REVERSE))
    {
        old_routine = 1;
    }
    bemfcounter = 0;
    zcfound = 0;
    if (use_speed_control_loop && running)
    {
        input_override += doPidCalculations(&speedPid, e_com_time, target_e_com_time) / 10000;
        if (input_override > 2047)
        {
            input_override = 2047;
        }
        if (input_override < 0)
        {
            input_override = 0;
        }
        if (zero_crosses < 100)
        {
            speedPid.integral = 0;
        }
    }
}

void PeriodElapsedCallback()
{

    COM_TIMER->DIER &= ~((0x1UL << (0U))); // disable interrupt
    commutation_interval = ((3 * commutation_interval) + thiszctime) >> 2;
    commutate();
    advance = (commutation_interval >> 3) * advance_level; // 60 divde 8 7.5 degree increments
    waitTime = (commutation_interval >> 1) - advance;
    if (!old_routine)
    {
        enableCompInterrupts(); // enable comp interrupt
    }
    if (zero_crosses < 10000)
    {
        zero_crosses++;
    }
}

void interruptRoutine()
{
    if (average_interval > 125)
    {
        if ((INTERVAL_TIMER->CNT < 125) && (duty_cycle < 600) && (zero_crosses < 500))
        { // should be impossible, desync?exit anyway
            return;
        }
        if (INTERVAL_TIMER->CNT < (commutation_interval >> 1))
        {
            return;
        }
        stuckcounter++; // stuck at 100 interrupts before the main loop happens again.
        if (stuckcounter > 100)
        {
            maskPhaseInterrupts();
            zero_crosses = 0;
            return;
        }
    }
    thiszctime = INTERVAL_TIMER->CNT;
    if (rising)
    {
        for (int i = 0; i < filter_level; i++)
        {
#ifdef MCU_F031
            if (!(current_GPIO_PORT->IDR & current_GPIO_PIN))
            {
#else
            if (LL_COMP_ReadOutputLevel(active_COMP) == LL_COMP_OUTPUT_LEVEL_HIGH)
            {
#endif
                return;
            }
        }
    }
    else
    {
        for (int i = 0; i < filter_level; i++)
        {
#ifdef MCU_F031
            if ((current_GPIO_PORT->IDR & current_GPIO_PIN))
            {
#else
            if (LL_COMP_ReadOutputLevel(active_COMP) == LL_COMP_OUTPUT_LEVEL_LOW)
            {
#endif
                return;
            }
        }
    }
    maskPhaseInterrupts();
    thiszctime += INTERVAL_TIMER->CNT - thiszctime;
    INTERVAL_TIMER->CNT = 0;
    waitTime = waitTime >> fast_accel;
    COM_TIMER->CNT = 0;
    COM_TIMER->ARR = waitTime;
    COM_TIMER->SR = 0x00;
    COM_TIMER->DIER |= (0x1UL << (0U)); // enable COM_TIMER interrupt
}

void startMotor()
{
    if (running == 0)
    {
        commutate();
        commutation_interval = 10000;
        INTERVAL_TIMER->CNT = 5000;
        running = 1;
    }
    enableCompInterrupts();
}

void tenKhzRoutine()
{

    tenkhzcounter++;
    if (tenkhzcounter > 10000)
    { // 1s sample interval 10000
        consumed_current = (float)actual_current / 360 + consumed_current;
        switch (dshot_extended_telemetry)
        {

        case 1:
            send_extended_dshot = 0b0010 << 8 | degrees_celsius;
            dshot_extended_telemetry = 2;
            break;
        case 2:
            send_extended_dshot = 0b0110 << 8 | (uint8_t)actual_current / 50;
            dshot_extended_telemetry = 3;
            break;
        case 3:
            send_extended_dshot = 0b0100 << 8 | (uint8_t)(battery_voltage / 25);
            dshot_extended_telemetry = 1;
            break;
        }

        tenkhzcounter = 0;
    }
    if (!armed && (cell_count == 0))
    {
        if (inputSet)
        {
            if (adjusted_input == 0)
            {
                armed_timeout_count++;
                if (armed_timeout_count > 10000)
                { // one second
                    if (zero_input_count > 30)
                    {
                        armed = 1;
#ifdef USE_LED_STRIP
                        //	send_LED_RGB(0,0,0);
                        delayMicros(1000);
                        send_LED_RGB(0, 255, 0);
#endif
#ifdef USE_RGB_LED
                        GPIOB->BRR = LL_GPIO_PIN_3;  // turn on green
                        GPIOB->BSRR = LL_GPIO_PIN_8; // turn on green
                        GPIOB->BSRR = LL_GPIO_PIN_5;
#endif
                        if ((cell_count == 0) && LOW_VOLTAGE_CUTOFF)
                        {
                            cell_count = battery_voltage / 370;
                            for (int i = 0; i < cell_count; i++)
                            {
                                playInputTune();
                                delayMillis(100);
                                LL_IWDG_ReloadCounter(IWDG);
                            }
                        }
                        else
                        {
                            playInputTune();
                        }
                        if (!servoPwm)
                        {
                            RC_CAR_REVERSE = 0;
                        }
                    }
                    else
                    {
                        inputSet = 0;
                        armed_timeout_count = 0;
                    }
                }
            }
            else
            {
                armed_timeout_count = 0;
            }
        }
    }

    if (TLM_ON_INTERVAL)
    {
        telem_ms_count++;
        if (telem_ms_count > telemetry_interval_ms * 10)
        {
            send_telemetry = 1;
            telem_ms_count = 0;
        }
    }
#ifndef BRUSHED_MODE
    if (!stepper_sine)
    {
        if (input >= 47 + (80 * use_sin_start) && armed)
        {
            if (running == 0)
            {
                allOff();
                if (!old_routine)
                {
                    startMotor();
                }
                running = 1;
                last_duty_cycle = min_startup_duty;
            }
            if (use_sin_start)
            {
                duty_cycle = map(input, 137, 2047, minimum_duty_cycle, TIMER1_MAX_ARR);
            }
            else
            {
                duty_cycle = map(input, 47, 2047, minimum_duty_cycle, TIMER1_MAX_ARR);
            }
            if (tenkhzcounter % 10 == 0)
            { // 1khz PID loop
                if (use_current_limit && running)
                {
                    use_current_limit_adjust -= (int16_t)(doPidCalculations(&currentPid, actual_current, CURRENT_LIMIT * 100) / 10000);
                    if (use_current_limit_adjust < minimum_duty_cycle)
                    {
                        use_current_limit_adjust = minimum_duty_cycle;
                    }
                    if (use_current_limit_adjust > duty_cycle)
                    {
                        use_current_limit_adjust = duty_cycle;
                    }
                }

                if (stall_protection && running)
                { // this boosts throttle as the rpm gets lower, for crawlers and rc cars only, do not use for multirotors.
                    stall_protection_adjust += (doPidCalculations(&stallPid, commutation_interval, stall_protect_target_interval)) / 10000;
                    if (stall_protection_adjust > 150)
                    {
                        stall_protection_adjust = 150;
                    }
                    if (stall_protection_adjust <= 0)
                    {
                        stall_protection_adjust = 0;
                    }
                }
                //			  if(use_speed_control_loop && running){
                //			  input_override += doPidCalculations(&speedPid, e_com_time, target_e_com_time)/10000;
                //			  if(input_override > 2047){
                //				  input_override = 2047;
                //			  }
                //			  if(input_override < 0){
                //				  input_override = 0;
                //			  }
                //			  if(zero_crosses < 100){
                //				  speedPid.integral = 0;
                //			  }
                //		}
            }
            if (!RC_CAR_REVERSE)
            {
                prop_brake_active = 0;
            }
        }
        if (input < 47 + (80 * use_sin_start))
        {
            if (play_tone_flag != 0)
            {
                if (play_tone_flag == 1)
                {
                    playDefaultTone();
                }
                if (play_tone_flag == 2)
                {
                    playChangedTone();
                }
                play_tone_flag = 0;
            }

            if (!comp_pwm)
            {
                duty_cycle = 0;
                if (!running)
                {
                    old_routine = 1;
                    zero_crosses = 0;
                    if (brake_on_stop)
                    {
                        fullBrake();
                    }
                    else
                    {
                        if (!prop_brake_active)
                        {
                            allOff();
                        }
                    }
                }
                if (RC_CAR_REVERSE && prop_brake_active)
                {
#ifndef PWM_ENABLE_BRIDGE
                    duty_cycle = getAbsDif(1000, newinput) + 1000;
                    if (duty_cycle == 2000)
                    {
                        fullBrake();
                    }
                    else
                    {
                        proportionalBrake();
                    }
#endif
                }
            }
            else
            {
                if (!running)
                {
                    duty_cycle = 0;
                    old_routine = 1;
                    zero_crosses = 0;
                    bad_count = 0;
                    if (brake_on_stop)
                    {
                        if (!use_sin_start)
                        {
#ifndef PWM_ENABLE_BRIDGE
                            duty_cycle = (TIMER1_MAX_ARR - 19) + drag_brake_strength * 2;
                            proportionalBrake();
                            prop_brake_active = 1;
#else
                            // todo add proportional braking for pwm/enable style bridge.
#endif
                        }
                    }
                    else
                    {
                        allOff();
                        duty_cycle = 0;
                    }
                }

                phase_A_position = ((step - 1) * 60) + enter_sine_angle;
                if (phase_A_position > 359)
                {
                    phase_A_position -= 360;
                }
                phase_B_position = phase_A_position + 119;
                if (phase_B_position > 359)
                {
                    phase_B_position -= 360;
                }
                phase_C_position = phase_A_position + 239;
                if (phase_C_position > 359)
                {
                    phase_C_position -= 360;
                }

                if (use_sin_start == 1)
                {
                    stepper_sine = 1;
                }
            }
        }
        if (!prop_brake_active)
        {
            if (zero_crosses < (20 >> stall_protection))
            {
                if (duty_cycle < min_startup_duty)
                {
                    duty_cycle = min_startup_duty;
                }
                if (duty_cycle > startup_max_duty_cycle)
                {
                    duty_cycle = startup_max_duty_cycle;
                }
            }

            if (duty_cycle > duty_cycle_maximum)
            {
                duty_cycle = duty_cycle_maximum;
            }
            if (use_current_limit)
            {
                if (duty_cycle > use_current_limit_adjust)
                {
                    duty_cycle = use_current_limit_adjust;
                }
            }

            if (stall_protection_adjust > 0)
            {

                duty_cycle = duty_cycle + (uint16_t)stall_protection_adjust;
            }
            if (maximum_throttle_change_ramp)
            {
                //	max_duty_cycle_change = map(k_erpm, low_rpm_level, high_rpm_level, 1, 40);
#ifdef VOLTAGE_BASED_RAMP
                uint16_t voltage_based_max_change = map(battery_voltage, 800, 2200, 10, 1);
                if (average_interval > 200)
                {
                    max_duty_cycle_change = voltage_based_max_change;
                }
                else
                {
                    max_duty_cycle_change = voltage_based_max_change * 3;
                }
#else
                if (average_interval > 300)
                {
                    max_duty_cycle_change = RAMP_SPEED_LOW_RPM;
                }
                else
                {
                    max_duty_cycle_change = RAMP_SPEED_LOW_RPM * 3;
                }
#endif

                if ((duty_cycle - last_duty_cycle) > max_duty_cycle_change)
                {
                    duty_cycle = last_duty_cycle + max_duty_cycle_change;
                    if (commutation_interval > 500)
                    {
                        fast_accel = 1;
                    }
                    else
                    {
                        fast_accel = 0;
                    }
                }
                else if ((last_duty_cycle - duty_cycle) > max_duty_cycle_change)
                {
                    duty_cycle = last_duty_cycle - max_duty_cycle_change;
                    fast_accel = 0;
                }
                else
                {
                    fast_accel = 0;
                }
            }
        }
        if ((armed && running) && input > 47)
        {
            if (VARIABLE_PWM)
            {
                tim1_arr = map(commutation_interval, 96, 200, TIMER1_MAX_ARR / 2, TIMER1_MAX_ARR);
            }
            adjusted_duty_cycle = ((duty_cycle * tim1_arr) / TIMER1_MAX_ARR) + 1;
        }
        else
        {
            if (prop_brake_active)
            {
                adjusted_duty_cycle = TIMER1_MAX_ARR - ((duty_cycle * tim1_arr) / TIMER1_MAX_ARR) + 1;
            }
            else
            {
                adjusted_duty_cycle = DEAD_TIME * running;
            }
        }
        last_duty_cycle = duty_cycle;
        TIM1->ARR = tim1_arr;

        TIM1->CCR1 = adjusted_duty_cycle;
        TIM1->CCR2 = adjusted_duty_cycle;
        TIM1->CCR3 = adjusted_duty_cycle;
    }
    average_interval = e_com_time / 3;
    if (desync_check && zero_crosses > 10)
    {
        //	if(average_interval < last_average_interval){
        //
        //	}
        if ((getAbsDif(last_average_interval, average_interval) > average_interval >> 1) && (average_interval < 2000))
        { // throttle resitricted before zc 20.
            zero_crosses = 0;
            desync_happened++;
            running = 0;
            old_routine = 1;
            if (zero_crosses > 100)
            {
                average_interval = 5000;
            }
            last_duty_cycle = min_startup_duty / 2;
        }
        desync_check = 0;
        //	}
        last_average_interval = average_interval;
    }
#ifndef MCU_F031
    if (commutation_interval > 400)
    {
        NVIC_SetPriority(IC_DMA_IRQ_NAME, 0);
        NVIC_SetPriority(ADC1_COMP_IRQn, 1);
    }
    else
    {
        NVIC_SetPriority(IC_DMA_IRQ_NAME, 1);
        NVIC_SetPriority(ADC1_COMP_IRQn, 0);
    }
#endif // mcu f031

#endif // ndef brushed_mode

    if (send_telemetry)
    {
#ifdef USE_SERIAL_TELEMETRY
        makeTelemPackage(degrees_celsius,
                         battery_voltage,
                         actual_current,
                         (uint16_t)consumed_current,
                         e_rpm);
        send_telem_DMA();
        send_telemetry = 0;
#endif
    }
#if defined(FIXED_DUTY_MODE) || defined(FIXED_SPEED_MODE)

    if (INPUT_PIN_PORT->IDR & INPUT_PIN)
    {
        signaltimeout++;
        if (signaltimeout > 10000)
        {
            NVIC_SystemReset();
        }
    }
    else
    {
        signaltimeout = 0;
    }
#else
    signaltimeout++;
    if (signaltimeout > 5000)
    { // half second timeout when armed;
        if (armed)
        {
            allOff();
            armed = 0;
            input = 0;
            inputSet = 0;
            zero_input_count = 0;
            TIM1->CCR1 = 0;
            TIM1->CCR2 = 0;
            TIM1->CCR3 = 0;
            IC_TIMER_REGISTER->PSC = 0;
            IC_TIMER_REGISTER->CNT = 0;
            for (int i = 0; i < 64; i++)
            {
                dma_buffer[i] = 0;
            }
            NVIC_SystemReset();
        }

        if (signaltimeout > 25000)
        { // 2.5 second
            allOff();
            armed = 0;
            input = 0;
            inputSet = 0;
            zero_input_count = 0;
            TIM1->CCR1 = 0;
            TIM1->CCR2 = 0;
            TIM1->CCR3 = 0;
            IC_TIMER_REGISTER->PSC = 0;
            IC_TIMER_REGISTER->CNT = 0;
            for (int i = 0; i < 64; i++)
            {
                dma_buffer[i] = 0;
            }
            NVIC_SystemReset();
        }
    }
#endif
}

void advanceincrement()
{
    if (!forward)
    {
        phase_A_position++;
        if (phase_A_position > 359)
        {
            phase_A_position = 0;
        }
        phase_B_position++;
        if (phase_B_position > 359)
        {
            phase_B_position = 0;
        }
        phase_C_position++;
        if (phase_C_position > 359)
        {
            phase_C_position = 0;
        }
    }
    else
    {
        phase_A_position--;
        if (phase_A_position < 0)
        {
            phase_A_position = 359;
        }
        phase_B_position--;
        if (phase_B_position < 0)
        {
            phase_B_position = 359;
        }
        phase_C_position--;
        if (phase_C_position < 0)
        {
            phase_C_position = 359;
        }
    }
#ifdef GIMBAL_MODE
    TIM1->CCR1 = ((2 * pwmSin[phase_A_position]) + gate_drive_offset) * TIMER1_MAX_ARR / 2000;
    TIM1->CCR2 = ((2 * pwmSin[phase_B_position]) + gate_drive_offset) * TIMER1_MAX_ARR / 2000;
    TIM1->CCR3 = ((2 * pwmSin[phase_C_position]) + gate_drive_offset) * TIMER1_MAX_ARR / 2000;
#else
    TIM1->CCR1 = (((2 * pwmSin[phase_A_position] / SINE_DIVIDER) + gate_drive_offset) * TIMER1_MAX_ARR / 2000) * sine_mode_power / 10;
    TIM1->CCR2 = (((2 * pwmSin[phase_B_position] / SINE_DIVIDER) + gate_drive_offset) * TIMER1_MAX_ARR / 2000) * sine_mode_power / 10;
    TIM1->CCR3 = (((2 * pwmSin[phase_C_position] / SINE_DIVIDER) + gate_drive_offset) * TIMER1_MAX_ARR / 2000) * sine_mode_power / 10;
#endif
}

void zcfoundroutine()
{ // only used in polling mode, blocking routine.
    thiszctime = INTERVAL_TIMER->CNT;
    INTERVAL_TIMER->CNT = 0;
    commutation_interval = (thiszctime + (3 * commutation_interval)) / 4;
    advance = commutation_interval / advancedivisor;
    waitTime = commutation_interval / 2 - advance;
    while (INTERVAL_TIMER->CNT < waitTime)
    {
        if (zero_crosses < 10)
        {
            break;
        }
    }
    commutate();
    bemfcounter = 0;
    bad_count = 0;

    zero_crosses++;
    if (stall_protection || RC_CAR_REVERSE)
    {
        if (zero_crosses >= 20 && commutation_interval <= 2000)
        {
            old_routine = 0;
            enableCompInterrupts(); // enable interrupt
        }
    }
    else
    {
        if (zero_crosses > 30)
        {
            old_routine = 0;
            enableCompInterrupts(); // enable interrupt
        }
    }
}

#ifdef BRUSHED_MODE
void runBrushedLoop()
{

    uint16_t brushed_duty_cycle = 0;

    if (brushed_direction_set == 0 && adjusted_input > 48)
    {
        if (forward)
        {
            allOff();
            delayMicros(10);
            twoChannelForward();
        }
        else
        {
            allOff();
            delayMicros(10);
            twoChannelReverse();
        }
        brushed_direction_set = 1;
    }

    brushed_duty_cycle = map(adjusted_input, 48, 2047, 0, TIMER1_MAX_ARR - 50);

    if (degrees_celsius > TEMPERATURE_LIMIT)
    {
        duty_cycle_maximum = map(degrees_celsius, TEMPERATURE_LIMIT, TEMPERATURE_LIMIT + 20, TIMER1_MAX_ARR / 2, 1);
    }
    else
    {
        duty_cycle_maximum = TIMER1_MAX_ARR - 50;
    }
    if (brushed_duty_cycle > duty_cycle_maximum)
    {
        brushed_duty_cycle = duty_cycle_maximum;
    }

    if (use_current_limit)
    {
        use_current_limit_adjust -= (int16_t)(doPidCalculations(&currentPid, actual_current, CURRENT_LIMIT * 100) / 10000);
        if (use_current_limit_adjust < minimum_duty_cycle)
        {
            use_current_limit_adjust = minimum_duty_cycle;
        }

        if (brushed_duty_cycle > use_current_limit_adjust)
        {
            brushed_duty_cycle = use_current_limit_adjust;
        }
    }
    if ((brushed_duty_cycle > 0) && armed)
    {
        TIM1->CCR1 = brushed_duty_cycle;
        TIM1->CCR2 = brushed_duty_cycle;
        TIM1->CCR3 = brushed_duty_cycle;
    }
    else
    {
        TIM1->CCR1 = 0; //
        TIM1->CCR2 = 0;
        TIM1->CCR3 = 0;
        brushed_direction_set = 0;
    }
}
#endif

int main(void)
{
    // 从bootloader跳过来，重新设置中断向量表
    initAfterJump(); //

    initCorePeripherals(); // 外设初始化

    /* 打开 TIM1 通道 的输出开关
    让 各个Channel 开始输出 PWM 波形。*/
    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1);
    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH2);
    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH3);
    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1N);
    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH2N);
    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH3N);

    // 消隐定时器
    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH5);

    //  LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH4);      // timer used for timing adc read
    //  TIM1->CCR4 = 100;  // set in 10khz loop to match pwm cycle timed to end of pwm on

    /* 	启动定时器计数
    这个执行的先后顺序和MCU有关，最终目的是让引脚上输出 PWM */
    LL_TIM_EnableCounter(TIM1);
    LL_TIM_EnableAllOutputs(TIM1);

    /* 手动刷新一次 TIM1 的所有预装载值，并让计数器从 0 重新开始计数，确保 PWM 初始化后的状态正确、同步。 */
    LL_TIM_GenerateEvent_UPDATE(TIM1);
    // LL_TIM_EnableIT_UPDATE(TIM1);

    // 飞控油门信号接收，PWM或者数字编码信号，具体取决于使用的协议格式
    LL_TIM_CC_EnableChannel(IC_TIMER_REGISTER, IC_TIMER_CHANNEL);
    LL_TIM_EnableCounter(IC_TIMER_REGISTER);

#ifdef USE_LED_STRIP
    send_LED_RGB(255, 0, 0);
#endif

#ifdef USE_RGB_LED
    LED_GPIO_init();
    GPIOB->BRR = LL_GPIO_PIN_8; // turn on red
    GPIOB->BSRR = LL_GPIO_PIN_5;
    GPIOB->BSRR = LL_GPIO_PIN_3; //
#endif

#ifndef BRUSHED_MODE
    // 计算换向延时用
    LL_TIM_EnableCounter(COM_TIMER);
    LL_TIM_GenerateEvent_UPDATE(COM_TIMER);
    LL_TIM_EnableIT_UPDATE(COM_TIMER);
    COM_TIMER->DIER &= ~((0x1UL << (0U))); // 先关闭更新中断。检测到过零时打开，过零一段时间后再换向
#endif

    LL_TIM_EnableCounter(UTILITY_TIMER);
    LL_TIM_GenerateEvent_UPDATE(UTILITY_TIMER);

    LL_TIM_EnableCounter(INTERVAL_TIMER);
    LL_TIM_GenerateEvent_UPDATE(INTERVAL_TIMER);

    LL_TIM_EnableCounter(TEN_KHZ_TIMER); // 10khz timer
    LL_TIM_GenerateEvent_UPDATE(TEN_KHZ_TIMER);
    TEN_KHZ_TIMER->DIER |= (0x1UL << (0U)); // 启用中断

#ifdef USE_ADC
    ADC_Init();      // 初始化ADC，采集电流、电压、芯片温度
    enableADC_DMA(); // 激活DMA传输ADC数据
    activateADC();   // 激活ADC
#endif

#ifndef MCU_F031
    __IO uint32_t wait_loop_index = 0;
    /* Enable comparator */
    LL_COMP_Enable(MAIN_COMP);

#ifdef N_VARIANT // needs comp 1 and 2
    LL_COMP_Enable(COMP1);
#endif

    // 比较器内部有一个 voltage scaler（电压缩放器），用于设置比较阈值。这个电路从初始化到输出稳定需要一定时间。
    // 如果不等这段时间就读取比较器输出：比较结果可能不稳定、可能产生虚假过零信号，导致换相错误
    wait_loop_index = ((LL_COMP_DELAY_STARTUP_US * (SystemCoreClock / (100000 * 2))) / 10);
    while (wait_loop_index != 0)
    {
        wait_loop_index--;
    }

#endif

    /* 加载参数 */
    loadEEpromSettings();
    //  EEPROM_VERSION = *(uint8_t*)(0x08000FFC);
    if (firmware_info.version_major != eepromBuffer[3] || firmware_info.version_minor != eepromBuffer[4])
    {
        // 如果固件有更新，重置EEPROM中的数据，避免因为数据结构变化导致的错误。
        eepromBuffer[3] = firmware_info.version_major;
        eepromBuffer[4] = firmware_info.version_minor;
        for (int i = 0; i < 12; i++)
        {
            eepromBuffer[5 + i] = firmware_info.device_name[i];
        }
        saveEEpromSettings();
    }

    if (use_sin_start) // 如果使用弦波启动
    {
        min_startup_duty = sin_mode_min_s_d; // 最小启动占空比和六步换相不一样
    }

    /* 是否反转
    针对接线接反的情况，只需要修改一下该标志位即可，不用重新焊接 */
    if (dir_reversed == 1)
    {
        forward = 0;
    }
    else
    {
        forward = 1;
    }

    tim1_arr = TIMER1_MAX_ARR;

    // 数据转换，把启动时最大启动占空比，转换为适配当前TIMER的CCR值
    // 例如最大占空比是30%，TIMER1的ARR数值是1500，那么CCR的数值应该是450，才能得到30%的占空比。
    // 又因为有死区，所以额外加上死区补偿值，确保实际输出的占空比能达到预期的30%
    startup_max_duty_cycle = startup_max_duty_cycle * TIMER1_MAX_ARR / 2000 + dead_time_override; // adjust for pwm frequency

    // 低转速时允许的最大占空比换算
    throttle_max_at_low_rpm = throttle_max_at_low_rpm * TIMER1_MAX_ARR / 2000; // adjust to new pwm frequency
    // 高转速时允许的最大占空比换算
    throttle_max_at_high_rpm = TIMER1_MAX_ARR; // adjust to new pwm frequency

    if (!comp_pwm)
    {
        // 如果硬件没有启用互补PWM，就强制关闭正弦启动模式。
        use_sin_start = 0;
    }

    /* 针对车模模式进行参数修改
     车模和多旋翼的负载特性不一样 */
    if (RC_CAR_REVERSE)
    { // overrides a whole lot of things!
        throttle_max_at_low_rpm = 1000;
        bi_direction = 1;
        use_sin_start = 0;
        low_rpm_throttle_limit = 1;
        VARIABLE_PWM = 0;
        // stall_protection = 1;
        comp_pwm = 0;
        stuck_rotor_protection = 0;
        minimum_duty_cycle = minimum_duty_cycle + 50;
        stall_protect_minimum_duty = stall_protect_minimum_duty + 50;
        min_startup_duty = min_startup_duty + 50;
    }

#ifdef USE_CRSF_INPUT
    inputSet = 1;
    playStartupTune();
    MX_IWDG_Init();
    LL_IWDG_ReloadCounter(IWDG);
#else

#if defined(FIXED_DUTY_MODE) || defined(FIXED_SPEED_MODE)
    MX_IWDG_Init();
    LL_IWDG_ReloadCounter(IWDG);
    inputSet = 1;
    armed = 1;
    adjusted_input = 48;
    newinput = 48;
    advance_level = 3;
#ifdef FIXED_SPEED_MODE
    use_speed_control_loop = 1;
    use_sin_start = 0;
    target_e_com_time = 60000000 / FIXED_SPEED_MODE_RPM / (motor_poles / 2);
    input = 48;
#endif

#else
#ifdef BRUSHED_MODE
    // bi_direction = 1;
    commutation_interval = 5000;
    use_sin_start = 0;
    maskPhaseInterrupts();
    playBrushedStartupTune();
#else
    playStartupTune(); /* 让电机发出启动音乐 */
#endif

    zero_input_count = 0;

    MX_IWDG_Init();
    LL_IWDG_ReloadCounter(IWDG);

#ifdef GIMBAL_MODE
    bi_direction = 1;
    use_sin_start = 1;
#endif

#ifdef USE_ADC_INPUT
    armed_count_threshold = 5000;
    inputSet = 1;
#else
    /* 可能是外部刷写工具要更新固件
    可能是飞控未启动
    可能是信号线接错 */
    checkForHighSignal(); // will reboot if signal line is high for 10ms

    receiveDshotDma(); // 接收飞控发过来的命令

    if (drive_by_rpm)
    {
        // 如果是通过转速来控制的，那么就启用速度控制环，直接把输入当成转速目标，PID调节占空比来达到目标转速
        use_speed_control_loop = 1;
    }
#endif

#endif // end fixed duty mode ifdef
#endif // end crsf input

    while (1)
    {
        LL_IWDG_ReloadCounter(IWDG); // 喂狗

        adc_counter++;
        if (adc_counter > 10) // 降低频率
        {
            // timer1的channel4没启用，所以这一段目前没用
            // 预期作用：把ADC采样的时刻和PWM输出的时刻同步，确保在电流较为稳定的时候去采集数据
            ADC_CCR = TIM1->CCR3 * 2 / 3 + 1;
            if (ADC_CCR > tim1_arr)
            {
                ADC_CCR = tim1_arr;
            }
            TIM1->CCR4 = ADC_CCR;

            // 获取温度值，转换成摄氏度，滤波
            ADC_raw_temp = ADC_raw_temp - (temperature_offset);
            converted_degrees = __LL_ADC_CALC_TEMPERATURE(3300, ADC_raw_temp, LL_ADC_RESOLUTION_12B);
            degrees_celsius = ((7 * degrees_celsius) + converted_degrees) >> 3;

            // 获取电压值，转换成实际电压，滤波
            battery_voltage = ((7 * battery_voltage) + ((ADC_raw_volts * 3300 / 4095 * VOLTAGE_DIVIDER) / 100)) >> 3;
            // smoothed = (63 × 旧值 + 1 × 新值) / 64
            // 电流采样噪声大，所以用很重的平滑，让电流值变化更平稳，避免保护电路误触发。
            smoothed_raw_current = ((63 * smoothed_raw_current + (ADC_raw_current)) >> 6);
            /*12 位 ADC、3.3 V 参考电压，理论上是 raw * 3300 / 4095（单位 mV）。
            这里用 / 41 近似 / 4095 * 100，相当于把结果放大 100 倍，变成 0.01 mV（百分之一毫伏） 的固定点精度，避免浮点运算。
            MILLIVOLT_PER_AMP： 表示电流传感器每安培输出多少毫伏（mV/A），即运放的放大倍数 */
            actual_current = ((smoothed_raw_current * 3300 / 41) - (CURRENT_OFFSET * 100)) / (MILLIVOLT_PER_AMP);
            if (actual_current < 0)
            {
                actual_current = 0;
            }

            // 重新开始ADC采样
            LL_ADC_REG_StartConversion(ADC1);

            // 低压截止保护 功能：当电池电压过低时，自动关闭电机，防止电池过放损坏。
            // 用户可配置参数
            if (LOW_VOLTAGE_CUTOFF)
            {
                // 电池电压低于每节电池的截止电压乘以电池数量时，增加低电压计数器。
                if (battery_voltage < (cell_count * low_cell_volt_cutoff))
                {
                    /*如果电池持续低压，且持续的时间超过了设定阈值（正弦启动时阈值更低、反应更快），就立即停机保护电池。
                    正弦启动时电机电流大、电池负载重，电压跌落更明显，也更容易把电池电压拉得过低。所以在这段时间里，
                    低压保护稍微敏感一点，提前约 900 个计数周期动作，防止电池在启动大电流冲击下被过放。*/
                    low_voltage_count++;
                    if (low_voltage_count > (20000 - (stepper_sine * 900)))
                    {
                        input = 0;
                        allOff();
                        maskPhaseInterrupts();
                        running = 0;
                        zero_input_count = 0;
                        armed = 0;
                    }
                }
                else
                {
                    low_voltage_count = 0;
                }
            }
            adc_counter = 0;

#ifdef USE_ADC_INPUT
            if (ADC_raw_input < 10)
            {
                zero_input_count++;
            }
            else
            {
                zero_input_count = 0;
            }
#endif
        }

#ifdef USE_ADC_INPUT
        signaltimeout = 0;
        ADC_smoothed_input = (((10 * ADC_smoothed_input) + ADC_raw_input) / 11);
        newinput = ADC_smoothed_input / 2;
        if (newinput > 2000)
        {
            newinput = 2000;
        }
#endif

        stuckcounter = 0;

        // 双向旋转模式，非DSHOT模式（DSHOT是数据命令接收模式）
        if (bi_direction == 1 && dshot == 0)
        {
            if (RC_CAR_REVERSE) // 给攀爬车准备的
            {
                // 命令为正转
                if (newinput > (1000 + (servo_dead_band << 1)))
                {
                    // 电机方向和输入方向相反
                    if (forward == dir_reversed)
                    {
                        adjusted_input = 0; // 先不给电机驱动力
                        if (running)        // 如果电机正在转
                        {
                            prop_brake_active = 1; // 开启刹车，把电机刹停
                        }
                        else
                        {
                            // 如果电机本来就停了，直接切换方向标志为正转
                            forward = 1 - dir_reversed;
                        }
                    }

                    // 如果当前没有在刹车，就把 PWM 输入映射成内部油门值
                    if (prop_brake_active == 0)
                    {
                        adjusted_input = map(newinput, 1000 + (servo_dead_band << 1), 2000, 47, 2047);
                    }
                }

                // 命令为反转，其余逻辑和正转类似
                if (newinput < (1000 - (servo_dead_band << 1)))
                {
                    if (forward == (1 - dir_reversed))
                    {
                        if (running)
                        {
                            prop_brake_active = 1;
                        }
                        else
                        {
                            forward = dir_reversed;
                        }
                        adjusted_input = 0;
                    }
                    if (prop_brake_active == 0)
                    {
                        adjusted_input = map(newinput, 0, 1000 - (servo_dead_band << 1), 2047, 47);
                    }
                }

                // 是否在死区，如果在死区就不给电机驱动力，顺便把刹车状态也关了。
                if (newinput >= (1000 - (servo_dead_band << 1)) && newinput <= (1000 + (servo_dead_band << 1)))
                {
                    adjusted_input = 0;
                    prop_brake_active = 0;
                }
            }
            else // 不是攀爬车，普通双向控制
            {
                // 命令为正向旋转
                if (newinput > (1000 + (servo_dead_band << 1)))
                {
                    if (forward == dir_reversed) // 电机当前方向和命令相反
                    {
                        /*commutation_interval：两次换相之间的时间间隔，单位是微秒或定时器 tick。
                                                这个值越大，表示电机转得越慢。
                        reverse_speed_threshold：允许反向的转速阈值。
                                                当 commutation_interval > reverse_speed_threshold 时，说明电机已经转得够慢了，可以安全换向。
                        stepper_sine：如果当前还在正弦启动阶段，也允许换向。*/
                        if ((commutation_interval > reverse_speed_threshold) || stepper_sine)
                        {
                            forward = 1 - dir_reversed; // 把方向标志切到正转
                            zero_crosses = 0;           // 清零过零计数，重新检测 BEMF

                            /* 回到开环/旧检测模式
                             屏蔽换相中断，改为在主循环里检测过零，这样可以在换向时有更大的容错，避免因为过零检测不准导致的电机抖动或熄火。
                             换相节奏更保守，适合启动、失步恢复、方向切换等不稳定状态。 */
                            old_routine = 1;
                            maskPhaseInterrupts();     // 屏蔽换相中断
                            brushed_direction_set = 0; // 有刷模式方向标志复位
                        }
                        else
                        {
                            /*如果电机反转还很快，现在强行切正转会冲击电调和电机。
                            所以把油门强制置为中位（1000），相当于让电机自由滑行减速，等转速降下来 */
                            newinput = 1000;
                            newinput = 1000;
                        }
                    }

                    // 获取调整后的油门数值
                    adjusted_input = map(newinput, 1000 + (servo_dead_band << 1), 2000, 47, 2047);
                }

                // 命令为反向，其它和正向测代码一致
                if (newinput < (1000 - (servo_dead_band << 1)))
                {
                    if (forward == (1 - dir_reversed))
                    {
                        if ((commutation_interval > reverse_speed_threshold) || stepper_sine)
                        {
                            zero_crosses = 0;
                            old_routine = 1;
                            forward = dir_reversed;
                            maskPhaseInterrupts();
                            brushed_direction_set = 0;
                        }
                        else
                        {
                            newinput = 1000;
                        }
                    }
                    adjusted_input = map(newinput, 0, 1000 - (servo_dead_band << 1), 2047, 47);
                }

                if (newinput >= (1000 - (servo_dead_band << 1)) && newinput <= (1000 + (servo_dead_band << 1)))
                {
                    adjusted_input = 0;
                    brushed_direction_set = 0;
                }
            }
        }
        else if (dshot && bi_direction) // 双向旋转 + DSHOT（数字协议）模式，除了命令值范围不一样，逻辑和上面类似
        {
            if (newinput > 1047) // 正转
            {

                if (forward == dir_reversed)
                {
                    if (commutation_interval > reverse_speed_threshold || stepper_sine)
                    {
                        forward = 1 - dir_reversed;
                        zero_crosses = 0;
                        old_routine = 1;
                        maskPhaseInterrupts();
                        brushed_direction_set = 0;
                    }
                    else
                    {
                        newinput = 0;
                    }
                }
                adjusted_input = ((newinput - 1048) * 2 + 47) - reversing_dead_band;
            }
            if (newinput <= 1047 && newinput > 47) // 反转
            {
                //	startcount++;

                if (forward == (1 - dir_reversed))
                {
                    if (commutation_interval > reverse_speed_threshold || stepper_sine)
                    {
                        zero_crosses = 0;
                        old_routine = 1;
                        forward = dir_reversed;
                        maskPhaseInterrupts();
                        brushed_direction_set = 0;
                    }
                    else
                    {
                        newinput = 0;
                    }
                }
                adjusted_input = ((newinput - 48) * 2 + 47) - reversing_dead_band;
            }

            if (newinput < 48) // 停转
            {
                adjusted_input = 0;
                brushed_direction_set = 0;
            }
        }
        else // 其它模式，直接把输入映射成油门值
        {
            adjusted_input = newinput;
        }

#ifndef BRUSHED_MODE
        if ((zero_crosses > 1000) || (adjusted_input == 0))
        {
            /* 电机已经成功检测了很多次过零点，说明运行稳定。之前累计的超时计数可能是偶发噪声，清零 */
            /* 油门为 0，电机本来就在停转或自由滑行，没有 BEMF 是正常的，不算故障 */
            bemf_timeout_happened = 0;

#ifdef USE_RGB_LED
            if (adjusted_input == 0 && armed)
            {
                GPIOB->BSRR = LL_GPIO_PIN_8; // off red
                GPIOB->BRR = LL_GPIO_PIN_5;  // on green
                GPIOB->BSRR = LL_GPIO_PIN_3; // off blue
            }
#endif
        }
        if (zero_crosses > 100 && adjusted_input < 200)
        {
            /* 经成功检测了一些过零点（zero_crosses > 100），但油门很低（adjusted_input < 200）
               低转速时 BEMF 信号弱，容易误报超时，所以清零。 */
            bemf_timeout_happened = 0;
        }
        if (use_sin_start && adjusted_input < 160)
        {
            /* 正在使用正弦启动（开环拖动），此时根本不走 BEMF 检测流程。
               低油门下更不需要判断 BEMF 超时 */
            bemf_timeout_happened = 0;
        }

        if (crawler_mode) // 车模模式
        {
            if (adjusted_input < 400)
            {
                /* 爬行模式下车速极慢，电机转速很低，BEMF 信号非常弱甚至断断续续。
                   这种情况下很容易发生 BEMF 超时，但其实电机并没有卡死，只是在慢慢爬。
                   所以低油门时（adjusted_input < 400），直接把超时计数器清零，不触发任何保护。 */
                bemf_timeout_happened = 0;
            }
        }
        else
        {
            /* 这里调整的是超时阈值 */
            if (adjusted_input < 150)
            {
                /* 启动时占空比很低，电机慢慢加速，BEMF 弱是正常的。把阈值放宽到 100，避免启动阶段就误触发保护。 */
                bemf_timeout = 100;
            }
            else
            {
                bemf_timeout = 10; // 正常油门时严格
            }
        }

        /* 当连续检测不到反电动势过零点的时间超过阈值 && 开启了“转子卡死保护”功能 */
        if (bemf_timeout_happened > bemf_timeout * (1 + (crawler_mode * 100)) && stuck_rotor_protection)
        {
            allOff();                    // 关闭所有 MOSFET，停止驱动电机
            maskPhaseInterrupts();       // 屏蔽比较器中断
            input = 0;                   // 内部油门清零
            bemf_timeout_happened = 102; // 把计数固定在一个很大的值，确保一定会触发保护逻辑

#ifdef USE_RGB_LED
            GPIOB->BRR = LL_GPIO_PIN_8;  // on red
            GPIOB->BSRR = LL_GPIO_PIN_5; //
            GPIOB->BSRR = LL_GPIO_PIN_3;
#endif
        }
        else // 调整参数，正常驱动电机
        {
#ifdef FIXED_DUTY_MODE
            input = FIXED_DUTY_MODE_POWER * 20 + 47;
#else
            if (use_sin_start) // 正弦启动
            {
                if (adjusted_input < 30)
                {
                    // 死区，不输出油门
                    input = 0;
                }

                // 正弦启动阶段，把油门输入映射成一个较低的范围，给电机一个温和的启动过程，保护电机和电调，适合需要平稳启动的应用，例如车模、机械臂等。
                if (adjusted_input > 30 && adjusted_input < (sine_mode_changeover_thottle_level * 20))
                {
                    input = map(adjusted_input, 30, (sine_mode_changeover_thottle_level * 20), 47, 160);
                }

                // 正常换相区
                if (adjusted_input >= (sine_mode_changeover_thottle_level * 20))
                {
                    input = map(adjusted_input, (sine_mode_changeover_thottle_level * 20), 2047, 160, 2047);
                }
            }
            else
            {
                /* 速度控制环总开关。
                   当它开启时，ESC 不再直接把 adjusted_input 当油门
                   而是用 PID 计算一个油门修正量 input_override，再用它驱动电机 */
                if (use_speed_control_loop)
                {
                    /* “按转速驱动”模式，也就是把油门输入映射成目标电机转速，ESC 自动用 PID 调节占空比来维持这个转速
                       适合需要精确控制转速的应用，例如风扇、水泵等 */
                    if (drive_by_rpm)
                    {
                        // 计算电气换相周期
                        target_e_com_time = 60000000 / map(adjusted_input, 47, 2047, MINIMUM_RPM_SPEED_CONTROL, MAXIMUM_RPM_SPEED_CONTROL) / (motor_poles / 2);

                        // 死区
                        if (adjusted_input < 47)
                        {
                            input = 0;
                            speedPid.error = 0;
                            input_override = 0;
                        }
                        else
                        {
                            // 在10KHz中断里计算，这里直接获取PID输出的占空比覆盖掉原来的油门输入
                            input = (uint16_t)input_override; // speed control pid override
                            if (input_override > 2047)
                            {
                                input = 2047;
                            }
                            if (input_override < 48)
                            {
                                input = 48;
                            }
                        }
                    }
                    else
                    {
                        input = (uint16_t)input_override; // speed control pid override
                        if (input_override > 2047)
                        {
                            input = 2047;
                        }
                        if (input_override < 48)
                        {
                            input = 48;
                        }
                    }
                }
                else
                {
                    // 普通模式，直接把调整后的油门输入给电机
                    input = adjusted_input;
                }
            }
#endif
        }

        if (stepper_sine == 0) // 正常的六步换向模式
        {
            // 6 次换相 = 360° 电角度 = 1 个电气周期
            // e_com_time： 一个电气周期的时间，单位 µs
            // 电气转速：每分钟完成多少个电气周期
            // e_rpm： 电气转速的 1/100，即 "百eRPM"
            // k_erpm： 电气转速的 1/1000，即 "千eRPM"
            e_rpm = running * (600000 / e_com_time); // = (60,000,000 / e_com_time) / 100
            k_erpm = e_rpm / 10;                     // 转换成小数据，节省空间、方便比较、避免大数运算

            if (low_rpm_throttle_limit)
            {
                // some hardware doesn't need this, its on by default to keep hardware / motors protected but can slow down the response in the very low end a little.

                // 低转速时，限制最大油门，用于保护电调和电机
                // 电机转速低的时候，反电动势很小，绕组阻抗也低。
                // 如果这时给 full throttle（大占空比）
                // 电流会非常大，容易：烧毁 MOS、退磁电机、触发过流保护、损坏电池
                duty_cycle_maximum = map(k_erpm, low_rpm_level, high_rpm_level, throttle_max_at_low_rpm, throttle_max_at_high_rpm); // for more performance lower the high_rpm_level, set to a consvervative number in source.
            }

            /* 温度过高时，限制最大油门，用于保护电调和电机 */
            if (degrees_celsius > TEMPERATURE_LIMIT)
            {
                duty_cycle_maximum = map(degrees_celsius, TEMPERATURE_LIMIT - 10, TEMPERATURE_LIMIT + 10, throttle_max_at_high_rpm / 2, 1);
            }

            // zero_crosses < 100：刚启动，过零次数还很少
            // commutation_interval > 500：换相周期大于 500，说明转速很低
            // 根据电机转速，动态调整 比较器消隐时间 和 BEMF 过零检测滤波等级。
            if (zero_crosses < 100 || commutation_interval > 500)
            {
#ifdef MCU_G071
                TIM1->CCR5 = 100; // 消隐时间长，避开开关噪声
#endif
                filter_level = 12; // 重滤波，防止误判
            }
            else
            {
#ifdef MCU_G071
                TIM1->CCR5 = 5; // 消隐时间很短，因为高速时 BEMF 强，不需要屏蔽太久
#endif
                filter_level = map(average_interval, 100, 500, 3, 10); // 转速越快，滤波等级越低，响应更快；转速越慢，滤波等级越高，稳定性更好
            }
            if (commutation_interval < 100)
            {
                // 换向间隔小于100us，说明转速已经很高了，BEMF 信号很强，几乎不受噪声干扰了，可以把滤波等级调到最低，获得最快的响应。
                filter_level = 2;
            }

            if (motor_kv < 500)
            {
                // 针对低 KV 电机的额外滤波加强
                filter_level = filter_level * 2;
            }

            /**************** old routine*********************/
            if (old_routine && running) // 手动查询过零点 && 电机正在转
            {
                maskPhaseInterrupts(); // 关闭比较器中断
                getBemfState();        // 主动读取比较器输出电平
                if (!zcfound)          // 如果还没找到过零点
                {
                    if (rising) // 期望上升沿过零
                    {
                        // bemfcounter：连续检测到预期状态的次数
                        if (bemfcounter > min_bemf_counts_up)
                        {
                            zcfound = 1; // 标记：找到过零点

                            zcfoundroutine(); // 找到过零，执行换相
                        }
                    }
                    else
                    {
                        if (bemfcounter > min_bemf_counts_down)
                        {
                            zcfound = 1;
                            zcfoundroutine();
                        }
                    }
                }
            }

            // BEMF 超时处理，也就是 ESC 检测不到反电动势过零点时的自救逻辑。
            // INTERVAL_TIMER->CNT：从上一次过零/换相到现在经过的时间
            // >45000：太长时间没有检测到过零点
            // running == 1：电机本来应该在转
            if (INTERVAL_TIMER->CNT > 45000 && running == 1)
            {
                // 超时计数器 +1。这个值累积多了会触发前面说过的“转子卡死保护”
                bemf_timeout_happened++;

                // 关闭比较器中断。因为当前中断已经不可靠，继续开着可能误触发。
                maskPhaseInterrupts();

                // 切换到保守的 old_routine 模式，用轮询方式重新检测 BEMF
                old_routine = 1;

                // 如果油门已经很小（<48），说明用户松油门了，直接把电机判为不运行。
                if (input < 48)
                {
                    running = 0;
                }

                // 清零过零计数，重新积累
                zero_crosses = 0;

                // 强制推进一次换相。虽然没有真正检测到过零点，但 ESC 会按旧的换相时序“赌一把”，尝试让电机重新同步
                zcfoundroutine();
                // if(stall_protection){
                // min_startup_duty = 130;
                // minimum_duty_cycle = minimum_duty_cycle + 10;
                // if(minimum_duty_cycle > 80){
                // minimum_duty_cycle = 80;
                // }
                // }
            }
        }
        else // 开环正弦启动模式
        {    // stepper sine

#ifdef GIMBAL_MODE
            step_delay = 300;
            maskPhaseInterrupts();
            allpwm();
            if (newinput > 1000)
            {
                desired_angle = map(newinput, 1000, 2000, 180, 360);
            }
            else
            {
                desired_angle = map(newinput, 0, 1000, 0, 180);
            }
            if (current_angle > desired_angle)
            {
                forward = 1;
                advanceincrement();
                delayMicros(step_delay);
                current_angle--;
            }
            if (current_angle < desired_angle)
            {
                forward = 0;
                advanceincrement();
                delayMicros(step_delay);
                current_angle++;
            }
#else

            if (input > 48 && armed) // 油门大于最小启动值 && ESC 已经解锁
            {
                if (input > 48 && input < 137) // 开环正弦启动，转速由油门决定，不依赖 BEMF
                {
                    if (do_once_sinemode)
                    {
                        COM_TIMER->DIER &= ~((0x1UL << (0U))); // 关闭换相中断
                        maskPhaseInterrupts();                 // 关闭比较器中断

                        // 让三相 PWM 占空比先归零，给正弦启动一个干净的初始状态，避免模式切换时产生电流冲击
                        TIM1->CCR1 = 0;
                        TIM1->CCR2 = 0;
                        TIM1->CCR3 = 0;

                        allpwm(); // 进入三相正弦 PWM 模式

                        do_once_sinemode = 0; // 确保同一启动过程中不再重复初始化
                    }

                    advanceincrement();                                                      // 推进一歩正弦相位
                    step_delay = map(input, 48, 120, 7000 / motor_poles, 810 / motor_poles); // 随油门从 48→120 线性减小：转速逐渐加
                    delayMicros(step_delay);                                                 // 延时等待
                    e_rpm = 600 / step_delay;                                                // 估算电气转速 e_rpm
                }
                else
                {
                    do_once_sinemode = 1; // 下次再跌回正弦区时重新初始化
                    advanceincrement();   // 继续推进正弦相位
                    if (input > 200)
                    {
                        // 如果油门很大（>200）
                        // 强制把 A 相相位归零并把步进延时降到 80，准备立即切出正弦模式
                        phase_A_position = 0;
                        step_delay = 80;
                    }

                    delayMicros(step_delay);
                    if (phase_A_position == 0) // 当 A 相相位回到 0 时，认为正弦波完成了一个完整周期，可以干净地切换到六步换相
                    {
                        stepper_sine = 0; // 退出正弦模式
                        running = 1;      // 进入正常运行
                        old_routine = 1;  // 使用旧版换相例程
                        commutation_interval = 9000;
                        average_interval = 9000;
                        last_average_interval = average_interval;
                        INTERVAL_TIMER->CNT = 9000; // 初始化换相间隔定时器
                        zero_crosses = 10;          // 让 BEMF 检测认为已经稳定
                        prop_brake_active = 0;      // 关闭比例制动
                        step = changeover_step;     // 设置到与正弦 0 位置对齐的六步状态

                        if (stall_protection)
                        {
                            // 防止低速重载下电机堵转的功能开关，主要用于 crawler / RC 车 等需要大扭矩低速运行的场景，不用于多旋翼
                            // 核心作用：转速越低，自动加大油门
                            minimum_duty_cycle = stall_protect_minimum_duty;
                        }

                        commutate(); // 执行第一次六步换相
                        // enableCompInterrupts();
                        LL_TIM_GenerateEvent_UPDATE(TIM1); // 强制 TIM1 更新，让新占空比/死区立即生效
                        //	  zcfoundroutine();
                    }
                }
            }
            else // 电机当前处于正弦模式，但油门 ≤ 48 或 ESC 未解锁。也就是正弦模式下松油门/未解锁时的停机处理
            {
                do_once_sinemode = 1; // // 重置正弦初始化标志，下次再进入正弦区时重新初始化

                if (brake_on_stop) //  停止时主动制动
                {
#ifndef PWM_ENABLE_BRIDGE
                    duty_cycle = (TIMER1_MAX_ARR - 19) + drag_brake_strength * 2;
                    adjusted_duty_cycle = TIMER1_MAX_ARR - ((duty_cycle * tim1_arr) / TIMER1_MAX_ARR) + 1;
                    proportionalBrake();
                    TIM1->CCR1 = adjusted_duty_cycle;
                    TIM1->CCR2 = adjusted_duty_cycle;
                    TIM1->CCR3 = adjusted_duty_cycle;

                    prop_brake_active = 1;
#else
                    // todo add braking for PWM /enable style bridges.
#endif
                }
                else // 停止时自由滑行
                {
                    TIM1->CCR1 = 0;
                    TIM1->CCR2 = 0;
                    TIM1->CCR3 = 0;
                    allOff();
                }
            }

#endif // gimbal mode
        } // stepper/sine mode end
#endif // end of brushless mode

#ifdef BRUSHED_MODE
        runBrushedLoop();
#endif
    }
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
