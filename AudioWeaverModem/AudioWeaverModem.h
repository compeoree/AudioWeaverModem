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
    int16_t n_coeffs   = 70;
    int16_t coeffs[70] = {
         67,     43,     52,     59,     61,     58,     47,     27,     -2,    -40,
        -87,   -141,   -199,   -257,   -311,   -355,   -383,   -391,   -372,   -323,
       -238,   -117,     40,    233,    457,    706,    973,   1249,   1523,   1784,
       2022,   2226,   2388,   2500,   2558,   2558,   2500,   2388,   2226,   2022,   
       1784,   1523,   1249,    973,    706,    457,    233,     40,   -117,   -238,
       -323,   -372,   -391,   -383,   -355,   -311,   -257,   -199,   -141,    -87,
        -40,     -2,     27,     47,     58,     61,     59,     52,     43,     67
    }; 
};
#endif
