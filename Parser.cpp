//
//  Parser.cpp
//  Ini_parser_2.0
//
//  Created by Den Fedorov on 17.09.2022.
//

#include "Parser.h"

namespace ini
{
	ini::Parser::Parser() : _lines(0), _words(0), _duration(0.0f)
	{
	}

	Parser::~Parser()
	{
	}

	void Parser::read(const std::string & s)
	{
		_filename = s;
		std::ifstream fin{ _filename };

		if (!fin.is_open())
		{
			throw std::runtime_error("Could not open \"" + _filename + "\" for reading!");
		}

		bool skip{ false };													// to skip syntax errors
		std::string line;													// to read line by line
		std::string section;												// to hold section name
		std::cmatch pieces;													// to hold regex results
		static const std::regex rgx_word{ R"x((\S+))x" };
		static const std::regex rgx_section{ R"x(\s*\[([^\]]+)\](\n?|\r?)$)x" };
		static const std::regex rgx_variable{ R"x(\s*(\w*)(\s*=\s*)(\S*)\s*(;?|#?)\s*\w*\s*$)x" };
		auto begin{ std::chrono::system_clock::now() };

		while (std::getline(fin, line))
		{
			++_lines;
			// count amount of words in file
			_words += std::distance(std::regex_token_iterator<std::string::iterator>(line.begin(), line.end(), rgx_word), {});
			// if string contains '\r' macos consider it is not empty
			if (line.empty() || *line.begin() == ';' || *line.begin() == '#' || *line.begin() == '\r')
			{
				continue;
			}
			else
			{
				if (std::regex_match(line.c_str(), pieces, rgx_section))
				{
					section = pieces[0];
				#if defined (__APPLE__)
					section.pop_back();										// remove '\r' symbol
				#endif
					if (!_file.empty() && _file.contains(section))			// if already added
					{
						continue;
					}
					else													// add new otherwise
					{
						_file[section][""] = "";
					}

					if (skip)												// reset flag for new section
					{
						skip = false;
					}
				}
				else if (std::regex_match(line.c_str(), pieces, rgx_variable))
				{	// skip all the values if section name is bad
					if (skip && !_file.contains(section))
					{
						continue;
					}
					// check if there is an empty variable in section
					else
					{	
						if (_file.at(section).contains(""))
						{
							auto temp{ _file.at(section).extract("") };
							temp.key() = pieces[1];
							_file.at(section).insert(std::move(temp));
							_file[section][pieces[1]] = pieces[3];
						}
						else
						{
							_file[section][pieces[1]] = pieces[3];
						}
					}
				}
				else
				{	// in case bad line is a section name clear it for no further match
					if (line.find_first_of("[]") != std::string::npos)
					{
						section.clear();
						_errors.push_back("Incorrect section name at line " + std::to_string(_lines));
					}
					else
					{
						_errors.push_back("Incorrect variable name at line " + std::to_string(_lines));
					}

					skip = true;
				}
			}
		}

		_duration = std::chrono::system_clock::now() - begin;
		fin.close();
	}

	void Parser::display() const
	{
		std::cout << "File: " << _filename << '\n';
		std::cout << "Lines: " << _lines << '\n';
		std::cout << "Words: " << _words << '\n';
		std::cout.precision(2);
		std::cout << "Parsing time: " << _duration.count() << " s\n\n";
		std::unordered_map<std::string, std::unordered_map<std::string, std::string>>::const_iterator i;

		for (auto outer_map{ _file.cbegin() }; outer_map != _file.cend(); outer_map = i)
		{
			std::cout << outer_map->first << '\n';
			auto key_range = _file.equal_range(outer_map->first);

			for (i = key_range.first; i != key_range.second; ++i)
			{
				for (auto j = outer_map->second.cbegin(); j != outer_map->second.cend(); ++j)
				{
					if (j->first.empty())
					{
						std::cout << "<empty>\n";
					}
					else
					{
						std::cout << std::left << std::setw(28);
						std::cout << j->first;
						std::cout << " : ";
						std::cout << j->second << '\n';
					}
				}
				std::cout << '\n';
			}
		}

		if (_errors.size() == 0)
		{
			std::cout << "No errors found!\n";
		}
		else
		{
			for (auto const & line : _errors)
			{
				std::cout << line << '\n';
			}
		}

		std::cout << '\n';
	}

	std::string Parser::find(const std::string & s) const
	{
		std::string section("[");
		std::string variable;
		std::cmatch pieces;
		static const std::regex rgx{ R"x((\w*)(\.)(\w*))x" };

		if (std::regex_match(s.c_str(), pieces, rgx))
		{
			section += pieces[1];
			section.append("]");
			variable = pieces[3];

			if (_file.contains(section))
			{
				if (_file.at(section).contains(variable))
				{
					if (_file.at(section).find(variable)->second.empty())
					{
						throw std::invalid_argument("Variable \"" + variable + "\" is empty!");
					}
					else
					{
						return _file.at(section).find(variable)->second;
					}
				}
				else
				{
					throw std::invalid_argument("No variable found at section " + section + "!");
				}
			}
			else
			{
				throw std::invalid_argument("No section found!");
			}
		}
		else
		{
			throw std::invalid_argument("Invalid argument passed!");
		}
	}
}
