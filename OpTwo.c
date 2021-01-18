/*
 * File: OpTwp.c
 *
 * NTS-1 Two Operator Frequency or Phase Modulation Oscillator
 *
 */
#include <OpTwo.h>

// Saw oscillator, radian should be positive
float oscSawtooth(float radian) {
  return ((uint8_t) si_roundf(radian / M_PI) % 2) ? (-1.f + fmod(radian, M_PI) / M_PI) : (fmod(radian, M_PI) / M_PI);
}

// Square oscillator, radian should be positive
float oscSquare(float radian) {
  return ((uint8_t) si_roundf(radian / M_PI) % 2) ? -1.f : 1.f;
}

// Triangle oscillator, radian should be positive
float oscTriangle(float radian) {
  float sample = 0.f;
  float radianWithinDoublePi = fmod(radian, DOUBLE_PI);
  float ratioInQuadrant = fmod(radianWithinDoublePi, HALF_PI) / HALF_PI;
  if(radianWithinDoublePi < HALF_PI) {
    sample = 0.f + ratioInQuadrant;
  }else if(radianWithinDoublePi < M_PI){
    sample = 1.f - ratioInQuadrant;
  }else if(radianWithinDoublePi < M_PI + HALF_PI){
    sample = 0.f - ratioInQuadrant;
  }else{
    sample = -1.f + ratioInQuadrant;
  }
  return sample;
}

// Multi wave oscillator (waveform selector inside)
float oscWave(float radian, uint8_t operatorId){
  float sample = 0.f;
  switch(OP[operatorId].waveform){
    case WAVEFORM_SIN:
      sample = fastsinfullf(radian);
      break;
    case WAVEFORM_TRI:
      sample = oscTriangle(radian);
      break;
    case WAVEFORM_SQU:
      sample = oscSquare(radian);
      break;
    case WAVEFORM_SAW:
      sample = oscSawtooth(radian);
      break;
    default:
      break;
  }
  return sample;
}

float osc_fm(float modAmount){
  // Wave generation
  float fbVal = OP[1].feedback * OP[1].lastVal; // Modulator feedback value
  OP[1].lastVal = OP[1].level * ((1.f - MAX_MOD_RATIO) + MAX_MOD_RATIO * modAmount) * oscWave(DOUBLE_PI * (OP[1].phase + fbVal), 1);
  OP[0].lastVal = OP[0].level * oscWave(DOUBLE_PI * (OP[0].phase + OP[1].lastVal), 0);

  return OP[0].lastVal;
}

float osc_pm(float modAmount){
  // Wave generation
  float fbVal = OP[1].feedback * OP[1].lastVal; // Modulator feedback value
  OP[1].lastVal = OP[1].level * ((1.f - MAX_MOD_RATIO) + MAX_MOD_RATIO * modAmount) * oscWave(DOUBLE_PI * OP[1].phase + fbVal, 1);
  OP[0].lastVal = OP[0].level * oscWave(DOUBLE_PI * OP[0].phase + OP[1].lastVal, 0);

  return OP[0].lastVal;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

// Initialize function
//  called once on instantiation of the oscillator
void OSC_INIT(uint32_t platform, uint32_t api)
{
  // Initialize
  MOD_MODE = MODE_FM;
  for(uint8_t operatorId = 0; operatorId < NUM_OPERATORS; operatorId++){
    OP[operatorId].waveform = WAVEFORM_SIN;
    OP[operatorId].ratio = 1.f;                     // Carrier is fixed to this value
    OP[operatorId].level = 1.f;                     // Carrier is fixed to this value
    OP[operatorId].phase = _osc_rand() / UINT_MAX;  // Randomize phase ratio within 0 to 1
    OP[operatorId].feedback = 0.f;                  // Not used for non feedback capable operator
    OP[operatorId].lastVal = 0.f;
  }
}

// Wave Generation function
//  callbacked for each sample (or frames)
void OSC_CYCLE(const user_osc_param_t * const params,
               int32_t *yn,
               const uint32_t frames)
{
  // Pointer to output buffer
  q31_t * __restrict y = (q31_t *) yn;

  // Last address of output buffer
  const q31_t * y_e = y + frames;

  // MIDI note# of current process
  uint8_t note = params->pitch >> 8;

  // Corresponding frequency of the MIDI note#
  // Not only notenumber but also pitchbend and built-in LFO pitch modulation is taken into account
  float baseFreq = osc_notehzf(note);

  // Current LFO value within -1 to 1
  float lfo = q31_to_f32(params->shape_lfo);

  // Process one sample by sample in frames
  while( y != y_e ) {
    float sample = 0.f;
    // Generate wave sample
    switch(MOD_MODE){
      case MODE_FM:
        sample = osc_fm(lfo);
        break;
      case MODE_PM:
        sample = osc_pm(lfo);
        break;
      default:
        break;
    }
    // Convert sample into Q31 format, and write to output
    *(y++) = f32_to_q31(sample);

    // Step operator phase
    for(uint8_t operatorId = 0; operatorId < NUM_OPERATORS; operatorId++){
      // Step a phase ratio
      OP[operatorId].phase += baseFreq * OP[operatorId].ratio / k_samplerate;
      // Keep the phase ratio within 0 <= phase < 1
      OP[operatorId].phase -= (uint32_t) OP[operatorId].phase;
    }
  }
}

// MIDI note-on event process function
//  * This function is not hooked if active note is already exist
void OSC_NOTEON(const user_osc_param_t * const params)
{
  // Nothing to do
}

// MIDI note-off event process function
//  * This function is not hooked if active note remains
void OSC_NOTEOFF(const user_osc_param_t * const params)
{
  for(uint8_t operatorId = 0; operatorId < NUM_OPERATORS; operatorId++){
    OP[operatorId].lastVal = 0.f;
  }
}

// Parameter change event process funnction
void OSC_PARAM(uint16_t index, uint16_t value)
{
  // 0-200 for bipolar percent parameters. 0% at 100, -100% at 0.
  // 0-100 for unipolar percent and typeless parameters.
  // 10 bit resolution for shape/shift-shape.
  switch (index) {
    case k_user_osc_param_id1: // OP1 Waveform (0-3)
      // Choose this param by pressing OSC and tweaking TYPE knob, then input by B knob
      OP[0].waveform = (uint8_t) value;
      break;
    case k_user_osc_param_id2: // OP2 Waveform (0-3)
      // Choose this param by pressing OSC and tweaking TYPE knob, then input by B knob
      OP[1].waveform = (uint8_t) value;
      break;
    case k_user_osc_param_id3: // OP2(Modulator) feedback level (0-100)
      // Choose this param by pressing OSC and tweaking TYPE knob, then input by B knob
      OP[1].feedback = (float) value / MAX_FB_PARAM;
      break;
    case k_user_osc_param_id4: // Mode select (0-1, 1-2 indisplay)
      // Choose this param by pressing OSC and tweaking TYPE knob, then input by B knob
      MOD_MODE = (uint8_t) value;
      break;
    case k_user_osc_param_id5: // Not used
      // Choose this param by pressing OSC and tweaking TYPE knob, then input by B knob
      break;
    case k_user_osc_param_id6: // Not used
      // Choose this param by pressing OSC and tweaking TYPE knob, then input by B knob
      break;
    case k_user_osc_param_shape: // OP2(Modulator) ratio (0-63)
      // 10bit parameter, 0 <= value <= 1023
      // Mapped to OSC mode A knob(shape) and MIDI CC#54
      // Convert input value to 7bit and limit to MAX_LUT_RATIO to use as ratio LUT index
      OP[1].ratio = lutRatio[MIN(MAX_LUT_RATIO_ID, value>>3)];
      break;
    case k_user_osc_param_shiftshape: // OP2(Modulator) level
      // Similar to k_user_osc_param_shape, but mapped to OSC mode B knob(alt) and MIDI CC#55
      OP[1].level = param_val_to_f32(value);
      break;
    default:
      break;
  }
}
#pragma GCC diagnostic pop
