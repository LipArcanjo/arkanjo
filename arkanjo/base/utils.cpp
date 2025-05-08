
#include "utils.hpp"

void Utils::ensure_file_is_open(std::ifstream &file, string file_name){
	if(!file.is_open()){
		cout << "Attempted to open file: " << file_name << " ";
		cout << "but a Error ocurred. Check if the file exist." << endl;
		exit(0);
	}
}

vector<string> Utils::read_file_generic(string string_path){
	std::ifstream filein;
	string line;
	vector<string> ret;
	filein.open(string_path);
	ensure_file_is_open(filein,string_path);
	while(getline(filein,line)){
		ret.push_back(line);
	}
	filein.close();
	return ret;
}

void Utils::write_file_generic(string file_path, vector<string> content){
	std::ofstream fileout;
	create_parents_folder_of_file_path(file_path);
	fileout.open(file_path);

	for(auto line : content){
		fileout << line << '\n';
	}

	fileout.close();
}

void Utils::create_parents_folder_of_file_path(string file_path){
	vector<string> parents;
	for(size_t i = 0; i < file_path.size(); i++){
		if(file_path[i] == '/'){
			string s = "";
			for(size_t j = 0; j < i; j++){
				s += file_path[j];
			}
			parents.push_back(s);
		}
	}
	for(auto folder : parents){
		const char *cfolder = folder.c_str();
		mkdir(cfolder,MKDIR_FLAG);
	}
}

Json::Value Utils::read_json(string string_path){
	ifstream json_file(string_path,std::ifstream::binary);
	ensure_file_is_open(json_file,string_path);
	Json::Value json;
	json_file >> json;
	json_file.close();
	return json;
}

bool Utils::does_file_exist(string file_path){
	if (FILE *file = fopen(file_path.c_str(), "r")) {
		fclose(file);
		return true;
	} else {
		return false;
	} 	
}

bool Utils::is_regular_file(string path){
	struct stat path_stat;
	auto path_c_str = path.c_str();
	stat(path_c_str,&path_stat);
	return S_ISREG(path_stat.st_mode);
}

string Utils::format_colored_message(string message, COLOR color){
	if (is_bg_color_dark()){
		return COLOR_TOKENS_UTILS_LIGTH[color] + message + COLOR_TOKENS_UTILS_LIGTH[RESET];
	}
	else{
	return COLOR_TOKENS_UTILS_DARK[color] + message + COLOR_TOKENS_UTILS_DARK[RESET];

	}
}

bool Utils::is_empty_char(char c){
	if(c == ' '){
		return true;
	}
	if(c <= 20){
		return true;
	}
	return false;
}

bool Utils::is_special_char(char c){
	if(c >= 'a' && c <= 'z'){
		return false;
	}
	if(c >= 'A' && c <= 'Z'){
		return false;
	}
	if(c >= '0' && c <= '9'){
		return false;
	}
	if(c == '_'){
		return false;
	}
	return true;
}

vector<string> Utils::split_string(string s, char delimiter){
	string cur_token;
	vector<string> ret;
	for(auto c : s){
		if(c == delimiter){
			if(!cur_token.empty()){
				ret.push_back(cur_token);
			}
			cur_token = "";
		}else{
			cur_token.push_back(c);
		}
	}
	if(!cur_token.empty()){
		ret.push_back(cur_token);
	}
	return ret;
}

int Utils::convert_16_bit_to_8_bit(const std::string& hex16) {
  try {
      unsigned long value = std::stoul(hex16, nullptr, 16);
      return static_cast<int>(value / 256);  
  } catch (...) {
      return 0; 
  }
}

tuple<int, int, int> Utils::parse_terminal_color_response(const string& response) {
  // std::string start_pos;
  const size_t start_pos = response.find("rgb:");
  if (start_pos == string::npos) {
      return {0, 0, 0};  // Return black if format not found
  }

  const size_t end_pos = response.find("\033\\", start_pos);
  if (end_pos == string::npos) {
      return {0, 0, 0};
  }

  string rgb_str = response.substr(start_pos + 4, end_pos - (start_pos + 4));

  replace(rgb_str.begin(), rgb_str.end(), '/', ' ');

  istringstream iss(rgb_str);
  string r_hex, g_hex, b_hex;
  iss >> r_hex >> g_hex >> b_hex;

  int r = convert_16_bit_to_8_bit(r_hex);
  int g = convert_16_bit_to_8_bit(g_hex);
  int b = convert_16_bit_to_8_bit(b_hex);

  return {r, g, b};
}

string Utils::capture_terminal_response() {
  termios oldt, newt;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  int oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  cout << "\033]11;?\033\\";
  cout.flush();

  string response;
  char ch;
  timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 100000; // 100ms timeout

  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(STDIN_FILENO, &readfds);

  while (select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv) > 0) {
      if (read(STDIN_FILENO, &ch, 1) > 0) {
          response += ch;
      } else {
          break;
      }
  }

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  return response;
}

int Utils::get_terminal_bg_color_mean() {
  if (!isatty(STDOUT_FILENO)) {
      return 0;
  }

  string color_str = capture_terminal_response();
  auto [r, g, b] = parse_terminal_color_response(color_str);
  int mean_color = (r+g+b)/3;
  return mean_color;
}

bool Utils::is_bg_color_dark(){
	#ifdef _WIN32
			CONSOLE_SCREEN_BUFFER_INFOEX csbi;
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

			if (GetConsoleScreenBufferInfoEx(hConsole, &csbi)) {
					COLORREF backgroundColor = csbi.ColorTable[csbi.wAttributes & 0x0F];

					int r = GetRValue(backgroundColor);
					int g = GetGValue(backgroundColor);
					int b = GetBValue(backgroundColor);

					if ((r+g+b)/3 < 128){
							return true;
					}
					else{
							return false;
					}
			} else {
					std::cerr << "Error getting console screen buffer info." << std::endl;
					return true;
			}
	#elif __linux__
			if (get_terminal_bg_color_mean() < 128){
					return true;
			}
			else{
					return false;
			}

	#elif __APPLE__
			if (get_terminal_bg_color_mean() < 128){
				return true;
		}
		else{
				return false;
		}
	#else
			return true;
	#endif

}
