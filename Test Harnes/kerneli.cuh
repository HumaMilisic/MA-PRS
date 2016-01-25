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
//#define cucheck_dev(call) { cudaError_t cucheck_err = (call); if (cucheck_err != cudaSuccess) 	{		const char *err_str = cudaGetErrorString(cucheck_err);  			printf("%s (%d): %s\n", __FILE__, __LINE__, err_str);   			assert(0);	}}

__device__  __managed__ bool done = true;
__device__  __managed__ int doneI = 1;
__device__ __managed__  long iteration = 0;
__device__ __managed__ long currentSize;
__device__ __managed__ long maxSize;

//__global__ 
//void kernel_Red(const long *V,long sizeV,const long *E,long sizeE)
//{
//	int i = blockDim.x * blockIdx.x + threadIdx.x;
//	if (i<sizeV)
//}

//__global__
//void testRedPush(long *red)
//{
//	int i = threadIdx.x;
//	pushAtomic(i, red);
//}
//
//__device__
//void pushAtomic(long value,long *red)
//{
//	/*long trenutni = currentSize++;
//	red[trenutni] = value;*/
//	long temp = atomicInc(&currentSize, maxSize);
//	red[temp] = value;
//}


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
			if (done)
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
			if (done)
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
			//doneI &= 0;
			if (doneI)		
				atomicAnd(&doneI, 0);
			long pozP = V[i],
				pozK = i + 1 < sizeV ? V[i + 1] : sizeE;
			for (long j = pozP; j < pozK; j++)
			{
				long susjed = E[j];
				if (C[susjed]>iteration)
					C[susjed] = iteration + 1;
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
void obradaSusjeda(long offset,const long*E,long sizeE,long pozP,long pozK,long *C)//const long*E, long sizeE, long*C, long sizeV, long pozP, long pozK)
{
	int i = blockDim.x * blockIdx.x + threadIdx.x;
	//i += offset;
	if (i >= pozP && i < pozK)
	{
		long susjed = E[i];
		if (C[susjed] > iteration)
			C[susjed] = iteration + 1;
	}
}

__global__ 
void kernel_1_Dynamic(const long *V, long sizeV, const long*E, long sizeE, long*C)
{
	//__device__ __shared__ long lokalIte;
	//lokalIte = iteration;
	// = iteration;
	int i = blockDim.x * blockIdx.x + threadIdx.x;
	if (i < sizeV)
	{
		if (C[i] == iteration)
		{
			done = false;
			//kernel call
			long pozP = V[i],
				pozK = i + 1 < sizeV ? V[i + 1] : sizeE;
			long offset = pozK - pozP;
			//int threadsPerBlock = 256<pozK ? 256 : pozK;
			//int blocksPerGrid = (sizeV + threadsPerBlock - 1) / threadsPerBlock;
			//int threadsPerBlock = 256 < (pozK - offset) ? 256 : (pozK - offset);
			//int blocksPerGRid = (pozK - offset + threadsPerBlock-1) / threadsPerBlock;
			int threadsPerBlock = 256 < pozK ? 256 : pozK;
			int blocksPerGRid = (pozK + threadsPerBlock - 1) / threadsPerBlock;
			__syncthreads();
			obradaSusjeda<< <blocksPerGRid,threadsPerBlock>> >(offset,E,sizeE,pozP,pozK,C);
			cudaDeviceSynchronize();
			__syncthreads();
			//obradaSusjeda<< < blocksPerGRid, threadsPerBlock > >>(E, sizeE, C, sizeV, pozP, pozK);

			//obradaSusjeda<< <blocksPerGRid,threadsPerBlock> >>(E, sizeE, C, V, pozP, pozK);
		}
	}
}

__global__ void child_launch(int *data) 
{ 
	data[threadIdx.x] = data[threadIdx.x] + 1; 
} 
__global__ void parent_launch(int *data) 
{ 
	data[threadIdx.x] = threadIdx.x; 
	__syncthreads(); 
	if (threadIdx.x == 0) 
	{ 
		child_launch << < 1, 256 >> >(data); 
		cudaDeviceSynchronize(); 
	} 
	__syncthreads(); 
}


