#include "../../hal/GPIO.h"
#include "S32K312.h"
#include "math.h"
#include "arm_math.h"

#define CYCLES 1000000U
#define ATAN_COEFS_F32 64
#define TEST6

static q31_t float_to_q31(float value)
{
	if (value <= -1.0f)
	{
		return INT32_MIN;
	}
	if (value >= 1.0f)
	{
		return INT32_MAX;
	}

	return (q31_t)(value * 2147483648.0f);
}
static float q29_atan2_to_rad(q31_t angle)
{
	return (float)angle / 536870912.0f; 
}
static float q31_atan2_to_rad(q31_t angle)
{
	return (float)angle / 2147483648.0f;
}
static q31_t q31_mul(q31_t a, q31_t b)
{
	return (q31_t)(((int64_t)a * b) >> 31);
}

static q31_t atan_taylor_q31(q31_t z) {
	// arctan(z) = z - z3/3 + z5/5 - z7/7 + z9/9 (factorials are precomputed)
	const q31_t q31_one = 0x7FFFFFFF;
	const q31_t neg_one_third = -715827883;
	const q31_t one_fifth = 429496730;
	const q31_t neg_one_seventh = -306783378;
	const q31_t one_ninth = 238609294;
	q31_t z2 = q31_mul(z, z);
	q31_t polynomial = neg_one_third +
		q31_mul(z2, one_fifth +
		q31_mul(z2, neg_one_seventh + q31_mul(z2, one_ninth)));

	return q31_mul(z, q31_one + q31_mul(z2, polynomial));
}
static void atan_adjust_f32(float y, float x, float * theta) {
	if (y < 0) {
		if (x < 0) {
			*theta-= (float)M_PI;
		}
	} else {
		if (x < 0) {
			*theta+= (float)M_PI;
		}
	}
}
static float atan2_taylor_f32(float y, float x) {
	// arctan(z) = z - z3/3 + z5/5 - z7/7 + z9/9 (factorials are precomputed)
	float z = (y/x);
	float z2 = z * z; // z^2
	float theta = z * (1 + z2 * ((-1.0f/3.0f) + z2 * ((1.0f/5.0f) + z2 * ((-1.0f/7.0f)+(z2/9.0f)))));
	atan_adjust_f32(y, x, &theta);
	return theta;
}
static float atan_coefs_f32[ATAN_COEFS_F32+1] = {
	0.000000000f, 0.015623729f, 0.031239834f, 0.046840712f, 0.062418811f, 0.077966631f,
	0.093476780f, 0.108941957f, 0.124354996f, 0.139708877f, 0.154996738f, 0.170211926f,
	0.185347944f, 0.200398549f, 0.215357706f, 0.230219588f, 0.244978666f, 0.259629637f,
	0.274167448f, 0.288587362f, 0.302884877f, 0.317055762f, 0.331096083f, 0.345002174f,
	0.358770669f, 0.372398436f, 0.385882676f, 0.399220765f, 0.412410438f, 0.425449640f,
	0.438336551f, 0.451069653f, 0.463647604f, 0.476069331f, 0.488333941f, 0.500440836f,
	0.512389481f, 0.524179637f, 0.535811245f, 0.547284365f, 0.558599293f, 0.569756448f,
	0.580756366f, 0.591599703f, 0.602287352f, 0.612820208f, 0.623199344f, 0.633425891f,
	0.643501103f, 0.653426349f, 0.663203001f, 0.672832549f, 0.682316542f, 0.69165659f,
	0.700854421f, 0.709911585f, 0.718829989f, 0.727611303f, 0.736257434f, 0.74477011f,
	0.753151298f, 0.761402786f, 0.769526482f, 0.777524292f, 0.785398185f
};
static float atan2_lut_f32(float y, float x)
{

	float z_temp = y / x;
	float z = z_temp < 0 ? z_temp*-1.0f : z_temp;
	float sign = z_temp < 0 ? -1.0f : 1.0;
    if (z >= 1.0f)
        return atan_coefs_f32[ATAN_COEFS_F32];

    float pos  = z * (float)ATAN_COEFS_F32;

    uint32_t idx = (uint32_t)pos;

    float frac = pos - (float)idx;

    float theta = sign * (atan_coefs_f32[idx] +
           frac * (atan_coefs_f32[idx + 1] - atan_coefs_f32[idx]));
    atan_adjust_f32(y, x, &theta);
	return theta;
}

int main(void) {

	// Pin Init to measure execution time
	GPIO_Init_TimePin();

    // Note: The following code is a benchmark for different implementations of 
    // the atan() function,tables.Minimum resolution of 0.5 degrees.

	// ********** variables declaration *******************

	float x;
	float y;
	float z;
	q31_t x_q;
	q31_t y_q;
	q31_t z_q;

	// x = 0.99f;  y = 0.98f;   // ~pi/4 0.785398163
	//x =  0.80f; y = 0.20f;   // I | `0.244979` |
	//x = -0.80f; y = 0.40f;   // II | `2.677945` |
	//x = -0.90f; y = -0.30f;   // III | `-2.819842` |
	x =  0.70f; y = -0.56f;   // IV | `-0.674741` |
	z = y/x;
	x_q = float_to_q31(x);
	y_q = float_to_q31(y);
	z_q = float_to_q31(z);

    // ********** Test 1: atan() from math.h ************
	#if defined(TEST1)
	volatile float theta_rad_1;
	SET_TIME_PIN(HIGH);
	theta_rad_1 = atan2f(y, x);
	SET_TIME_PIN(LOW);

	// ********** Test 2: CMSIS DSP Library atan2(float, float) ************
	#elif defined (TEST2)
	volatile float theta_rad_2;
	SET_TIME_PIN(HIGH);
	arm_atan2_f32(y, x, &theta_rad_2);
	SET_TIME_PIN(LOW);

    // ********** Test 3: CMSIS DSP Library atan2(q31, q31) ************
	#elif defined(TEST3)
	q31_t theta_rad_3_q;
	volatile float theta_rad_3;
	SET_TIME_PIN(HIGH);
	arm_atan2_q31(y_q, x_q, &theta_rad_3_q);
	SET_TIME_PIN(LOW);
	theta_rad_3 = q29_atan2_to_rad(theta_rad_3_q);

    // ********** Test 4: Taylor series expansion of atan float ************
	#elif defined(TEST4)
	volatile float theta_rad_4;
	SET_TIME_PIN(HIGH);
	theta_rad_4 = atan2_taylor_f32(y, x);
	SET_TIME_PIN(LOW);

    // ********** Test 5: Taylor series expansion of atan q31 ************
	#elif defined(TEST5)
	q31_t theta_rad_5_q;
	volatile float theta_rad_5;
	SET_TIME_PIN(HIGH);
	theta_rad_5_q = atan_taylor_q31(z_q);
	SET_TIME_PIN(LOW);
	theta_rad_5 = q31_atan2_to_rad(theta_rad_5_q); // NOTE: this one doesnt do quadrant correction

    // ********** Test 6: lookup Tables ************
	// to create the values in the table:
	// for (int i=0; i<= ATAN_COEFS_F32; i++){
	// 	float x = (float)i / ATAN_COEFS_F32;
	// 	atan_coefs_f32[i] = atanf(x);
	// }
	#elif defined(TEST6)
	volatile float theta_rad_6;
	SET_TIME_PIN(HIGH);
	theta_rad_6 = atan2_lut_f32(y, x);
	SET_TIME_PIN(LOW);
	#endif
    while(1)
    {

    }
    return 0;
}
