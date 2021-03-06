#ifndef __LAYERS_BASE_CU_H__
#define __LAYERS_BASE_CU_H__

#include "../common/cuMatrix.h"
#include "../common/cuMatrixVector.h"


class LayerBase
{
public:
	LayerBase():cost(new cuMatrix<double>(1, 1, 1)){}
	virtual void feedforward() = 0;
	virtual void backpropagation() = 0;
	virtual void getGrad() = 0;
	virtual void updateWeight() = 0;
	virtual void clearMomentum() = 0;
	virtual void save(FILE* file) = 0;
	virtual void initFromCheckpoint(FILE* file) = 0;
	virtual void calCost() = 0;

	virtual cuMatrix<double>* getOutputs() = 0;
	virtual cuMatrix<double>* getCurDelta() = 0;

	virtual void printParameter() = 0;

	double getCost(){
		if(cost != NULL){
			cost->toCpu();
			return cost->get(0, 0, 0);
		}
		return 0.0;
	}
	void printCost(){
		if(cost != NULL){
			cost->toCpu();
			printf("%10s cost = %lf\n", m_name.c_str(), cost->get(0, 0, 0));
		}
	}
	~LayerBase(){
		delete cost;
	}
	std::string m_name;
	std::vector<std::string> m_preLayer;
	cuMatrix<double>* cost;
};

class ConvLayerBase: public LayerBase
{
public:
	int inputDim ;
	int outputDim;
	int inputAmount;
	int outputAmount;
};

class Layers
{
public:
	static Layers* instance(){
		static Layers* layers= new Layers();
		return layers;
	}
	LayerBase* get(std::string name);
	void set(std::string name, LayerBase* layer);

	void setInputs(cuMatrixVector<double>*m_inputs);
	cuMatrixVector<double>* getInputs();

private:
	std::map<std::string, LayerBase*>m_maps;
	cuMatrixVector<double>*m_inputs;
};
#endif
