//---------------------------------------------------------------------------------
// File:    AudioWeaverModem.h
//          Helper file for ''Teensy'' Audio Object AudioWeaverModem for Weaver SSB
//          modulation/demodulation.
//
// Note:   This version uses FIR filtering.  An IIR version is being prepared.
//
// Author:  Derek Rowell
//
// Date:    Feb. 19, 2017
//
// Copyright (c) 2017, Derek Rowell
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
//  1) The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 2) THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN 
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//---------------------------------------------------------------------------------
//
#ifndef weaver_modem_h_
#define weaver_modem_h_
#include "Arduino.h"
#include "AudioStream.h"
#include "arm_math.h"
#define FIR_MAX_COEFFS 200
class AudioWeaverModem : public AudioStream {
  public:
    AudioWeaverModem() : AudioStream(1,inputQueueArray) {
      sinPhase1 = 0;            // Initial phase for oscillator 1
      cosPhase1 = 16384;        // Corresponds to phase shift of pi/2 for cosine
      sinPhase2 = 0;            // Initial phase for oscillator 2
      cosPhase2 = 16384;        // Corresponds to phase shift of pi/2 for cosine
      arm_fir_init_q15(&filterI, n_coeffs, coeffs, FIRstateI, AUDIO_BLOCK_SAMPLES);
      arm_fir_init_q15(&filterQ, n_coeffs, coeffs, FIRstateQ, AUDIO_BLOCK_SAMPLES);
   }

//--------
     virtual void update(void);


//--- Set internal oscillator 1 frequency
    void Oscillator1_freq(float oscFreq1) {
     if (oscFreq1 < 0.0) oscFreq1 = 0.0;
      else if (oscFreq1 > AUDIO_SAMPLE_RATE_EXACT/2) oscFreq1 = AUDIO_SAMPLE_RATE_EXACT/2;
      phaseInc1 = uint16_t(oscFreq1*(65536.0/AUDIO_SAMPLE_RATE_EXACT));
     }
    
//--- Set internal oscillator 2 frequency
    void Oscillator2_freq(float oscFreq2) {
      if (oscFreq2 < 0.0) oscFreq2 = 0.0;
      else if (oscFreq2 > AUDIO_SAMPLE_RATE_EXACT/2) oscFreq2 = AUDIO_SAMPLE_RATE_EXACT/2;
      phaseInc2 = uint16_t(oscFreq2*(65536.0/AUDIO_SAMPLE_RATE_EXACT)); 
    }
    
//--- Set to LSB mode
    void LSB(void){lowSB = true;}
    
//--- Set USB mode
    void USB(void){lowSB = false;}

//
  private:
    audio_block_t *inputQueueArray[1];
    uint16_t sinPhase1;
    uint16_t cosPhase1;
    uint16_t phaseInc1;
    uint16_t sinPhase2;
    uint16_t cosPhase2;
    uint16_t phaseInc2;
    boolean  lowSB = true;
    arm_fir_instance_q15 filterI;
    arm_fir_instance_q15 filterQ;
    q15_t FIRstateI[AUDIO_BLOCK_SAMPLES + FIR_MAX_COEFFS];
    q15_t FIRstateQ[AUDIO_BLOCK_SAMPLES + FIR_MAX_COEFFS];
    int16_t n_coeffs  = 126;
    int16_t coeffs[126] = 
      { -42,     41,     45,     58,     75,     95,    114,    133,    149,    160,
        165,    162,    152,    133,    107,     73,     33,    -10,    -54,    -96,
       -134,   -163,   -181,   -187,   -178,   -155,   -117,    -67,     -7,     59,
        126,    189,    244,    284,    306,    305,    280,    230,    157,     62,
        -47,   -166,   -285,   -396,   -489,   -555,   -585,   -572,   -509,   -395,
       -229,    -12,    250,    549,    874,   1214,   1554,   1880,   2178,   2435,
       2639,   2780,   2853,   2853,   2780,   2639,   2435,   2178,   1880,   1554,
       1214,    874,    549,    250,    -12,   -229,   -395,   -509,   -572,   -585,
       -555,   -489,   -396,   -285,   -166,    -47,     62,    157,    230,    280,
        305,    306,    284,    244,    189,    126,     59,     -7,    -67,   -117,
       -155,   -178,   -187,   -181,   -163,   -134,    -96,    -54,    -10,     33,
         73,    107,    133,    152,    162,    165,    160,    149,    133,    114,
         95,     75,     58,     45,     41,    -42};
};
#endif
