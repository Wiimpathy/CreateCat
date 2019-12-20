#ifndef COMMON_H
#define COMMON_H

#include <string>

using namespace std;

#define MAXPATHLEN 1024
#define KB (1024)
#define MB (1024*1024)

// Types
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

// Path separator
extern char separator;
extern bool debug;

char *GetFilename(char *path);
bool FileExist(const std::string& path);
bool containsSpecialChar(const std::string& name) ;
bool containsOnlyASCII(const std::string& filePath);
bool findStringIC(const std::string & strHaystack, const std::string & strNeedle);

int nthSubstr(int n, const string& s, const string& p);
size_t LevenshteinDistance(const std::string &s1, const std::string &s2);
std::vector<std::string> split(const std::string &s, char delim);

string read_file(string filename);
string upperCase(string text);
string ltrim(string s);
string rtrim(string s);
string replaceAll(string in, string from, string to);

void ClearScreen();
#endif
