// Win32 Console.cpp : Defines the entry polong for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <algorithm>
#include <vector>
#include <cmath>
#include <queue>
#include <ctime>
//#include <boost/progress.hpp>
#define F(x) out<<#x" = "<<x<<endl;
#define FF(x) cout<<#x" = "<<x<<endl;
#define FFC(x) copy(x.begin(),x.end(),ostream_iterator<long>(cout,","));cout<<endl;

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
		br += a*pow(10, size-1);
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
		if(br>=0)	rez.push_back(br);
	}
	return rez;
}

int _tmain()
{
	cout << sizeof(float) << endl;
	system("pause");
	return 0;
}

//int _tmain(int argc, _TCHAR* argv[])
//{
//	//ofstream out("test.out");
//	//out << "test" << endl;
//	//out.close();
//	//vector<vector<long> >graf;
//	ifstream in("simple.graph");
//	//ifstream in("luxembourg.osm.graph");
//	//ifstream in("ecology1.graph");
//	//ifstream in("ecology2.graph");
//	//ifstream in("af_shell9.graph");
//	//ifstream in("af_shell10.graph");
//	//ifstream in("audikw1.graph");
//	//ifstream in("ldoor.graph");
//	//
//	//ifstream in("kron_g500-simple-logn21.graph");
//	//
//	//ofstream out("big2.log");
//	long n=0, m=0;
//	bool flagUnm = false;
//	//in >> n >> m;
//
//	queue<long>redCvorova;
//	vector<bool>posjecenCvor;
//	vector<bool>dodanURedCvor;
//	vector<int>redoslijed;
//
//	//for (long i = 0; i < n+1; i++)
//	//{
//	//	//F(i);
//	//	string line;
//	//	std::getline(in, line);
//	//	if (line.size() == 0)
//	//		continue;
//	//	graf.push_back(strURedBrojeva(line));
//	//	posjecenCvor.push_back(false);
//	//	dodanURedCvor.push_back(false);
//	//}
//	string line="";
//	clock_t t0;
//	t0 = clock();
//	while (getline(in, line))
//	{
//		//FF(line);
//		if (line[0] == '%')
//			continue;
//		//FF(line);
//		if (!flagUnm)
//		{
//			vector<long> temp(strURedBrojeva(line));
//			//FFC(temp);
//			n = temp[0]+1;
//			m = temp[1]+1;
//			flagUnm = true;
//			FF(n);
//			FF(m);
//			graf.reserve(n);
//			continue;
//		}
//		//if (graf.size() % 5000 == 0)
//		//{
//		//	FF(graf.size());
//		//	//FF(line);
//		//}
//		graf.push_back(strURedBrojeva(line));
//		//posjecenCvor.push_back(false);
//		//dodanURedCvor.push_back(false);
//	}
//	in.close();
//	double diff0 = (double)(clock() - t0) / CLOCKS_PER_SEC;
//	cout << endl << "input: " << diff0 << endl;
//	posjecenCvor.resize(n, false);
//	dodanURedCvor.resize(n, false);
//	//cout << "Ispis grafa: " << endl;
//	//for (int i = 0; i < graf.size(); i++)
//	//{
//	//	out << "Cvor " << i << ": ";
//	//	copy(graf[i].begin(), graf[i].end(), ostream_iterator<int>(out, ","));
//	//	
//	//	out << endl;
//	//}
//	for (int brIzvrsavanja = 1; brIzvrsavanja<=6; brIzvrsavanja++)
//	{
//		std::fill(posjecenCvor.begin(), posjecenCvor.end(), false);
//		std::fill(dodanURedCvor.begin(), dodanURedCvor.end(), false);
//		//redCvorova.
//	cout << "BFS pocinje" << endl;
//	redCvorova.push(0);
//	dodanURedCvor[0] = true;
//	clock_t t1;
//	t1 = clock();
//	long brPosjecenih = 0;
//	while (redCvorova.size() > 0&& brPosjecenih<n)
//	{
//		//F(brPosjecenih);
//		//F(redCvorova.size());
//		long trenutniCvor = redCvorova.front();		
//		redCvorova.pop();
//		brPosjecenih++;
//		
//		if (posjecenCvor[trenutniCvor])
//			continue;
//		//redoslijed.push_back(trenutniCvor);
//		posjecenCvor[trenutniCvor] = true;
//
//		for (int i = 0; i < graf[trenutniCvor].size(); i++)
//		{
//			//F(graf[trenutniCvor][i]);
//			if (graf[trenutniCvor][i] == -1)
//				continue;
//			if (posjecenCvor[graf[trenutniCvor][i]] || dodanURedCvor[graf[trenutniCvor][i]])
//				continue;
//			redCvorova.push(graf[trenutniCvor][i]);
//			dodanURedCvor[graf[trenutniCvor][i]] = true;
//		}		
//	}
//	double diff = (double)(clock() - t1) / CLOCKS_PER_SEC;
//	cout << endl << brIzvrsavanja<<": The time taken for Breadth first search: " << diff << endl;
//	//brIzvrsavanja++;
//	}
//
//	//out << "Br neposjecenih: "<<count(posjecenCvor.begin(), posjecenCvor.end(), false) << endl;
//	//out << "raspored: " << endl;
//	//cout << "raspored: " << endl;
//	////for (int i = 0; i < redoslijed.size(); i++)
//	////	redoslijed[i]++;
//	//copy(redoslijed.begin(), redoslijed.end(), ostream_iterator<int>(out, ","));
//	//copy(redoslijed.begin(), redoslijed.end(), ostream_iterator<int>(cout, ","));
//	
//	system("pause");
//	return 0;
//}

