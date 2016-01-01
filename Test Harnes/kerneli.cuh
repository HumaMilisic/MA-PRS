#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <thrust\device_vector.h>
#include <thrust\copy.h>
#include <thrust\reduce.h>
#include <thrust\fill.h>
#include <thrust\device_ptr.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <iterator>
#include <queue>
#include <ctime>
#include <algorithm>
#include <climits>
#include "cuda_profiler_api.h"

__device__  __managed__ bool done = true;
__device__  __managed__ int doneI = 1;
__device__ __managed__  long iteration = 0;

__global__
void postaviGlobalneLol(bool &done, long &iteration, bool vdone, long viter)
{
	done = vdone;
	iteration = viter;
}

__global__
void vectorAdd(const float *A, const float *B, float *C, int numElements)
{
	int i = blockDim.x * blockIdx.x + threadIdx.x;

	if (i < numElements)
	{
		C[i] = A[i] + B[i];
	}
}

__global__
void prazno(const long *V, long sizeV, const long*E, long sizeE, long *F, long*X, long*C) // V - vertice, E - susjedi, F - za posjetiti, X posjeceno, C cost do ture
{
	int i = blockDim.x * blockIdx.x + threadIdx.x;
	if (i < sizeV)
	{
		if (F[i])
		{
			F[i] = false;
			X[i] = true;
			long pozP = V[i],
				pozK = i + 1 < sizeV ? V[i + 1] : sizeE;
			for (long j = pozP; j < pozK; j++)
			{
				long susjed = E[j];
				if (!X[susjed])
				{
					F[susjed] = 1;
					C[susjed] = C[i] + 1;
				}
			}
		}
	}
	//printf("%d\n", i);
}

__global__
void edge(const long*V, long sizeV, const long *E, long sizeE, long *F, long *X, long *C)
{
	int i = blockDim.x*blockIdx.x + threadIdx.x;
	if (i < sizeE)
	{
		long cvor = E[i];
		if (F[cvor])
		{
			F[cvor] = false;
			X[cvor] = true;

			for (long j = 0; j < sizeV; j++)
			{
				long cv = X[j];
				if (!cv)
				{
					F[j] = true;
					C[j] = C[cvor] + 1;
				}
			}
		}
	}
}

__global__
void kernel_1(const long *V, long sizeV, const long*E, long sizeE, long*C)/*long *F, long*X, */
{
	int i = blockDim.x * blockIdx.x + threadIdx.x;
	if (i < sizeV)
	{
		if (C[i] == iteration)
		{
			//if (done)
			done = false;
			long pozP = V[i],
				pozK = i + 1 < sizeV ? V[i + 1] : sizeE;
			for (long j = pozP; j < pozK; j++)
			{
				long susjed = E[j];
				if (C[susjed]>iteration)
					C[susjed] = iteration + 1;
				//long j = C[susjed];
			}
		}
	}
}

__global__
void kernel_1_Edge(const long *V, long sizeV, const long*E, long sizeE, long*C)/*long *F, long*X, */
{
	int i = blockDim.x * blockIdx.x + threadIdx.x;
	if (i < sizeE)
	{
		long cvor = E[i];
		if (C[cvor] == iteration)
		{
			//if (done)
			done = false;
			for (long j = 0; j < sizeV; j++)
			{
				if (C[j] > iteration + 1)
					C[j] = iteration + 1;
			}
		}
	}
	//__syncthreads();
}

__global__
void kernel_1_Share(const long *V, long sizeV, const long*E, long sizeE, long*C)/*long *F, long*X, */
{
	//bool lokalDone = done;
	__device__ __shared__ long lokalIte;
	lokalIte = iteration;
		// = iteration;
	int i = blockDim.x * blockIdx.x + threadIdx.x;
	if (i < sizeV)
	{
		if (C[i] == lokalIte)
		{
			//if (done)
			//if (lokalDone)
			done = false;
			//if (doneI)
			//	atomicAnd(&doneI, 1);
			long pozP = V[i],
				pozK = i + 1 < sizeV ? V[i + 1] : sizeE;
			for (long j = pozP; j < pozK; j++)
			{
				long susjed = E[j];
				if (C[susjed]>lokalIte)
					C[susjed] = lokalIte + 1;
				//long j = C[susjed];
			}
		}
	}

}

__global__
void kernel_1_ShareAtomics(const long *V, long sizeV, const long*E, long sizeE, long*C)/*long *F, long*X, */
{
	//bool lokalDone = done;
	__device__ __shared__ long lokalIte;
	lokalIte = iteration;
	// = iteration;
	int i = blockDim.x * blockIdx.x + threadIdx.x;
	if (i < sizeV)
	{
		if (C[i] == lokalIte)
		{
			//if (done)
			//if (lokalDone)
			//done = false;
			if (doneI)
				atomicAnd(&doneI, 0);
			long pozP = V[i],
				pozK = i + 1 < sizeV ? V[i + 1] : sizeE;
			for (long j = pozP; j < pozK; j++)
			{
				long susjed = E[j];
				if (C[susjed]>lokalIte)
					C[susjed] = lokalIte + 1;
				//long j = C[susjed];
			}
		}
	}

}

__global__ 
void kernel_1_DynamicShare(const long *V, long sizeV, const long*E, long sizeE, long*C)
{
	__device__ __shared__ long lokalIte;
	lokalIte = iteration;
	// = iteration;
	int i = blockDim.x * blockIdx.x + threadIdx.x;
	if (i < sizeV)
	{
		if (C[i] == lokalIte)
		{
			done = false;
			//kernel call
			long pozP = V[i],
				pozK = i + 1 < sizeV ? V[i + 1] : sizeE;
		}
	}
}

__device__ 
void obradaSusjeda(const long*E,long sizeE,long*C,long sizeV,long pozP,long pozK)
{
	__device__ __shared__ long lokalIte;
	lokalIte = iteration;
	// = iteration;
	int i = blockDim.x * blockIdx.x + threadIdx.x;
	if (i < pozK && i>=pozP)
	{
		long susjed = E[i];
		if (C[susjed]>lokalIte)
			C[susjed] = lokalIte + 1;
	}
}

__global__
void kernel_1_Atomics(const long *V, long sizeV, const long*E, long sizeE, long*C)/*long *F, long*X, */
{
	//bool lokalDone = done;
	//__device__ __shared__ long lokalIte;
	//lokalIte = iteration;
	// = iteration;
	int i = blockDim.x * blockIdx.x + threadIdx.x;
	if (i < sizeV)
	{
		if (C[i] == iteration)
		{
			//if (done)
			//if (lokalDone)
			//done = false;
			if (doneI)
				atomicAnd(&doneI, 0);
			long pozP = V[i],
				pozK = i + 1 < sizeV ? V[i + 1] : sizeE;
			for (long j = pozP; j < pozK; j++)
			{
				long susjed = E[j];
				if (C[susjed]>iteration)
					C[susjed] = iteration + 1;
				//long j = C[susjed];
			}
		}
	}

}
