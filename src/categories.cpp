/****************************************************************************
 * categories.cpp
 *
 * Create Category for WiiFlow
 *
 * Wiimpathy 2019
 *
 ***************************************************************************/
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <cstring>
#include <cassert>
#include <algorithm>
#include <unistd.h>
#include <locale>
#include <codecvt>
#include <regex>
#include <experimental/filesystem>
#include "common.h"
#include "categories.h"
#include "utf8.h"

namespace fs = std::experimental::filesystem;

Categories::Categories()
{
	vector<string> DataFiles = {CAT_DEFAULT_PLUGIN, CAT_DEFAULT_WIITDB, CAT_INPUT_FILE, PLATFORM_FILE, CAT_INPUT_FILE, MOBY_FILE};

	for(auto File : DataFiles)
	{
		if(!FileExist(File))
			throw std::runtime_error("Error opening: " + File);
	}
}

// Extract the fields from the platform database(ex: SUPERNES_list.txt) 
// It should return all found fields seperated by commas.
// idx: field index, s: the whole game line, delimiter: the pipe '|'
string extract(int idx, const string& s, const string& delimiter)
{
	string token;

	// Find last token when the number of total tokens is unknown
	if(idx == -1)
	{
		token = s.substr( s.find_last_of("|") + 1);
		return token;
	}

	int count = 0;
	auto start = 0U;
	auto end = s.find(delimiter);

	while (end != std::string::npos)
	{
		if(count == idx)
		{
			token = s.substr(start, end - start);
			return token;
		}
		start = end + delimiter.length();
		end = s.find(delimiter, start);
		count++; 
	}
	
	// Last found token
	if(count > 1)
		token =  s.substr(start, end);

	return token;
}

int FuzzySearch(string &test, string &pattern)
{
	size_t dist = LevenshteinDistance(test, pattern);
	int max_distance;

	if(pattern.length() >= 7 && pattern.length() <= 10)
	{
		max_distance = 1;
	}
	else if(pattern.length() > 10 && pattern.length() <= 20)
	{
		max_distance = 3;
	}
	else if(pattern.length() > 20)
	{
		max_distance = 4;
	}

	if(dist > 0 && dist <= max_distance)
		return dist;
	else
		return 0;
}

// Field 4 to 6 : Genres/Rating|Player #|Coop
// Field 7 to 9 : Dev/Publisher/Year
// Those latter fields could be added too.
int FormatFoundCategories(string &line, string &genre)
{
	for(int i=4; i<7; i++)
	{
		genre += extract(i, line, "|");

		// Add possibly missing commas that'd break the final parsing
		if(isprint(genre.back()) && genre.back() != ',')
			genre += ',';
	}
return 0;
}

/* Retrieve a category list for the given game. */
//
// The simpler case is for Wii/GC. Search by ID in WII_list.txt
// For plugins, it will search the corresponding database by Title if ID search has failed.
// If not found, a fuzzy search will start if the option is enabled.
// At last, the MobyGenres.csv could be used.
// This file is a Mobygames offline and incomplete database from MetropolisLauncher.
// The games genres is this csv are not platform specific.
string Categories::GetCategory(string pattern, bool fuzzy, bool moby, bool onlyID)
{
	istringstream genrelist(genrebuffer);
	istringstream mobylist(mobybuffer);
	string genre = "";
	string line;

	// Disable fuzzy search for short title
	if(pattern.length() <= 6)
		fuzzy = false;

	// Search in the Platform database
	while(std::getline(genrelist, line))
	{
		if(onlyID)
		{
			if(line.find(pattern) != std::string::npos)
			{
				FormatFoundCategories(line, genre);

				if(!genre.empty())
					break;
			}
		}
		else
		{
			if(findStringIC(line, pattern))
			{
				FormatFoundCategories(line, genre);

				if(!genre.empty())
					break;
			}

			if(fuzzy)
			{
				string test = extract(1, line, "|");
				int SimilarMatch = FuzzySearch(test, pattern);
				
				if(SimilarMatch)
				{
					FormatFoundCategories(line, genre);

					if(!genre.empty())
					{
						if(debug)
							cout << "Data Similarity=" << SimilarMatch << " : " << test << endl;
						break;
					}
				}
			}
		}
	}

	// Search the title in the Mobygames csv
	if(moby && !onlyID)
	{
		if(genre.empty())
		{
			while(std::getline(mobylist, line, '\n'))
			{
				if(findStringIC(line, pattern))
				{
					FormatFoundCategories(line, genre);

					if(!genre.empty())
					{
						if(debug)
							cout << "MobyGenres.csv: " << line << endl;
						break;
					}
				}

				if(fuzzy && genre.empty())
				{
					string test = extract(2, line, "|");
					int SimilarMatch = FuzzySearch(test, pattern);
			
					if(SimilarMatch)
					{
						FormatFoundCategories(line, genre);

						if(!genre.empty())
						{
							if(debug)
								cout << "Moby Similarity=" << SimilarMatch << " : " << test << endl;
							break;
						}
					}
				}
			}
		}
	}

	return genre;
}

// Look for hidden games in an existing categories_lite.ini
// Return the character corresponding to the old hidden.
// Assign the character from the default categories_lite.ini
// to the new hidden category.
char Categories::CheckWiiflow(string &path)
{
	WiiflowPath = path.substr(0, path.find("cache") ) ;
	WiiflowPath = WiiflowPath + "settings" + separator + CAT_OUTPUT_FILE;
	string OldHidden;

	if(debug)
		cout << "Wiiflow categories_lite: " << WiiflowPath << endl << endl;

	if(!FileExist(WiiflowPath))
	{
		cout << "No categories_lite.ini found in: " << WiiflowPath << endl;
		return 0;
	}
	else
	{
		bool FoundDomain = false;
		string Domain;
		string line;

		if(Platform == "WII")
			Domain = "[GENERAL]";
		else
			Domain = "[PLUGINS]";

		ifstream CatFile(CAT_INPUT_FILE, ios::in);

		while(getline(CatFile, line))
		{
			if (line.find(Domain) != std::string::npos)
			{
				FoundDomain = true;
			}

			if (FoundDomain && line.find("hidden_categories=") != std::string::npos)
			{
				string hidden = line.substr(line.find_last_of("=") + 1, line.length());
				if(!hidden.empty())
				{
					if(debug)
						cout << "New Hidden cat: " << hidden[0] << endl;

					NewHidden = hidden[0];
					break;
				}
			}
		}
		CatFile.close();

		static bool FoundMagic = false;
		static int length = 0;
		ifstream OldWiiflowFile(WiiflowPath, ios::in);
		OldWiiflowFile.seekg(0);

		// Search if a hidden category exists
		while(getline(OldWiiflowFile, line))
		{
			if (line.find("hidden_categories=") != std::string::npos)
			{
				OldHidden = line.substr(line.find_last_of("=") + 1, line.length());

				if(!OldHidden.empty())
				{
					if(debug)
						cout << "Old Hidden cat: " << OldHidden[0] << endl;

					break;
				}
			}
		}

		// Find all games in the hidden category and save them in Catlist
		OldWiiflowFile.clear();
		OldWiiflowFile.seekg(0);

		while(getline(OldWiiflowFile, line))
		{
			if (!FoundMagic)
			{
				if(findStringIC(line, CatDomain) )
				{
					FoundMagic = true;
					continue;
				}
			}

			if(FoundMagic)
			{
				if(line[0] == '[')
				{
					break;
				}

				string foundcat = line.substr( line.find_last_of("=") + 1);

				if (foundcat.find(OldHidden[0]) != std::string::npos)
					CatList.push_back(line);
			}
		}
		OldWiiflowFile.close();
		
		if(!OldHidden.empty())
			return OldHidden[0];
	}
	return 0;
}

//Check if the current game was hidden
bool Categories::CheckHidden(string &Title)
{
	static int length = 0;
	static bool FoundDomain = false;
	bool specialChar = containsSpecialChar(Title);
	string regex_escaped;

	for(auto Cat : CatList)
	{
		if (specialChar)
		{
			std::regex specialChars { R"([-[\]{}()*+?.,\^$|#\s])" };
			std::string sanitized = std::regex_replace( Title, specialChars, R"(\$&)" );
			regex_escaped = "^" + sanitized + "=";
			std::regex pattern(regex_escaped, std::regex::optimize);

			if(std::regex_match(Cat, pattern))
			{
				string foundcat = Cat.substr(Cat.find_last_of("=") + 1);

				if (foundcat.find(OldHidden) != std::string::npos)
				{
					return true;
				}
			}
		}
		else
		{
			regex_escaped = "^" + Title + "=";
			std::regex pattern(regex_escaped);

			if(std::regex_search(Cat, pattern))
			{
				string foundcat = Cat.substr(Cat.find_last_of("=") + 1);

				if (foundcat.find(OldHidden) != std::string::npos)
				{
					return true;
				}
			}
		}
	}
	return false;
}

void Categories::parse_cat(const vector<string> &tmp_cat, string &Categories)
{
	Categories.clear();

	for(auto FoundCat : tmp_cat)
	{
		for(int i = 0; i < DefaultCategories.size(); i++)
		{
			for(int j = 0; j < DefaultCategories[i].size(); j++)
			{
				if(FoundCat == DefaultCategories[i][j])
				{
					char cCh = static_cast<char>((i + 1) + 32);
					size_t cat_exist = Categories.find(cCh);

					// Skip already parsed category
					if(cat_exist == std::string::npos)
					{
						Categories += cCh;
					}

					if(debug)
					{
						string cat_name = "|Cat Name: " + DefaultCategories[i][j];
						string cat_idx = "|cat" + std::to_string(i+1) + "=";
					}
				}
			}
		}
	}
	// Append hidden character if necessary
	if(OldHidden && CheckHidden(GameTitle))
	{
		Categories += NewHidden;

		if(debug)
			cout << "Hidden cat: " << GameTitle << " " << Categories << endl;
	}
}

void Categories::SetDefaultCategories()
{
	std::ifstream CustomCatFile;

	if(Platform == "WII" || Platform == "GAMECUBE")
	{
		CustomCatFile.open(CAT_DEFAULT_WIITDB, std::ios::in | std::ios::binary);
	}
	else
	{
		CustomCatFile.open(CAT_DEFAULT_PLUGIN, std::ios::in | std::ios::binary);
	}

	int cat_idx = 0;
	int maxcat = 0;
	string CatToken;

	while(getline(CustomCatFile, CatToken))
	{
		// Map the default categories.
		DefaultCategories[maxcat++];
		if (CatToken.find('=') != std::string::npos)
		{
			CatToken = CatToken.substr( CatToken.find_last_of("=") + 1);
			std::vector<std::string> tmp_cat = split(CatToken, ',');

			for(auto cat : tmp_cat)
			{
				DefaultCategories[cat_idx].push_back( cat );
				if(debug)
					cout << "cat.ini: " <<  cat_idx << "=" << cat << endl;
			}
			cat_idx++;
		}
		maxcat++;
	}
	CustomCatFile.close();

	if(debug)
		printf("\ndir_discHdr size:%lu\n\n", sizeof (dir_discHdr));
}

bool Categories::GetPlatform(string &filename)
{
	bool isDbFile = false;

	if (filename.find("_") != std::string::npos)
	{
		Magic = strrchr(filename.c_str(), '_') + 1;

		if(Magic.find(".db") != std::string::npos)
		{
			Magic = Magic.substr(0, Magic.find(".db"));

			if(debug)
				cout << "Magic:" << Magic << endl;

			isDbFile = true;
		}
	}

	if(!isDbFile)
	{
		cout<<"Error! Not a .db file!\n";
		exit(1);
	}

	int found_Magic = 0;
	string PlatformDomainTxt;

	if(Magic == "wii")
	{
		Platform = "WII";
		CatDomain = "[" + Platform + "]";
		PlatformDomainTxt = Platform + CatDomain;
		found_Magic = 1;
	}
	else if(Magic == "gamecube")
	{
		// GC/Wii games share the same database(WII_list.txt) hence same Platform name here.
		Platform = "WII";
		CatDomain = "[" + upperCase(Magic) + "]";
		PlatformDomainTxt = upperCase(Magic) + CatDomain;
		found_Magic = 1;
	}
	else
	{
		CatDomain = "[" + Magic + "]";

		// Get plugin's platform name
		ifstream inputFile(PLATFORM_FILE, ios::in);
		string line;

		while(getline(inputFile, line))
		{
			if(findStringIC(line, Magic) )
			{
				found_Magic = 1;
				Platform = line.substr(line.find_last_of("=") + 1, line.length());
				PlatformDomainTxt = Platform + CatDomain;
				break;
			}
		}
		inputFile.close();
	}

	if(!found_Magic)
	{
		cout << "Can't find platform name!" << CatDomain << endl;
		return false;
	}
	else
	{
		cout << "\nPLATFORM: " << PlatformDomainTxt << endl;
		return true;
	}
}

void Categories::ReadCache(string &filename, bool skiphidden)
{
	char DataFile[1024];
	snprintf(DataFile, sizeof(DataFile), DATABASE_FILE, Platform.c_str(), Platform.c_str());

	if(debug)
		cout << "Opening database: " << DataFile << endl;

	if(!FileExist(DataFile))
	{
		cout << "Can't open database: " << DataFile << endl;
			exit(0);
	}

	if(skiphidden)
	{
		OldHidden = '0';
	}
	else
	{
		OldHidden = CheckWiiflow(filename);
	}

	if(!FileExist(CAT_OUTPUT_FILE))
	{
		fs::copy(CAT_INPUT_FILE, CAT_OUTPUT_FILE);
	}
	else
	{
		ifstream inputFile(CAT_OUTPUT_FILE, ios::in);
		string line;

		while(getline(inputFile, line))
		{
			if(findStringIC(line, CatDomain) )
			{
				cout << CatDomain << " already in " << CAT_OUTPUT_FILE << endl;
				exit(0);
			}
		}
		inputFile.close();
	}

	genrebuffer = read_file(DataFile);
	mobybuffer = read_file(MOBY_FILE);

	FILE* db_cache = NULL; 
	db_cache = fopen(filename.c_str(), "rb");

	if(!db_cache)
		exit(1);
	
	if(debug)
		printf("Loading DB: %s\n", filename.c_str());

	dir_discHdr tmp;

	fseek(db_cache, 0, SEEK_END);
	u64 fileSize = ftell(db_cache);
	fseek(db_cache, 0, SEEK_SET);

	GamesCount = (u32)(fileSize / sizeof(dir_discHdr));
	list.reserve(GamesCount + list.size());

	if(debug)
		printf("char:%lu||wchar_t:%lu||u8:%lu||u32:%lu\n", sizeof (char), sizeof (wchar_t), sizeof (u8), sizeof (u32));

	for(u32 i = 0; i < GamesCount; i++)
	{
		fseek(db_cache, i * sizeof(dir_discHdr), SEEK_SET);
		fread((void *)&tmp, 1, sizeof(dir_discHdr), db_cache);
		list.push_back(tmp);
	}

	fclose(db_cache);

	if(debug)
		cout << "GamesCount: " << GamesCount << endl << endl;
}


void Categories::Parse(bool fuzzysearch, bool mobysearch)
{
	ofstream CatOutput;
	CatOutput.open(CAT_OUTPUT_FILE, std::ios_base::app | std::ios::binary);
	CatOutput << endl;
	CatOutput << CatDomain << endl;

	ofstream Log;
	Log.open(LOG_FILE, std::ios_base::app | std::ios::binary);
	Log << endl;
	Log << CatDomain << endl;

	if(GamesCount > 1000 && (fuzzysearch || mobysearch ))
	{
		cout << "Writing categories..." << endl;
	}
	
	for(vector<dir_discHdr>::iterator hdr = list.begin(); hdr != list.end(); ++hdr)
	{
		string GameId = hdr->id;
		string Categories;

#ifdef WIN32
		// setlocale(LC_CTYPE, "UTF-8");
		
		// FIXME: 2 choices to badly handle wchar_t conversion here. Both are hacks.
		// It'd be better to use Windows native solutions.
		// But since I crosscompile on Linux, it's easier this way.
		
		// 1) Using UTF-8 lib from http://utfcpp.sourceforge.net/
		string tmp_title;
		utf8::utf16to8(hdr->title, hdr->title + 124, back_inserter(tmp_title));
		GameTitle = replaceAll(tmp_title, {'\0'}, "");
  		   	
		// 2) Using codecvt from c++11
		/*
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		for(int i=0;i<125;i+=2)
		{
			std::string tmp = converter.to_bytes(&hdr->title[i]);
			GameTitle += tmp;
		}*/	
#else
		string tmp_title;
		utf8::utf32to8(hdr->title, hdr->title + 62, back_inserter(tmp_title));
		GameTitle = replaceAll(tmp_title, {'\0'}, "");

		// codecvt will abort with invalid character on Linux. On Windows there's no error!

		/*wchar_t tmp_title[252];
		wmemcpy(tmp_title, hdr->title, 252);
		
		try
		
			GameTitle = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>{}.to_bytes(tmp_title);
		}

		catch (const std::range_error & exception)
		{
			cout << "Error converting GameTitle" << endl;
				continue;
		}*/
#endif

		if(debug)
			cout << "#### ID:" << GameId << " Title:" << GameTitle << " ####" << endl;

		if(GameTitle.empty())
		{
			if(debug)
				cout << "GameTitle is empty!\n";
				continue;
		}
		else
		{
			// Skip duplicate titles
			if (std::find(TitleList.begin(), TitleList.end(), GameTitle) != TitleList.end())
			{
				if(debug)
					cout << "Already parsed!\n";

				parsed_cat++;
				continue;
			}
			else
			{
				TitleList.push_back(GameTitle);
			}
		}

		string GameCat = GetCategory(GameId, fuzzysearch, mobysearch, true);
		std::vector<std::string> tmp_cat;

		if(GameCat.empty())
		{
			if(Platform != "WII")
			{
				string ShortName = GameTitle.substr(0, GameTitle.find(" (")).substr(0, GameTitle.find(" ["));
				string pattern = "|" + ShortName ;
				GameCat = GetCategory(pattern, fuzzysearch, mobysearch, false);

				if(debug)
					cout << "FOUND[Title]: " << GameCat << endl;
			}
		}
		else
		{
			if(debug)
				cout << "FOUND[ID]: " << GameCat << endl;
		}

		if(!GameCat.empty())
		{
			size_t multi_cat = GameCat.find(',');

			if(multi_cat != std::string::npos)
			{
				tmp_cat = split(GameCat, ',');
			}
			else
			{
				tmp_cat.push_back(GameCat);
			}
			parse_cat(tmp_cat, Categories);
		}

		string GameCatTxt;

		if(Platform == "WII")
		{
			GameCatTxt =  GameId + "=";
		}
		else
		{
			GameCatTxt =  GameTitle + "=";
		}

		if(!Categories.empty())
		{
			CatOutput << GameCatTxt << Categories << endl;
			parsed_cat++;
		}
		else
		{
			Log << GameCatTxt << endl;
		}
	}

	cout << "Total found: " << parsed_cat << "/" << GamesCount << endl << endl;
	CatOutput.close();
	Log.close();
}
