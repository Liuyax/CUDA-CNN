#include "Config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>
using namespace std;


bool Config::get_word_bool(string &str, string name){
	size_t pos = str.find(name);    
	int i = pos + 1;
	bool res = true;
	while(1){
		if(i == str.length()) break;
		if(str[i] == ';') break;
		++ i;
	}
	string sub = str.substr(pos, i - pos + 1);
	if(sub[sub.length() - 1] == ';'){
		string content = sub.substr(name.length() + 1, sub.length() - name.length() - 2);
		if(!content.compare("true")) res = true;
		else res = false;
	}
	str.erase(pos, i - pos + 1);
	return res;
}

int Config::get_word_int(string &str, string name){
		size_t pos = str.find(name);    
		int i = pos + 1;
		int res = 1;
		while(1){
			if(i == str.length()) break;
			if(str[i] == ';') break;
			++ i;
		}
		string sub = str.substr(pos, i - pos + 1);
		if(sub[sub.length() - 1] == ';'){
			string content = sub.substr(name.length() + 1, sub.length() - name.length() - 2);
			res = atoi(content.c_str());
		}
		str.erase(pos, i - pos + 1);
		return res;
}

double Config::get_word_double(string &str, string name){
		size_t pos = str.find(name);    
		int i = pos + 1;
		double res = 0.0;
		while(1){
			if(i == str.length()) break;
			if(str[i] == ';') break;
			++ i;
		}
		string sub = str.substr(pos, i - pos + 1);
		if(sub[sub.length() - 1] == ';'){
			string content = sub.substr(name.length() + 1, sub.length() - name.length() - 2);
			res = atof(content.c_str());
		}
		str.erase(pos, i - pos + 1);
		return res;
}

string Config::get_word_type(string &str, string name){
		size_t pos = str.find(name);    
		if(pos == str.npos){
			return "NULL";
		}

		int i = pos + 1;
		int res = 0;
		while(1){
			if(i == str.length()) break;
			if(str[i] == ';') break;
			++ i;
		}
		string sub = str.substr(pos, i - pos + 1);
		string content;
		if(sub[sub.length() - 1] == ';'){
			content = sub.substr(name.length() + 1, sub.length() - name.length() - 2);
		}
		str.erase(pos, i - pos + 1);
		return content;
}

std::vector<string> Config::get_name_vector(string &str, string name){
	std::vector<std::string>result;
	
	size_t pos = str.find(name);    
	if(pos == str.npos){
		return result;
	}

	int i = pos + 1;
	int res = 0;
	while(1){
		if(i == str.length()) break;
		if(str[i] == ';') break;
		++ i;
	}
	string sub = str.substr(pos, i - pos + 1);
	string content;
	if(sub[sub.length() - 1] == ';'){
		content = sub.substr(name.length() + 1, sub.length() - name.length() - 2);
	}
	str.erase(pos, i - pos + 1);

	while(content.size()){
		size_t pos = content.find(',');
		if(pos == str.npos){
			result.push_back(content);
			break;
		}else{
			result.push_back(content.substr(0, pos));
			content.erase(0, pos + 1);
		}
	}



	return result;
}


void Config:: get_layers_config(string &str){
	vector<string> layers;
	if(str.empty()) return;
	int head = 0;
	int tail = 0;
	while(1){
		if(head == str.length()) break;
		if(str[head] == '['){
			tail = head + 1;
			while(1){
				if(tail == str.length()) break;
				if(str[tail] == ']') break;
				++ tail;
			}
			string sub = str.substr(head, tail - head + 1);
			if(sub[sub.length() - 1] == ']'){
				sub.erase(sub.begin() + sub.length() - 1);
				sub.erase(sub.begin());
				layers.push_back(sub);
			}
			str.erase(head, tail - head + 1);
		}else ++ head;
	}
	for(int i = 0; i < layers.size(); i++){
		string type = get_word_type(layers[i], "LAYER");
		std::string name = get_word_type(layers[i], "NAME");
		std::string input = get_word_type(layers[i], "INPUT");
		std::string subInput = get_word_type(layers[i], "SUBINPUT");

		ConfigBase* layer;
		if(type == string("CONV")) {
			int ks = get_word_int(layers[i], "KERNEL_SIZE");
			int ka = get_word_int(layers[i], "KERNEL_AMOUNT");
			int pd = get_word_int(layers[i], "PADDING");
			int cfm= get_word_int(layers[i], "COMBINE_FEATRUE_MAPS");
			double initW = get_word_double(layers[i], "initW");
			std::string initType = get_word_type(layers[i], "initType");

			double wd = get_word_double(layers[i], "WEIGHT_DECAY");
			string non_linearity = get_word_type(layers[i], "NON_LINEARITY");
			m_nonLinearity = new ConfigNonLinearity(non_linearity);

			layer = new ConfigConv(name, input, subInput, type, ks, pd, ka, wd, cfm,
				initW, initType, m_nonLinearity->getValue());

			printf("\n\n********conv layer********\n");
			printf("NAME          : %s\n", name.c_str());
			printf("INPUT         : %s\n", input.c_str());
			printf("SUBINPUT      : %s\n", subInput.c_str());
			printf("KERNEL_SIZE   : %d\n", ks);
			printf("KERNEL_AMOUNT : %d\n", ka);
			printf("CFM           : %d\n", cfm);
			printf("PADDING       : %d\n", pd);
			printf("WEIGHT_DECAY  : %lf\n", wd);
			printf("initW         : %lf\n", initW);
			printf("non_linearity : %s\n", non_linearity.c_str());
		}
		else if(type == string("BRANCHLAYER")){
			std::vector<std::string>outputs = get_name_vector(layers[i], "OUTPUTS");
			layer = new ConfigBranchLayer(name, input, subInput, type, outputs);

			printf("\n\n********branch layer********\n");
			printf("NAME          : %s\n", name.c_str());
			printf("INPUT         : %s\n", input.c_str());
			printf("SUBINPUT      : %s\n", subInput.c_str());
			printf("OUTPUTS       :");
			for(int i = 0; i < outputs.size(); i++){
				printf("%s,", outputs[i].c_str());
			}printf("\n");
		}
		else if(type == string("COMBINELAYER")){
			std::vector<std::string>inputs = get_name_vector(layers[i], "SUBINPUTS");
			layer = new ConfigCombineLayer(name, inputs, subInput, type);

			printf("\n\n********combine Layer********\n");
			printf("NAME          : %s\n", name.c_str());
			printf("SUBINPUT      : %s\n", subInput.c_str());
			printf("INPUTS        :");

			for(int i = 0; i < inputs.size(); i++){
				printf("%s,", inputs[i].c_str());
			}printf("\n");
		}
		else if(type == string("NIN")){
			double wd = get_word_double(layers[i], "WEIGHT_DECAY");
			layer = new ConfigNIN(name, input, subInput, type, wd);
			
			printf("\n\n********NIN layer********\n");
			printf("NAME          : %s\n", name.c_str());
			printf("INPUT         : %s\n", input.c_str());
			printf("SUBINPUT      : %s\n", subInput.c_str());
			printf("WEIGHT_DECAY  : %lf\n", wd);
		}
		else if(type == string("POOLING"))
		{
			int size = get_word_int(layers[i], "SIZE");
			int skip = get_word_int(layers[i], "SKIP");
			string non_linearity = get_word_type(layers[i], "NON_LINEARITY");

			m_nonLinearity = new ConfigNonLinearity(non_linearity);

			layer = new ConfigPooling(name, input, subInput, type, size, skip, m_nonLinearity->getValue());

			printf("\n\n********pooling layer********\n");
			printf("NAME          : %s\n", name.c_str());
			printf("INPUT         : %s\n", input.c_str());
			printf("SUBINPUT      : %s\n", subInput.c_str());
			printf("size          : %d\n", size);
			printf("skip          : %d\n", skip);
			printf("non_linearity : %s\n", non_linearity.c_str());
		}
		else if(string("LOCAL") == type){
			int ks = get_word_int(layers[i], "KERNEL_SIZE");
			double initW = get_word_double(layers[i], "initW");
			double wd = get_word_double(layers[i], "WEIGHT_DECAY");
			string non_linearity = get_word_type(layers[i], "NON_LINEARITY");
			std::string initType = get_word_type(layers[i], "initType");
			m_nonLinearity = new ConfigNonLinearity(non_linearity);

			layer = new ConfigLocal(name, input, subInput, type, ks, wd,
				initW, initType, m_nonLinearity->getValue());

			printf("\n\n********local connect layer********\n");
			printf("NAME          : %s\n", name.c_str());
			printf("INPUT         : %s\n", input.c_str());
			printf("SUBINPUT      : %s\n", subInput.c_str());
			printf("KERNEL_SIZE   : %d\n", ks);
			printf("WEIGHT_DECAY  : %lf\n", wd);
			printf("initW         : %lf\n", initW);
			printf("non_linearity : %s\n", non_linearity.c_str());
		}
		else if(string("LRN") == type){
			double lrn_k     = get_word_double(layers[i], "LRN_K");
			int    lrn_n     = get_word_int(layers[i], "LRN_N");
			double lrn_alpha = get_word_double(layers[i], "LRN_ALPHA");
			double lrn_belta = get_word_double(layers[i], "LRN_BALTA");
			string non_linearity = get_word_type(layers[i], "NON_LINEARITY");
			m_nonLinearity = new ConfigNonLinearity(non_linearity);

			layer = new ConfigLRN(name, input, subInput, type, lrn_k, lrn_n, lrn_alpha, lrn_belta, 
				m_nonLinearity->getValue());

			printf("\n\n********local Response Normalization layer********\n");
			printf("NAME          : %s\n", name.c_str());
			printf("INPUT         : %s\n", input.c_str());
			printf("SUBINPUT      : %s\n", subInput.c_str());
			printf("lrn_k         : %lf\n", lrn_k);
			printf("lrn_n         : %d\n",  lrn_n);
			printf("lrn_alpha     : %lf\n", lrn_alpha);
			printf("lrn_belta     : %lf\n", lrn_belta);
			printf("non_linearity : %s\n", non_linearity.c_str());
		}
		else if(type == string("FC"))
		{
			int hn = get_word_int(layers[i], "NUM_FULLCONNECT_NEURONS");
			double wd = get_word_double(layers[i], "WEIGHT_DECAY");
			double drop = get_word_double(layers[i], "DROPCONNECT_RATE");
			double initW= get_word_double(layers[i], "initW");
			string non_linearity = get_word_type(layers[i], "NON_LINEARITY");
			std::string initType = get_word_type(layers[i], "initType");
			m_nonLinearity = new ConfigNonLinearity(non_linearity);

			layer = new ConfigFC(name, input, subInput, type, hn, wd,
				drop, initW, initType, m_nonLinearity->getValue());

			printf("\n\n********Full Connect Layer********\n");
			printf("NAME                    : %s\n", name.c_str());
			printf("INPUT                   : %s\n", input.c_str());
			printf("SUBINPUT                : %s\n", subInput.c_str());
			printf("NUM_FULLCONNECT_NEURONS : %d\n", hn);
			printf("WEIGHT_DECAY            : %lf\n", wd);
			printf("DROPCONNECT_RATE        : %lf\n", drop);
			printf("initW                   : %lf\n", initW);
			printf("non_linearity           : %s\n", non_linearity.c_str());
		}
		else if(type == string("SOFTMAX"))
		{
			int numClasses = get_word_int(layers[i], "NUM_CLASSES");
			double weightDecay = get_word_double(layers[i], "WEIGHT_DECAY");
			double initW= get_word_double(layers[i], "initW");
			string non_linearity = get_word_type(layers[i], "NON_LINEARITY");
			std::string initType = get_word_type(layers[i], "initType");
			m_nonLinearity = new ConfigNonLinearity(non_linearity);
			layer = new ConfigSoftMax(name, input, subInput, type, numClasses, weightDecay, 
				initW, initType, m_nonLinearity->getValue());
			m_classes = numClasses;
			printf("\n\n********SoftMax Layer********\n");
			printf("NAME         : %s\n", name.c_str());
			printf("INPUT        : %s\n", input.c_str());
			printf("SUBINPUT     : %s\n", subInput.c_str());
			printf("NUM_CLASSES  : %d\n", numClasses);
			printf("WEIGHT_DECAY : %lf\n", weightDecay);
			printf("initW        : %lf\n", initW);
			printf("non_linearity: %s\n", non_linearity.c_str());
		}

		insertLayerByName(name, layer);
		if(input == std::string("data")){
			m_firstLayers.push_back(layer);
		}
		else{
			if(layer->m_input == string("NULL")){
				//for(int i = 0; i < layer->m_inputs.size(); i++){
				//	ConfigBase* preLayer = getLayerByName(layer->m_input[i]);
				//	preLayer->m_next.push_back(layer);
				//}
			}
			else{
				ConfigBase* preLayer = getLayerByName(layer->m_input);
				preLayer->m_next.push_back(layer);
			}
		}
	}
}

void Config::init(std::string path)
{
	printf("\n\n*******************CONFIG*******************\n");
	/*read the string from file "Config.txt"*/
	/*delete the comment and spaces*/
	m_configStr = read_2_string(path);
	deleteComment();
	deleteSpace();

	/*IS_GRADIENT_CHECKING*/
	bool is_gradient_checking = get_word_bool(m_configStr, "IS_GRADIENT_CHECKING");
	m_isGrandientChecking = new ConfigGradient(is_gradient_checking);
	printf("Is Grandient Checking : %d\n", is_gradient_checking);

	/*BATCH_SIZE*/
	int batch_size = get_word_int(m_configStr, "BATCH_SIZE");
	m_batchSize = new ConfigBatchSize(batch_size);
	printf("batch Size            : %d\n", batch_size);

	/*CHANNELS*/
	int channels = get_word_int(m_configStr, "CHANNELS");
	m_channels = new ConfigChannels(channels);
	printf("channels              : %d\n", channels);

	/*crop*/
	int crop = get_word_int(m_configStr, "CROP");
	m_crop = new ConfigCrop(crop);
	printf("crop                  : %d\n", crop);

	/*scale*/
	double scale = get_word_double(m_configStr, "SCALE");
	m_scale = new ConfigScale(scale);
	printf("scale                 : %lf\n", scale);

	/*rotation*/
	double rotation = get_word_double(m_configStr, "ROTATION");
	m_rotation = new ConfigRotation(rotation);
	printf("rotation              : %lf\n", rotation);

	/*distortion*/
	double distortion = get_word_double(m_configStr, "DISTORTION");
	m_distortion = new ConfigDistortion(distortion);
	printf("distortion            : %lf\n", distortion);

	/*ImageShow*/
	bool imageShow = get_word_bool(m_configStr, "SHOWIMAGE");
	m_imageShow = new ConfigImageShow(imageShow);
	printf("imageShow             : %d\n", imageShow);

	/*Horizontal*/
	bool horizontal = get_word_bool(m_configStr, "HORIZONTAL");
	m_horizontal = new ConfigHorizontal(horizontal);
	printf("HORIZONTAL            : %d\n", horizontal);

	/*Test Epoch*/
	int test_epoch = get_word_int(m_configStr, "TEST_EPOCH");
	m_test_epoch = new ConfigTestEpoch(test_epoch);
	printf("Test_Epoch            : %d\n", test_epoch);

	/*WHITE_NOISE*/
	double stdev = get_word_double(m_configStr, "WHITE_NOISE");
	m_white_noise = new ConfigWhiteNoise(stdev);
	printf("White Noise           : %f\n", stdev);
	
	/*Layers*/
	get_layers_config(m_configStr);
	printf("\n\n\n");
}

void Config::deleteSpace()
{
	if(m_configStr.empty()) return;
	int i = 0;
	while(1){
		if(i == m_configStr.length()) break;
		if(m_configStr[i] == '\t' || m_configStr[i] == '\n' || m_configStr[i] == ' '){
			m_configStr.erase(m_configStr.begin() + i);
		}else ++ i;
	}
}

void Config::deleteComment()
{
	size_t pos1, pos2;
	do 
	{
		pos1 = m_configStr.find("#");
		if(pos1 == std::string::npos)
			break;
		m_configStr.erase(pos1, 1);
		pos2 = m_configStr.find("#");
		m_configStr.erase(pos1, pos2 - pos1 + 1);
	} while (pos2 != std::string::npos);
}

string 
	Config::read_2_string(string File_name){
		char *pBuf;
		FILE *pFile = NULL;   
		if(!(pFile = fopen(File_name.c_str(),"r"))){
			printf("Can not find this file.");
			return 0;
		}
		//move pointer to the end of the file
		fseek(pFile, 0, SEEK_END);
		//Gets the current position of a file pointer.offset 
		int len = ftell(pFile);
		pBuf = new char[len];
		//Repositions the file pointer to the beginning of a file
		rewind(pFile);
		fread(pBuf, 1, len, pFile);
		fclose(pFile);
		string res = pBuf;
		return res;
}
