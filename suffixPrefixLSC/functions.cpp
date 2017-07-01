#include<string>
#include<vector>
#include<iostream>
#include<fstream>
#include<algorithm>
#include<random>
#include<time.h>
#include"functions.h"

std::vector< std::vector<int> > calculateSuffixPrefixLCS(const std::string &A, const std::string &B) {
	int n = A.length();
	int m = B.length();
	int*** crP = new int**[n + 1]; //critical points 
	for (int i = 0; i <= n; ++i) {
		crP[i] = new int*[m + 1];
	}
	for (int i = 0; i <= n; ++i) {
		for (int j = 0; j <= m; ++j) {
			crP[i][j] = new int[2];
		}
	}

	for (int i = 0; i <= n; ++i)
		for (int j = 0; j <= m; ++j) {
			crP[i][j][0] = 0;
			crP[i][j][1] = 0;
		}
	for (int j = 0; j <= m; ++j) {
		crP[0][j][0] = 0;
	}

	for (int i = 0; i <= n; ++i) {
		crP[i][0][1] = i + 1;
	}
	for (int j = 0; j < m; ++j)
		for (int i = 0; i < n; ++i) {
			int vert = crP[i][j][0];
			int hor = crP[i][j][1];
			if (vert < hor) {
				if (A[i] == B[j]) {
					crP[i + 1][j][0] = hor;
					crP[i][j + 1][1] = vert;
				}
				else {
					crP[i + 1][j][0] = vert;
					crP[i][j + 1][1] = hor;
				}
			}
			else {
				crP[i + 1][j][0] = hor;
				crP[i][j + 1][1] = vert;
			}
		}
	//--------------------------------------------— 
	std::vector< std::vector<int> > table;
	table.resize(n + 1);
	for (int i = 0; i <= n; ++i) {
		table[i].resize(m + 1);
	}
	for (int i = 0; i <= n; ++i) {
		table[i][m] = 0;
	}
	for (int i = 0; i <= m; ++i) {
		table[0][i] = 0;
	}
	for (int j = 1; j <= m; ++j) {
		int vert = crP[n][j - 1][0];
		for (int i = 0; (i < vert && i <= n); ++i)
			table[i][j] = table[i][j - 1] + 1;
		for (int i = vert; i <= n; ++i)
			table[i][j] = table[i][j - 1];
	}
	for (int i = 0; i <= n; ++i)
		for (int j = 0; j <= m; ++j) {
			delete[] crP[i][j];
		}
	for (int i = 0; i <= n; ++i) {
		delete[] crP[i];
	}
	delete[] crP;
	//--— 
	return table;
}

int LCS(const std::string &A, const std::string &B) {
	int la = A.length();
	int lb = B.length();
	std::vector< std::vector<int> > answer(la + 1);
	for (int i = 0; i <= la; i++) {
		answer[i].resize(lb + 1);
	}

	for (int i = 0; i < la; i++) {
		answer[i][0] = 0;
	}
	for (int i = 0; i < lb; i++) {
		answer[0][i] = 0;
	}

	for (int i = 1; i <= la; i++)
		for (int j = 1; j <= lb; j++)
		{
			if (A[i - 1] == B[j - 1])
			{
				answer[i][j] = answer[i - 1][j - 1] + 1;
			}
			else
			{
				answer[i][j] = std::max(answer[i][j - 1], answer[i - 1][j]);
			}
		}
	return answer[la][lb];
}

std::vector< std::vector<int> > naiveSuffixPrefixLCS(const std::string &A, const std::string &B) {
	int la = A.length();
	int lb = B.length();

	std::vector< std::vector<int> > answer;
	answer.resize(la + 1);
	for (int i = 0; i <= la; i++) {
		answer[i].resize(lb + 1);
	}

	for (int i = 0; i <= la; i++)
		for (int j = 0; j <= lb; j++) {
			answer[i][j] = LCS(A.substr(i, la), B.substr(0, j));
		}

	return answer;
}

void print(const std::vector<std::vector<int> > &v, std::fstream &f) {
	for (int i = 0; i < v[0].size(); ++i) {
		for (int j = 0; j < v.size(); ++j)
			f << v[j][i] << " ";
		f << "\n";
	}
}

void printAnswer() {
	std::string A;
	std::string B;

	std::cin >> A;
	std::cin >> B;
	std::vector<std::vector<int> > v;

	v = calculateSuffixPrefixLCS(A, B);

	for (int i = 0; i < v[0].size(); ++i) {
		for (int j = 0; j < v.size(); ++j)
			std::cout << v[j][i] << " ";
		std::cout << "\n";
	}
}

void checkFromFile(std::string fileName) {
	std::string A;
	std::string B;
	std::string nameTest;
	std::vector<std::vector<int> > v1;
	std::vector<std::vector<int> > v2;
	bool res = true;

	std::fstream file;
	file.open(fileName, std::fstream::in);
	while (!file.eof()) {
		file >> nameTest;
		std::cout << '>' << nameTest << std::endl;
		file >> A;
		file >> B;
		v2.resize(A.length() + 1);
		for (int i = 0; i < v2.size(); ++i) {
			v2[i].resize(B.length() + 1);
		}
		v1 = calculateSuffixPrefixLCS(A, B);
		for (int i = 0; i < v1[0].size(); ++i) {
			for (int j = 0; j < v1.size(); ++j) {
				std::cout << v1[j][i] << " ";
			}
			std::cout << "\n";
		}
		for (int j = 0; j <= B.size(); ++j) {
			for (int i = 0; i <= A.size(); ++i) {
				file >> v2[i][j];
				if (v1[i][j] != v2[i][j]) res = false;
			}
		}
		if (res == false) {
			std::cout << "error\n\n";
			res = true;
		}
		else {
			std::cout << "checked\n\n";
		}
	}
	file.close();
}

void generateTestToFile(std::string fileName) {
	int num = 0;
	std::fstream file;
	file.open(fileName, std::fstream::out);
	std::string a[8];
	srand(time(NULL));
	for (int i = 0; i < 8; ++i) {
		int length = rand() % 60;
		for (int j = 0; j < length; ++j) {
			a[i].resize(length);
			int k = rand() % 26;
			k += 'a';
			a[i][j] = static_cast<char>(k);
		}
	}
	for (int i = 0; i < 7; ++i)
		for (int j = i + 1; j < 8; ++j) {
			file << "Test" << ++num << "\n";
			file << a[i] << "\n";
			file << a[j] << "\n";
			std::vector<std::vector<int> > v = naiveSuffixPrefixLCS(a[i], a[j]);
			print(v, file);
		}
	file.close();
}
