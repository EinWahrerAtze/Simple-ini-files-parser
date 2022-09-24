//
//  Parser.h
//  Ini_parser_2.0
//
//  Created by Den Fedorov on 17.09.2022.
//

#pragma once
#include <unordered_map>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <variant>
#include <iomanip>
#include <vector>
#include <string>
#include <chrono>
#include <regex>

namespace ini
{
	class Parser
	{
	public:
		Parser();
		~Parser();
		void read(const std::string & s);
		void display() const;
		template<typename T>
		std::variant<T, std::vector<T>> get_value(const std::string & s) const;
	private:
		std::string find(const std::string & s) const;
		std::size_t _words;
		std::size_t _lines;
		std::string _filename;
		std::vector<std::string> _errors;
		std::chrono::duration<float> _duration;
		std::unordered_map<std::string, std::unordered_map<std::string, std::string>> _file;
	};

	template<typename T>
	std::variant<T, std::vector<T>> Parser::get_value(const std::string & s) const
	{
		bool is_array{ false };
		std::string value;
		std::vector<std::string> values;

		value = this->find(s);

		if (value.find_first_of(',') != std::string::npos)
		{
			std::regex rgx{ ',' };
			std::regex_token_iterator<std::string::iterator> rgx_begin{ value.begin(), value.end(), rgx, -1 };
			std::regex_token_iterator<std::string::iterator> rgx_end;

			while (rgx_begin != rgx_end)
			{
				values.push_back(*rgx_begin);
				++rgx_begin;
			}

			is_array = true;
		}

		if constexpr (std::is_same_v<T, int>)
		{
			if (is_array)
			{
				std::vector<int> temp;
				temp.reserve(values.size());

				std::transform(values.begin(), values.end(), std::back_inserter(temp), [](const std::string & s) -> int
				{
					return std::stoi(s);
				});

				return temp;
			}
			else
			{
				return std::stoi(value);
			}
		}
		else if constexpr (std::is_same_v<T, long>)
		{
			if (is_array)
			{
				std::vector<long> temp;
				temp.reserve(values.size());

				std::transform(values.begin(), values.end(), std::back_inserter(temp), [](const std::string & s) -> long
				{
					return std::stol(s);
				});

				return temp;
			}
			else
			{
				return std::stol(value);
			}
		}
		else if constexpr (std::is_same_v<T, long long>)
		{
			if (is_array)
			{
				std::vector<long long> temp;
				temp.reserve(values.size());

				std::transform(values.begin(), values.end(), std::back_inserter(temp), [](const std::string & s) -> long long
				{
					return std::stoll(s);
				});

				return temp;
			}
			else
			{
				return std::stoi(value);
			}
		}
		else if constexpr (std::is_same_v<T, unsigned long>)
		{
			if (is_array)
			{
				std::vector<unsigned long> temp;
				temp.reserve(values.size());

				std::transform(values.begin(), values.end(), std::back_inserter(temp), [](const std::string & s) -> unsigned long
				{
					return std::stoul(s);
				});

				return temp;
			}
			else
			{
				return std::stoul(value);
			}
		}
		else if constexpr (std::is_same_v<T, unsigned long long>)
		{
			if (is_array)
			{
				std::vector<unsigned long long> temp;
				temp.reserve(values.size());

				std::transform(values.begin(), values.end(), std::back_inserter(temp), [](const std::string & s) -> unsigned long long
				{
					return std::stoull(s);
				});

				return temp;
			}
			else
			{
				return std::stoull(value);
			}
		}
		else if constexpr (std::is_same_v<T, float>)
		{
			if (is_array)
			{
				std::vector<float> temp;
				temp.reserve(values.size());

				std::transform(values.begin(), values.end(), std::back_inserter(temp), [](const std::string & s) -> float
				{
					return std::stoi(s);
				});

				return temp;
			}
			else
			{
				return std::stof(value);
			}
		}
		else if constexpr (std::is_same_v<T, double>)
		{
			if (is_array)
			{
				std::vector<double> temp;
				temp.reserve(values.size());

				std::transform(values.begin(), values.end(), std::back_inserter(temp), [](const std::string & s) -> double
				{
					return std::stod(s);
				});

				return temp;
			}
			else
			{
				return std::stod(value);
			}
		}
		else if constexpr (std::is_same_v<T, long double>)
		{
			if (is_array)
			{
				std::vector<long double> temp;
				temp.reserve(values.size());

				std::transform(values.begin(), values.end(), std::back_inserter(temp), [](const std::string & s) -> long double
				{
					return std::stold(s);
				});

				return temp;
			}
			else
			{
				return std::stold(value);
			}
		}
		else if constexpr (std::is_same_v<T, std::string>)
		{
			if (is_array)
			{
				return values;
			}
			else
			{
				return value;
			}
		}
		else
		{
			throw std::invalid_argument("Wrong type!");
		}
	}
}
