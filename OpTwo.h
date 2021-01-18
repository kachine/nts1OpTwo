/*
 * File: OpTwo.h
 *
 * NTS-1 Two Operator Frequency or Phase Modulation Oscillator
 *
 */
#include "userosc.h"
#include <limits.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define NUM_OPERATORS     2
#define MAX_LUT_RATIO_ID  63
#define MAX_FB_PARAM      100
#define MAX_MOD_RATIO     0.5
#define DOUBLE_PI         M_TWOPI
#define HALF_PI           M_PI_2

enum{
  MODE_FM = 0,
  MODE_PM = 1
};
uint8_t MOD_MODE;  // 0: FM(Frequency Modulation) / 1: PM(Phase Modulation)

enum {
  WAVEFORM_SIN = 0,
  WAVEFORM_TRI = 1,
  WAVEFORM_SQU = 2,
  WAVEFORM_SAW = 3,
};

struct _operator {
  uint8_t waveform; // waveform (0 to 3)
  float ratio;      // Frequency ratio
  float level;      // Level ratio within 0 to 1
  float phase;      // Phase ratio within 0 to 1
  float feedback;   // Feedback ratio within 0 to 1 (use only in last modulator)
  float lastVal;    // Last output value (use only in last modulator)
};
struct _operator OP[NUM_OPERATORS]; // Global variable to keep operator parameters

//                    1,    2,    3,    4, 5,    6,    7,    8, 9,   10,11,   12,   13,14,   15,   16,17,   18,   19,20,   21,   22,23,   24,   25,26,   27,   28,29,   30,   31, 32,    33,    34, 35,   36, 37,    38,    39,    40, 41,    42, 43,   44,    45, 46,    47,    48,   49,    50,    51,   52,    53,    54,    55,    56,    57,    58,   59,    60,    61,    62,    63,    64
static float lutRatio[] = {0.5, 0.71, 0.78, 0.87, 1, 1.41, 1.57, 1.73, 2, 2.82, 3, 3.14, 3.46, 4, 4.24, 4.71, 5, 5.19, 5.65, 6, 6.28, 6.92, 7, 7.07, 7.85, 8, 8.48, 8.65, 9, 9.42, 9.89, 10, 10.38, 10.99, 11, 11.3, 12, 12.11, 12.56, 12.72, 13, 13.84, 14, 14.1, 14.13, 15, 15.55, 15.57, 15.7, 16.96, 17.27, 17.3, 18.37, 18.84, 19.03, 19.78, 20.41, 20.76, 21.2, 21.98, 22.49, 23.55, 24.22, 25.95};

float oscSawtooth(float radian);
float oscSquare(float radian);
float oscTriangle(float radian);
float oscWave(float radian, uint8_t operatorId);
float osc_fm(float modAmount);
float osc_pm(float modAmount);
void OSC_INIT(uint32_t platform, uint32_t api);
void OSC_CYCLE(const user_osc_param_t * const params,
               int32_t *yn,
               const uint32_t frames);
void OSC_NOTEON(const user_osc_param_t * const params);
void OSC_NOTEOFF(const user_osc_param_t * const params);
void OSC_PARAM(uint16_t index, uint16_t value);
