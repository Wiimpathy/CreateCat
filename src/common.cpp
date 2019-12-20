/****************************************************************************
 * common.cpp
 *
 * Create Category for WiiFlow
 *
 * Wiimpathy 2019
 *
 ***************************************************************************/
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include "common.h"

using namespace std;

bool debug = false;

#ifdef WIN32
char separator = '\\';
#else
char separator = '/';
#endif

char *GetFilename(char *path)
{
	char *s = strrchr(path, separator);
	
	if (!s)
		return strdup(path);
	else
		return strdup(s + 1);
}

bool FileExist(const std::string& path)
{
	FILE * f;
	f = fopen(path.c_str(), "rb");
	if(f)
	{
		fclose(f);
		return true;
	}
	return false;
}

bool containsSpecialChar(const std::string& name) 
{
	bool specialChar = false;

	for (int i = 0; name[i] != '\0'; i++)
	{
		if (isspace(name[i]) || name[i] == ':' || name[i] == '\'')
			continue;

		if (!isalnum(name[i]))
		{
			specialChar = true;
			break;
		}
	}
	return specialChar;
}

bool containsOnlyASCII(const std::string& filePath) 
{
	for (auto c: filePath)
	{
		if (static_cast<unsigned char>(c) > 127)
		{
			return false;
		}
	}
	return true;
}

int nthSubstr(int n, const string& s, const string& p)
{
	string::size_type i = s.find(p);     // Find the first occurrence

	int j;
	for (j = 1; j < n && i != string::npos; ++j)
		i = s.find(p, i+1); // Find the next occurrence

	if (j == n)
		return(i);
	else
		return(-1);
}

string upperCase(string text)
{
	char c;
	for (string::size_type i = 0; i < text.size(); ++i)
	{
		c = text[i];
		if (c >= 'a' && c <= 'z')
			text[i] = c & 0xDF;
	}
	return text;
}

// trim from start
string ltrim(string s)
{
	s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
	return s;
}

// trim from end
string rtrim(string s)
{
	s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
	return s;
}

string replaceAll( string in, string from, string to )
{
	string tmp = in;

	if ( from.empty())
	{
		return in;
	}

	size_t start_pos = 0;

	// tmp.find() fails to match on "\0"
	while (( start_pos = tmp.find( from, start_pos )) != std::string::npos )
	{
		tmp.replace( start_pos, from.length(), to );
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}

	return tmp;
}

string read_file(string filename)
{
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw std::runtime_error("Error opening: " + filename);
}

bool findStringIC(const std::string & strHaystack, const std::string & strNeedle)
{
	auto it = std::search(
	strHaystack.begin(), strHaystack.end(),
	strNeedle.begin(),   strNeedle.end(),
	[](char ch1, char ch2) { return std::toupper(ch1) == std::toupper(ch2); }
	);
	return (it != strHaystack.end() );
}

size_t LevenshteinDistance(const std::string &s1, const std::string &s2)
{
	const size_t m(s1.size());
	const size_t n(s2.size());

	if( m==0 ) return n;
	if( n==0 ) return m;

	size_t *costs = new size_t[n + 1];

	for( size_t k=0; k<=n; k++ ) costs[k] = k;

	size_t i = 0;
	for ( std::string::const_iterator it1 = s1.begin(); it1 != s1.end(); ++it1, ++i )
	{
		costs[0] = i+1;
		size_t corner = i;

		size_t j = 0;
		for ( std::string::const_iterator it2 = s2.begin(); it2 != s2.end(); ++it2, ++j )
		{
			size_t upper = costs[j+1];
			if( *it1 == *it2 )
			{
				costs[j+1] = corner;
			}
			else
			{
				size_t t(upper<corner?upper:corner);
				costs[j+1] = (costs[j]<t?costs[j]:t)+1;
			}

			corner = upper;
		}
	}
 
	size_t result = costs[n];
	delete [] costs;
 
	return result;
}

template <typename Out>
void split(const std::string &s, char delim, Out result)
{
	std::istringstream iss(s);
	std::string item;
	while (std::getline(iss, item, delim))
	{
		result++ = ltrim(item);
	}
}

std::vector<std::string> split(const std::string &s, char delim)
{
	std::vector<std::string> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
}

void ClearScreen()
{
#ifdef WIN32
	system("cls");
#else
	printf("\x1b[2J");
	printf("\x1b[1;1H\n");
#endif
}
