// Defines util functions used across all files, does not define anything specific
// to the project, only defines generic functions

#ifndef UTILS_HPP
#define UTILS_HPP
#pragma once
#include <bits/stdc++.h> 
#include <jsoncpp/json/json.h>
#include <sys/stat.h>

#include <iostream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <vector>
#include <sstream>
#include <iomanip>

#ifdef _WIN32
    #include <windows.h>
#elif __linux__
    #include <sys/utsname.h>
#elif __APPLE__
    #include <TargetConditionals.h>
#endif

using namespace std;


namespace Utils{

	const string LIMITER_PRINT = "---------------------";

	const int MKDIR_FLAG = 0700;

	const vector<string> COLOR_TOKENS_UTILS_LIGTH = {
		"\033[0m",    //RESET
		"\033[31m",   //RED
		"\033[32m",   //GREEN
		"\033[33m",   //YELLOW
		"\033[34m",   //BLUE
		"\033[35m",   //MAGENTA
		"\033[36m",   //CYAN
		"\033[37m",   //GRAY
		"\033[97m",   //WHITE
		"\033[33;1m", //BRIGHT_YELLOW
		"\033[30m",   //BLACK
	};

  const vector<string> COLOR_TOKENS_UTILS_DARK = {
		"\033[0m",    // RESET
    "\033[31;2m", // DARK_RED
    "\033[32;2m", // DARK_GREEN
    "\033[33;2m", // DARK_YELLOW
    "\033[34;2m", // DARK_BLUE
    "\033[35;2m", // DARK_MAGENTA
    "\033[36;2m", // DARK_CYAN
    "\033[37;2m", // DAGRAYRK_GRAY
    "\033[30m",   // BLACK
    "\033[33m",   // YELLOW
    "\033[97m"    // WHITE
	};
	
	enum COLOR{
		RESET,
		RED,
		GREEN,
		YELLOW,
		BLUE,
		MAGENTA,
		CYAN,
		GRAY,
		WHITE,
		BRIGHT_YELLOW,
		BLACK
	};

	void ensure_file_is_open(std::ifstream &file, string file_name);
	vector<string> read_file_generic(string string_path);
	void write_file_generic(string file_path, vector<string> content);
	void create_parents_folder_of_file_path(string file_path);
	Json::Value read_json(string string_path);
	bool does_file_exist(string file_path);
	bool is_regular_file(string path);

	string format_colored_message(string message, COLOR color);

	bool is_empty_char(char c);
	bool is_special_char(char c);

	vector<string> split_string(string s, char delimiter);

  int convert_16_bit_to_8_bit(const string& hex16);
  tuple<int, int, int> parse_terminal_color_response(const string& response);
  string capture_terminal_response();
  bool is_bg_dark();
  string capture_terminal_response();
  bool is_bg_color_dark();
	int get_terminal_bg_color_mean();
};



#endif
