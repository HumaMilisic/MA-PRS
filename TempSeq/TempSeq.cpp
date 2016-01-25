#include "stdafx.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <ctime>
#include <cstring>
#include <numeric>
#include <cmath>
#include <queue>
#include <omp.h>

#include "tinydir.h"

#define TEST_STR "Input//simple.prs"
#define F(x) cout<< #x " = "<<x<<"\n";
//#define FL(x) _log<<" "#x " = "<<x<<" ";
#define FI 0.0001
const int BR_PONAVLJANJA = 1;

using namespace std;

string fileName = "seqIntelReleasev2[avx2,O3,oi].log";
ofstream _log;// (fileName);

using namespace std;

inline void parseA(string &line, long *E, long&sizeE);
inline void dodajUNiz(long *V, long &sizeV, long sizeE, long n);
inline bool parse2(const string &line, vector<long>&rezB);
double inputAVE(const string&path, long **V, long**E, long &sizeV, long &sizeE);
bool testPoredjenje(vector<string>&putanje);
bool putanjeGrafova(vector<string>&putanje);

double BFSAVEseq3(const long *V, const long *E, const long sizeV, const long sizeE)
{
	vector<bool> dodaniURed(sizeV, false);
	vector<bool> posjecenCvor(sizeV, false);
	vector<long>redoslijed;
	queue<long>redCvorova;

	long pocetniCvor = 0;
	redCvorova.push(pocetniCvor);
	dodaniURed[pocetniCvor] = true;
	//redoslijed.push_back(pocetniCvor);

	long brPosjecenih = 0;
	clock_t t1 = clock();
	long trenutniCvor, pozP, pozK, susjed; //, susjed;
	//while (redCvorova.size()>0 && brPosjecenih<sizeV)
	while (brPosjecenih<sizeV)
	{
		//mozda ovo sve skupa moze u
		//#pragma omp parallel private(trenutniCvor)
		//pa onda kuplenje cvoraa u
		//#pragma omp critical
		//i mozda dinamyc rasporedjivanje zbog continue
		//redoslijjed opet u critical i mozda ordered

		//mozda jos treba provjera je li red prazan
#pragma omp parallel private(trenutniCvor, pozP, pozK, susjed) default(shared)
		{
#pragma omp critical(redCvorova)
			{
				//izgleda da je sa zakomentarisanim ifom sporiji...
				if (!redCvorova.empty()) {

					trenutniCvor = redCvorova.front();
					redCvorova.pop();

				}
				else
					trenutniCvor = pocetniCvor;
			}

			if (!posjecenCvor[trenutniCvor]) {

#pragma omp atomic
				brPosjecenih++;

				posjecenCvor[trenutniCvor] = true;

				//#pragma omp critical(redoslijed)
				//{
				//    redoslijed.push_back(trenutniCvor);
				//}

				pozP = V[trenutniCvor];
				pozK = trenutniCvor + 1 < sizeV ? V[trenutniCvor + 1] : sizeE;
				//F(pozP);
				//F(pozK);

				//pokusaj...
				long susjed;

				//izgleda da ovaj for ne radi...
				#pragma omp for private(susjed)
				for (int i = pozP; i < pozK; i++)
				{
					susjed = E[i];
					if (!(posjecenCvor[susjed] || dodaniURed[susjed]))
					{
#pragma omp critical(redCvorova)
						{
							redCvorova.push(susjed);
						}
						dodaniURed[susjed] = true;
					}
				}
			}
		}
	}
	double diff = (double)(clock() - t1) / CLOCKS_PER_SEC;
	//_log << "\nBFSAVEseq: " << diff << endl;
	cout << endl << ": The time taken for seq Breadth first search: " << diff << endl;
	//cout << "\nBFSAVEseq: ";
	//copy(redoslijed.begin(), redoslijed.end(), ostream_iterator<long>(cout, ", "));

	return diff;

}

double BFSAVEseq2(const long *V, const long *E, const long sizeV, const long sizeE)
{
	vector<bool> dodaniURed(sizeV, false);
	vector<bool> posjecenCvor(sizeV, false);
	vector<long>redoslijed;
	queue<long>redCvorova;

	long pocetniCvor = 0;
	redCvorova.push(pocetniCvor);
	dodaniURed[pocetniCvor] = true;
	//redoslijed.push_back(pocetniCvor);

	long brPosjecenih = 0;
	clock_t t1 = clock();
	while (redCvorova.size()>0 && brPosjecenih<sizeV)
	{
		long trenutniCvor = redCvorova.front();
		redCvorova.pop();

		if (posjecenCvor[trenutniCvor])
			continue;

		brPosjecenih++;
		posjecenCvor[trenutniCvor] = true;
		redoslijed.push_back(trenutniCvor);

		long pozP = V[trenutniCvor],
			pozK = trenutniCvor + 1 < sizeV ? V[trenutniCvor + 1] : sizeE;
		//F(pozP);
		//F(pozK);
		long susjed;
#pragma omp parallel for private(susjed) default(shared)
		for (int i = pozP; i < pozK; i++)
		{
			susjed = E[i];

			if (posjecenCvor[susjed] || dodaniURed[susjed])
			{
				continue;
			}
#pragma omp critical(redCvorova)
			{
				redCvorova.push(susjed);
			}
			dodaniURed[susjed] = true;
		}
	}
	double diff = (double)(clock() - t1) / CLOCKS_PER_SEC;
	//_log << "\nBFSAVEseq: " << diff << endl;
	cout << endl << ": The time taken for seq Breadth first search: " << diff << endl;
	//cout << "\nBFSAVEseq: ";
	//copy(redoslijed.begin(), redoslijed.end(), ostream_iterator<long>(cout, ", "));

	return diff;

}

double BFSAVEseq(const long *V, const long *E, const long sizeV, const long sizeE)
{
	vector<bool> dodaniURed(sizeV, false);
	vector<bool> posjecenCvor(sizeV, false);
	vector<long>redoslijed;
	queue<long>redCvorova;

	long pocetniCvor = 0;
	redCvorova.push(pocetniCvor);
	dodaniURed[pocetniCvor] = true;
	//redoslijed.push_back(pocetniCvor);

	long brPosjecenih = 0;
	clock_t t1 = clock();
	while (redCvorova.size()>0 && brPosjecenih<sizeV)
	{
		long trenutniCvor = redCvorova.front();
		redCvorova.pop();

		if (posjecenCvor[trenutniCvor])
			continue;

		brPosjecenih++;
		posjecenCvor[trenutniCvor] = true;
		redoslijed.push_back(trenutniCvor);

		long pozP = V[trenutniCvor],
			pozK = trenutniCvor + 1 < sizeV ? V[trenutniCvor + 1] : sizeE;
		//F(pozP);
		//F(pozK);

		for (int i = pozP; i < pozK; i++)
		{
			long susjed = E[i];

			if (posjecenCvor[susjed] || dodaniURed[susjed])
			{
				continue;
			}
			redCvorova.push(susjed);

			dodaniURed[susjed] = true;
		}
	}
	double diff = (double)(clock() - t1) / CLOCKS_PER_SEC;
	//_log << "\nBFSAVEseq: " << diff << endl;
	cout << endl << ": The time taken for seq Breadth first search: " << diff << endl;
	//cout << "\nBFSAVEseq: ";
	//copy(redoslijed.begin(), redoslijed.end(), ostream_iterator<long>(cout, ", "));

	return diff;

}

double BFSAVEseq4(const long *V, const long *E, const long sizeV, const long sizeE)
{
	vector<bool> dodaniURed(sizeV, false);
	vector<bool> posjecenCvor(sizeV, false);
	vector<long>redoslijed;
	queue<long>redCvorova;

	long pocetniCvor = 0;
	redCvorova.push(pocetniCvor);
	dodaniURed[pocetniCvor] = true;
	//redoslijed.push_back(pocetniCvor);

	long brPosjecenih = 0;
	clock_t t1 = clock();
	long trenutniCvor, pozP, pozK, susjed; //, susjed;
	//while (redCvorova.size()>0 && brPosjecenih<sizeV)
	while (brPosjecenih<sizeV)
	{
		//mozda ovo sve skupa moze u
		//#pragma omp parallel private(trenutniCvor)
		//pa onda kuplenje cvoraa u
		//#pragma omp critical
		//i mozda dinamyc rasporedjivanje zbog continue
		//redoslijjed opet u critical i mozda ordered

		//mozda jos treba provjera je li red prazan
#pragma omp parallel private(trenutniCvor, pozP, pozK, susjed) default(shared)
		{
#pragma omp critical(redCvorova)
			{
				if (!redCvorova.empty()) {

					trenutniCvor = redCvorova.front();
					redCvorova.pop();

				}
				else
					trenutniCvor = pocetniCvor;
			}

			if (!posjecenCvor[trenutniCvor]) {

#pragma omp atomic
				brPosjecenih++;

				posjecenCvor[trenutniCvor] = true;

#pragma omp critical(redoslijed)
				{
					redoslijed.push_back(trenutniCvor);
				}

				pozP = V[trenutniCvor];
				pozK = trenutniCvor + 1 < sizeV ? V[trenutniCvor + 1] : sizeE;
				//F(pozP);
				//F(pozK);

				//pokusaj...
				long susjed;
				#pragma omp for private(susjed)
				for (int i = pozP; i < pozK; i++)
				{
					susjed = E[i];
					if (!(posjecenCvor[susjed] || dodaniURed[susjed]))
					{
#pragma omp critical(redCvorova)
						{
							redCvorova.push(susjed);
						}
						dodaniURed[susjed] = true;
					}
				}
			}
		}
	}
	double diff = (double)(clock() - t1) / CLOCKS_PER_SEC;
	//_log << "\nBFSAVEseq: " << diff << endl;
	cout << endl << ": The time taken for seq Breadth first search: " << diff << endl;
	//cout << "\nBFSAVEseq: ";
	//copy(redoslijed.begin(), redoslijed.end(), ostream_iterator<long>(cout, ", "));

	return diff;

}

int _tmain()
{
	std::time_t result = std::time(nullptr);
	std::string s = std::to_string(result);

	fileName = s + fileName;

	_log = ofstream(fileName);
	_log << "graf;n;m;algoritam;vrijeme" << endl;

	vector<string>putanje;
	if (putanjeGrafova(putanje))
	{
		cerr << "problemi\n";
		exit(EXIT_FAILURE);
	}
	copy(putanje.begin(), putanje.end(), ostream_iterator<string>(cout, ", "));
	cout << "\n\nOtvoriti sve iz input foldera\n";

	testPoredjenje(putanje);



	system("pause");
	return 0;
}
bool putanjeGrafova(vector<string>&putanje)
{
	tinydir_dir dir;
	bool fail = true;
	if (tinydir_open(&dir, "Input") != 1)
	{
		while (dir.has_next)
		{
			tinydir_file file;

			if (tinydir_readfile(&dir, &file) == -1)
			{
				fail = true;
				break;
			}

			if (!file.is_dir)
				putanje.push_back(file.path);

			tinydir_next(&dir);
			fail = false;
		}
	}
	tinydir_close(&dir);
	return fail;
}
bool testPoredjenje(vector<string>&putanje)
{
	cout << "Poredjenje:\n";
	for (int i = 0; i < putanje.size(); i++)
	{
		long *h_V(NULL), *h_E(NULL), sizeE(0), sizeV(0);
		cout << "inputAVE\n";
		_log << putanje[i] << ";";
		inputAVE(putanje[i], &h_V, &h_E, sizeV, sizeE);
		for (int k = 0; k < BR_PONAVLJANJA; k++)
		{
			//F(k);

			double _vrijeme = (BFSAVEseq(h_V, h_E, sizeV, sizeE));
			//vrijeme.push_back(_vrijeme);
			_log << "BFSAVEseq: " << _vrijeme << endl;

			_vrijeme = (BFSAVEseq2(h_V, h_E, sizeV, sizeE));
			//vrijeme.push_back(_vrijeme);
			_log << "BFSAVEseq2(openmp1): " << _vrijeme << endl;
			try
			{
				_vrijeme = (BFSAVEseq3(h_V, h_E, sizeV, sizeE));
			}
			catch (exception e)
			{

			}
			_log << "BFSAVEseq3:(openmp2) " << _vrijeme << endl;
			_vrijeme = (BFSAVEseq4(h_V, h_E, sizeV, sizeE));
			_log << "BFSAVEseq4:(openmp1.5) " << _vrijeme << endl;
		}
		free(h_V);
		free(h_E);
	}
	return 0;

}
inline void parseA(string &line, long *E, long&sizeE)
{
	char * temp = new char[line.length() + 1];
	std::strcpy(temp, line.c_str());
	char * pch = strtok(temp, " ");
	std::string::size_type sz;
	while (pch != NULL)
	{
		//rezB.push_back(atol(pch) - 1);
		E[sizeE] = atol(pch) - 1;
		sizeE++;
		pch = strtok(NULL, " ");
	}
	delete[] temp;
}
inline void dodajUNiz(long *V, long &sizeV, long sizeE, long n)
{
	if (sizeV < n)
	{
		V[sizeV] = sizeE;
		sizeV++;
	}
}
inline bool parse2(const string &line, vector<long>&rezB)
{
	char * temp = new char[line.length() + 1];
	std::strcpy(temp, line.c_str());
	char * pch = strtok(temp, " ");
	std::string::size_type sz;
	while (pch != NULL)
	{
		rezB.push_back(atol(pch) - 1);
		pch = strtok(NULL, " ");
	}
	delete[] temp;
	return 0;
}
double inputAVE(const string&path, long **V, long**E, long &sizeV, long &sizeE)
{
	//_log << "inputAVE: " << path;// >> endl;
	ifstream in(path);

	long n(0), m(0);
	bool nNM = true;

	clock_t t0;
	if (in.is_open())
	{
		cout << path << " is open\n";
		string line = "";
		t0 = clock();
		while (getline(in, line))
		{
			if (in.bad())
				break;
			if (line[0] == '%')
				continue;

			if (nNM)
			{
				vector<long>rezB;
				parse2(line, rezB);
				n = rezB[0] + 1;
				m = rezB[1] + 1;
				//_log << n << ";" << m << ";BFSAVEseq;";
				//FL(n);
				//FL(m);
				//FL(double(m / n));
				*V = (long*)malloc(n*sizeof(long));
				*E = (long*)malloc((2 * m + 10)*sizeof(long));
				//sizeV = n;
				//sizeE = 2 * m + 10;
				nNM = false;
				//memset(niz, 0, size*sizeof(int));
				memset(*V, 0, n*sizeof(long));
				memset(*E, 0, 2 * m*sizeof(long));
				sizeV++;
				continue;
				//break;
			}

			parseA(line, *E, sizeE);

			dodajUNiz(*V, sizeV, sizeE, n);

		}

		double diff = (double)(clock() - t0) / CLOCKS_PER_SEC;
		//FL(n); FL(m);
		//_log << " " << diff << " \n";
		in.close();
		return diff;
	}
	return -1;
}