#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>

#define BUFFER_SIZE 2048

struct General
{
	static std::vector<std::string> split(const std::string& s)
	{
		std::vector<std::string> strings;
		std::istringstream strngs;
		strngs.str(s);
		std::string nstr{};
		for (std::string str{}; std::getline(strngs, str);)
		{
			std::cout << str << std::endl;
			strings.push_back(str);
		}
		for (int i{}; i < strings.size(); i++)
		{
			if (i == strings.size() - 1)
			{
				for (int j{}; j < strings[i].size(); j++)
				{

					if (strings[i][j] == 'Ì' && strings[i][++j] == 'Ì')
					{
						nstr.push_back('\0');
						break;
					}
					nstr.push_back(strings[i][j]);
				}
				std::swap(strings[i], nstr);
			}
		}
		return strings;
	}

	static std::string merge_strings(std::vector<std::string>& s)
	{
		int i{};
		std::string rstr;
		for (auto str : s)
		{
			i++;
			rstr += str;
			if (i != str.size())
				rstr += '\n';
		}
		return rstr;
	}

	static BOOL insertRow(DWORD row, LPWSTR wstr, std::vector<std::string>& s)
	{
		std::vector<std::string> ns;

		std::string str;
		bool postisr{ false };

		wstring_to_string(wstr, str);

		for (int i{}; i <= s.size(); i++)
		{
			if (i == row - 1)
			{
				ns.push_back(str);
				postisr = true;
				continue;
			}
			if (postisr)
			{
				ns.push_back(s[i - 1]);
				continue;
			}
			ns.push_back(s[i]);
		}
		s = ns;
		return TRUE;
	}

	static void string_to_wstring(const std::string& src, std::wstring& dest)
	{
		std::wstring tmp;
		tmp.resize(src.size());
		std::transform(src.begin(), src.end(), tmp.begin(), btowc);
		tmp.swap(dest);
	}

	static void wstring_to_string(const std::wstring& src, std::string& dest)
	{
		std::string tmp;
		tmp.resize(src.size());
		std::transform(src.begin(), src.end(), tmp.begin(), wctob);
		tmp.swap(dest);
	}

	static std::string GetFileName(LPWSTR filePath)
	{
		std::string fileName;
		int i;
		for (i = std::wcslen(filePath); (filePath[i] != '\\') && (filePath[i] != '/') && i >= 0; --i)
			;
		while (filePath[++i] != '\0')
			fileName += filePath[i];
		return fileName;
	}

};