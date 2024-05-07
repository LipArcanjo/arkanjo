#include "similarity_table.hpp"

int Similarity_Table::find_id_path(Path path){
	if(path_id.find(path) == path_id.end()){
		path_id[path] = paths.size();
		paths.push_back(path);
		vector<pair<int,double>> empty_vec;
		similarity_graph.push_back(empty_vec);
	}
	return path_id[path];
}

void Similarity_Table::read_comparation(ifstream &table_file){
	string string_path1, string_path2;
	double similarity;
	table_file >> string_path1 >> string_path2 >> similarity;

	int id1 = find_id_path(Path(string_path1));
	int id2 = find_id_path(Path(string_path2));

	if(id1 > id2){
		swap(id1,id2);
	}

	similarity_graph[id1].push_back(make_pair(id2,similarity));
	similarity_graph[id2].push_back(make_pair(id1,similarity));
	similarity_table[make_pair(id1,id2)] = similarity;
}

void Similarity_Table::read_file_table(ifstream &table_file){
	int number_comparations;
	table_file >> number_comparations;
	for(int i = 0; i < number_comparations; i++){
		read_comparation(table_file);
	}
}

void Similarity_Table::init_similarity_table(){
	std::ifstream table_file;
	table_file.open(SIMILARITY_TABLE_FILE_NAME);
	Utils::ensure_file_is_open(table_file,SIMILARITY_TABLE_FILE_NAME);

	read_file_table(table_file);

	table_file.close();
}

Similarity_Table::Similarity_Table(double _similarity_threshold){
	similarity_threshold= _similarity_threshold;
	init_similarity_table();
}

Similarity_Table::Similarity_Table(){
	similarity_threshold = DEFAULT_SIMILARITY;
	init_similarity_table();
}

void Similarity_Table::update_similarity(double new_similarity_threshold){
	similarity_threshold = new_similarity_threshold;
}

double Similarity_Table::get_similarity(Path path1, Path path2){
	int id1 = find_id_path(path1);
	int id2 = find_id_path(path2);

	if(id1 == id2){
		return MAXIMUM_SIMILARITY;
	}
	if(id1 > id2){
		swap(id1,id2);
	}
	pair<int,int> aux = make_pair(id1,id2);
	if(similarity_table.find(aux) != similarity_table.end()){
		return similarity_table[aux];
	}
	return MINIMUM_SIMILARITY;
}

double Similarity_Table::is_similar(Path path1, Path path2){
	double similarity = get_similarity(path1, path2);
	return similarity_threshold <= similarity + EPS_ERROR_MARGIN;
}
