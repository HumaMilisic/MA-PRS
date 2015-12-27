
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
#include <windows.h>
#include "kerneli.cuh"
#define F(x) cout<<#x " = "<<x<<endl;

using namespace std;


ofstream _log1("costArrayProvjeraDebug.log");



extern "C" void uhvatiNiz(long *niz, long size)
{
	cout << "uhvati niz\n";
	copy(niz, niz + size, ostream_iterator<long>(cout, ", "));
}

extern "C" double paralelniBFS(long *h_V, long *h_E, long sizeV, long sizeE)
{
	cout << "paralelniBFS" << endl;
	
	cout << "Alokacija host\n";
	long *h_F(NULL), *h_X(NULL), *h_C(NULL);
	h_F = (long*)malloc(sizeV*sizeof(long));
	h_X = (long*)malloc(sizeV*sizeof(long));
	h_C = (long*)malloc(sizeV*sizeof(long));
	memset(h_F, 0, sizeV*sizeof(long));
	memset(h_X, 0, sizeV*sizeof(long));
	memset(h_C, 127, sizeV*sizeof(long));

	//pocetne postavke za BFS
	long pocetniCvor = 0;
	h_F[pocetniCvor] = 1;
	h_C[pocetniCvor] = 0;
	
	//alokacija na device
	cout << "Alokacija device\n";
	long *d_E(NULL), *d_V(NULL),
		*d_F(NULL), *d_X(NULL), *d_C(NULL);
	cudaMalloc((void**)&d_E, sizeE*sizeof(long));
	cudaMalloc((void**)&d_V, sizeV*sizeof(long));
	cudaMalloc((void**)&d_F, sizeV*sizeof(long));
	cudaMalloc((void**)&d_X, sizeV*sizeof(long));
	cudaMalloc((void**)&d_C, sizeV*sizeof(long));

	//kopiranje na device
	cout << "Kopiranje na device\n";
	cudaMemcpy(d_E, h_E, sizeE*sizeof(long), cudaMemcpyHostToDevice);
	cudaMemcpy(d_V, h_V, sizeV*sizeof(long), cudaMemcpyHostToDevice);
	cudaMemcpy(d_F, h_F, sizeV*sizeof(long), cudaMemcpyHostToDevice);
	cudaMemcpy(d_X, h_X, sizeV*sizeof(long), cudaMemcpyHostToDevice);
	cudaMemcpy(d_C, h_C, sizeV*sizeof(long), cudaMemcpyHostToDevice);


	//potrebno za BFS
	thrust::device_ptr<long> dev_ptr(d_F);
	//bool paralelno = true, seq = false;
	cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	cudaDeviceSynchronize();

	cout << "pocinje BFS paralelni\n";
	clock_t p1;
	p1 = clock();
	cudaEventRecord(start);
	while (thrust::reduce(dev_ptr, dev_ptr + sizeV))
	{
		//cout << "While petlja\n";
		int threadsPerBlock = 256<sizeV ? 256 : sizeV;
		int blocksPerGrid = (sizeV + threadsPerBlock - 1) / threadsPerBlock;
		//printf("CUDA kernel launch with %d blocks of %d threads\n", blocksPerGrid, threadsPerBlock);
		
		prazno << <blocksPerGrid, threadsPerBlock >> >(d_V, sizeV, d_E, sizeE, d_F, d_X, d_C);
		
		//vectorAdd << <blocksPerGrid, threadsPerBlock >> >(d_A, d_B, d_C, numElements);
		//break;
		//cudaDeviceSynchronize();
	}
	cudaEventRecord(stop);
	double diff = (double)(clock() - p1) / CLOCKS_PER_SEC;
	//cudaDeviceSynchronize();
	cudaEventSynchronize(stop);
	float milliseconds = 0;
	double sec;
	cudaEventElapsedTime(&milliseconds, start, stop);
	sec = milliseconds / 1000.0;
	//_log1  << endl << ": The time taken for paralel Breadth first search: " << diff << endl;
	cout << endl << ": The time taken for paralel Breadth first search: " << diff << endl;
	cout << endl << ": event: " << sec << " s\n";
	cout << "Copy C to host\n";
	cudaMemcpy(h_C, d_C, sizeV*sizeof(long), cudaMemcpyDeviceToHost);
	_log1 << " ; " << endl;
	copy(h_C, h_C + sizeV, ostream_iterator<long>(_log1, " "));
	_log1 << endl;

	cout << "Oslobadjanje memorije\n";
	cudaFree(d_E);
	cudaFree(d_V);
	cudaFree(d_F);
	cudaFree(d_X);
	cudaFree(d_C);

	free(h_F);
	free(h_X);
	free(h_C);

	return sec;
}

extern "C" double paralelniBFS_64(long *h_V, long *h_E, long sizeV, long sizeE)
{
	cout << "paralelniBFS" << endl;

	cout << "Alokacija host\n";
	long *h_F(NULL), *h_X(NULL), *h_C(NULL);
	h_F = (long*)malloc(sizeV*sizeof(long));
	h_X = (long*)malloc(sizeV*sizeof(long));
	h_C = (long*)malloc(sizeV*sizeof(long));
	memset(h_F, 0, sizeV*sizeof(long));
	memset(h_X, 0, sizeV*sizeof(long));
	memset(h_C, 127, sizeV*sizeof(long));

	//pocetne postavke za BFS
	long pocetniCvor = 0;
	h_F[pocetniCvor] = 1;
	h_C[pocetniCvor] = 0;

	//alokacija na device
	cout << "Alokacija device\n";
	long *d_E(NULL), *d_V(NULL),
		*d_F(NULL), *d_X(NULL), *d_C(NULL);
	cudaMalloc((void**)&d_E, sizeE*sizeof(long));
	cudaMalloc((void**)&d_V, sizeV*sizeof(long));
	cudaMalloc((void**)&d_F, sizeV*sizeof(long));
	cudaMalloc((void**)&d_X, sizeV*sizeof(long));
	cudaMalloc((void**)&d_C, sizeV*sizeof(long));

	//kopiranje na device
	cout << "Kopiranje na device\n";
	cudaMemcpy(d_E, h_E, sizeE*sizeof(long), cudaMemcpyHostToDevice);
	cudaMemcpy(d_V, h_V, sizeV*sizeof(long), cudaMemcpyHostToDevice);
	cudaMemcpy(d_F, h_F, sizeV*sizeof(long), cudaMemcpyHostToDevice);
	cudaMemcpy(d_X, h_X, sizeV*sizeof(long), cudaMemcpyHostToDevice);
	cudaMemcpy(d_C, h_C, sizeV*sizeof(long), cudaMemcpyHostToDevice);


	//potrebno za BFS
	thrust::device_ptr<long> dev_ptr(d_F);
	//bool paralelno = true, seq = false;
	cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	cudaDeviceSynchronize();

	cout << "pocinje BFS paralelni\n";
	clock_t p1;
	p1 = clock();
	cudaEventRecord(start);
	while (thrust::reduce(dev_ptr, dev_ptr + sizeV))
	{
		//cout << "While petlja\n";
		int m = 65;
		int threadsPerBlock = m<sizeV ? m : sizeV;
		int blocksPerGrid = (sizeV + threadsPerBlock - 1) / threadsPerBlock;
		//printf("CUDA kernel launch with %d blocks of %d threads\n", blocksPerGrid, threadsPerBlock);

		prazno << <blocksPerGrid, threadsPerBlock >> >(d_V, sizeV, d_E, sizeE, d_F, d_X, d_C);

		//vectorAdd << <blocksPerGrid, threadsPerBlock >> >(d_A, d_B, d_C, numElements);
		//break;
		//cudaDeviceSynchronize();
	}
	cudaEventRecord(stop);
	double diff = (double)(clock() - p1) / CLOCKS_PER_SEC;
	//cudaDeviceSynchronize();
	cudaEventSynchronize(stop);
	float milliseconds = 0;
	double sec;
	cudaEventElapsedTime(&milliseconds, start, stop);
	sec = milliseconds / 1000.0;
	//_log1  << endl << ": The time taken for paralel Breadth first search: " << diff << endl;
	cout << endl << ": The time taken for paralel Breadth first search: " << diff << endl;
	cout << endl << ": event: " << sec << " s\n";
	//cout << "Copy C to host\n";
	//cudaMemcpy(h_C, d_C, sizeV*sizeof(long), cudaMemcpyDeviceToHost);
	//copy(h_C, h_C + sizeV, ostream_iterator<long>(cout, ","));


	cout << "Oslobadjanje memorije\n";
	cudaFree(d_E);
	cudaFree(d_V);
	cudaFree(d_F);
	cudaFree(d_X);
	cudaFree(d_C);

	free(h_F);
	free(h_X);
	free(h_C);

	return sec;
}



extern "C" double paralelniBFS_1(long *h_V, long *h_E, long sizeV, long sizeE)
{
	cout << "paralelniBFS" << endl;

	cout << "Alokacija host\n";
	long *h_C(NULL);
	h_C = (long*)malloc(sizeV*sizeof(long));
	memset(h_C, 127, sizeV*sizeof(long));

	//pocetne postavke za BFS
	long pocetniCvor = 0;
	//h_F[pocetniCvor] = 1;
	h_C[pocetniCvor] = 0;

	//alokacija na device
	cout << "Alokacija device\n";
	long *d_E(NULL), *d_V(NULL),*d_C(NULL);
	cudaMalloc((void**)&d_E, sizeE*sizeof(long));
	cudaMalloc((void**)&d_V, sizeV*sizeof(long));
	cudaMalloc((void**)&d_C, sizeV*sizeof(long));

	//kopiranje na device
	cout << "Kopiranje na device\n";
	cudaMemcpy(d_E, h_E, sizeE*sizeof(long), cudaMemcpyHostToDevice);
	cudaMemcpy(d_V, h_V, sizeV*sizeof(long), cudaMemcpyHostToDevice);
	cudaMemcpy(d_C, h_C, sizeV*sizeof(long), cudaMemcpyHostToDevice);


	//potrebno za BFS
	iteration = 0;
	cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	cudaDeviceSynchronize();

	cout << "pocinje BFS paralelni\n";
	clock_t p1;
	p1 = clock();
	cudaEventRecord(start);
	do
	{
		done = true;
		int threadsPerBlock = 256<sizeV ? 256 : sizeV;
		int blocksPerGrid = (sizeV + threadsPerBlock - 1) / threadsPerBlock;
		//printf("CUDA kernel launch with %d blocks of %d threads\n", blocksPerGrid, threadsPerBlock);

		kernel_1 << <blocksPerGrid, threadsPerBlock >> >(d_V, sizeV, d_E, sizeE, d_C);
		
		cudaDeviceSynchronize();
		iteration++;
	} while (!done);
	cudaEventRecord(stop);
	double diff = (double)(clock() - p1) / CLOCKS_PER_SEC;
	//cudaDeviceSynchronize();
	cudaEventSynchronize(stop);
	float milliseconds = 0;
	double sec;
	cudaEventElapsedTime(&milliseconds, start, stop);
	sec = milliseconds / 1000.0;
	//_log1  << endl << ": The time taken for paralel Breadth first search: " << diff << endl;
	cout << endl << ": The time taken for paralel Breadth first search: " << diff << endl;
	cout << endl << ": event: " << sec << " s\n";
	cout << "Copy C to host\n";
	cudaMemcpy(h_C, d_C, sizeV*sizeof(long), cudaMemcpyDeviceToHost);
	_log1 << " ; "<< endl;
	copy(h_C, h_C + sizeV, ostream_iterator<long>(_log1, " "));
	_log1 <<  endl;

	cout << "Oslobadjanje memorije\n";
	cudaFree(d_E);
	cudaFree(d_V);
	cudaFree(d_C);

	free(h_C);
	cout << "done" << endl;
	_log1.close();
	return sec;
}

extern "C" double paralelniBFS_1_Share(long *h_V, long *h_E, long sizeV, long sizeE)
{
	cout << "paralelniBFS" << endl;

	cout << "Alokacija host\n";
	long *h_C(NULL);
	h_C = (long*)malloc(sizeV*sizeof(long));
	memset(h_C, 127, sizeV*sizeof(long));

	//pocetne postavke za BFS
	long pocetniCvor = 0;
	//h_F[pocetniCvor] = 1;
	h_C[pocetniCvor] = 0;

	//alokacija na device
	cout << "Alokacija device\n";
	long *d_E(NULL), *d_V(NULL), *d_C(NULL);
	cudaMalloc((void**)&d_E, sizeE*sizeof(long));
	cudaMalloc((void**)&d_V, sizeV*sizeof(long));
	cudaMalloc((void**)&d_C, sizeV*sizeof(long));

	//kopiranje na device
	cout << "Kopiranje na device\n";
	cudaMemcpy(d_E, h_E, sizeE*sizeof(long), cudaMemcpyHostToDevice);
	cudaMemcpy(d_V, h_V, sizeV*sizeof(long), cudaMemcpyHostToDevice);
	cudaMemcpy(d_C, h_C, sizeV*sizeof(long), cudaMemcpyHostToDevice);


	//potrebno za BFS
	iteration = 0;
	cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	cudaDeviceSynchronize();

	cout << "pocinje BFS paralelni\n";
	clock_t p1;
	p1 = clock();
	cudaEventRecord(start);
	do
	{
		done = true;
		//doneI = 0;
		int threadsPerBlock = 256<sizeV ? 256 : sizeV;
		int blocksPerGrid = (sizeV + threadsPerBlock - 1) / threadsPerBlock;
		//printf("CUDA kernel launch with %d blocks of %d threads\n", blocksPerGrid, threadsPerBlock);
		//kernel_1_Share<< <blocksPerGrid,threadsPerBlock> >>(d_V, sizeV, d_E, sizeE, d_C);
		kernel_1_Share << <blocksPerGrid, threadsPerBlock >> >(d_V, sizeV, d_E, sizeE, d_C);
		
		cudaDeviceSynchronize();
		iteration++;
	} while (!doneI);
	cudaEventRecord(stop);
	double diff = (double)(clock() - p1) / CLOCKS_PER_SEC;
	//cudaDeviceSynchronize();
	cudaEventSynchronize(stop);
	float milliseconds = 0;
	double sec;
	cudaEventElapsedTime(&milliseconds, start, stop);
	sec = milliseconds / 1000.0;
	//_log1  << endl << ": The time taken for paralel Breadth first search: " << diff << endl;
	cout << endl << ": The time taken for paralel Breadth first search: " << diff << endl;
	cout << endl << ": event: " << sec << " s\n";
	cout << "Copy C to host\n";
	cudaMemcpy(h_C, d_C, sizeV*sizeof(long), cudaMemcpyDeviceToHost);
	_log1 << " ; "<< endl;
	copy(h_C, h_C + sizeV, ostream_iterator<long>(_log1, " "));
	_log1 <<  endl;

	cout << "Oslobadjanje memorije\n";
	cudaFree(d_E);
	cudaFree(d_V);
	cudaFree(d_C);

	free(h_C);
	cout << "done" << endl;
	_log1.close();
	return sec;
}

extern "C" double paralelniBFS_1_ShareAtomics(long *h_V, long *h_E, long sizeV, long sizeE)
{
	cout << "paralelniBFS" << endl;

	cout << "Alokacija host\n";
	long *h_C(NULL);
	h_C = (long*)malloc(sizeV*sizeof(long));
	memset(h_C, 127, sizeV*sizeof(long));

	//pocetne postavke za BFS
	long pocetniCvor = 0;
	//h_F[pocetniCvor] = 1;
	h_C[pocetniCvor] = 0;

	//alokacija na device
	cout << "Alokacija device\n";
	long *d_E(NULL), *d_V(NULL), *d_C(NULL);
	cudaMalloc((void**)&d_E, sizeE*sizeof(long));
	cudaMalloc((void**)&d_V, sizeV*sizeof(long));
	cudaMalloc((void**)&d_C, sizeV*sizeof(long));

	//kopiranje na device
	cout << "Kopiranje na device\n";
	cudaMemcpy(d_E, h_E, sizeE*sizeof(long), cudaMemcpyHostToDevice);
	cudaMemcpy(d_V, h_V, sizeV*sizeof(long), cudaMemcpyHostToDevice);
	cudaMemcpy(d_C, h_C, sizeV*sizeof(long), cudaMemcpyHostToDevice);


	//potrebno za BFS
	iteration = 0;
	cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	cudaDeviceSynchronize();

	cout << "pocinje BFS paralelni\n";
	clock_t p1;
	p1 = clock();
	cudaEventRecord(start);
	do
	{
		//done = true;
		doneI = 1;
		int threadsPerBlock = 256<sizeV ? 256 : sizeV;
		int blocksPerGrid = (sizeV + threadsPerBlock - 1) / threadsPerBlock;
		//printf("CUDA kernel launch with %d blocks of %d threads\n", blocksPerGrid, threadsPerBlock);
		//kernel_1_Share<< <blocksPerGrid,threadsPerBlock> >>(d_V, sizeV, d_E, sizeE, d_C);
		kernel_1_ShareAtomics<< <blocksPerGrid, threadsPerBlock >> >(d_V, sizeV, d_E, sizeE, d_C);
		
		cudaDeviceSynchronize();
		iteration++;
	} while (!doneI);
	cudaEventRecord(stop);
	double diff = (double)(clock() - p1) / CLOCKS_PER_SEC;
	//cudaDeviceSynchronize();
	cudaEventSynchronize(stop);
	float milliseconds = 0;
	double sec;
	cudaEventElapsedTime(&milliseconds, start, stop);
	sec = milliseconds / 1000.0;
	//_log1  << endl << ": The time taken for paralel Breadth first search: " << diff << endl;
	cout << endl << ": The time taken for paralel Breadth first search: " << diff << endl;
	cout << endl << ": event: " << sec << " s\n";
	cout << "Copy C to host\n";
	cudaMemcpy(h_C, d_C, sizeV*sizeof(long), cudaMemcpyDeviceToHost);
	_log1 << " ; " << endl;
	copy(h_C, h_C + sizeV, ostream_iterator<long>(_log1, " "));
	_log1 << endl;

	cout << "Oslobadjanje memorije\n";
	cudaFree(d_E);
	cudaFree(d_V);
	cudaFree(d_C);

	free(h_C);
	cout << "done" << endl;
	_log1.close();
	return sec;
}

extern "C" double paralelniBFSEdge(long *h_V, long *h_E, long sizeV, long sizeE)
{
	cout << "paralelniBFS" << endl;

	cout << "Alokacija host\n";
	long *h_F(NULL), *h_X(NULL), *h_C(NULL);
	h_F = (long*)malloc(sizeV*sizeof(long));
	h_X = (long*)malloc(sizeV*sizeof(long));
	h_C = (long*)malloc(sizeV*sizeof(long));
	memset(h_F, 0, sizeV*sizeof(long));
	memset(h_X, 0, sizeV*sizeof(long));
	memset(h_C, 127, sizeV*sizeof(long));

	//pocetne postavke za BFS
	long pocetniCvor = 0;
	h_F[pocetniCvor] = 1;
	h_C[pocetniCvor] = 0;

	//alokacija na device
	cout << "Alokacija device\n";
	long *d_E(NULL), *d_V(NULL),
		*d_F(NULL), *d_X(NULL), *d_C(NULL);
	cudaMalloc((void**)&d_E, sizeE*sizeof(long));
	cudaMalloc((void**)&d_V, sizeV*sizeof(long));
	cudaMalloc((void**)&d_F, sizeV*sizeof(long));
	cudaMalloc((void**)&d_X, sizeV*sizeof(long));
	cudaMalloc((void**)&d_C, sizeV*sizeof(long));

	//kopiranje na device
	cout << "Kopiranje na device\n";
	cudaMemcpy(d_E, h_E, sizeE*sizeof(long), cudaMemcpyHostToDevice);
	cudaMemcpy(d_V, h_V, sizeV*sizeof(long), cudaMemcpyHostToDevice);
	cudaMemcpy(d_F, h_F, sizeV*sizeof(long), cudaMemcpyHostToDevice);
	cudaMemcpy(d_X, h_X, sizeV*sizeof(long), cudaMemcpyHostToDevice);
	cudaMemcpy(d_C, h_C, sizeV*sizeof(long), cudaMemcpyHostToDevice);


	//potrebno za BFS
	thrust::device_ptr<long> dev_ptr(d_F);
	//bool paralelno = true, seq = false;
	cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	cudaDeviceSynchronize();

	cout << "pocinje BFS paralelni\n";
	clock_t p1;
	p1 = clock();
	cudaEventRecord(start);
	while (thrust::reduce(dev_ptr, dev_ptr + sizeV))
	{
		//cout << "While petlja\n";
		int threadsPerBlock = 256<sizeE ? 256 : sizeE;
		int blocksPerGrid = (sizeE + threadsPerBlock - 1) / threadsPerBlock;
		//printf("CUDA kernel launch with %d blocks of %d threads\n", blocksPerGrid, threadsPerBlock);
		edge << <blocksPerGrid, threadsPerBlock >> >(d_V, sizeV, d_E, sizeE, d_F, d_X, d_C);

		//vectorAdd << <blocksPerGrid, threadsPerBlock >> >(d_A, d_B, d_C, numElements);
		//break;
		//cudaDeviceSynchronize();
	}
	cudaEventRecord(stop);
	double diff = (double)(clock() - p1) / CLOCKS_PER_SEC;
	//cudaDeviceSynchronize();
	cudaEventSynchronize(stop);
	float milliseconds = 0;
	double sec;
	cudaEventElapsedTime(&milliseconds, start, stop);
	sec = milliseconds / 1000.0;
	//_log1  << endl << ": The time taken for paralel Breadth first search: " << diff << endl;
	cout << endl << ": The time taken for paralel Breadth first search: " << diff << endl;
	cout << endl << ": event: " << sec << " s\n";
	cout << "Copy C to host\n";
	cudaMemcpy(h_C, d_C, sizeV*sizeof(long), cudaMemcpyDeviceToHost);
	_log1 << " ; " << endl;
	copy(h_C, h_C + sizeV, ostream_iterator<long>(_log1, " "));
	_log1 << endl;

	cout << "Oslobadjanje memorije\n";
	cudaFree(d_E);
	cudaFree(d_V);
	cudaFree(d_F);
	cudaFree(d_X);
	cudaFree(d_C);

	free(h_F);
	free(h_X);
	free(h_C);

	return sec;
}

extern "C" double paralelniBFS_1_Edge(long *h_V, long *h_E, long sizeV, long sizeE)
{
	cout << "paralelniBFS" << endl;

	cout << "Alokacija host\n";
	long *h_C(NULL);
	h_C = (long*)malloc(sizeV*sizeof(long));
	memset(h_C, 127, sizeV*sizeof(long));

	//pocetne postavke za BFS
	long pocetniCvor = 0;
	//h_F[pocetniCvor] = 1;
	h_C[pocetniCvor] = 0;

	//alokacija na device
	cout << "Alokacija device\n";
	long *d_E(NULL), *d_V(NULL),*d_C(NULL);
	cudaMalloc((void**)&d_E, sizeE*sizeof(long));
	cudaMalloc((void**)&d_V, sizeV*sizeof(long));
	cudaMalloc((void**)&d_C, sizeV*sizeof(long));

	//kopiranje na device
	cout << "Kopiranje na device\n";
	cudaMemcpy(d_E, h_E, sizeE*sizeof(long), cudaMemcpyHostToDevice);
	cudaMemcpy(d_V, h_V, sizeV*sizeof(long), cudaMemcpyHostToDevice);
	cudaMemcpy(d_C, h_C, sizeV*sizeof(long), cudaMemcpyHostToDevice);


	//potrebno za BFS
	iteration = 0;
	cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	cudaDeviceSynchronize();

	cout << "pocinje BFS paralelni\n";
	clock_t p1;
	p1 = clock();
	cudaEventRecord(start);
	do
	{
		done = true;
		int threadsPerBlock = 256<sizeE ? 256 : sizeE;
		int blocksPerGrid = (sizeE + threadsPerBlock - 1) / threadsPerBlock;
		//printf("CUDA kernel launch with %d blocks of %d threads\n", blocksPerGrid, threadsPerBlock);
		kernel_1_Edge<< <blocksPerGrid, threadsPerBlock >> >(d_V, sizeV, d_E, sizeE, d_C);

		cudaDeviceSynchronize();
		iteration++;
	} while (!done);
	cudaEventRecord(stop);
	double diff = (double)(clock() - p1) / CLOCKS_PER_SEC;
	//cudaDeviceSynchronize();
	cudaEventSynchronize(stop);
	float milliseconds = 0;
	double sec;
	cudaEventElapsedTime(&milliseconds, start, stop);
	sec = milliseconds / 1000.0;
	//_log1  << endl << ": The time taken for paralel Breadth first search: " << diff << endl;
	cout << endl << ": The time taken for paralel Breadth first search: " << diff << endl;
	cout << endl << ": event: " << sec << " s\n";
	cout << "Copy C to host\n";
	cudaMemcpy(h_C, d_C, sizeV*sizeof(long), cudaMemcpyDeviceToHost);
	_log1 << " ; " << endl;
	copy(h_C, h_C + sizeV, ostream_iterator<long>(_log1, " "));
	_log1 << endl;

	cout << "Oslobadjanje memorije\n";
	cudaFree(d_E);
	cudaFree(d_V);
	cudaFree(d_C);

	free(h_C);
	cout << "done" << endl;
	_log1.close();
	return sec;
}
