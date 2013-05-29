/*
 * kernel_partial_1024_dp.cl
 *
 *  Created on: Jul 13, 2011
 *      Author: Matthew Wezowicz
 */

#define BLOCK_SIZE 16
#pragma OPENCL EXTENSION cl_khr_fp64 : enable

__kernel void matrixMuladdKernelModular1024DP(__global double* D, double alpha, __global double* A, __global double* B,
		double beta, __global double* C, const int widthA, const int widthB, const double mod){
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
	__local double As[BLOCK_SIZE][BLOCK_SIZE];
	__local double Bs[BLOCK_SIZE][BLOCK_SIZE];

	//Temporary storage for result
	double Dsub = 0;

	//Counter for modulus every 32 iterations
	int mCount = 0;

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
		}
		mCount++;

		//fmod every 1024 iterations
		if(mCount == 64){
			Dsub = fmod(Dsub, mod);
			mCount = 0;
		}

		//Synchronize threads
		barrier(CLK_LOCAL_MEM_FENCE);
	}

	Dsub = fmod(Dsub, mod);

	//Calculates the offset in the result matrix
	int d = widthB * BLOCK_SIZE * by + BLOCK_SIZE * bx;

	//Scale Dsub by alpha
	Dsub = alpha * Dsub;
	Dsub = fmod(Dsub, mod);
	if(Dsub < 0){
		Dsub = mod + Dsub;
	}

	//Scalse Csub by beta
	double Csub = C[d + ty * widthB + tx];
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
