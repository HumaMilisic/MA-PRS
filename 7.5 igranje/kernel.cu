
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
#define F(x) cout<<#x " = "<<x<<endl;

using namespace std;

long strToBr(string line)
{
	long br = 0;
	long size = line.size();
	//F(size);
	//F(pow(10, 3));
	for (long i = 0; i < line.size(); i++)
	{
		char a = line[i];
		a -= '0';
		//F((long)a);
		br += a*pow(10, size - 1);
		size--;
	}
	//F(br);
	br--;
	return br;
}

vector<string> token(string line)
{
	vector<string>tempNiz;
	string temp = "";
	for (long i = 0; i < line.size(); i++)
	{
		if (line[i] == ' ')
		{
			tempNiz.push_back(temp);
			temp = "";
			continue;
		}
		temp += line[i];
	}
	//if (temp != "")
	tempNiz.push_back(temp);
	return tempNiz;
}

vector<long> strURedBrojeva(string line)
{
	vector<long>rez;
	vector<string>temp = token(line);
	for (long i = 0; i < temp.size(); i++)
	{
		//if (temp[i] != "")
		int br = strToBr(temp[i]);
		if (br >= 0)	rez.push_back(br);
	}
	return rez;
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
void prazno(const long *V,long sizeV,const long*E,long sizeE,long *F,long*X,long*C)
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

void sekvencijalno(const vector<bool>&graf, int n, long i=0)
{
	queue<long>redCvorova;
	vector<long>redoslijed;
	vector<bool>posjecenCvor;
	vector<bool>dodanURedCvor;
	posjecenCvor.resize(n, false);
	dodanURedCvor.resize(n, false);
	redCvorova.push(0);
	dodanURedCvor[0] = true;

	clock_t t1;
	t1 = clock();
	long brPosjecenih = 0;
	redoslijed.push_back(0);
	while (redCvorova.size() > 0 && brPosjecenih < n)
	{
		long trenutniCvor = redCvorova.front();
		redCvorova.pop();
		brPosjecenih++;

		if (posjecenCvor[trenutniCvor])
			continue;

		posjecenCvor[trenutniCvor] = true;
		//i = trenutni cvor
		for (int j = 0; j < n; j++)
		{
			int index = trenutniCvor*n + j;
			if (!graf[index])
				continue;
			if (posjecenCvor[j] || dodanURedCvor[j])
				continue;
			redCvorova.push(j);
			dodanURedCvor[j] = true;
			redoslijed.push_back(j);
			//if (graf[index])
		}
	}
	double diff = (double)(clock() - t1) / CLOCKS_PER_SEC;
	cout << endl << i << ": The time taken for Breadth first search: " << diff << endl;
	//FFC(redoslijed)
}


int main()
{
	/*size_t N = 10;
	int *raw_ptr;
	cudaMalloc((void**)&raw_ptr,N*sizeof(int));
	thrust::device_ptr<int> dev_ptr(raw_ptr);
	thrust::fill(dev_ptr, dev_ptr + N, (int)1);
	int rez = thrust::reduce(dev_ptr, dev_ptr + N);
	F(rez);
	char t = getchar();
	*/
	
	//input grafa

	long *h_V(NULL), *h_E(NULL),// *d_V(NULL), *d_E(NULL),
		n(0), m(0);

	//int *h_F(NULL), *h_X(NULL), *h_C(NULL);

	size_t sizeV(0), sizeE(0);

	cout << "Input grafa\n";
	ifstream in("_input/simple.prs");
	//ifstream in("_input/luxembourg.osm.graph");
	//ifstream in("_input/ecology1.graph");
	//ifstream in("input/kron_g500-simple-logn21.graph");
	string line;
	vector<long>tempV, tempE;
	bool flagUnm = false;

	long brI = 0, brJ = 0;
	std::cout << "Input started: " << endl;
	while (getline(in, line))
	{
		if (line[0] == '%')
			continue;

		if (!flagUnm)
		{
			flagUnm = true;
			vector<long> temp(strURedBrojeva(line));
			n = temp[0] + 1;
			m = temp[1] + 1;

			//sizeV = n * sizeof(long);
			//sizeE = m * sizeof(long);
			tempV.reserve(n);
			tempE.reserve(m);
			tempV.push_back(0);
			brI++;
			//h_V[brI++] = 0;
			continue;
		}
		vector<long> temp = strURedBrojeva(line);
		for (long i = 0; i < temp.size(); i++)
		{
			tempE.push_back(temp[i]);
			brJ++;
		}
		if (tempV.size()<n)
			tempV.push_back(tempE.size());
	}

	sizeV = tempV.size()*sizeof(long); n = tempV.size();
	sizeE = tempE.size()*sizeof(long); m = tempE.size();

	h_V = (long *)malloc(sizeV);
	std::copy(tempV.begin(), tempV.end(), h_V);
	tempV.clear();

	h_E = (long *)malloc(sizeE);
	std::copy(tempE.begin(), tempE.end(), h_E);
	tempE.clear();

	in.close();
	cout << "Input gotov" << endl;

	//alokacija h_F, h_X i h_C
	cout << "Alokacija host\n";
	long *h_F(NULL), *h_X(NULL), *h_C(NULL);
	h_F = (long*)malloc(sizeV);
	h_X = (long*)malloc(sizeV);
	h_C = (long*)malloc(sizeV);
	memset(h_F, 0, sizeV);
	memset(h_X, 0, sizeV);
	memset(h_C, 127, sizeV);

	//pocetne postavke za BFS
	long pocetniCvor = 0;
	h_F[pocetniCvor] = 1;
	h_C[pocetniCvor] = 0;

	//alokacija na device
	cout << "Alokacija device\n";
	long *d_E(NULL), *d_V(NULL), 
		*d_F(NULL), *d_X(NULL), *d_C(NULL);
	cudaMalloc((void**)&d_E, sizeE);
	cudaMalloc((void**)&d_V, sizeV);
	cudaMalloc((void**)&d_F, sizeV);
	cudaMalloc((void**)&d_X, sizeV);
	cudaMalloc((void**)&d_C, sizeV);


	//kopiranje na device
	cout << "Kopiranje na device\n";
	cudaMemcpy(d_E, h_E, sizeE, cudaMemcpyHostToDevice);
	cudaMemcpy(d_V, h_V, sizeV, cudaMemcpyHostToDevice);
	cudaMemcpy(d_F, h_F, sizeV, cudaMemcpyHostToDevice);
	cudaMemcpy(d_X, h_X, sizeV, cudaMemcpyHostToDevice);
	cudaMemcpy(d_C, h_C, sizeV, cudaMemcpyHostToDevice);

	//potrebno za BFS
	thrust::device_ptr<long> dev_ptr(d_F);
	cudaDeviceSynchronize();
	cout << "pocinje BFS paralelni\n";
	while (thrust::reduce(dev_ptr, dev_ptr + n))
	{
		//cout << "While petlja\n";
		int threadsPerBlock = 256<n?256:n;
		int blocksPerGrid = (n + threadsPerBlock - 1) / threadsPerBlock;
		//printf("CUDA kernel launch with %d blocks of %d threads\n", blocksPerGrid, threadsPerBlock);

		prazno <<<blocksPerGrid,threadsPerBlock >>>(d_V,sizeV,d_E,sizeE,d_F,d_X,d_C);
		//vectorAdd << <blocksPerGrid, threadsPerBlock >> >(d_A, d_B, d_C, numElements);

		//break;
		cudaDeviceSynchronize();
	}
	cout << "Copy C to host\n";
	cudaMemcpy(h_C, d_C, sizeV, cudaMemcpyDeviceToHost);
	//copy(h_C, h_C + n, ostream_iterator<long>(cout, ","));

	cout << "\nOslobadjanje host\n";
	free(h_E);
	free(h_V);
	free(h_F);
	free(h_X);
	free(h_C);

	cout << "Oslobadjanje device\n";
	cudaFree(d_E);
	cudaFree(d_V);
	cudaFree(d_F);
	cudaFree(d_X);
	cudaFree(d_C);
	cout << "\nDone!!!!!!!!!!!!!" << endl;
	char t = getchar();
    return 0;
}

