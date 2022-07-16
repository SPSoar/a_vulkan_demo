#ifndef S_SPSTRINGTOOLS_H_
#define S_SPSTRINGTOOLS_H_

#include "SPVariable.h"


class SPStringTools
{
public:
	struct SPHashInfo
	{
		sp_uint_32 hash0;
		sp_uint_32 hash1;
		sp_uint_32 hash2;

		inline bool operator < (const SPHashInfo& a) const
		{
			if (hash0 < a.hash0) return true;
			if (hash0 > a.hash0) return false;
			if (hash1 < a.hash1) return true;
			if (hash1 > a.hash1) return false;
			return hash2 < a.hash2;
		}

		inline sp_bool operator == (const SPHashInfo& a) const
		{
			return (a.hash0 == hash0) && (a.hash1 == hash1) && (a.hash2 == hash2);
		}
	};

	static void Trim(sp_string& str, sp_bool left = true, sp_bool right = true);

	static std::vector<sp_string> Split(const sp_string& str, const sp_string& delims = "\t\n ", sp_uint_32 maxSplits = 0, sp_bool preserveDelims = false);

	static std::vector<sp_string> Tokenise(const sp_string& str, const sp_string& delims = "\t\n ", const sp_string& doubleDelims = "\"", sp_uint_32 maxSplits = 0);

	static void To_Lower_Case(sp_string& str);
	static void To_Upper_Case(sp_string& str);
	static void To_Title_Case(sp_string& str);

	static bool Starts_With(const sp_string& str, const sp_string& pattern, sp_bool lowerCase = true);
	static bool Ends_With(const sp_string& str, const sp_string& pattern, sp_bool lowerCase = true);

	static void Standardise_Path(sp_string& path);
	static void Split_File_Name(const sp_string& qualifiedName, sp_string& outBasename, sp_string& outPath);
	static void Split_Full_File_Name(const sp_string& qualifiedName, sp_string& outBasename, 
		sp_string& outExtention, sp_string& outPath);
	static void Split_Base_File_Name(const sp_string& fullName, sp_string& outBasename, sp_string& outExtention);
	static void Remove_Trailing_Path_Separator(sp_string& path);
	static bool Match(const sp_string& str, const sp_string& pattern, sp_bool caseSensitive = true);
	static const sp_string Replace_All(const sp_string& source, const sp_string& replaceWhat, const sp_string& replaceWithWhat);
	static const sp_string Replace_String_Characters(const sp_string& source, const sp_string& replaceWhat, const sp_string& replaceWithWhat);
	static sp_bool Is_Digit(const sp_string& str);

	static sp_uint_32 Hash_String(sp_uint_32 type, const sp_char* str_in);
	static sp_uint_32 Hash_String(sp_uint_32 type, const sp_char* str_in, sp_uint_64 len);
	static SPHashInfo Get_Hash_Info_From_String(const sp_string& str);

#ifdef _WIN32
	static sp_string String_To_Utf8_String(const sp_string& str);
	static sp_string Utf8_String_To_String(const sp_string& str);
#endif

};
#endif