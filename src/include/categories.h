#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <locale>
#include <codecvt>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;
using namespace std;

#define DATA_DIR             "data/"
#define DATABASE_FILE        DATA_DIR"%s/%s_list.txt"
#define MOBY_FILE            DATA_DIR"MobyGenres.csv"
#define CAT_DEFAULT_PLUGIN   DATA_DIR"cat_plugin.ini"
#define CAT_DEFAULT_WIITDB   DATA_DIR"cat_wiitdb.ini"
#define CAT_INPUT_FILE       DATA_DIR"categories_lite.ini"
#define CAT_OUTPUT_FILE      "categories_lite.ini"
#define PLATFORM_FILE        DATA_DIR"platform.ini"
#define LOG_FILE             "missing.txt"

// Types
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

#ifdef WIN32
struct dir_discHdr
{
	char id[7]; //6+1 for null character
	char path[258]; // full path including partion - path - rom.ext or game.iso or id6.wbfs
	wchar_t title[125]; // wide character title : Windows wchar_t is 2 bytes!
	u8 filler1;		
	u32 magic[2];
	u8 type;
	char filler2[9];
}__attribute__((packed));
#else
struct dir_discHdr
{
	char id[7]; //6+1 for null character
	char path[259]; // full path including partion - path - rom.ext or game.iso or id6.wbfs
	wchar_t title[63]; // wide character title : Unix wchar_t is 4 bytes.
	u8 filler1;
	u32 magic[2];
	u8 type;
	char filler2[10];
}__attribute__((packed));
#endif

class Categories
{
	public:
	Categories();
//	~Categories();
	bool GetPlatform(string &filename);
	void SetDefaultCategories();
	void GetCacheContent(string &filename);
	void ReadCache(string &filename, bool skiphidden);
	void Parse(bool fuzzysearch, bool mobysearch);

	private:
	std::vector<std::string> TitleList;
	std::vector<std::string> CatList;
	std::map< int, std::vector<std::string> > DefaultCategories;
	vector<dir_discHdr> list;
	u32 GamesCount;
	string Platform;
	string Magic;
	string CatDomain;
	string GameTitle;
	string WiiflowPath;
	string genrebuffer;
	string mobybuffer;
	string catbuffer;
	char OldHidden;
	char NewHidden;
	int parsed_cat = 0;
	bool CheckHidden(string &Title);
	char CheckWiiflow(string &path);
	void parse_cat(const vector<string> &tmp_cat, string &Categories);
	string GetCategory(string title, bool fuzzy, bool moby, bool onlyID);
};
