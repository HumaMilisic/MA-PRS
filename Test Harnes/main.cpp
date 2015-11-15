
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

//#include <stdio.h>
//#include <stdlib.h>
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

#include "tinydir.h"

#define TEST_STR "Input//simple.prs"
#define F(x) cout<< #x " = "<<x<<"\n";
#define FL(x) _log<<" "#x " = "<<x<<" ";
#define FI 0.0001
const int BR_PONAVLJANJA = 5;

using namespace std;

ofstream _log("temp.log");

extern "C" bool runTest();

extern "C" double paralelniBFS(long *h_V, long*h_E, long sizeV, long sizeE);

int iOfSmallest(vector<double>&niz)
{
	int i = 0;
	for (int k = 0; k < niz.size(); k++)
	{
		if (niz[k] < niz[i])
			i = k;
	}
	return i;
}

inline long strToBr(string line)
{
	long br = 0;
	long size = line.size();
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

inline vector<string> token(string line)
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

inline vector<long> strURedBrojeva(string line)
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

inline bool parse(const string &line, vector<long>&rezB)
{
	char * temp = new char[line.length() + 1];
	std::strcpy(temp, line.c_str());
	char * pch = strtok(temp, " ");
	std::string::size_type sz;
	while (pch != NULL)
	{
		rezB.push_back(stol(pch, &sz)-1);
		pch = strtok(NULL, " ");
	}
	delete[] temp;
	return 0;
}

inline bool parse2(const string &line, vector<long>&rezB)
{
	char * temp = new char[line.length() + 1];
	std::strcpy(temp, line.c_str());
	char * pch = strtok(temp, " ");
	std::string::size_type sz;
	while (pch != NULL)
	{
		rezB.push_back(atol(pch)-1);
		pch = strtok(NULL, " ");
	}
	delete[] temp;
	return 0;
}

inline bool parse2_1(const string &line, vector<long>&rezB)
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

//inline bool parse2_1A(const string &line, long *E,long &size)
//{
//	char * temp = new char[line.length() + 1];
//	std::strcpy(temp, line.c_str());
//	char * pch = strtok(temp, " ");
//	//std::string::size_type sz;
//	while (pch != NULL)
//	{
//		//rezB.push_back(atol(pch) - 1);
//		//E[size] = atol(pch) - 1;
//		size++;
//		pch = strtok(NULL, " ");
//	}
//	delete[] temp;
//	return 0;
//}

double inputCV(const string&path, vector<vector<long>>&graf,ostream &log)
{
	_log << "inputCV: " << path;// >> endl;
	ifstream in(path);

	long n, m;
	bool nNM = true;

	clock_t t0; 
	if (in.is_open())
	{
		cout << path << " is open\n";
		string line = "";
		t0 = clock();
		while (getline(in,line))
		{
			if (in.bad())
				break;
			if (line[0] == '%')
				continue; 

			vector<long>rezB;
			parse(line, rezB);

			if (nNM)
			{
				n = rezB[0];
				m = rezB[1];
				graf.reserve(n);
				nNM = false;
				continue;
			}

			graf.push_back(rezB);
		}
		double diff = (double)(clock() - t0) / CLOCKS_PER_SEC;
		//cout << endl << "The time taken for inputCV(const string&path, vector<vector<long>>&graf): " << diff << "ms" << endl;
		FL(n); FL(m);
		_log << " "<<diff << " ms\n";
		return diff;
	}

	in.close();
	return 0;
}

double inputCV2(const string&path, vector<vector<long>>&graf, ofstream &log)
{
	_log << "inputCV2: " << path;// >> endl;
	ifstream in(path);

	long n, m;
	bool nNM = true;

	clock_t t0;
	if (in.is_open())
	{
		cout << path << " is open\n";
		string line = "";
		t0 = clock();
		while (getline(in, line))
		{
			if (line[0] == '%')
				continue;

			vector<long>rezB;
			parse2(line, rezB);

			if (nNM)
			{
				n = rezB[0];
				m = rezB[1];
				graf.reserve(n);
				nNM = false;
				continue;
			}

			graf.push_back(rezB);
		}
		double diff = (double)(clock() - t0) / CLOCKS_PER_SEC;
		//cout << endl << "The time taken for inputCV(const string&path, vector<vector<long>>&graf): " << diff << "ms" << endl;
		FL(n); FL(m);
		_log << " "<<diff << " ms\n";
		return  diff;
	}

	in.close();
	return 0;
}

double inputCV3(const string&path, vector<vector<long>>&graf,ofstream &log)
{
	_log << "inputCV3: " << path;// >> endl;
	ifstream in(path);

	long n, m;
	bool nNM = true;

	clock_t t0;
	if (in.is_open())
	{
		cout << path << " is open\n";
		string line = "";
		t0 = clock();
		while (getline(in, line))
		{
			if (line[0] == '%')
				continue;

			vector<long>rezB (strURedBrojeva(line));
			if (nNM)
			{
				n = rezB[0];
				m = rezB[1];
				graf.reserve(n);
				nNM = false;
				continue;
			}
			graf.push_back(rezB);
		}
		double diff = (double)(clock() - t0) / CLOCKS_PER_SEC;
		//cout << endl << "The time taken for inputCV(const string&path, vector<vector<long>>&graf): " << diff << "ms" << endl;
		FL(n); FL(m);
		_log << " "<<diff <<  " ms\n";
		return diff;
	}

	in.close();
	return 0;
}

double inputCVE(const string&path, vector<long> &V, vector<long>&E)
{
	_log << "inputCVE: " << path;// >> endl;
	ifstream in(path);

	long n, m;
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

			vector<long>rezB;
			parse(line, rezB);

			if (nNM)
			{
				n = rezB[0];
				m = rezB[1];
				V.reserve(n);
				E.reserve(2*m+10);
				nNM = false;
				continue;
			}

			for (int k = 0; k < rezB.size(); k++)
				E.push_back(rezB[k]);
			//if (V.size() < n)
				V.push_back(E.size());
			//graf.push_back(rezB);
		}
		double diff = (double)(clock() - t0) / CLOCKS_PER_SEC;
		cout << endl << "The time taken for inputCVE: " << diff << endl;
		FL(n); FL(m); //FL(E.size()); FL(E.capacity());
		_log << " " << diff << " ms\n";
		
		return diff;
	}

	in.close();
	return 0;
}

double inputCVE2(const string&path, vector<long> &V, vector<long>&E)
{
	_log << "inputCVE2: " << path;// >> endl;
	ifstream in(path);

	long n, m;
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

			vector<long>rezB;
			parse2(line, rezB);

			if (nNM)
			{
				n = rezB[0];
				m = rezB[1];
				V.reserve(n);
				E.reserve(2 * m + 10);
				nNM = false;
				continue;
			}

			for (int k = 0; k < rezB.size(); k++)
				E.push_back(rezB[k]);
			//if (V.size() < n)
				V.push_back(E.size());
			//graf.push_back(rezB);
		}
		double diff = (double)(clock() - t0) / CLOCKS_PER_SEC;
		//cout << endl << "The time taken for inputCV(const string&path, vector<vector<long>>&graf): " << diff << "ms" << endl;
		FL(n); FL(m);
		_log << " " << diff << " ms\n";
		return diff;
	}

	in.close();
	return 0;
}

double inputCVE2_1(const string&path, vector<long> &V, vector<long>&E)
{
	_log << "inputCVE2_1: " << path;// >> endl;
	ifstream in(path);

	long n, m;
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
				//F(rezB[0]);
				//F(rezB[1]);
				n = rezB[0]+1;
				m = rezB[1]+1;
				//F(n);
				//F(m);
				V.reserve(n);
				E.reserve(2 * m+10);
				V.push_back(0);
				nNM = false;
				continue;
			}

			parse2_1(line, E);

			//for (int k = 0; k < rezB.size(); k++)
			//	E.push_back(rezB[k]);
			if (V.size() < n)
				V.push_back(E.size());
			//graf.push_back(rezB);
		}
		double diff = (double)(clock() - t0) / CLOCKS_PER_SEC;
		//cout << endl << "The time taken for inputCV(const string&path, vector<vector<long>>&graf): " << diff << "ms" << endl;
		FL(n); FL(m);
		_log << " " << diff << " \n";

		//cout << "V: ";
		//copy(V.begin(), V.end(), ostream_iterator<long>(cout, ", "));
		//cout << "\nE: ";
		//copy(E.begin(), E.end(), ostream_iterator<long>(cout, ", "));
		in.close();

		return diff;
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

inline void dodajUNiz(long *V, long &sizeV,long sizeE,long n)
{
	if (sizeV < n)
	{
		V[sizeV] = sizeE;
		sizeV++;
	}
}

double inputAVE(const string&path, long **V, long**E, long &sizeV, long &sizeE)
{
	_log << "inputAVE: " << path;// >> endl;
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
				FL(n);
				FL(m);
				FL(double(m / n));
				*V = (long*)malloc(n*sizeof(long));
				*E = (long*)malloc((2*m+10)*sizeof(long));
				//sizeV = n;
				//sizeE = 2 * m + 10;
				nNM = false;
				//memset(niz, 0, size*sizeof(int));
				memset(*V, 0, n*sizeof(long));
				memset(*E, 0, 2*m*sizeof(long));
				sizeV++;
				continue;
				//break;
			}
				
			parseA(line, *E, sizeE);

			dodajUNiz(*V, sizeV, sizeE, n);
			
		}
		
		double diff = (double)(clock() - t0) / CLOCKS_PER_SEC;
		//FL(n); FL(m);
		_log << " " << diff << " \n";
		in.close();
		return diff;
	}
	return -1;
}


//double inputCVEA2_1(const string&path, long **V,long **E,long &n,long &m, long &sizeV,long &sizeE)
//{
//	_log << "inputCVEA2_1: " << path;// >> endl;
//	ifstream in(path);
//
////	long n, m; //long sizeE(0),sizeV(0);
//	bool nNM = true;
//
//	clock_t t0;
//	if (in.is_open())
//	{
//		cout << path << " is open\n";
//		string line = "";
//		t0 = clock();
//		while (getline(in, line))
//		{
//			if (in.bad())
//				break;
//			if (line[0] == '%')
//				continue;
//
//
//
//			if (nNM)
//			{
//				vector<long>rezB;
//				parse2(line, rezB);
//				//F(rezB[0]);
//				//F(rezB[1]);
//				n = rezB[0] + 1;
//				m = rezB[1] + 1;
//				//F(n);
//				//F(m);
//				//sizeV = tempV.size()*sizeof(long); n = tempV.size();
//				*V = (long *)malloc(n*sizeof(long));
//				*E = (long *)malloc((2*m+10)*sizeof(long));
//				//*V[0] = sizeE;
//				sizeV++;
//				//V.reserve(n);
//				//E.reserve(2 * m + 10);
//				//V.push_back(0);
//				nNM = false;
//				continue;
//			}
//			//long sizeE = 0;
//			//parse2_1(line, E);
//			//parse2_1A(line, *E, sizeE);
//
//			//if (sizeV < n)
//			//	*V[sizeV++] = sizeE;
//
//			////for (int k = 0; k < rezB.size(); k++)
//			////	E.push_back(rezB[k]);
//			//if (V.size() < n)
//			//	V.push_back(E.size());
//			//graf.push_back(rezB);
//		}
//		double diff = (double)(clock() - t0) / CLOCKS_PER_SEC;
//		//cout << endl << "The time taken for inputCV(const string&path, vector<vector<long>>&graf): " << diff << "ms" << endl;
//		FL(n); FL(m);
//		_log << " " << diff << " \n";
//
//		cout << "V: ";
//		copy(V, V+sizeV, ostream_iterator<long>(cout, ", "));
//		cout << "\nE: ";
//		copy(E, E+sizeE, ostream_iterator<long>(cout, ", "));
//		//in.close();
//
//		return diff;
//	}
//
//	return 0;
//}


bool input(const string &path,vector<string>&rez)
{
	//vector<string>rez;
	ifstream in(path);
	bool fail = true;
	clock_t t0;
	if (in.is_open())
	{
		//cout <<path <<  " is open\n";
		string line = "";
		fail = false;
		t0 = clock();
		while (getline(in, line))
		{
			if (line[0] == '%')
				continue;
			rez.push_back(line);
			fail = false;
			//break;
		}
		double diff = (double)(clock() - t0) / CLOCKS_PER_SEC;
		cout << endl << "The time taken for input(const string &path,vector<string>&rez): " << diff <<"ms"<< endl;
	}
	in.close();
	return fail;
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

bool testInput(vector<string>&putanje)
{
	for (int i = 0; i < putanje.size(); i++)
	{
		//vector<vector<long> > sadrzaj;
		//if (inputCV(putanje[i],sadrzaj))
		//{
		//	break;
		//}
		////cout << "sadrzaj.size = "<<sadrzaj.size() << endl;
		//sadrzaj.clear();
		vector<double>prosjeci;
		vector<double>vrijeme;
		double suma;

		//for (int k = 0; k < BR_PONAVLJANJA; k++)
		//{
		//	F(k);
		//	vector<vector<long> >temp;
		//	vrijeme.push_back(inputCV(putanje[i], temp, _log));
		//}
		//suma = std::accumulate(vrijeme.begin(), vrijeme.end(), 0.0f);
		//_log << endl;
		//_log << "Prosjek " << suma / vrijeme.size() << endl << endl;
		//prosjeci.push_back(suma / vrijeme.size());
		//vrijeme.clear();
		//
		//_log << endl;

		//for (int k = 0; k < BR_PONAVLJANJA; k++)
		//{
		//	F(k);
		//	vector<vector<long> >temp;
		//	vrijeme.push_back(inputCV2(putanje[i], temp, _log));
		//}
		//suma = std::accumulate(vrijeme.begin(), vrijeme.end(), 0.0f);
		//_log << endl;
		//_log << "Prosjek " << suma / vrijeme.size() << endl << endl;
		//prosjeci.push_back(suma / vrijeme.size());
		//vrijeme.clear();

		//for (int k = 0; k < BR_PONAVLJANJA; k++)
		//{
		//	F(k);
		//	vector<long>tempE,tempV;
		//	vrijeme.push_back(inputCVE(putanje[i], tempV, tempE));
		//}
		//suma = std::accumulate(vrijeme.begin(), vrijeme.end(), 0.0f);
		//_log << endl;
		//_log << "Prosjek " << suma / vrijeme.size() << endl << endl;
		//prosjeci.push_back(suma / vrijeme.size());
		//vrijeme.clear();

		//for (int k = 0; k < BR_PONAVLJANJA; k++)
		//{
		//	F(k);
		//	vector<long>tempE, tempV;
		//	vrijeme.push_back((inputCVE2(putanje[i], tempV, tempE)));
		//}
		//suma = std::accumulate(vrijeme.begin(), vrijeme.end(), 0.0f);
		//_log << endl;
		//_log << "Prosjek " << suma / vrijeme.size() << endl << endl;
		//prosjeci.push_back(suma / vrijeme.size());
		//vrijeme.clear();
		for (int k = 0; k < BR_PONAVLJANJA; k++)
		{
			F(k);
			vector<long>tempE, tempV;
			vrijeme.push_back((inputCVE2_1(putanje[i], tempV, tempE)));
		}
		suma = std::accumulate(vrijeme.begin(), vrijeme.end(), 0.0f);
		_log << endl;
		_log << "Prosjek " << suma / vrijeme.size() << endl << endl;
		prosjeci.push_back(suma / vrijeme.size());
		vrijeme.clear();

		//for (int k = 0; k < BR_PONAVLJANJA; k++)
		//{
		//	F(k);
		//	//vector<long>tempE, tempV;
		//	long *h_E(NULL), *h_V(NULL),sizeE(0),sizeV(0),n(0),m(0);
		//	vrijeme.push_back(inputCVEA2_1(putanje[i], &h_V, &h_E, n, m, sizeV, sizeE));

		//	cout << "V: ";
		//	copy(h_V, h_V + sizeV, ostream_iterator<long>(cout, ", "));
		//	cout << "\nE: ";
		//	copy(h_E, h_E + sizeE, ostream_iterator<long>(cout, ", "));

		//	free(h_E);
		//	free(h_V);
		//}
		//suma = std::accumulate(vrijeme.begin(), vrijeme.end(), 0.0f);
		//_log << endl;
		//_log << "Prosjek " << suma / vrijeme.size() << endl << endl;
		//prosjeci.push_back(suma / vrijeme.size());
		//vrijeme.clear();


		//for (int k = 0; k < BR_PONAVLJANJA; k++)
		//{
		//	F(k);
		//	vector<vector<long> >temp;
		//	inputCV3(putanje[i], temp,_log);
		//}
		//_log << endl;
		//int rei = iOfSmallest(prosjeci);
		//_log << "\nNajmanji prosjek za graf " << putanje[i] << " je: " << prosjeci[rei] << " na mjestu " << rei << endl << endl;
	}
	return 0;
}

double BFSseq(vector<long>&V,vector<long>&E)
{
	vector<bool> dodaniURed(V.size(), false);
	vector<bool> posjecenCvor(V.size(), false);
	vector<long>redoslijed;
	queue<long>redCvorova;

	long pocetniCvor = 0;
	redCvorova.push(pocetniCvor);
	dodaniURed[pocetniCvor] = true;
	//redoslijed.push_back(pocetniCvor);

	long brPosjecenih = 0;
	clock_t t1 = clock();
	while (redCvorova.size()>0 && brPosjecenih<V.size())
	{
		long trenutniCvor = redCvorova.front();
		redCvorova.pop();

		if (posjecenCvor[trenutniCvor])
			continue;

		brPosjecenih++;
		posjecenCvor[trenutniCvor] = true;
		redoslijed.push_back(trenutniCvor);

		long pozP = V[trenutniCvor], 
			pozK = trenutniCvor + 1 < V.size() ? V[trenutniCvor + 1] : E.size();
		//F(pozP);
		//F(pozK);
		for (int i = pozP; i < pozK; i++)
		{
			long susjed = E[i];

			if (posjecenCvor[susjed]||dodaniURed[susjed])
			{
				continue;
			}

			redCvorova.push(susjed);
			dodaniURed[susjed] = true;
		}
	}
	double diff = (double)(clock() - t1) / CLOCKS_PER_SEC;
	//cout << "\nBFSseq: ";
	//copy(redoslijed.begin(), redoslijed.end(), ostream_iterator<long>(cout, ", "));

	return diff;
}

double BFSAVEseq(const long *V,const long *E,const long sizeV,const long sizeE)
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

bool testBFSseq(vector<string>&putanje)
{
	for (int i = 0; i < putanje.size(); i++)
	{
		vector<double>prosjeci;
		vector<double>vrijeme;
		double suma;
		vector<long>tempE, tempV;
		cout << "inputCVE2_1\n";
		inputCVE2_1(putanje[i], tempV, tempE);

		long *h_V(NULL), *h_E(NULL), sizeE(0), sizeV(0);
		cout << "inputAVE\n";
		inputAVE(putanje[i], &h_V, &h_E, sizeV, sizeE);

		for (int k = 0; k < BR_PONAVLJANJA; k++)
		{
			F(k);			
			vrijeme.push_back(BFSseq(tempV, tempE));
		}
		suma = std::accumulate(vrijeme.begin(), vrijeme.end(), 0.0f);
		_log << endl;
		_log << putanje[i]<<": BFSseq Prosjek " << suma / vrijeme.size() << endl << endl;
		prosjeci.push_back(suma / vrijeme.size());
		vrijeme.clear();


		for (int k = 0; k < BR_PONAVLJANJA; k++)
		{
			F(k);
			vrijeme.push_back(BFSAVEseq(h_V, h_E, sizeV, sizeE));
		}
		free(h_V);
		free(h_E);
		suma = std::accumulate(vrijeme.begin(), vrijeme.end(), 0.0f);
		//_log << endl;
		_log << putanje[i] << ": BFSAVEseq Prosjek " << suma / vrijeme.size() << endl << endl;
		prosjeci.push_back(suma / vrijeme.size());
		vrijeme.clear();
	}
	return 0;
}

bool testPoredjenje(vector<string>&putanje)
{
	for (int i = 0; i < putanje.size(); i++)
	{
		vector<double>prosjeci;
		vector<double>vrijeme;
		double suma;
		//vector<long>tempE, tempV;
		//cout << "inputCVE2_1\n";
		//inputCVE2_1(putanje[i], tempV, tempE);

		long *h_V(NULL), *h_E(NULL), sizeE(0), sizeV(0);
		cout << "inputAVE\n";
		inputAVE(putanje[i], &h_V, &h_E, sizeV, sizeE);

		//for (int k = 0; k < BR_PONAVLJANJA; k++)
		//{
		//	F(k);
		//	vrijeme.push_back(BFSseq(tempV, tempE));
		//}
		//suma = std::accumulate(vrijeme.begin(), vrijeme.end(), 0.0f);
		//_log << endl;
		//_log << putanje[i] << ": BFSseq Prosjek " << suma / vrijeme.size() << endl << endl;
		//prosjeci.push_back(suma / vrijeme.size());
		//vrijeme.clear();


		for (int k = 0; k < BR_PONAVLJANJA; k++)
		{
			F(k);
			vrijeme.push_back(BFSAVEseq(h_V, h_E, sizeV, sizeE));
		}
		//free(h_V);
		//free(h_E);
		suma = std::accumulate(vrijeme.begin(), vrijeme.end(), 0.0f);
		//_log << endl;
		_log << putanje[i] << ": BFSAVEseq Prosjek " << suma / vrijeme.size() << endl;
		prosjeci.push_back(suma / vrijeme.size());
		vrijeme.clear();

		//for (int k = 0; k < BR_PONAVLJANJA; k++)
		//{
		//	F(k);
		//	vrijeme.push_back(paralelniBFS(h_V, h_E, sizeV, sizeE));
		//}
		//suma = std::accumulate(vrijeme.begin(), vrijeme.end(), 0.0f);
		////_log << endl;
		//_log << putanje[i] << ": paralelniBFS Prosjek " << suma / vrijeme.size() << endl << endl;
		//prosjeci.push_back(suma / vrijeme.size());
		//vrijeme.clear();

		free(h_V);
		free(h_E);
	}
	return 0;

}

void fi1(int *niz, int size)
{
	for (int i = 0; i < size; i++)
		niz[i] = -1;
}

void alloc(int **niz,int size)
{
	*niz = NULL;
	*niz = (int*)malloc(size*sizeof(int));
	fi1(*niz, size);
}



extern "C" void uhvatiNiz(long *niz, long size);

int main()
{
	vector<string>putanje;
	if (putanjeGrafova(putanje))
	{
		cerr << "problemi\n";
		exit(EXIT_FAILURE);
	}
	copy(putanje.begin(), putanje.end(), ostream_iterator<string>(cout, ", "));
	cout << "\n\nOtvoriti sve iz input foldera\n";
	
	testPoredjenje(putanje);

	//int *niz, size(10);
	//alloc(&niz, size);
	////memset(niz, 0, size*sizeof(int));
	//copy(niz, niz + size, ostream_iterator<int>(cout, ", "));
	//free(niz);

	//long *h_V(NULL), *h_E(NULL), sizeE(0), sizeV(0);
	////vector<long>E, V;
	//
	//double rez(inputAVE(putanje[0], &h_V, &h_E, sizeV, sizeE));// ,
	////	rez2(inputCVE2_1(TEST_STR,V,E));

	//BFSAVEseq(h_V, h_E, sizeV, sizeE);
	//paralelniBFS(h_V, h_E, sizeV, sizeE);

	//long *niz(NULL), size(10);
	////h_F = (long*)malloc(sizeV);
	//niz = (long*)malloc(size*sizeof(long));
	//memset(niz, 0, size*sizeof(long));
	//uhvatiNiz(niz, size);
	//free(niz);

	//if (rez != -1)
	//{
	//		cout << "V: ";
	//		copy(h_V, h_V+sizeV, ostream_iterator<long>(cout, ", "));
	//		cout << "\nE: ";
	//		copy(h_E, h_E+sizeE, ostream_iterator<long>(cout, ", "));
	//}
	//BFSseq(V, E);
	//cout << "\n";
	//BFSAVEseq(h_V, h_E, sizeV, sizeE);
	//free(h_V);
	//free(h_E);
	//testInput(putanje);
	//testBFSseq(putanje);

	//vector<long> E, V;

	//cout << "Input: ";
	//inputCVE2_1(TEST_STR,V, E);
	//cout << "\nV: ";
	//copy(V.begin(), V.end(), ostream_iterator<long>(cout, ", "));
	//cout << "\nE: ";
	//copy(E.begin(), E.end(), ostream_iterator<long>(cout, ", "));
	//cout << endl;

	//vector<long> red(BFSseq(V,E));

	//copy(red.begin(), red.end(), ostream_iterator<long>(cout, ", "));

	_log.close();
	//char t = getchar();
	return 0;
}

