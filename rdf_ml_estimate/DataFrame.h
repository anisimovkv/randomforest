#pragma once
#include "..\alglib-3.11.0.cpp.gpl\cpp\src\dataanalysis.h"
#include "..\alglib-3.11.0.cpp.gpl\cpp\src\ap.h"

class DataFrame
{
public:
	DataFrame();
	~DataFrame();
	void load(const std::string & filepath);
	void create_train_test_sets(float _train, float _test);
	void lexicographically_sort();
	void preprocessing();	
	alglib::real_2d_array & get_train_set();
	alglib::real_2d_array & get_test_set();
	alglib::real_2d_array & get_set();
	
	
private:
	alglib::real_2d_array m_data;	
	alglib::real_2d_array m_data_test;
	alglib::real_2d_array m_data_train;
	std::vector<int> m_test_index;
	std::vector<int> m_train_index;	
};

