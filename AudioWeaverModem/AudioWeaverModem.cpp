//---------------------------------------------------------------------------------
// File:    AudioWeaverModem.cpp
//          A ''Teensy'' Audio Object function for Weaver SSB demodulation
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
// 1) The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 2) THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN 
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//---------------------------------------------------------------------------------
#include "AudioWeaverModem.h"
extern "C" {extern const int16_t AudioWaveformSine[257];}

int16_t Oscillator(uint16_t Phase){
  uint16_t  index;                   // Index into sine table
  uint16_t  delta;                   // Remainder for interpolation,
  int16_t   val1, val2;
  index   = Phase>>8;                              // index into lookup table
  delta   = Phase&0xFF;                            // remainder 
  val1    = AudioWaveformSine[index];
  val2    = AudioWaveformSine[index+1];            // table has 257 entries
  return val1 + (((val2 - val1)*delta)>>8);  // linear interpolation
}

void AudioWeaverModem::update(void) {
  int16_t  oscOutI, oscOutQ;
  int16_t  TempI1[AUDIO_BLOCK_SAMPLES], TempQ1[AUDIO_BLOCK_SAMPLES];
  int16_t  TempI2[AUDIO_BLOCK_SAMPLES], TempQ2[AUDIO_BLOCK_SAMPLES];
  audio_block_t *blockIn, *blockOut, *blockDebug;
  //
  blockIn = receiveReadOnly(0);
  if (!blockIn) return;
  blockOut   = allocate();                           // create new output block
  blockDebug = allocate();                         // create new output block
  if (!blockOut)  return;
  //
  //--- First multiplication stage
  for (uint16_t i=0; i < AUDIO_BLOCK_SAMPLES; i++) {  
    blockDebug->data[i] = blockIn->data[i];
    TempI1[i]  = (blockIn->data[i]*Oscillator(sinPhase1))>>15;
    TempQ1[i]  = (blockIn->data[i]*Oscillator(cosPhase1))>>15;
    sinPhase1 += phaseInc1;
    cosPhase1 += phaseInc1;
  }
//--- Low-pass filter both blocks
  arm_fir_fast_q15(&filterI, TempI1, TempI2, AUDIO_BLOCK_SAMPLES);
  arm_fir_fast_q15(&filterQ, TempQ1, TempQ2, AUDIO_BLOCK_SAMPLES);
//   
//-- Second multiplication stage
  for (uint16_t i=0; i < AUDIO_BLOCK_SAMPLES; i++) { 
 //     blockDebug->data[i] = TempQ2[i];
    oscOutI = Oscillator(sinPhase2);
    oscOutQ = Oscillator(cosPhase2);
//
//--- Sum or difference to produce the output.
    if (lowSB) blockOut->data[i] = ((TempI2[i]*oscOutI)>>15) - ((TempQ2[i]*oscOutQ)>>15);
    else       blockOut->data[i] = ((TempI2[i]*oscOutI)>>15) + ((TempQ2[i]*oscOutQ)>>15);
    sinPhase2 += phaseInc2;
    cosPhase2 += phaseInc2;
    }
//--- End of processing...
  transmit(blockOut,   0);
  transmit(blockDebug, 1);
  release(blockDebug);
  release(blockOut);
  release(blockIn);
  return;
}



