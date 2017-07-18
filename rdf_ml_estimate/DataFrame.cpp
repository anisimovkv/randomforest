#include "stdafx.h"
#include "DataFrame.h"

using namespace std;

DataFrame::DataFrame()
{
}


DataFrame::~DataFrame()
{
}

void DataFrame::load(const std::string & filepath)
{
	try
	{	
		cout << "Load csv file:" << filepath << endl;
		//const char *filename, char separator, int flags, alglib::real_2d_array &out				
		alglib::read_csv(filepath.c_str(), ',', alglib::CSV_DEFAULT, m_data);
	}
	catch (alglib::ap_error & err)
	{
		cout << "problem with file csv read" << endl;
		cout <<  err.msg << endl;

	}
}

void DataFrame::create_train_test_sets(float _train, float _test)
{
	if (_train < _test) {
		cout << "Erorr: Train < Test" << endl;
		return;
	}

	int rows = m_data.rows();	
	int train_rows = rows * _train;
	int test_rows = rows - train_rows;
	
	list<int> list_index;

	for (int i = 0; i< rows; i++)
	{
		list_index.push_back(i);
	}

	//cout << endl << "size list :" << list_index.size() << endl;

	std::srand(unsigned(std::time(0)));
	int i = 0;
	std::list<int>::iterator range_begin = list_index.begin();		
	
	int j = 0;
	m_data_train.setlength(train_rows, m_data.cols());
	while (list_index.size() != test_rows)
	{
		//cout << "size:" << list_index.size() << endl;
		i = std::rand() % (list_index.size());
		//cout << "index:" << i << endl;
		std::advance(range_begin, i);
		//cout << " " << *range_begin;
		list_index.erase(range_begin);
		range_begin = list_index.begin();
		//cout << " element:";
		for (int ii = 0; ii < m_data.cols(); ii++) {			
			m_data_train[j][ii] = m_data[i][ii];
			//cout << m_data_train[j][ii] << " ";
		}
		//cout << endl;
		j++;
		
	}

	//cout << " train set:" << endl;
	//for (int j = 0; j < m_data_train.rows(); j++) {
	//	//cout << " element:";
	//	for (int ii = 0; ii < m_data_train.cols(); ii++) {
	//		//cout << m_data_train[j][ii] << " ";
	//	}
	//	//cout << endl;
	//}

	m_data_test.setlength(test_rows, m_data.cols());
	
	i = 0;
	for (auto & j : list_index)
	{
		//cout << j << " ";
		for (int ii = 0; ii < m_data.cols(); ii++) {
			m_data_test[i][ii] = m_data[j][ii];
			//cout << m_data_test[i][ii] << " ";
		}
		i++;
		//cout << endl;
	}

	/*cout << " test set:" << endl;	
	for (i = 0; i < m_data_test.rows(); i++) {
		cout << " element:";
		for (int ii = 0; ii < m_data.cols(); ii++) {
			cout << m_data_test[i][ii] << " ";
		}
		cout << endl;
	}

	cout << endl;*/

	//cout << "stop" << endl;
}

void  DataFrame::lexicographically_sort()
{
	std::vector<std::vector<double>> arr_2d;
	vector<double> row;
	auto & matrix = m_data;

	for (int i = 0; i < matrix.rows(); i++) 
	{		
		for (int ii = 0; ii < matrix.cols(); ii++) {
			row.push_back(matrix[i][ii]);
		}
		arr_2d.push_back(row);
		row.clear();
	}

	cout << "Start lexicographically sort" << endl;

	std::sort(arr_2d.begin(), arr_2d.end());
	
	cout << "End lexicographically sort" << endl;

	for (int i = 0; i < matrix.rows(); i++)
	{
		for (int ii = 0; ii < matrix.cols(); ii++) {
			matrix[i][ii] = arr_2d[i][ii];			
		}		
	}

	

}

void  DataFrame::preprocessing()
{
	ofstream log_temp("../temp/vec_0_str.txt");
	cout << "\t\t\tStart preprocessing" << endl;
	vector<string> vec_0_str;
	vector<string> vec_1_str;
	vector<double> row;
	auto & matrix = m_data;
	alglib::real_1d_array input;
	for (int i = 0; i < matrix.rows(); i++)
	{
		for (int ii = 0; ii < matrix.cols(); ii++) {
			row.push_back(matrix[i][ii]);
		}
		input.setcontent(matrix.cols(), row.data());
		row.clear();
		
		if(matrix[i][33] == 1)
			vec_1_str.push_back(input.tostring(2));
		
		if (matrix[i][33] == 0)
			vec_0_str.push_back(input.tostring(2));
	}
		
	cout << "Start lexicographically sort 2" << endl;
	std::sort(vec_0_str.begin(), vec_0_str.end());
	std::sort(vec_1_str.begin(), vec_1_str.end());

	//std::sort(vec_1_str.begin(), vec_1_str.end());
	
	//std::sort(vec_1_str.begin(), vec_1_str.end());
	cout << "End lexicographically sort 2" << endl;
	
	// delete duplicate
	vec_1_str.erase(std::unique(vec_1_str.begin(), vec_1_str.end()), vec_1_str.end());
	vec_0_str.erase(std::unique(vec_0_str.begin(), vec_0_str.end()), vec_0_str.end());
	
	//cut sets
	//vec_0_str.erase(vec_0_str.begin() + 70000, vec_0_str.end());
	
	//merge sets
	//vec_1_str.insert(vec_1_str.end(), vec_0_str.begin(), vec_0_str.end());

	//random insert
	std::vector<string>::iterator range_begin = vec_0_str.begin();
	while (vec_1_str.size() != 0)
	{
		//cout << "size:" << list_index.size() << endl;
		int i = std::rand() % (vec_0_str.size());
		//cout << "index:" << i << endl;
		std::advance(range_begin, i);
		vec_0_str.insert(range_begin, vec_1_str.back());		
		vec_1_str.pop_back();
		//cout << " " << *range_begin;		
		range_begin = vec_0_str.begin();		
	}


	for (const auto &v : vec_0_str) {
		log_temp << v << endl;
	}

	
	
	matrix.setlength(vec_0_str.size(), matrix.cols());

	for (int i = 0; i < matrix.rows(); i++)
	{
		alglib::real_1d_array out(vec_0_str[i].c_str());
		for (int ii = 0; ii < matrix.cols(); ii++) {
			matrix[i][ii] = out[ii];			
		}
	}	
	cout << "\t\t\tStop preprocessing" << endl;
}


alglib::real_2d_array & DataFrame::get_train_set()
{
	return m_data_train;
}

alglib::real_2d_array & DataFrame::get_test_set()
{
	return m_data_test;	
}


alglib::real_2d_array & DataFrame::get_set()
{
	return m_data;
}
