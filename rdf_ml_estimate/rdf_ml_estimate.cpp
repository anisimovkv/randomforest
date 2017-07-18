// rdf_ml_estimate.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "DataFrame.h"

#include "..\alglib-3.11.0.cpp.gpl\cpp\src\dataanalysis.h"
#include "..\alglib-3.11.0.cpp.gpl\cpp\src\ap.h"



using namespace std;

void print_rep_info(const alglib::dfreport & _rep, const alglib::ae_int_t & _info)
{
	cout << "\t----- df report: -----" << endl;
	cout << "\t|avgerror:" << _rep.avgerror << endl;
	cout << "\t|---------------------" << endl;
	cout << "\t|avgce:" << _rep.avgce << endl;
	cout << "\t|---------------------" << endl;
	cout << "\t|avgrelerror:" << _rep.avgrelerror << endl;
	cout << "\t|---------------------" << endl;
	cout << "\t|oobavgce:" << _rep.oobavgce << endl;
	cout << "\t|---------------------" << endl;
	cout << "\t|info:" << _info << endl;

	if (_info != 1)
		throw("info problems!");
}

void create_train_serialize_model()
{
	// 17 min
	cout << "\t ******** Start ********" << endl;
	DataFrame test_data_frame, train_data_frame;

	string filename_train = "../data/pBP0Dr1q_train_code_list.csv";
	train_data_frame.load(filename_train);

	string filename_test = "../data/pBP0Dr1q_test_code_list.csv";
	test_data_frame.load(filename_test);

	cout << endl << "Created forest, and report." << endl;

	alglib::dfreport rep;
	alglib::decisionforest dforest;
	alglib::ae_int_t info;
	alglib::ae_int_t nclasses = 2;
	alglib::ae_int_t ntrees = 50;
	double r = 0.66;
	alglib::ae_int_t npoints = train_data_frame.get_set().rows();
	alglib::ae_int_t nvars = train_data_frame.get_set().cols();

	cout << endl << "Train forest." << endl;
	//nvars имеетс€ ввиду незовисимых, последний столбец зависмый 
	//т.е. номер класса не указываем nvars-1 
	alglib::dfbuildrandomdecisionforest(train_data_frame.get_set(), npoints, nvars - 1, nclasses, ntrees, r, info, dforest, rep);

	print_rep_info(rep, info);

	try {
		double error = alglib::dfavgrelerror(dforest, test_data_frame.get_set(), test_data_frame.get_set().rows());
		cout << endl << "\tAverage error is = " << error << endl;
	}
	catch (alglib::ap_error & err)
	{
		cout << "problem with dfavgrelerror" << endl;
		cout << err.msg << endl;

	}

	//save model 
	string str_model;

	alglib::dfserialize(dforest, str_model);

	ofstream out("../models/rdf_model.txt");
	out << str_model;
	out.close();
}

void unserialize_estimate_model()
{
	cout << "\t ******** Start ********" << endl;
	alglib::decisionforest dforest;

	//load model 

	//ifstream ifs("../models/rdf_model.txt");
	//string str_model((std::istreambuf_iterator<char>(ifs)),
	//					(std::istreambuf_iterator<char>()));
	//
	//alglib::dfunserialize(str_model, dforest);

	string filename_test = "../data/pBP0Dr1q_test_code_list.csv";
	DataFrame test_data_frame;
	test_data_frame.load(filename_test);
	//
	//try {
	//	double err = alglib::dfrelclserror(dforest, test_data_frame.get_set(), test_data_frame.get_set().rows());
	//	cout << endl << "\tRelative classification error on the test set is = " << err << endl;
	//}
	//catch (alglib::ap_error & err)
	//{
	//	cout << "problem with dfrelclserror" << endl;
	//	cout << err.msg << endl;

	//}
	
	
	cout << "Estimate:" << endl;

	const auto & matrix = test_data_frame.get_set();
	vector<double> row;
	int indep_vars = matrix.cols() - 1;
	int num_right_answer = 0;

	// build and classify the input set
	alglib::real_1d_array input;
	alglib::real_1d_array estims;

	for (int i = 0; i < matrix.rows(); i++) {
		//char * p = matrix[i];
		for (int ii = 0; ii < indep_vars; ii++) {
			row.push_back(matrix[i][ii]);
		}
		
		input.setcontent(indep_vars, row.data());
		row.clear();

		alglib::dfprocessi(dforest, input, estims);

		//cout << "value:" << matrix[i][13] << " estim: \t\t" << estims.tostring(0) << endl;

		if ("[1,0]" == estims.tostring(0) && matrix[i][33] == 0)
			num_right_answer++;

		if ("[0,1]" == estims.tostring(0) && matrix[i][33] == 1)
			num_right_answer++;	
	}

	double persent = 100.0 * (double)num_right_answer / (double)matrix.rows();
	cout << endl << "right answer:" << num_right_answer << " from " << matrix.rows() << " or persent :" << persent << " %" << endl;

}

void create_train_check()
{
	cout << "\t ******** Start ********" << endl;
	DataFrame test_data_frame, train_data_frame_positive, train_data_frame_realist;

	ofstream log("../models/log_a.txt");
	log << "Start" << endl;
	
	string str_file = "../data/train_positive.csv";
	train_data_frame_positive.load(str_file);
			
	train_data_frame_realist.load("../data/train_realist.csv");

	test_data_frame.load("../data/pBP0Dr1q_test_code_list.csv");	
	log << "load: " << str_file << endl;

	cout << endl << "Created forest, and report." << endl;
	

	alglib::dfreport rep;
	alglib::decisionforest dforest_positive;
	alglib::decisionforest dforest_realist;
	alglib::ae_int_t info;
	alglib::ae_int_t nclasses = 2;
	alglib::ae_int_t ntrees = 100;
	double r = 0.65;
	alglib::ae_int_t npoints = train_data_frame_positive.get_set().rows();
	alglib::ae_int_t nvars = train_data_frame_positive.get_set().cols();

	cout << endl << "Train forest." << endl;
	//nvars имеетс€ ввиду незовисимых, последний столбец зависмый 
	//т.е. номер класса не указываем nvars-1 
	alglib::dfbuildrandomdecisionforest(train_data_frame_positive.get_set(), npoints, nvars - 1, nclasses, ntrees, r, info, dforest_positive, rep);
	print_rep_info(rep, info);

	ntrees = 50;
	r = 0.15;
	npoints = train_data_frame_realist.get_set().rows();
	nvars = train_data_frame_realist.get_set().cols();

	alglib::dfbuildrandomdecisionforest(train_data_frame_realist.get_set(), npoints, nvars - 1, nclasses, ntrees, r, info, dforest_realist, rep);
	print_rep_info(rep, info);

	try {
		cout << endl << "\tPositive Forest :)" << endl;
		double error = alglib::dfavgrelerror(dforest_positive, test_data_frame.get_set(), test_data_frame.get_set().rows());
		cout << endl << "\tAverage error is = " << error << endl;
		
		cout << endl << "\tRealist Forest :(" << endl;
		error = alglib::dfavgrelerror(dforest_realist, test_data_frame.get_set(), test_data_frame.get_set().rows());
		cout << endl << "\tAverage error is = " << error << endl;
	}
	catch (alglib::ap_error & err)
	{
		cout << "problem with dfavgrelerror" << endl;
		cout << err.msg << endl;
	}

	cout << "Estimate:" << endl;
	
	log << "Estimate: " <<endl;

	const auto & matrix = test_data_frame.get_set();
	vector<double> row;
	int indep_vars = matrix.cols() - 1;
	int num_click = 0;
	int num_right_answers_click = 0;
	int num_right_answer_noclick = 0;
	int num_positiv_answer = 0 ;
	int num_all_right_answer = 0;

	int num_positiv_answer_realist = 0;
	int num_all_right_answer_realist = 0;
	int num_right_answer_noclick_realist = 0;
	int num_right_answers_click_realist = 0;

	// build and classify the input set
	alglib::real_1d_array input;
	alglib::real_1d_array estims;

	for (int i = 0; i < matrix.rows(); i++) {
		//char * p = matrix[i];
		for (int ii = 0; ii < indep_vars; ii++) {
			row.push_back(matrix[i][ii]);
		}

		input.setcontent(indep_vars, row.data());
		row.clear();

		alglib::dfprocess(dforest_positive, input, estims);

		//cout << "value:" << matrix[i][13] << " estim: \t\t" << estims.tostring(0) << endl;
		
		/*if(estims[1] >= 0.1)
			num_positiv_answer++;

		if (estims[1] >= 0.1  && matrix[i][33] == 1)
			num_right_answers_click++;

		if (estims[1]<0.1 && matrix[i][33] == 0)
			num_right_answer_noclick++;*/


		if ("[1,0]" == estims.tostring(0) && matrix[i][33] == 0) {
			num_right_answer_noclick++;
			num_all_right_answer++;
		}

		if ("[0,1]" == estims.tostring(0) && matrix[i][33] == 1) {
			num_right_answers_click++;
			num_all_right_answer++;
		}
		
		if ("[0,1]" == estims.tostring(0))
			num_positiv_answer++;

		if (matrix[i][33] == 1)
			num_click++;			

		log << "clicks :" << matrix[i][33] << " estimate:" << estims.tostring(0) << " 0:"<< estims[0] << " 1:" << estims[1] << "  :" << estims.tostring(1) << endl;

		alglib::dfprocess(dforest_realist, input, estims);

		if ("[1,0]" == estims.tostring(0) && matrix[i][33] == 0) {
			num_right_answer_noclick_realist++;
			num_all_right_answer_realist++;
		}

		if ("[0,1]" == estims.tostring(0) && matrix[i][33] == 1) {
			num_right_answers_click_realist++;
			num_all_right_answer_realist++;
		}

		if ("[0,1]" == estims.tostring(0))
			num_positiv_answer_realist++;
	}

	double persent_clicks = 100.0 * (double)num_right_answers_click / (double)num_click;
	double persent = 100.0 * (double)num_right_answer_noclick / (double)matrix.rows();
	double persent_positive = 100.0 * (double)num_positiv_answer / (double)matrix.rows();
	double persent_all = 100.0 * (double)num_all_right_answer / (double)matrix.rows();
	
	cout << endl << "\t\tPositive Forest :)" << endl;
	cout << endl << "no click answer:" << num_right_answer_noclick << " from " << matrix.rows() << " or persent :" << persent << " %" << endl;
	cout << endl << "click right answer:" << num_right_answers_click << " from " << num_click << " or persent :" << persent_clicks << " %" << endl;
	cout << endl << "Positive answers for sets:" << num_positiv_answer << " or all set: " << persent_positive << " %" << endl;
	cout << endl << "All answers right for sets:" << num_all_right_answer << " or all set: " << persent_all << " %" << endl;


	persent_clicks = 100.0 * (double)num_right_answers_click_realist / (double)num_click;
	persent = 100.0 * (double)num_right_answer_noclick_realist / (double)matrix.rows();
	persent_positive = 100.0 * (double)num_positiv_answer_realist / (double)matrix.rows();
	persent_all = 100.0 * (double)num_all_right_answer_realist / (double)matrix.rows();

	cout << endl << "\tRealist Forest :(" << endl;
	cout << endl << "no click answer:" << num_right_answer_noclick_realist << " from " << matrix.rows() << " or persent :" << persent << " %" << endl;
	cout << endl << "click right answer:" << num_right_answers_click_realist << " from " << num_click << " or persent :" << persent_clicks << " %" << endl;
	cout << endl << "Positive answers for sets:" << num_positiv_answer_realist << " or all set: " << persent_positive << " %" << endl;
	cout << endl << "All answers right for sets:" << num_all_right_answer_realist << " or all set: " << persent_all << " %" << endl;

	log << "No click right answer:" << num_right_answer_noclick << " from " << matrix.rows() << " or persent :" << persent << " %" << endl;
	log << "Click right answer:" << num_right_answers_click << " from " << num_click << " or persent :" << persent_clicks << " %" << endl;
	log << "Positive answers for sets:" << num_positiv_answer << " or all set: " << persent_positive << " %" << endl;
	log << "All answers right for sets:" << num_all_right_answer << " or all set: " << persent_all << " %" << endl;
	log << "Stop";
}

void create_train_check_v2()
{
	cout << "\t ******** Start ********" << endl;
	DataFrame test_data_frame, train_data_frame;

	ofstream log("../models/log.txt");
	log << "Start" << endl;
	/*	
	train_data_frame.load("../data/sets/train_raw/code_list.csv");
	test_data_frame.load("../data/sets/test/test_without_duplicates_pBP0Dr1q_3days.csv");*/

	train_data_frame.load("../data/sets/train/code_list2__train.csv");
	test_data_frame.load("../data/sets/test/code_list2__testing.csv");
	
	//train_data_frame.preprocessing();

	cout << endl << "Created forest, and report." << endl;


	alglib::dfreport rep;
	alglib::decisionforest dforest;
	
	alglib::ae_int_t info;
	alglib::ae_int_t nclasses = 2;
	alglib::ae_int_t ntrees = 100;
	double r = 0.3;
	alglib::ae_int_t npoints = train_data_frame.get_set().rows();
	alglib::ae_int_t nvars = train_data_frame.get_set().cols();

	cout << endl << "Train forest." << endl;
	//nvars имеетс€ ввиду незовисимых, последний столбец зависмый 
	//т.е. номер класса не указываем nvars-1 
	alglib::dfbuildrandomdecisionforest(train_data_frame.get_set(), npoints, nvars - 1, nclasses, ntrees, r, info, dforest, rep);
	print_rep_info(rep, info);

	try {
		cout << endl << "\t Forest :" << endl;
		double error = alglib::dfavgrelerror(dforest, test_data_frame.get_set(), test_data_frame.get_set().rows());
		cout << endl << "\tAverage error is = " << error << endl;
		log << "\tAverage error is = " << error << endl;
	}
	catch (alglib::ap_error & err)
	{
		cout << "problem with dfavgrelerror" << endl;
		cout << err.msg << endl;
	}

	cout << "Estimate:" << endl;
	log << "Estimate: " << endl;

	const auto & matrix = test_data_frame.get_set();
	vector<double> row;
	int indep_vars = matrix.cols() - 1;
	int num_click = 0;
	int num_right_answers_click = 0;
	int num_right_answer_noclick = 0;
	int num_positiv_answer = 0;
	int num_all_right_answer = 0;
	int num_experemtal = 0;
	int num_experemtal_all = 0;
	
	// build and classify the input set
	alglib::real_1d_array input;
	alglib::real_1d_array estims;

	for (int i = 0; i < matrix.rows(); i++) {
		//char * p = matrix[i];
		for (int ii = 0; ii < indep_vars; ii++) {
			row.push_back(matrix[i][ii]);
		}

		input.setcontent(indep_vars, row.data());
		row.clear();

		alglib::dfprocess(dforest, input, estims);		

		if ("[1,0]" == estims.tostring(0) && matrix[i][33] == 0) {
			num_right_answer_noclick++;
			num_all_right_answer++;
		}

		if ("[0,1]" == estims.tostring(0) && matrix[i][33] == 1) {
			num_right_answers_click++;
			num_all_right_answer++;
		}

		if ("[0,1]" == estims.tostring(0))
			num_positiv_answer++;

		if (matrix[i][33] == 1)
			num_click++;

		if (estims[1]>0.45 && matrix[i][33] == 1)
			num_experemtal++;
		if (estims[1]>0.45)
			num_experemtal_all++;

		log << "clicks :" << matrix[i][33] << " estimate:" << estims.tostring(0) << " 0:" << estims[0] << " 1:" << estims[1] << "  :" << estims.tostring(1) << endl;
		
	}

	double persent_clicks = 100.0 * (double)num_right_answers_click / (double)num_click;
	double persent = 100.0 * (double)num_right_answer_noclick / (double)matrix.rows();
	double persent_positive = 100.0 * (double)num_positiv_answer / (double)matrix.rows();
	double persent_all = 100.0 * (double)num_all_right_answer / (double)matrix.rows();

	cout << endl << "\t\tForest RESULT :" << endl;
	cout << endl << "no click answer:" << num_right_answer_noclick << " / " << matrix.rows() << " or persent :" << persent << " %" << endl;
	cout << endl << "click right answer:" << num_right_answers_click << " / " << num_click << " or persent :" << persent_clicks << " %" << endl;
	cout << endl << "Positive answers for sets:" << num_positiv_answer << " or all set: " << persent_positive << " %" << endl;
	cout << endl << "All answers right for sets:" << num_all_right_answer <<" / " << matrix.rows() << " or all set: " << persent_all << " %" << endl;
	cout << endl << "Experemental answers right for sets:" << num_experemtal << " / " << num_click << endl;
	cout << endl << "Experemental answers all for sets:" << num_experemtal_all << " / " <<  matrix.rows() << " or all set: " << ((double)num_experemtal_all*100.0 / (double)matrix.rows()) << " %" << endl;
	cout << endl;
	
	log << "No click right answer:" << num_right_answer_noclick << " from " << matrix.rows() << " or persent :" << persent << " %" << endl;
	log << "Click right answer:" << num_right_answers_click << " from " << num_click << " or persent :" << persent_clicks << " %" << endl;
	log << "Positive answers for sets:" << num_positiv_answer << " or all set: " << persent_positive << " %" << endl;
	log << "All answers right for sets:" << num_all_right_answer << " or all set: " << persent_all << " %" << endl;
	log << "Experemental answers right for sets:" << num_experemtal << " / " << num_click << endl;
	log << "Experemental answers all for sets:" << num_experemtal_all << " / " << matrix.rows() << " or all set: " << ((double)num_experemtal_all*100.0 / (double)matrix.rows()) << " %" << endl;
	log << "Stop";
}

void pca_analyze(alglib::real_2d_array & input_x, alglib::real_2d_array & output, ofstream  & log_file)
{

	/*INPUT PARAMETERS :
	X - dataset, array[0..NPoints - 1, 0..NVars - 1].
		matrix contains ONLY INDEPENDENT VARIABLES.
		NPoints - dataset size, NPoints >= 0
		NVars - number of independent variables, NVars >= 1

		¬џ’ќƒЌџ≈ ѕј–јћ≈“–џ :
	Info - return code :
		*-4, if SVD subroutine haven't converged
		* -1, if wrong parameters has been passed(NPoints<0,
			NVars<1)
		* 1, if task is solved
		S2 - array[0..NVars - 1].variance values corresponding
		to basis vectors.
		V - array[0..NVars - 1, 0..NVars - 1]
		matrix, whose columns store basis vectors.*/
		
	cout << "\t ******** Start PCA********" << endl;
	
	alglib::ae_int_t npoints;
	alglib::ae_int_t nvars;
	alglib::ae_int_t info;
	alglib::real_1d_array s2;
	alglib::real_2d_array v;
		
	npoints = input_x.rows();
	nvars = input_x.cols();

	cout << "\t PCA is working " << endl;

	try {
		alglib::pcabuildbasis(input_x, npoints, nvars, info, s2, v);
	}
	catch (alglib::ap_error& err) {
		cout << err.msg << endl;
	}

	cout << "\t info :" << info << endl;
	log_file << endl << "info: " << info << endl;

	log_file << endl << "\S2 - array[0..NVars - 1].variance values corresponding to basis vectors. " << endl;

	log_file << endl << "s2: " << s2.tostring(2) <<endl;
	cout << endl << "s2: " << s2.tostring(2) << endl;
	//log_pca << endl << "\tarray[0..NVars - 1, 0..NVars - 1]	matrix, whose columns store basis vectors : " << endl;

	/*for (int i = 0; i < v.rows(); i++) {
		for (int ii = 0; ii < v.cols(); ii++) {
			log_pca << " " << v[i][ii];
		}
		log_pca << endl;
	}*/

	int index_stop = 0;
	
	for (;index_stop<s2.length(); index_stop++) {		
		if (s2[index_stop] < 0.1) {			
			break;
		}
	}
	cout << "index stop:" << index_stop << endl;
	log_file << "index stop:" << index_stop << endl;

	int number = index_stop + 2;
	output.setlength(input_x.rows(), number);

	
	for (int i = 0; i < output.rows(); i++) {
		for (int ii = 0; ii < output.cols()-1; ii++) {
			output[i][ii] = input_x[i][ii];
			log_file << " " << output[i][ii];
		}
		output[i][index_stop+1] = input_x[i][input_x.cols()-1];
		log_file << " " << output[i][index_stop + 1];
		log_file << endl;
	}

	cout << "\t ******** Stop PCA********" << endl;
	log_file << "\t ******** Stop PCA********" << endl;
}


void kmeans_analyze(alglib::real_2d_array & _xy, alglib::real_2d_array & arr_class0, alglib::real_2d_array & arr_class1, ofstream & log_file)
{
	/*
	INPUT PARAMETERS :
	XY - dataset, array[0..NPoints - 1, 0..NVars - 1].
		NPoints - dataset size, NPoints >= K
		NVars - number of variables, NVars >= 1
		K - desired number of clusters, K >= 1
		Restarts - number of restarts, Restarts >= 1
		
	OUTPUT PARAMETERS:
    Info        -   return code:
                    * -3, if task is degenerate (number of distinct points is
                          less than K)
                    * -1, if incorrect NPoints/NFeatures/K/Restarts was passed
                    *  1, if subroutine finished successfully
    C           -   array[0..NVars-1,0..K-1].matrix whose columns store
                    cluster's centers
    XYC         -   array[NPoints], which contains cluster indexes
	*/
	

	cout << "\t\t ******** Start K-Means ********" << endl;
	log_file << "\t\t ******** Start K-Means ********" << endl;		
	
	
	alglib::real_2d_array xy_new;
	alglib::ae_int_t npoints;
	alglib::ae_int_t nvars;
	alglib::ae_int_t k;
	alglib::ae_int_t restarts;
	alglib::ae_int_t info;
	alglib::real_2d_array c;
	alglib::integer_1d_array xyc;
		

	// на одно поле меньше так это зовисимый параметр
	xy_new.setlength(_xy.rows(), _xy.cols()-1);
	npoints = xy_new.rows();
	nvars = xy_new.cols(); 
	k = 2;
	restarts = 4;
	
	for (int i = 0; i < xy_new.rows(); i++) {
		for (int ii = 0; ii < xy_new.cols(); ii++) {
			 xy_new[i][ii] = _xy[i][ii];			 
		}
	}
	
	cout << "K-Means is working" << endl;
	log_file << "K-Means is working" << endl;

	alglib::kmeansgenerate(xy_new, npoints, nvars, k, restarts, info, c, xyc);

	cout << "info: " << info << endl;
	log_file << endl << "info: " << info << endl;
	
	log_file << endl << "\t C    -   array[0..NVars-1,0..K-1].matrix whose columns store cluster's centers: " << endl;
	for (int i = 0; i < c.rows(); i++) {
		for (int ii = 0; ii < c.cols(); ii++) {
			log_file << " " << c[i][ii];
		}
		log_file << endl;
	}
	
	log_file << endl << "\t  XYC         -   array[NPoints], which contains cluster indexes: " << endl;
	
	int num_class_0 = 0;
	int num_class_1 = 0;
	int num_class_2 = 0;
	int num_class_3 = 0;

	vector<double> vec_class0;
	vector<double> vec_class1;
	vector<double> row;
	

	for (int i = 0; i < _xy.rows(); i++) {

		for (int ii = 0; ii < _xy.cols(); ii++) {
			log_file << " " << _xy[i][ii];
			row.push_back(_xy[i][ii]);
		}
		log_file << " xyc :" << xyc[i];
		log_file << endl;
		
		switch (xyc[i])
		{
		case 0: 
			num_class_0++;			
			vec_class0.insert(vec_class0.end(), row.begin(), row.end());
			break;
		case 1: 
			num_class_1++;
			vec_class1.insert(vec_class1.end(), row.begin(), row.end());
			break;
		case 2: 
			num_class_2++;
			break;
		case 3: 
			num_class_3++;
			break;
		}

		row.clear();
	}
	
	arr_class0.setcontent(num_class_0, _xy.cols(), vec_class0.data());		
	arr_class1.setcontent(num_class_1, _xy.cols(), vec_class1.data());

	cout << endl << "num class 0: " << num_class_0 << endl;
	cout << "num class 1: " << num_class_1 << endl;
	cout << "num class 2: " << num_class_2 << endl;
	cout << "num class 3: " << num_class_3 << endl;

	log_file << endl << "num class 0: " << num_class_0 << endl;
	log_file << "num class 1: " << num_class_1 << endl;
	log_file << "num class 2: " << num_class_2 << endl;

}


void train_forest(alglib::decisionforest & _dforest, alglib::real_2d_array & _xy, ofstream & log_file)
{
	cout << "\t ******** Start train forest ********" << endl;
	log_file << "\t ******** Start train forest ********" << endl;

	alglib::dfreport rep;	
	alglib::ae_int_t info;
	alglib::ae_int_t nclasses = 2;
	alglib::ae_int_t ntrees = 100;
	double r = 0.3;

	alglib::ae_int_t npoints = _xy.rows();
	alglib::ae_int_t nvars = _xy.cols();	

	alglib::dfbuildrandomdecisionforest(_xy, npoints, nvars - 1, nclasses, ntrees, r, info, _dforest, rep);

	print_rep_info(rep, info);

	cout << "\t ******** Stop train forest ********" << endl;
	log_file << "\t ******** Stop train forest ********" << endl;
}

void test_and_estimate_forest(alglib::decisionforest & _dforest, alglib::real_2d_array & _xy, ofstream & log_file)
{
	try {
		cout << endl << "\t Forest :" << endl;
		double error = alglib::dfavgrelerror(_dforest, _xy, _xy.rows());
		cout << endl << "\tAverage error is = " << error << endl;
		log_file << "\tAverage error is = " << error << endl;
	}
	catch (alglib::ap_error & err)
	{
		cout << "problem with dfavgrelerror" << endl;
		cout << err.msg << endl;
	}

	cout << "Estimate:" << endl;
	log_file << "Estimate: " << endl;
		
	vector<double> row;
	int indep_vars = _xy.cols() - 1;
	int num_click = 0;
	int num_right_answers_click = 0;
	int num_right_answer_noclick = 0;
	int num_positiv_answer = 0;
	int num_all_right_answer = 0;
	int num_experemtal = 0;
	int num_experemtal_all = 0;

	// build and classify the input set
	alglib::real_1d_array input;
	alglib::real_1d_array estims;


	int index_dependent_var = _xy.cols() - 1;

	for (int i = 0; i < _xy.rows(); i++) {
		//char * p = matrix[i];
		for (int ii = 0; ii < indep_vars; ii++) {
			row.push_back(_xy[i][ii]);
		}

		input.setcontent(indep_vars, row.data());
		row.clear();

		alglib::dfprocess(_dforest, input, estims);

		if ("[1,0]" == estims.tostring(0) && _xy[i][index_dependent_var] == 0) {
			num_right_answer_noclick++;
			num_all_right_answer++;
		}

		if ("[0,1]" == estims.tostring(0) && _xy[i][index_dependent_var] == 1) {
			num_right_answers_click++;
			num_all_right_answer++;
		}

		if ("[0,1]" == estims.tostring(0))
			num_positiv_answer++;

		if (_xy[i][index_dependent_var] == 1)
			num_click++;
	}

	double persent_clicks = 100.0 * (double)num_right_answers_click / (double)num_click;
	double persent = 100.0 * (double)num_right_answer_noclick / (double)_xy.rows();
	double persent_positive = 100.0 * (double)num_positiv_answer / (double)_xy.rows();
	double persent_all = 100.0 * (double)num_all_right_answer / (double)_xy.rows();

	cout << endl << "\t\tForest RESULT :" << endl;
	cout << endl << "no click answer:" << num_right_answer_noclick << " / " << _xy.rows() << " or persent :" << persent << " %" << endl;
	cout << endl << "click right answer:" << num_right_answers_click << " / " << num_click << " or persent :" << persent_clicks << " %" << endl;
	cout << endl << "Positive answers for sets:" << num_positiv_answer << " or all set: " << persent_positive << " %" << endl;
	cout << endl << "All answers right for sets:" << num_all_right_answer << " / " << _xy.rows() << " or all set: " << persent_all << " %" << endl;
	cout << endl;

	log_file << "No click right answer:" << num_right_answer_noclick << " from " << _xy.rows() << " or persent :" << persent << " %" << endl;
	log_file << "Click right answer:" << num_right_answers_click << " from " << num_click << " or persent :" << persent_clicks << " %" << endl;
	log_file << "Positive answers for sets:" << num_positiv_answer << " or all set: " << persent_positive << " %" << endl;
	log_file << "All answers right for sets:" << num_all_right_answer << " or all set: " << persent_all << " %" << endl;

}

void preprocessing(alglib::real_2d_array & _xy, ofstream & log_file)
{	
	cout << "\t\tStart preprocessing" << endl;
	log_file << "\t\tStart preprocessing" << endl;

	vector<string> vec_0_str;
	vector<string> vec_1_str;
	vector<double> row;	
	alglib::real_1d_array input;
	
	int index_dependent_var = _xy.cols() - 1;

	for (int i = 0; i < _xy.rows(); i++)
	{
		for (int ii = 0; ii < _xy.cols(); ii++) {
			row.push_back(_xy[i][ii]);
		}
		input.setcontent(_xy.cols(), row.data());
		row.clear();

		if (_xy[i][index_dependent_var] == 1)
			vec_1_str.push_back(input.tostring(2));

		if (_xy[i][index_dependent_var] == 0)
			vec_0_str.push_back(input.tostring(2));
	}

	cout << "\t\tStart lexicographically sort" << endl;
	log_file << "\t\tStart lexicographically sort" << endl;

	std::sort(vec_0_str.begin(), vec_0_str.end());
	std::sort(vec_1_str.begin(), vec_1_str.end());

	//std::sort(vec_1_str.begin(), vec_1_str.end());

	//std::sort(vec_1_str.begin(), vec_1_str.end());
	cout << "\t\tEnd lexicographically sort" << endl;
	log_file << "\t\tEnd lexicographically sort" << endl;

	// delete duplicate
	cout << "\t\tDelete duplicate" << endl;
	log_file << "\t\tDelete duplicate" << endl;

	vec_1_str.erase(std::unique(vec_1_str.begin(), vec_1_str.end()), vec_1_str.end());
	vec_0_str.erase(std::unique(vec_0_str.begin(), vec_0_str.end()), vec_0_str.end());

	//cut sets
	vec_0_str.erase(vec_0_str.begin() + 50000, vec_0_str.end());

	//merge sets
	//vec_1_str.insert(vec_1_str.end(), vec_0_str.begin(), vec_0_str.end());

	//random insert
	cout << "\t\tRandom merge" << endl;
	log_file << "\t\tRandom merge" << endl;

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
		log_file << v << endl;
	}

	_xy.setlength(vec_0_str.size(), _xy.cols());

	for (int i = 0; i < _xy.rows(); i++)
	{
		alglib::real_1d_array out(vec_0_str[i].c_str());
		for (int ii = 0; ii < _xy.cols(); ii++) {
			_xy[i][ii] = out[ii];
		}
	}
	cout << "\t\tStop preprocessing" << endl;
	log_file << "\t\tStop preprocessing" << endl;
}

void create_train_check_v3()
{
	cout << "\t ******** Start ********" << endl;
	DataFrame test_data_frame, train_data_frame;

	ofstream log("../models/log.txt");
	log << "Start" << endl;
	
	train_data_frame.load("../data/sets/train_raw/code_list.csv");
	test_data_frame.load("../data/sets/test/test_without_duplicates_pBP0Dr1q_3days.csv");

	cout << endl << "Start preprocessing without cut." << endl;
	train_data_frame.preprocessing();

	//pca
	alglib::real_2d_array result_pca;
//	pca_analyze(train_data_frame.get_set(), result_pca);

	cout << endl << "Start K-Menas analyze." << endl;
	
	alglib::real_2d_array cls0, cls1;
//	kmeans_analyze(result_pca, cls0, cls1);

	cout << endl << "Created forest, and report." << endl;
	
	alglib::dfreport rep;
	alglib::decisionforest dforest;

	alglib::ae_int_t info;
	alglib::ae_int_t nclasses = 2;
	alglib::ae_int_t ntrees = 100;
	double r = 0.3;
	alglib::ae_int_t npoints = cls0.rows();
	alglib::ae_int_t nvars = cls0.cols();

	cout << endl << "Train forest.cls0" << endl;
	//nvars имеетс€ ввиду незовисимых, последний столбец зависмый 
	//т.е. номер класса не указываем nvars-1 
	alglib::dfbuildrandomdecisionforest(cls0, npoints, nvars - 1, nclasses, ntrees, r, info, dforest, rep);
	print_rep_info(rep, info);

	alglib::real_2d_array test_output;	
	alglib::real_2d_array & test_input = test_data_frame.get_set();
	
	cout << "cls0 rows :" << cls0.rows() << endl;
	cout << "cls0 cols :" << cls0.cols() << endl;
	
	test_output.setlength(test_input.rows(), cls0.cols());
				
	int number = test_output.cols() - 1;
	int last_index = number;

	// trasform test sets

	for (int i = 0; i < test_output.rows(); i++) {
		for (int ii = 0; ii < number; ii++) {
			test_output[i][ii] = test_input[i][ii];
		}
		test_output[i][last_index] = test_input[i][33];
	}

	try {
		cout << endl << "\t Forest :" << endl;
		double error = alglib::dfavgrelerror(dforest, test_output, test_output.rows());
		cout << endl << "\tAverage error is = " << error << endl;
		log << "\tAverage error is = " << error << endl;
	}
	catch (alglib::ap_error & err)
	{
		cout << "problem with dfavgrelerror" << endl;
		cout << err.msg << endl;
	}

	cout << "Estimate:" << endl;
	log << "Estimate: " << endl;
	
	const auto & matrix = test_output;
	vector<double> row;
	int indep_vars = matrix.cols() - 1;
	int num_click = 0;
	int num_right_answers_click = 0;
	int num_right_answer_noclick = 0;
	int num_positiv_answer = 0;
	int num_all_right_answer = 0;
	int num_experemtal = 0;
	int num_experemtal_all = 0;

	// build and classify the input set
	alglib::real_1d_array input;
	alglib::real_1d_array estims;

	for (int i = 0; i < matrix.rows(); i++) {
		//char * p = matrix[i];
		for (int ii = 0; ii < indep_vars; ii++) {
			row.push_back(matrix[i][ii]);
		}

		input.setcontent(indep_vars, row.data());
		row.clear();

		alglib::dfprocess(dforest, input, estims);

		if ("[1,0]" == estims.tostring(0) && matrix[i][33] == 0) {
			num_right_answer_noclick++;
			num_all_right_answer++;
		}

		if ("[0,1]" == estims.tostring(0) && matrix[i][33] == 1) {
			num_right_answers_click++;
			num_all_right_answer++;
		}

		if ("[0,1]" == estims.tostring(0))
			num_positiv_answer++;

		if (matrix[i][33] == 1)
			num_click++;
	}

	double persent_clicks = 100.0 * (double)num_right_answers_click / (double)num_click;
	double persent = 100.0 * (double)num_right_answer_noclick / (double)matrix.rows();
	double persent_positive = 100.0 * (double)num_positiv_answer / (double)matrix.rows();
	double persent_all = 100.0 * (double)num_all_right_answer / (double)matrix.rows();

	cout << endl << "\t\tForest RESULT :" << endl;
	cout << endl << "no click answer:" << num_right_answer_noclick << " / " << matrix.rows() << " or persent :" << persent << " %" << endl;
	cout << endl << "click right answer:" << num_right_answers_click << " / " << num_click << " or persent :" << persent_clicks << " %" << endl;
	cout << endl << "Positive answers for sets:" << num_positiv_answer << " or all set: " << persent_positive << " %" << endl;
	cout << endl << "All answers right for sets:" << num_all_right_answer <<" / " << matrix.rows() << " or all set: " << persent_all << " %" << endl;	
	cout << endl;
	
	log << "No click right answer:" << num_right_answer_noclick << " from " << matrix.rows() << " or persent :" << persent << " %" << endl;
	log << "Click right answer:" << num_right_answers_click << " from " << num_click << " or persent :" << persent_clicks << " %" << endl;
	log << "Positive answers for sets:" << num_positiv_answer << " or all set: " << persent_positive << " %" << endl;
	log << "All answers right for sets:" << num_all_right_answer << " or all set: " << persent_all << " %" << endl;
	

	// ***************************************************************************************************************************
	// ******************************************************* othe forest *******************************************************
	// ***************************************************************************************************************************

	npoints = cls1.rows();
	nvars = cls1.cols();

	cout << endl << "Train New forest. cls1" << endl;
	//nvars имеетс€ ввиду незовисимых, последний столбец зависмый 
	//т.е. номер класса не указываем nvars-1 
	alglib::dfbuildrandomdecisionforest(cls1, npoints, nvars - 1, nclasses, ntrees, r, info, dforest, rep);
	print_rep_info(rep, info);

	try {
		cout << endl << "\t Forest :" << endl;
		double error = alglib::dfavgrelerror(dforest, test_data_frame.get_set(), test_data_frame.get_set().rows());
		cout << endl << "\tAverage error is = " << error << endl;
		log << "\tAverage error is = " << error << endl;
	}
	catch (alglib::ap_error & err)
	{
		cout << "problem with dfavgrelerror" << endl;
		cout << err.msg << endl;
	}

	cout << "Estimate:" << endl;
	log << "Estimate: " << endl;
		
	num_click = 0;
	num_right_answers_click = 0;
	num_right_answer_noclick = 0;
	num_positiv_answer = 0;
	num_all_right_answer = 0;
	
	// build and classify the input set
	
	row.clear();
	for (int i = 0; i < matrix.rows(); i++) {
		//char * p = matrix[i];
		for (int ii = 0; ii < indep_vars; ii++) {
			row.push_back(matrix[i][ii]);
		}

		input.setcontent(indep_vars, row.data());
		row.clear();

		alglib::dfprocess(dforest, input, estims);

		if ("[1,0]" == estims.tostring(0) && matrix[i][33] == 0) {
			num_right_answer_noclick++;
			num_all_right_answer++;
		}

		if ("[0,1]" == estims.tostring(0) && matrix[i][33] == 1) {
			num_right_answers_click++;
			num_all_right_answer++;
		}

		if ("[0,1]" == estims.tostring(0))
			num_positiv_answer++;

		if (matrix[i][33] == 1)
			num_click++;
	}

	persent_clicks = 100.0 * (double)num_right_answers_click / (double)num_click;
	persent = 100.0 * (double)num_right_answer_noclick / (double)matrix.rows();
	persent_positive = 100.0 * (double)num_positiv_answer / (double)matrix.rows();
	persent_all = 100.0 * (double)num_all_right_answer / (double)matrix.rows();

	cout << endl << "\t\tForest RESULT :" << endl;
	cout << endl << "no click answer:" << num_right_answer_noclick << " / " << matrix.rows() << " or persent :" << persent << " %" << endl;
	cout << endl << "click right answer:" << num_right_answers_click << " / " << num_click << " or persent :" << persent_clicks << " %" << endl;
	cout << endl << "Positive answers for sets:" << num_positiv_answer << " or all set: " << persent_positive << " %" << endl;
	cout << endl << "All answers right for sets:" << num_all_right_answer << " / " << matrix.rows() << " or all set: " << persent_all << " %" << endl;	
	cout << endl;

	log << "No click right answer:" << num_right_answer_noclick << " from " << matrix.rows() << " or persent :" << persent << " %" << endl;
	log << "Click right answer:" << num_right_answers_click << " from " << num_click << " or persent :" << persent_clicks << " %" << endl;
	log << "Positive answers for sets:" << num_positiv_answer << " or all set: " << persent_positive << " %" << endl;
	log << "All answers right for sets:" << num_all_right_answer << " or all set: " << persent_all << " %" << endl;	

}

void create_train_check_v4()
{
	cout << "\t ******** Start create_train_check_v4 ********" << endl;
	DataFrame train_set, test_set;
	train_set.load("../data/sets/train_raw/code_list.csv");
	test_set.load("../data/sets/test/test_without_duplicates_pBP0Dr1q_3days.csv");
	//train_set.load("../data/sets/train/code_list3.csv");
	auto & xy = train_set.get_set();
	auto & xy_test = test_set.get_set();
	
	ofstream log_prep("../temp/preprocessing.txt");
	preprocessing(xy, log_prep);
	log_prep.close();

	//ofstream log_pca("../temp/pca_log.txt");
	//alglib::real_2d_array result_pca;	
	//pca_analyze(train_set.get_set(), result_pca, log_pca);	
	//log_pca.close();

	//ofstream log_kmeans("../temp/log_kmeans.txt");
	//alglib::real_2d_array xy_cls0;
	//alglib::real_2d_array xy_cls1;
	//kmeans_analyze(result_pca, xy_cls0, xy_cls1, log_kmeans);
	//log_kmeans.close();

	ofstream log_forest("../temp/log_train_forest.txt");
	alglib::decisionforest dforest;
	train_forest(dforest, xy, log_forest);
	test_and_estimate_forest(dforest, xy_test, log_forest);
	log_forest.close();

}

int main()
{	
	clock_t tStart = clock();	
	//create_train_check_v2();
	//create_train_check_v3();
	create_train_check_v4();	
	
		
	cout << "Time taken :" << (double)(clock() - tStart) / CLOCKS_PER_SEC << endl;	
	cout << "Time taken :" << (double)(clock() - tStart) / (CLOCKS_PER_SEC*60) << endl;
    return 0;
}

