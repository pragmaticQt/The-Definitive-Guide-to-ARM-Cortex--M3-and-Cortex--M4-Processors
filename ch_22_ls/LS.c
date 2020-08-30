#include "arm_math.h"            /* Main include file for CMSIS DSP        */

#define NUMSAMPLES 51            /* Number of measurements                 */
#define NUMUNKNOWNS 3            /* Number of unknowns in polynomial fit   */

// Allocate memory for the matrix arrays.  Only t and x have initial data defined.

// Contains the times at which the data was sampled.  In this examples, the data
// is evenly spaced but this is not required for the least squares fit.
float32_t tData[NUMSAMPLES] = 
{
   0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f,
   0.8f, 0.9f, 1.0f, 1.1f, 1.2f, 1.3f, 1.4f, 1.5f, 
   1.6f, 1.7f, 1.8f, 1.9f, 2.0f, 2.1f, 2.2f, 2.3f,
   2.4f, 2.5f, 2.6f, 2.7f, 2.8f, 2.9f, 3.0f, 3.1f, 
   3.2f, 3.3f, 3.4f, 3.5f, 3.6f, 3.7f, 3.8f, 3.9f, 
   4.0f, 4.1f, 4.2f, 4.3f, 4.4f, 4.5f, 4.6f, 4.7f, 
   4.8f, 4.9f, 5.0f
};

// Contains the noisy position measurements
float32_t xData[NUMSAMPLES] = 
{
   7.4213f,  21.7231f,  -7.2828f,  21.2254f,  20.2221f,  10.3585f,  20.3033f,  29.2690f, 
   57.7152f,  53.6075f,  22.8209f,  59.8714f,  43.1712f,  38.4436f,  46.0499f,  39.8803f, 
   41.5188f,  55.2256f,  55.1803f,  55.6495f,  49.8920f,  34.8721f,  50.0859f,  57.0099f, 
   47.3032f,  50.8975f,  47.4671f,  38.0605f,  41.4790f,  31.2737f,  42.9272f,  24.6954f, 
   23.1770f,  22.9120f,   3.2977f,  35.6270f,  23.7935f,  12.0286f,  25.7104f,  -2.4601f, 
   6.7021f,   1.6804f,   2.0617f,  -2.2891f, -16.2070f, -14.2204f, -20.1870f, -18.9303f, 
   -20.4859f, -25.8338f, -47.2892f
};

float32_t AData[NUMSAMPLES * NUMUNKNOWNS];
float32_t ATData[NUMSAMPLES *NUMUNKNOWNS];
float32_t ATAData[NUMUNKNOWNS * NUMUNKNOWNS];
float32_t invATAData[NUMUNKNOWNS * NUMUNKNOWNS];
float32_t BData[NUMUNKNOWNS * NUMSAMPLES];
float32_t cData[NUMUNKNOWNS];

// Array instance structure initialization.  For each instance, the form is:
//    MAT = {numRows, numCols, pData};

// Column vector t
arm_matrix_instance_f32 t = {NUMSAMPLES, 1, tData};

// Column vector x
arm_matrix_instance_f32 x = {NUMSAMPLES, 1, xData};

// Matrix A
arm_matrix_instance_f32 A = {NUMSAMPLES, NUMUNKNOWNS, AData};

// Transpose of matrix A
arm_matrix_instance_f32 AT = {NUMUNKNOWNS, NUMSAMPLES, ATData};

// Matrix product AT * A
arm_matrix_instance_f32 ATA = {NUMUNKNOWNS, NUMUNKNOWNS, ATAData};

// Matrix inverse inv(AT*A)
arm_matrix_instance_f32 invATA = {NUMUNKNOWNS, NUMUNKNOWNS, invATAData};

// Intermediate result invATA * AT
arm_matrix_instance_f32 B = {NUMUNKNOWNS, NUMSAMPLES, BData};

// Solution
arm_matrix_instance_f32 c = {NUMUNKNOWNS, 1, cData};


/*----------------------------------------------------------------------------
main program
*----------------------------------------------------------------------------*/

int main (void)  {               /* execution starts here                     */
   int i;
   float y;
   
   y = sqrtf(xData[0]);
   cData[0] = y;
   

   // Fill in the values for matrix A.  Each row contains:
   // [1.0f t t*t]
   for(i=0; i<NUMSAMPLES; i++) {
      AData[i*NUMUNKNOWNS + 0] = 1.0f;
      AData[i*NUMUNKNOWNS + 1] = tData[i];
      AData[i*NUMUNKNOWNS + 2] = tData[i] * tData[i];
   }

   // Transpose
   arm_mat_trans_f32(&A, &AT);

   // Matrix multplication AT * A
   arm_mat_mult_f32(&AT, &A, &ATA);

   // Matrix inverse inv(ATA)
   arm_mat_inverse_f32(&ATA, &invATA);

   // Matrix multiplication invATA * x;
   arm_mat_mult_f32(&invATA, &AT, &B);

   // Final result.
   arm_mat_mult_f32(&B, &x, &c);

   // Examine cData in the debugger to see the final values
}
