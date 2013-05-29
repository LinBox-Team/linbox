/*
 * kernel_modulus_sp.cl
 *
 *  Created on: Jul 5, 2011
 *      Author: Matthew Wezowicz
 */

#define BLOCK_SIZE 16

__kernel void matrixMuladdKernelModular1SP(__global float* D, float alpha, __global float* A, __global float* B,
		float beta, __global float* C, const int widthA, const int widthB, const float mod){
	//Get Workgroup ID
	int bx = get_group_id(0);
	int by = get_group_id(1);

	//Get Local ID
	int tx = get_local_id(0);
	int ty = get_local_id(1);

	//Range of indecies for sub-matrix of A
	int aBegin = widthA * BLOCK_SIZE * by;
	int aEnd = aBegin + widthA - 1;
	int aStep = BLOCK_SIZE;

	//Range of indecies for sub-matrix of B
	int bBegin = BLOCK_SIZE * bx;
	int bStep = BLOCK_SIZE * widthB;

	//Local storage of sub-matrices of A and B
	__local float As[BLOCK_SIZE][BLOCK_SIZE];
	__local float Bs[BLOCK_SIZE][BLOCK_SIZE];

	//Temporary storage for result
	float Dsub = 0;

	//Loop over all the sub-matrices of A and B required to compute
	//the result sub-matrix
	for(int a = aBegin, b = bBegin; a < aEnd; a += aStep, b += bStep){
		//Load the matrices from global memory to local memory
		//Each thread loads one element of each sub-matrix
		As[ty][tx] = A[a + widthA * ty + tx];
		Bs[ty][tx] = B[b + widthB * ty + tx];

		//Synchronize threads
		barrier(CLK_LOCAL_MEM_FENCE);

		//Multiply the two sub-matrices together
		for(int i = 0; i < BLOCK_SIZE; i++){
			Dsub += As[ty][i] * Bs[i][tx];
			//Calls fmod every iteration to normalize the partial sum
			Dsub = fmod(Dsub, mod);
		}

		//Synchronize threads
		barrier(CLK_LOCAL_MEM_FENCE);
	}
	//Calculates the offset in the result matrix
	int d = widthB * BLOCK_SIZE * by + BLOCK_SIZE * bx;

	//Scale Dsub by alpha
	Dsub = alpha * Dsub;
	Dsub = fmod(Dsub, mod);
	if(Dsub < 0){
		Dsub = mod + Dsub;
	}

	//Scalse Csub by beta
	float Csub = C[d + ty * widthB + tx];
	Csub = beta * Csub;
	Csub = fmod(Csub, mod);
	if(Csub < 0){
		Csub = mod + Csub;
	}

	//Add Dsub and Dsub
	Dsub = Dsub + Csub;
	Dsub = fmod(Dsub, mod);

	//Add the sum to the appropriate spot
	D[d + ty * widthB + tx] = Dsub;
}
