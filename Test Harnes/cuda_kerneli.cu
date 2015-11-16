
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
#include "cuda_profiler_api.h"
#define F(x) cout<<#x " = "<<x<<endl;

using namespace std;

ofstream _log1("inputArrayDebug.log");

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


//int main()
//{
//	/*size_t N = 10;
//	int *raw_ptr;
//	cudaMalloc((void**)&raw_ptr,N*sizeof(int));
//	thrust::device_ptr<int> dev_ptr(raw_ptr);
//	thrust::fill(dev_ptr, dev_ptr + N, (int)1);
//	int rez = thrust::reduce(dev_ptr, dev_ptr + N);
//	F(rez);
//	char t = getchar();
//	*/
//
//	//input grafa
//
//	long *h_V(NULL), *h_E(NULL),// *d_V(NULL), *d_E(NULL),
//		n(0), m(0);
//
//	//int *h_F(NULL), *h_X(NULL), *h_C(NULL);
//
//	size_t sizeV(0), sizeE(0);
//
//	cout << "Input grafa\n";
//	//ifstream in("_input/simple.prs");
//	////ifstream in("_input/ldoor.graph");
//	//ifstream in("_input/audikw1.graph");
//	ifstream in("_input/luxembourg.osm.graph");
//	//ifstream in("_input/ecology1.graph");
//	//ifstream in("input/kron_g500-simple-logn21.graph");
//	string line;
//	vector<long>tempV, tempE;
//	bool flagUnm = false;
//
//	long brI = 0, brJ = 0;
//	std::cout << "Input started: " << endl;
//	while (getline(in, line))
//	{
//		if (line[0] == '%')
//			continue;
//
//		if (!flagUnm)
//		{
//			flagUnm = true;
//			vector<long> temp(strURedBrojeva(line));
//			n = temp[0] + 1;
//			m = temp[1] + 1;
//
//			//sizeV = n * sizeof(long);
//			//sizeE = m * sizeof(long);
//			tempV.reserve(n);
//			tempE.reserve(m);
//			tempV.push_back(0);
//			brI++;
//			//h_V[brI++] = 0;
//			continue;
//		}
//		vector<long> temp = strURedBrojeva(line);
//		for (long i = 0; i < temp.size(); i++)
//		{
//			tempE.push_back(temp[i]);
//			brJ++;
//		}
//		if (tempV.size()<n)
//			tempV.push_back(tempE.size());
//	}
//	if (n == 0 || m == 0)
//	{
//		cout << "Input prazan\n";
//		cudaProfilerStop();
//		return 0;
//	}
//	//return 0;
//	sizeV = tempV.size()*sizeof(long); n = tempV.size();
//	sizeE = tempE.size()*sizeof(long); m = tempE.size();
//
//	h_V = (long *)malloc(sizeV);
//	std::copy(tempV.begin(), tempV.end(), h_V);
//	tempV.clear();
//
//	h_E = (long *)malloc(sizeE);
//	std::copy(tempE.begin(), tempE.end(), h_E);
//	tempE.clear();
//
//	in.close();
//	cout << "Input gotov" << endl;
//
//	//alokacija h_F, h_X i h_C
//	cout << "Alokacija host\n";
//	long *h_F(NULL), *h_X(NULL), *h_C(NULL);
//	h_F = (long*)malloc(sizeV);
//	h_X = (long*)malloc(sizeV);
//	h_C = (long*)malloc(sizeV);
//	memset(h_F, 0, sizeV);
//	memset(h_X, 0, sizeV);
//	memset(h_C, 127, sizeV);
//
//	//pocetne postavke za BFS
//	long pocetniCvor = 0;
//	h_F[pocetniCvor] = 1;
//	h_C[pocetniCvor] = 0;
//
//	//alokacija na device
//	cout << "Alokacija device\n";
//	long *d_E(NULL), *d_V(NULL),
//		*d_F(NULL), *d_X(NULL), *d_C(NULL);
//	cudaMalloc((void**)&d_E, sizeE);
//	cudaMalloc((void**)&d_V, sizeV);
//	cudaMalloc((void**)&d_F, sizeV);
//	cudaMalloc((void**)&d_X, sizeV);
//	cudaMalloc((void**)&d_C, sizeV);
//
//
//	//kopiranje na device
//	cout << "Kopiranje na device\n";
//	cudaMemcpy(d_E, h_E, sizeE, cudaMemcpyHostToDevice);
//	cudaMemcpy(d_V, h_V, sizeV, cudaMemcpyHostToDevice);
//	cudaMemcpy(d_F, h_F, sizeV, cudaMemcpyHostToDevice);
//	cudaMemcpy(d_X, h_X, sizeV, cudaMemcpyHostToDevice);
//	cudaMemcpy(d_C, h_C, sizeV, cudaMemcpyHostToDevice);
//
//	//potrebno za BFS
//	thrust::device_ptr<long> dev_ptr(d_F);
//	bool paralelno = true, seq = false;
//	cudaDeviceSynchronize();
//	if (paralelno)
//	{
//		cout << "pocinje BFS paralelni\n";
//		clock_t p1;
//		p1 = clock();
//		while (thrust::reduce(dev_ptr, dev_ptr + n))
//		{
//			//cout << "While petlja\n";
//			int threadsPerBlock = 256<n ? 256 : n;
//			int blocksPerGrid = (n + threadsPerBlock - 1) / threadsPerBlock;
//			//printf("CUDA kernel launch with %d blocks of %d threads\n", blocksPerGrid, threadsPerBlock);
//
//			prazno << <blocksPerGrid, threadsPerBlock >> >(d_V, sizeV, d_E, sizeE, d_F, d_X, d_C);
//			//vectorAdd << <blocksPerGrid, threadsPerBlock >> >(d_A, d_B, d_C, numElements);
//
//			//break;
//			//cudaDeviceSynchronize();
//		}
//		cudaDeviceSynchronize();
//		double diff = (double)(clock() - p1) / CLOCKS_PER_SEC;
//		cout << endl << ": The time taken for paralel Breadth first search: " << diff << endl;
//		//cout << "Copy C to host\n";
//		//cudaMemcpy(h_C, d_C, sizeV, cudaMemcpyDeviceToHost);
//		//copy(h_C, h_C + n, ostream_iterator<long>(cout, ","));
//	}
//	cout << "\nOslobadjanje host\n";
//	free(h_E);
//	free(h_V);
//	free(h_F);
//	free(h_X);
//	free(h_C);
//
//	cout << "Oslobadjanje device\n";
//	cudaFree(d_E);
//	cudaFree(d_V);
//	cudaFree(d_F);
//	cudaFree(d_X);
//	cudaFree(d_C);
//	cout << "\nDone!!!!!!!!!!!!!" << endl;
//	//char t = getchar();
//	cudaProfilerStop();
//	return 0;
//}
