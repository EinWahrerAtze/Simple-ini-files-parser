//
//  main.cpp
//  Ini_parser_2.0
//
//  Created by Den Fedorov on 17.09.2022.
//

#include <filesystem>
#include <iostream>
#include "Parser.h"

void clear()
{
#if defined(_WIN32)
	system("cls");
#else
	system("clear");
#endif
}

int main(int argc, const char * argv[])
{
//	clear();

	if (argc == 2)
	{
		try
		{
			ini::Parser parser;
			parser.read(argv[1]);
			parser.display();
			exit(EXIT_SUCCESS);
		}
		catch (const std::runtime_error & ex)
		{
			std::cout << ex.what();
			exit(EXIT_FAILURE);
		}
	}

	std::string input;
	std::string message("Welcome to ini file parser!");
	ini::Parser parser;

	while (true)
	{
		std::cout << message << '\n' << '\n';
		std::cout << "Enter name of file to open or press 'A' to add automaticly ('X' to leave): ";
		std::getline(std::cin, input);

		if (input.empty())
		{
			clear();
			message = "Empty input!";
			continue;
		}
		else if (input.size() < 2)
		{
			switch (*input.begin())
			{
				case 'a':
				case 'A':
				{
					clear();
					bool found{ false };
					std::string extension(".ini");
					std::filesystem::path directory(std::filesystem::current_path());
					std::filesystem::directory_iterator directory_it(directory);
//					std::cout << "Currurent path: " << std::filesystem::current_path() << std::endl;

					for (auto const & i : directory_it)
					{
						if (i.path().filename().extension() == extension)
						{
							try
							{
								parser.read(i.path().filename().string());
								found = true;
								break;
							}
							catch (const std::runtime_error & ex)
							{
								message = ex.what();
								continue;
							}
						}
					}

					if (!found)
					{
						message = "Cound not find any " + extension + " files in current folder!";
						continue;
					}

					break;
				}
				case 'x':
				case 'X':
				{
					clear();
					std::cout << "Bye!\n";
					exit(EXIT_SUCCESS);
				}
				default:
				{
					message = "Wrong input!";
					continue;
				}
			}
		}
		else
		{
			try
			{
				parser.read(input);
				message = "File \"" + input + "\" was successfully read!";
			}
			catch (const std::runtime_error & ex)
			{
				clear();
				message = ex.what();
				continue;
			}
		}

		clear();
		message.clear();
		parser.display();
		message = "Enter section.variable to find its value ('X' to exit): ";

		while (true)
		{
			std::cout << message;
			std::getline(std::cin, input);

			if (input.empty())
			{
				message = "Repeat: ";
				continue;
			}
			else if ((*input.begin() == 'x' || *input.begin() == 'X') && input.size() < 2)
			{
				clear();
				std::cout << "Bye!\n";
				exit(EXIT_SUCCESS);
			}
			else
			{
				try
				{
					auto var = parser.get_value<int>(input);

					if (std::holds_alternative<int>(var))
					{
						try
						{
							std::cout << input << " contains a value: ";
							std::cout << std::get<int>(var) << '\n';
							message = "Input: ";
							continue;
						}
						catch (const std::bad_variant_access & ex)
						{
							std::cout << ex.what() << '\n';
							continue;
						}
					}
					else
					{
						try
						{
							std::cout << input << " contains an array: ";
							std::vector<int> vec = std::move(std::get<std::vector<int>>(var));
							message = "Input: ";

							for (auto const & i : vec)
							{
								std::cout << i << ' ';
							}

							std::cout << '\n';
							continue;
						}
						catch (const std::bad_variant_access & ex)
						{
							std::cout << ex.what() << '\n';
							continue;
						}
					}
				}
				catch (const std::invalid_argument & ex)
				{
					std::cout << ex.what() << '\n';
					continue;
				}
				catch (const std::out_of_range & ex)
				{
					std::cout << ex.what() << '\n';
					continue;
				}
				catch (...)
				{
					std::cout << "Something went wrong...\n";
					continue;
				}
			}
		}
	}


	return 0;
}
