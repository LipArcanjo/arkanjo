/*
This file intends to create a script to read a directory with 
code in c and extract every function/struct of every .c file as a new file

Example of expected behaviour: 
There is a file example.c with functions a and b, will be create two new 
files: example/a.c and example/b.c

TODO: Doesn't work correct if the file has an incorrect bracket sequence, 
even if the bracket sequence is in a commentary
*/

#include <bits/stdc++.h>
#include "utils.hpp"
using namespace std;

const vector<string> ALLOWED_EXTENSIONS = { "c","h" };
const string SOURCE_PATH = "tmp/source";
const string HEADER_PATH = "tmp/header";
const string INFO_PATH =   "tmp/info";
const int NUMBER_OF_LINES_BEFORE_FOR_FUNCTION_NAME = 10;
const int C_RELEVANT_DEPTH = 0;

set<array<int,3>> find_start_end_and_depth_of_brackets(vector<string> brackets_content){
	set<array<int,3>> start_ends;
	int open_brackets = 0;

	vector<int> not_processed_open_brackets;
	auto process_open = [&](int line_number){
		open_brackets++;
		not_processed_open_brackets.push_back(line_number);
	};
	auto process_close = [&](int line_number){
		open_brackets--;
		if(open_brackets <= -1){
			open_brackets = 0;
		}else{
			int matched_open_position = not_processed_open_brackets.back();
			not_processed_open_brackets.pop_back();
			int depth_of_open = not_processed_open_brackets.size();
			start_ends.insert({matched_open_position,line_number,depth_of_open});
		}
	};
	
	for(size_t i = 0; i < brackets_content.size(); i++){
		auto line = brackets_content[i];
		for(auto c : line){
			if(c == '{'){
				process_open(i);
			}
			if(c == '}'){
				process_close(i);
			}
		}
	}
	return start_ends;
}

set<pair<int,int>> find_start_end_of_brackets_of_given_depth(vector<string> brackets_content, int depth){
	set<pair<int,int>> ret;
	set<array<int,3>> bracket_pairs = find_start_end_and_depth_of_brackets(brackets_content);
	for(auto [start,end,dep] : bracket_pairs){
		if(dep == depth){
			ret.insert({start,end});
		}
	}
	return ret;
}

int find_position_first_open_bracket(string s){
	for(size_t i = 0; i < s.size(); i++){
		char c = s[i];
		if(c == '(' || c == '{'){
			return i;
		}
	}
	return -1;
}

string extract_last_token_of_string(string s){
	vector<string> tokens;
	string cur_token = "";
	for(size_t i = 0; i < s.size(); i++){
		char c = s[i];
		if( Utils::is_empty_char(c) || Utils::is_special_char(c)){
			if(!cur_token.empty()){
				tokens.push_back(cur_token);
			}
			cur_token = "";
		}else{
			cur_token += c;
		}
	}
	if(!cur_token.empty()){
		tokens.push_back(cur_token);
	}

	if(tokens.empty()){
		return "";
	}
	return tokens.back();
}

struct Line_code{
	int line_number;
	string content;
};

Line_code build_line_code(int line_number, string content){
	Line_code ret;
	ret.line_number = line_number;
	ret.content = content;
	return ret;
}

vector<Line_code> get_lines_before_body_function(const vector<string> &file_content, int line_start_body_function, int pos_bracket){	
	vector<Line_code> ret;
	Line_code line_bracket = build_line_code(line_start_body_function,file_content[line_start_body_function]);
	//remove everything after {
	while(int(line_bracket.content.size()) > pos_bracket){
		line_bracket.content.pop_back();
	}
	ret.push_back(line_bracket);

	int until = max(0,line_start_body_function-NUMBER_OF_LINES_BEFORE_FOR_FUNCTION_NAME);
	for(int i = line_start_body_function-1; i >= until; i--){
		ret.push_back(build_line_code(i,file_content[i]));
	}
	reverse(ret.begin(),ret.end());

	//remove empty lines/empty characters at the end of lines
	while(!ret.empty()){
		if(ret.back().content.empty()){
			ret.pop_back();
			continue;
		}
		if(Utils::is_empty_char(ret.back().content.back())){
			ret.back().content.pop_back();
			continue;
		}
		break;
	}
	return ret;
}

vector<Line_code> remove_parameters_of_declaration(vector<Line_code> code){
	if(code.empty() || code.back().content.back() != ')'){
		return code;
	}
	int count_close_parenteses = 0;

	while(!code.empty()){
		string content = code.back().content;
		while(!content.empty()){
			if(content.back() == ')'){
				count_close_parenteses++;
			}
			if(content.back() == '('){
				count_close_parenteses--;
			}
			content.pop_back();
			if(count_close_parenteses == 0){
				break;
			}
		}
		code.back().content = content;
		if(count_close_parenteses == 0){
			break;
		}
		code.pop_back();
	}
	return code;
}

pair<string,int> extract_function_name_and_line_from_declaration(const vector<string> &file_content, int line_start_body_function){
	int pos = find_position_first_open_bracket(file_content[line_start_body_function]);
	vector<Line_code> code_before_bracket = get_lines_before_body_function(file_content, line_start_body_function,pos);
	vector<Line_code> code = remove_parameters_of_declaration(code_before_bracket);
	if(code.empty()){
		return make_pair("",-1);
	}
	string ret = extract_last_token_of_string(code.back().content);
	return {ret,code.back().line_number};
}

string extract_extension(string file_path){
	string extension = "";
	int pos_last_dot = -1;
	for(size_t i = 0; i < file_path.size(); i++){
		if(file_path[i] == '.'){
			pos_last_dot = i;
		}
	}
	if(pos_last_dot == -1){
		return extension;
	}
	for(size_t i = pos_last_dot+1; i < file_path.size(); i++){
		extension += file_path[i];
	}
	return extension;
}

string build_source_path(string relative_path, string function_name){
	string extension = extract_extension(relative_path);
	string final_path = SOURCE_PATH + relative_path + "/";
	final_path += function_name + "." + extension;
	return final_path;
}

string build_header_path(string relative_path, string function_name){
	string extension = extract_extension(relative_path);
	string final_path = HEADER_PATH + relative_path + "/";
	final_path += function_name + "." + extension;
	return final_path;
}

string build_info_path(string relative_path, string function_name){
	string extension = extract_extension(relative_path);
	string final_path = INFO_PATH + relative_path + "/";
	final_path += function_name + ".json";
	return final_path;
}


void create_source_file(int start_number_line, int end_number_line, string relative_path, string function_name, const vector<string> &file_content){
	string path = build_source_path(relative_path, function_name);
	vector<string> function_content;
	
	string first_line = file_content[start_number_line];
	int to_remove = find_position_first_open_bracket(first_line);
	reverse(first_line.begin(),first_line.end());
	for(int i = 0; i < to_remove; i++){
		first_line.pop_back();
	}
	reverse(first_line.begin(),first_line.end());
	function_content.push_back(first_line);

	for(int i = start_number_line+1; i <= end_number_line; i++){
		function_content.push_back(file_content[i]);
	}
	Utils::write_file_generic(path, function_content);
}

void create_header_file(int start_number_line, int line_declaration, string relative_path, string function_name, const vector<string> &file_content){
	string path = build_header_path(relative_path, function_name);
	vector<string> function_content;
	for(int i = line_declaration; i < start_number_line; i++){
		function_content.push_back(file_content[i]);
	}

	string first_line = file_content[start_number_line];
	int to_keep = find_position_first_open_bracket(first_line);
	while(int(first_line.size()) > to_keep){
		first_line.pop_back();
	}
	function_content.push_back(first_line);

	Utils::write_file_generic(path, function_content);
}

/*This creates a json file*/
void create_info_file(int line_declaration, int start_number_line, int end_number_line, string relative_path, string function_name){
	vector<string> content;
	content.push_back("{\n");
	content.push_back("\"file_name\":\"" + relative_path + "\",\n");
	content.push_back("\"function_name\":\"" + function_name + "\",\n");
	content.push_back("\"line_declaration\":" + to_string(line_declaration) + ",\n");
	content.push_back("\"start_number_line\":" + to_string(start_number_line) + ",\n");
	content.push_back("\"end_number_line\":" + to_string(end_number_line) + "\n");
	content.push_back("}\n");
	string path = build_info_path(relative_path, function_name);
	Utils::write_file_generic(path, content);
}

void process_function(int start_number_line, int end_number_line, string relative_path, const vector<string> &file_content){
	string first_line = file_content[start_number_line];
	auto [function_name,line_declaration] = extract_function_name_and_line_from_declaration(file_content,start_number_line);
	if(function_name.empty()){
		return;
	}
	create_source_file(start_number_line,end_number_line,relative_path,function_name,file_content);
	create_header_file(start_number_line,line_declaration,relative_path,function_name,file_content);
	create_info_file(line_declaration,start_number_line,end_number_line,relative_path,function_name);
}

string file_path_from_folder_path(string file_path, string folder_path){
	string ret = "";
	for(size_t i = folder_path.size(); i < file_path.size(); i++){
		ret += file_path[i];
	}
	return ret;
}

bool is_allowed_extension(string extension){
	for(auto allowed_extension : ALLOWED_EXTENSIONS){
		if(extension == allowed_extension){
			return true;
		}
	}
	return false;
}

void file_breaker(string file_path, string folder_path){
	string extension = extract_extension(file_path);
	string relative_path = file_path_from_folder_path(file_path, folder_path);

	if(!is_allowed_extension(extension)){
		return;
	}

	vector<string> file_content = Utils::read_file_generic(file_path);
	set<pair<int,int>> start_end_of_functions = find_start_end_of_brackets_of_given_depth(file_content, C_RELEVANT_DEPTH);
	for(auto [start_line, end_line] : start_end_of_functions){
		process_function(start_line,end_line,relative_path, file_content);
	}
}

void function_breaker(string folder_path){
	for(const auto &dirEntry: std::filesystem::recursive_directory_iterator(folder_path)){
		string file_path = dirEntry.path().string();
		file_breaker(file_path,folder_path);
	}
}

int main(int argc, char *argv[]){
	if(argc <= 1){
		cout << "Required folder path to process was not given" << endl;
		return 0;
	}
	string folder_path(argv[1]);
	function_breaker(folder_path);

	return 0;
}
