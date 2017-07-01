#pragma once
#include<string>
#include<vector>
#include<fstream>

std::vector< std::vector<int> > calculateSuffixPrefixLCS(const std::string&, const std::string&);

std::vector< std::vector<int> > naiveSuffixPrefixLCS(const std::string&, const std::string&);

void print(const std::vector<std::vector<int> >&, std::fstream&);

void printAnswer();

void checkFromFile(std::string);

void generateTestToFile(std::string);

void generateBigRandomTestToFile(std::string);
