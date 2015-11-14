
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

#include "tinydir.h"

#define TEST_STR "input\\simple.prs"
#define F(x) cout<< #x " = "<<x<<endl;
#define FL(x) _log<<" "#x " = "<<x<<" ";
#define FI 0.0001
const int BR_PONAVLJANJA = 5;

using namespace std;

ofstream _log("sviGrafoviInputRelease.log");

extern "C" bool runTest();

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
				n = rezB[0];
				m = rezB[1];
				V.reserve(n);
				E.reserve(2 * m + 10);
				nNM = false;
				continue;
			}

			parse2_1(line, E);

			//for (int k = 0; k < rezB.size(); k++)
			//	E.push_back(rezB[k]);
			//if (V.size() < n)
				V.push_back(E.size());
			//graf.push_back(rezB);
		}
		double diff = (double)(clock() - t0) / CLOCKS_PER_SEC;
		//cout << endl << "The time taken for inputCV(const string&path, vector<vector<long>>&graf): " << diff << "ms" << endl;
		FL(n); FL(m);
		_log << " " << diff << " ms\n";

		//cout << "V: ";
		//copy(V.begin(), V.end(), ostream_iterator<long>(cout, ", "));
		//cout << "\nE: ";
		//copy(E.begin(), E.end(), ostream_iterator<long>(cout, ", "));
		//in.close();

		return diff;
	}

	return 0;
}

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

int main()
{
	
	//printf("test\n");
	//runTest();
	//if (input("input\\simple.prs"))
	//{
	//	cerr << "Problem sa otvaranjem file-a\n";
	//	char t = getchar();
	//	exit(EXIT_FAILURE);
	//}
	
	vector<string>putanje;
	if (putanjeGrafova(putanje))
	{
		cerr << "problemi\n";
		exit(EXIT_FAILURE);
	}
	copy(putanje.begin(), putanje.end(), ostream_iterator<string>(cout, ", "));
	cout << "\n\nOtvoriti sve iz input foldera\n";
	
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
		//	vector<vector<long> >temp;
		//	inputCV3(putanje[i], temp,_log);
		//}
		//_log << endl;
		//int rei = iOfSmallest(prosjeci);
		//_log << "\nNajmanji prosjek za graf " << putanje[i] << " je: " << prosjeci[rei] << " na mjestu " << rei << endl << endl;
	}

	//FILE *fp;
	//char *line = NULL;
	//size_t len = 0;
	//SSIZE_T read;

	//fp = fopen(TEST_STR,"r");
	//if (fp == NULL)
	//	exit(EXIT_FAILURE);

	//while ((read = getline(&line, &len, fp)) != -1)
	//{

	//}

	//fclose(fp);

	//char str[] = "45 87 22 ";
	//char * pch;
	//pch = strtok(str, " ");
	//while (pch != NULL)
	//{
	//	printf("%s\n", pch);
	//	pch = strtok(NULL, " ");
	//}

	//ifstream in(TEST_STR);
	//
	//string line = "";
	//while (getline(in,line))
	//{
	//	if (line[0] == '%')
	//		continue;
	//	//vector<string>rez;
	//	vector<long>rezB;
	//	char * temp = new char[line.length() + 1];
	//	std::strcpy(temp, line.c_str());
	//	char * pch  = strtok(temp, " ");
	//	std::string::size_type sz;
	//	while (pch != NULL)
	//	{
	//		//rez.push_back(pch);
	//		rezB.push_back(stol(pch,&sz));
	//		pch = strtok(NULL, " ");
	//	}
	//	//cout << "rez: ";
	//	//copy(rez.begin(), rez.end(), ostream_iterator<string>(cout, ", "));
	//	//cout << endl;
	//	cout << "rezB: ";
	//	copy(rezB.begin(), rezB.end(), ostream_iterator<long>(cout, ", "));
	//	cout << endl;
	//	delete[] temp;
	//}

	//in.close();
	_log.close();
	//char t = getchar();
	return 0;
}

