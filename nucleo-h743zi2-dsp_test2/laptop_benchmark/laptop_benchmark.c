/**
 * Yong Da Li
 * Saturday, July 2, 2022
 *
 * I have a vector*vector dot product speed test working on the MCU
 * 	using the arm_math.h and hardware accelerated functions
 *
 * parameters
 * - generate 2 vectors
 * 	- size	50,000 elements
 * 	- type	float_32
 * 	- elem 	values between 0.00 - 0.99
 *
 * - dot product them together
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>


void fill_with_rng(float *vec, uint32_t size){
  	uint32_t i = 0;
  	for (i=0; i<size; i++){
	  vec[i] = (float)(rand()%100) / 100; // decimals with 2 significant figures, from 0 - 0.99
  	}
}



 /**
  * software implementation of dot product that `arm_dot_prod_f32` is doing
  * @param vecA   [vector to dot product]
  * @param vecB   [vector to dot product]
  * @param size   [length of vectors, should be same size]
  * @param result [return via reference the result]
  */
void software_dot_product(float *vecA, float* vecB, uint32_t size, float *result){
	// clear output variable
	*result = 0;

	uint32_t i = 0;
	for (i=0; i<size; i++){
		*result = *result + vecA[i]*vecB[i];
	}
}


int main(void){
	// seed RNG
	srand(time(NULL));

	// setup variables
	uint32_t RNG_TEST_SIZE = 50000;
	float vectorA[RNG_TEST_SIZE];
	float vectorB[RNG_TEST_SIZE];
	float result;
	uint32_t count = 0;

	// check functional
	printf("hello world\n");

	// can't define a float32_t
	// just check that it's 32 bits long for equal comparison
	printf("floats are %2d bits long\n", 8*(int)sizeof(float));

	// stop at 100, for MCU comparison
	while(count<100){
		fill_with_rng(vectorA, RNG_TEST_SIZE);
		fill_with_rng(vectorB, RNG_TEST_SIZE);

		software_dot_product(vectorA, vectorB, RNG_TEST_SIZE, &result);
		printf("count=%3d, result: %.3f\n", count, result);
		count++;
	}

	return 0;
}