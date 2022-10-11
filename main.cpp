//
//  main.cpp
//  Ini_parser_2.0
//
//  Created by Den Fedorov on 06.10.2022.
//

#include "Parser.hpp"

int main(int argc, const char * argv[])
{
	std::string input("ddraw.ini");
	if (argc == 2)
	{
		input = argv[1];
	}
	else
	{
		std::cout << "filename or path\n: ";
		std::getline(std::cin, input);
		while (input.empty())
		{
			std::cout << "repeat: ";
			std::getline(std::cin, input);
		}
	}
	try
	{
		ini::Parser parser(input);
		std::cout << "type " << std::quoted("help") << " for info\n";

		while (true)
		{
			std::cout << ": ";
			std::getline(std::cin, input);
			if (parser.utility(input)) { continue; }
			try
			{
				auto value{ parser.get_value<int>(input) };
				std::cout << input << " = " << value << '\n';
			}
			catch (const std::invalid_argument & ex)
			{
				std::cout << ex.what() << '\n';
			}
		}

		return 0;
	}
	catch (const std::runtime_error & ex)
	{
		std::cout << ex.what() << '\n';
		return -1;
	}
}