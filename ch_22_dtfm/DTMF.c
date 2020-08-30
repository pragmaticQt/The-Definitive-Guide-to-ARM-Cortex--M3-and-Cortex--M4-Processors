#include "stm32f4xx.h" 
#include <stdio.h>

#include "arm_math.h"

#define L 512
#define SR 8000
#define FREQ 697
// #define FREQ 770
#define BLOCKSIZE 8

q15_t inSignalQ15[L];
q31_t inSignalQ31[L];
float inSignalF32[L];

q15_t outSignalQ15[L];
float outSignalF32[L];

q31_t fftSignalQ31[2*L];
q31_t fftMagnitudeQ31[2];

#define NUM_FIR_TAPS 202
q15_t hfir_coeffs_q15[NUM_FIR_TAPS] = {
   -9  ,   -29,   -40,   -40,   -28,    -7,    17,    38,
   49  ,    47,    29,     1,   -30,   -55,   -66,   -58, 
   -31 ,     9,    51,    82,    91,    72,    29,   -28, 
   -82 ,  -117,  -119,   -84,   -20,    57,   124,   160, 
   149 ,    91,     0,   -99,  -176,  -206,  -175,   -88, 
   33  ,   153,   235,   252,   193,    72,   -80,  -217, 
   -297,  -293,  -199,   -40,   141,   289,   358,   323, 
   189 ,    -9,  -213,  -364,  -412,  -339,  -161,    73, 
   294 ,   436,   453,   336,   114,  -149,  -376,  -499, 
   -477,  -312,   -51,   233,   456,   548,   480,   269, 
   -27 ,  -320,  -525,  -579,  -462,  -207,   113,   404, 
   580 ,   587,   422,   131,  -201,  -477,  -614,  -572, 
   -362,   -45,   287,   534,   626,   534,   287,   -45, 
   -362,  -572,  -614,  -477,  -201,   131,   422,   587, 
   580 ,   404,   113,  -207,  -462,  -579,  -525,  -320, 
   -27 ,   269,   480,   548,   456,   233,   -51,  -312, 
   -477,  -499,  -376,  -149,   114,   336,   453,   436, 
   294 ,    73,  -161,  -339,  -412,  -364,  -213,    -9, 
   189 ,   323,   358,   289,   141,   -40,  -199,  -293, 
   -297,  -217,   -80,    72,   193,   252,   235,   153, 
   33  ,   -88,  -175,  -206,  -176,   -99,     0,    91, 
   149 ,   160,   124,    57,   -20,   -84,  -119,  -117, 
   -82 ,   -28,    29,    72,    91,    82,    51,     9, 
   -31 ,   -58,   -66,   -55,   -30,     1,    29,    47, 
   49  ,    38,    17,    -7,   -28,   -40,   -40,   -29, 
   -9,     0};

q31_t hanning_window_q31[L];

q15_t hfir_state_q15[NUM_FIR_TAPS + BLOCKSIZE] = {0};

float biquad_coeffs_f32[5] = {0.01f, 0.0f, -0.01f, 1.690660431255413f, -0.9801f};
float biquad_state_f32[2] = {0};

/*----------------------------------------------------------------------------
main program
*----------------------------------------------------------------------------*/

int main (void)  {               /* execution starts here                     */
   int i, samp;
   arm_fir_instance_q15 DTMF_FIR;
   arm_rfft_instance_q31 DTMF_RFFT;
   arm_cfft_radix4_instance_q31 DTMF_CFFT;
   arm_biquad_cascade_df2T_instance_f32 DTMF_BIQUAD;

   // Generate the input sine wave
   // The signal will have an amplitude of 0.5 and a frequency of FREQ Hz
   // Create floating-point, Q31, and Q7 versions.
   
   for(i=0; i<L; i++) {
      inSignalF32[i] = 0.5f * sinf(2.0f * PI * FREQ * i / SR);
      inSignalQ15[i] = (q15_t) (32768.0f * inSignalF32[i]);
      inSignalQ31[i] = (q31_t) ( 2147483647.0f * inSignalF32[i]);
   }

   /* ----------------------------------------------------------------------
   ** Process with FIR filter
   ** ------------------------------------------------------------------- */

   if (arm_fir_init_q15(&DTMF_FIR, NUM_FIR_TAPS, &hfir_coeffs_q15[0], 
                        &hfir_state_q15[0], BLOCKSIZE) != ARM_MATH_SUCCESS) {
      // error condition
      // exit(1);
   }
   
   for(samp = 0; samp < L; samp += BLOCKSIZE) {
      arm_fir_q15(&DTMF_FIR, inSignalQ15 + samp, outSignalQ15 + samp, BLOCKSIZE);
   }

   /* ----------------------------------------------------------------------
   ** Process with a floating-point Biquad filter
   ** ------------------------------------------------------------------- */

   arm_biquad_cascade_df2T_init_f32(&DTMF_BIQUAD, 1, biquad_coeffs_f32, biquad_state_f32);

   for(samp = 0; samp < L; samp += BLOCKSIZE) {

      arm_biquad_cascade_df2T_f32(&DTMF_BIQUAD, inSignalF32 + samp, 
                                  outSignalF32 + samp, BLOCKSIZE);

   }

   /* ----------------------------------------------------------------------
   ** Process with Q31 FFT
   ** ------------------------------------------------------------------- */

   // Create the Hanning window.  This is usually done once at the
   // start of the program.

   for(i=0; i<L; i++) {
      hanning_window_q31[i] = 
          (q31_t) (0.5f * 2147483647.0f * (1.0f - cosf(2.0f*PI*i / L)));
   }

   // Apply the window to the input buffer
   arm_mult_q31(hanning_window_q31, inSignalQ31, inSignalQ31, L);

   arm_rfft_init_q31(&DTMF_RFFT, &DTMF_CFFT, 512, 0, 1);
   
   // Compute the FFT
   arm_rfft_q31(&DTMF_RFFT, inSignalQ31, fftSignalQ31);

   arm_cmplx_mag_q31(fftSignalQ31, fftMagnitudeQ31, L);
}
