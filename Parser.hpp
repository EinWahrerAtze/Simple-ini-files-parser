//
//  Parser.hpp
//  Parser
//
//  Created by Den Fedorov on 06.10.2022.
//

#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>
#include <filesystem>
#include <stdexcept>
#include <iomanip>
#include <chrono>
#include <regex>
#include <any>

namespace ini
{
	class Parser
	{
	public:
		Parser(std::string_view input);
		~Parser();
		template<typename T>
		T get_value(std::string_view line) const;
		bool utility(std::string_view line) const;
	private:
		enum class Mode
		{
			HELP,
			INFO,
			ERRORS,
		};
		void read();
		void display(Mode mode) const;
		void display(std::string_view section) const;
		void clear() const;
		std::size_t _lines;
		std::size_t _words;
		std::filesystem::path _path;
		std::vector<std::string> _errors;
		std::chrono::duration<float> _duration;
		std::unordered_map<std::string, std::unordered_map<std::string, std::any>> _file;
	};

	ini::Parser::Parser(std::string_view input) : _path(input), _words(0), _lines(0)
	{
		this->clear();
		// if file exists
		if (std::filesystem::is_regular_file(_path))
		{
			this->read();
		}
		// else search directory
		else
		{	
			bool found{ false };
			std::string filename{ _path.filename().string() };
			// if only filenames was passed
			if (!_path.has_root_path())
			{
				_path = std::filesystem::current_path();
			}
			// search for filename in current folder and subfolders
			for (const auto & i : std::filesystem::recursive_directory_iterator{ _path.remove_filename() })
			{
				if (i.path().filename() == filename)
				{
					_path = i.path().string();
					found = true;
					break;
				}
			}
			if (found)
			{
				this->read();
			}
			// search for any .ini file in current folder and subfolders
			else
			{
				for (const auto & i : std::filesystem::recursive_directory_iterator{ _path })
				{
					if (i.path().filename().extension() == ".ini")
					{
						_path = i.path().string();
						found = true;
						break;
					}
				}
				if (found)
				{
					this->read();
				}
				else
				{
					throw std::runtime_error("no .ini found at " + _path.string());

				}
			}
		}
	}

	ini::Parser::~Parser() { }

	template<typename T>
	T Parser::get_value(std::string_view line) const
	{
		std::string section;
		std::string variable;
		std::cmatch fragments;
		const std::regex line_data{ "(\\w*\\s*?\\w*)(\\.)(\\w*)" };

		if (std::regex_match(line.data(), fragments, line_data))
		{
			section = fragments[1];
			variable = fragments[3];
			if (_file.contains(section))
			{
				if (_file.at(section).contains(variable))
				{
					try
					{
						if constexpr (std::is_same_v<T, bool>)
						{
							if (_file.at(section).at(variable).type() == typeid(bool))
							{
								return std::any_cast<bool>(_file.at(section).at(variable));
							}
							else
							{
								throw std::invalid_argument("\"" + variable + "\" contains " + _file.at(section).at(variable).type().name());
							}
						}
						else if constexpr (std::is_same_v<T, int>)
						{
							if (_file.at(section).at(variable).type() == typeid(int))
							{
								return std::any_cast<int>(_file.at(section).at(variable));
							}
							else if (_file.at(section).at(variable).type() == typeid(bool))
							{
								return (std::any_cast<bool>(_file.at(section).at(variable)) ? 1 : 0);
							}
							else
							{
								throw std::invalid_argument("\"" + variable + "\" contains " + _file.at(section).at(variable).type().name());
							}
						}
						else if constexpr (std::is_same_v<T, float>)
						{
							if (_file.at(section).at(variable).type() == typeid(float))
							{
								return std::any_cast<float>(_file.at(section).at(variable));
							}
							else if (_file.at(section).at(variable).type() == typeid(int))
							{
								return static_cast<float>(std::any_cast<int>(_file.at(section).at(variable)));
							}
							else
							{
								throw std::invalid_argument("\"" + variable + "\" contains " + _file.at(section).at(variable).type().name());
							}
						}
						else if constexpr (std::is_same_v<T, std::string>)
						{
							if (_file.at(section).at(variable).type() == typeid(std::string))
							{
								return std::any_cast<std::string>(_file.at(section).at(variable));
							}
							else
							{
								throw std::invalid_argument("\"" + variable + "\" contains " + _file.at(section).at(variable).type().name());
							}
						}
						else if constexpr (std::is_same_v<T, std::vector<int>>)
						{
							if (_file.at(section).at(variable).type() == typeid(std::vector<int>))
							{
								return std::any_cast<std::vector<int>>(_file.at(section).at(variable));
							}
							else
							{
								throw std::invalid_argument("\"" + variable + "\" contains " + _file.at(section).at(variable).type().name());
							}
						}
						else if constexpr (std::is_same_v<T, std::vector<float>>)
						{
							if (_file.at(section).at(variable).type() == typeid(std::vector<float>) || _file.at(section).at(variable).type() == typeid(std::vector<int>))
							{
								return std::any_cast<std::vector<float>>(_file.at(section).at(variable));
							}
							else
							{
								throw std::invalid_argument("\"" + variable + "\" contains " + _file.at(section).at(variable).type().name());
							}
						}
						else if constexpr (std::is_same_v<T, std::vector<std::string>>)
						{
							if (_file.at(section).at(variable).type() == typeid(std::vector<std::string>))
							{
								return std::any_cast<std::vector<std::string>>(_file.at(section).at(variable));
							}
							else
							{
								throw std::invalid_argument("\"" + variable + "\" contains " + _file.at(section).at(variable).type().name());
							}
						}
						else
						{
							throw std::invalid_argument("wrong type");
						}
					}
					catch (const std::bad_any_cast & ex)
					{
						throw std::invalid_argument(ex.what());
					}
				}
				else
				{
					throw std::invalid_argument("no variable found");
				}
			}
			else
			{
				throw std::invalid_argument("no section found");
			}
		}
		else
		{
			throw std::invalid_argument("no match");
		}
	};

	void Parser::read()
	{
		std::ifstream fin{ _path.string() };
		if (!fin.is_open())
		{
			throw std::runtime_error("could not open \"" + _path.filename().string() + "\"");
		}
		std::string line;
		std::string section;
		std::cmatch fragments;
		const std::regex data_word{ "(\\S+)" };
		const std::regex data_section{ ".*?\\[(.*?)\\].*" };
		const std::regex data_variable{ "\\s*(\\w*)(\\s*=\\s*)(\\S*)\\s*(;?|#?)\\s*\\w*\\s*$" };
		const std::regex type_hex{ "0[xX][0-9a-fA-F]+" };
		const std::regex type_integer{ "^-?\\d+(\\d+)?$" };
		const std::regex type_float{ "^-?\\d+(\\.\\d+)?$" };
		const std::regex type_bool{ "(true)|(false)" };
		const std::regex type_array{ "(,)" };
		auto begin{ std::chrono::system_clock::now() };

		while (std::getline(fin, line))
		{
			++_lines;
			_words += std::distance(std::regex_token_iterator<std::string::iterator>(line.begin(), line.end(), data_word), {});
			if (line.empty() || *line.begin() == ';' || *line.begin() == '#')
			{
				continue;
			}
			else
			{
				if (std::regex_match(line.c_str(), fragments, data_section))
				{
					section = fragments[1];
					_file[section];
				}
				else if (std::regex_match(line.c_str(), fragments, data_variable))
				{	// fragments[1] - variable name, fragments[3] - variable value
					if (fragments[3].str().empty())
					{
						_file.at(section)[fragments[1]] = std::any();
					}
					else if (std::regex_match(fragments[3].str(), type_hex))
					{
						_file.at(section)[fragments[1]] = std::make_any<int>(std::stoi(fragments[3].str(), nullptr, 16));
					}
					else if (std::regex_match(fragments[3].str(), type_integer))
					{
						_file.at(section)[fragments[1].str()] = std::make_any<int>(std::stoi(fragments[3].str()));
					}
					else if (std::regex_match(fragments[3].str(), type_float))
					{
						_file.at(section)[fragments[1].str()] = std::make_any<float>(std::stof(fragments[3].str()));
					}
					else if (std::regex_match(fragments[3].str(), type_bool))
					{
						if (fragments[3].str() == "true")
						{
							_file.at(section)[fragments[1].str()] = std::make_any<bool>(true);
						}
						else
						{
							_file.at(section)[fragments[1].str()] = std::make_any<bool>(false);
						}
					}
					else if (std::regex_search(fragments[3].str(), type_array))
					{
						std::string s(fragments[3]);
						std::sregex_token_iterator i{ s.begin(), s.end(), type_array, -1 };
						std::sregex_token_iterator end;
						if (std::all_of(s.begin(), s.end(), [](unsigned char ch) -> bool
						{
							return ::isdigit(ch) || ch == ',' || ch == '-';
						}))
						{
							std::vector<int> v;
							try
							{
								while (i != end)
								{
									v.push_back(std::stoi(*i));
									++i;
								}
							}
							catch (const std::invalid_argument & ex)
							{
								throw std::runtime_error(ex.what());
							}
							catch (const std::out_of_range & ex)
							{
								throw std::runtime_error(ex.what());
							}
							_file.at(section)[fragments[1].str()] = std::make_any<std::vector<int>>(std::move(v));
						}
						else
						{
							std::vector<std::string> v;
							while (i != end)
							{
								v.push_back(*i);
								++i;
							}
							_file.at(section)[fragments[1].str()] = std::make_any<std::vector<std::string>>(std::move(v));
						}
					}
					else
					{
						_file.at(section)[fragments[1].str()] = std::make_any<std::string>(fragments[3]);
					}
				}
				else
				{
					_errors.push_back("incorrect syntax at line " + std::to_string(_lines));
				}
			}
		}

		_duration = std::chrono::system_clock::now() - begin;
		fin.close();
	}

	bool Parser::utility(std::string_view line) const
	{
		const std::regex help{ "\\s*?([H|h]elp)\\s*$" };
		const std::regex info{ "([I|i]nfo)" };
		const std::regex file{ "([F|f]ile)" };
		const std::regex quit{ "\\s*?([E|e]xit)\\s*$" };
		const std::regex clear{ "\\s*?([C|c]lear)\\s*$" };
		const std::regex errors{ "([E|e]rrors)" };
		const std::regex show_key{ "\\s*?([[S|s]how)[\\s*]+(\\w*)[\\s*]?(\\w*?)\\s*$" };
		std::cmatch fragments;

		if (std::regex_match(line.data(), help))
		{
			this->display(Mode::HELP);
			return true;
		}
		else if (std::regex_match(line.data(), fragments, show_key))
		{	// fragments[2] - section, fragments[3] - section name
			if (std::regex_match(fragments[2].str(), help))
			{
				this->display(Mode::HELP);
			}
			else if (std::regex_match(fragments[2].str(), info))
			{
				this->display(Mode::INFO);
			}
			else if (std::regex_match(fragments[2].str(), errors))
			{
				this->display(Mode::ERRORS);
			}
			else if (std::regex_match(fragments[2].str(), file))
			{
				this->clear();
				for (const auto & i : _file)
				{
					this->display(i.first);
				}
			}
			else
			{
				std::cout << '\n';
				this->display(fragments[2].str());
			}
			return true;
		}
		else if (std::regex_match(line.data(), quit))
		{
			this->clear();
			std::cout << "bye!\n";
			exit(EXIT_SUCCESS);
		}
		else if (std::regex_match(line.data(), clear))
		{
			this->clear();
			return true;
		}
		else
		{
			return false;
		}
	}
	
	void Parser::display(Mode mode) const
	{
		switch (mode)
		{
			case Mode::HELP:
			{
				this->clear();
				std::cout << "type " << std::quoted("section.variabe") << " to get value\n";
				std::cout << "type " << std::quoted("show section_name") << " to find all variables in section\n";
				std::cout << "type " << std::quoted("show info") << " to get some information about file\n";
				std::cout << "type " << std::quoted("show errors") << " to get errors report\n";
				std::cout << "type " << std::quoted("show file") << " to see entire file\n";
				std::cout << "type " << std::quoted("clear") << " to clear screen\n";
				std::cout << "type " << std::quoted("exit") << " to quit\n\n";
				break;
			}
			case Mode::INFO:
			{
				std::cout.setf(std::ios_base::fixed);
				std::cout.precision(2);
				std::cout << "\nfile: " << _path.filename() << '\n';
				try
				{
					std::cout << "size: " << std::filesystem::file_size(_path) << " bytes\n";
				}
				catch (const std::filesystem::filesystem_error & ex)
				{
					std::cout << ex.what() << '\n';
				}
				std::cout << "lines: " << _lines << '\n';
				std::cout << "words: " << _words << '\n';
				std::cout << "parsing time: " << _duration.count() << " s\n\n";
				break;
			}
			case Mode::ERRORS:
			{
				std::cout << '\n';
				if (_errors.size() == 0)
				{
					std::cout << "no errors found!\n\n";
				}
				else
				{
					for (const auto & line : _errors)
					{
						std::cout << line << '\n';
					}
					std::cout << '\n';
				}
				break;
			}
		}
	}

	void Parser::display(std::string_view section) const
	{
		std::cout.setf(std::ios_base::fixed);
		if (_file.contains(section.data()))
		{
			auto key_range = _file.equal_range(section.data());
			for (auto i{ key_range.first }; i != key_range.second; ++i)
			{
				std::cout << '[' << i->first << "]\n";
				for (auto j{ i->second.cbegin() }; j != i->second.cend(); ++j)
				{
					std::cout << std::left << std::setw(28);
					std::cout << j->first;
					std::cout << " : ";
					if (j->second.has_value())
					{
						try
						{
							if (j->second.type() == typeid(bool))
							{
								std::cout << std::boolalpha << std::any_cast<bool>(j->second) << '\n';;
							}
							else if (j->second.type() == typeid(int))
							{
								std::cout << std::any_cast<int>(j->second) << '\n';;
							}
							else if (j->second.type() == typeid(float))
							{
								std::cout << std::defaultfloat << std::any_cast<float>(j->second) << '\n';;
							}
							else if (j->second.type() == typeid(std::string))
							{
								std::cout << std::any_cast<std::string>(j->second) << '\n';;
							}
							else if (j->second.type() == typeid(std::vector<int>))
							{
								for (auto i : std::any_cast<std::vector<int>>(j->second))
								{
									std::cout << i << ' ';
								}
								std::cout << '\n';
							}
							else if (j->second.type() == typeid(std::vector<float>))
							{
								for (auto i : std::any_cast<std::vector<float>>(j->second))
								{
									std::cout << std::defaultfloat << i << ' ';
								}
								std::cout << '\n';
							}
							else if (j->second.type() == typeid(std::vector<std::string>))
							{
								for (const auto & i : std::any_cast<std::vector<std::string>>(j->second))
								{
									std::cout << i << ' ';
								}
								std::cout << '\n';
							}
							else
							{
								std::cout << "<unknown type>\n";
							}
						}
						catch (const std::bad_any_cast & ex)
						{
							std::cout << ex.what() << '\n';
						}
					}
					else
					{
						std::cout << '\n';
					}
				}
				if (i->second.empty())
				{
					continue;
				}
				else
				{
					std::cout << '\n';
				}
			}
		}
		else
		{
			std::cout << "no section found\n\n";
		}
	}

	void Parser::clear() const
	{
	#if defined(_WIN32)
		system("cls");
	#else
		system("clear");
	#endif
	}
}