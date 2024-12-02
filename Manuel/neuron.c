#include <stdio.h>
#include "neuron.h";


void update_neuron(Neuron* n,int numberNeuron,int* inputNextLayer) {
    // I control the thershold
    if (n->potential >= n->threshold) {
        n->spiked = 1;
        inputNextLayer[numberNeuron]=1;        
        n->potential = n->reset;     
    } else {
        n->spiked = 0;        
    }
    printf("Neuron -> %d ,potential: %.2f, threshold: %.2f, spiked: %d\n",
                numberNeuron,n->potential, n->threshold,n->spiked);

}


/*
void simulate(Neuron* neurons, int num_neurons, int weights[neuronFirstLevel][4] ,int* input,int* inputNextLayer) {
    for (int i=0;i<num_neurons;i++){
        for (int j=0;j<neurons[i].num_inputs;j++){
            if(input[j]==1)
                neurons[i].potential = neurons[i].potential + weights[i][j];
                update_neuron(&neurons[i],i,inputNextLayer);
        }
    }
    return;
}


void simulateSecondLayer(Neuron* neurons, int num_neurons, int weights[][neuronFirstLevel],int* input ,int* inputNextLayer) {
    for (int i=0;i<num_neurons;i++){
        for (int j=0;j<neurons[i].num_inputs;j++){
            if(input[j]==1)
                neurons[i].potential = neurons[i].potential + weights[i][j];
                update_neuron(&neurons[i],i,inputNextLayer);
        }
    }
    return;
}
*/


void simulate(Neuron* neurons, int num_neurons, int num_inputs, int weights[][num_inputs], int* input, int* inputNextLayer) {
    for (int i = 0; i < num_neurons; i++) {
        for (int j = 0; j < num_inputs; j++) {
            if (input[j] == 1) {
                neurons[i].potential += weights[i][j];
            }
            //To see better the evolution of neuron, put update_neuron here.
            update_neuron(&neurons[i], i, inputNextLayer);
        }
        //update_neuron(&neurons[i], i, inputNextLayer);
    }
}


void initilizeNeuron(Neuron* n, int num_neuron, double threshold, double resetValue, int num_inputs) {
    for (int i = 0; i < num_neuron; i++) {
        n[i].potential = 0.0;
        n[i].threshold = threshold;
        n[i].spiked = false;
        n[i].reset = resetValue;
        n[i].num_inputs = num_inputs;
    }
}


void init_output(int* input_to_the_Layer, int num_neuron_on_the_Level) {
    for (int j = 0; j < num_neuron_on_the_Level; j++) {
        input_to_the_Layer[j] = 0;
    }
}



int main(int argc, char*argv[]){

    //Definition of the two neurla levels
    Neuron firstLevel[neuronFirstLevel];
    Neuron secondLevel[neuronSecondLevel];

    //weight that connect primary inputs with first level neurons. Each row define connection to a neuron
    int weightsFirstLevel[neuronFirstLevel][4] = {{3, 2, 4, 1},
                                                {4,5,6,4},
                                                {3,10,1,-5},
                                                {4,1,2,-4}};
    
    //weight that connect outputs of the first layer with second level neurons. Each row define connection to a neuron
    int weightsSecondLevel[neuronSecondLevel][neuronFirstLevel] = {{0, -3, 3, 8},
                                                                    {0,5,0,0}};
    

    //definition of vectors that store outputs of each layer of neurons
    int inputSecondLayer[neuronFirstLevel];
    int inputThirdLayer[neuronSecondLevel];


    //Initialization of every neuron in the network
    initilizeNeuron(firstLevel,neuronFirstLevel,6.0,2.0,neuronFirstLevel);
    initilizeNeuron(secondLevel,neuronSecondLevel,6.0,2.0,neuronFirstLevel);




    //Printf to control everything is okay
    printf("First layer\n");
    for (int i = 0; i < neuronFirstLevel; i++) {
        printf("Neuron %d -> potential: %.2f, threshold: %.2f\n",
               i, firstLevel[i].potential, firstLevel[i].threshold);
    }

    printf("Second layer\n");
    for (int i = 0; i < neuronSecondLevel; i++) {
        printf("Neuron %d -> potential: %.2f, threshold: %.2f\n",
               i, secondLevel[i].potential, secondLevel[i].threshold);
    }

    //definition of primary inputs, according to # of timesteps
    int input[neuronFirstLevel][timestep] = {
        {1, 0},
        {0, 0},
        {0, 0},
        {1, 1}
    };

    //Simulation of the network
    for (int i = 0; i < timestep; i++) {

        //We initialize, in every timestep, all outputs of neurons to 0.
        init_output(inputSecondLayer, neuronFirstLevel);
        init_output(inputThirdLayer, neuronSecondLevel);

        //Simulation of the first layer, we pass all neurons of the layer, number of neurons of the layer, matrix of the weights, primary inputs and the vector to define neuron outputs
        printf("\n\n-------------------First layer-----------------------\n\n");
        //I'm passing to the simulate function the array related to the first colomn of the input,so inputs of the first timestep
        simulate(firstLevel,neuronFirstLevel,4,weightsFirstLevel,(int[]){input[0][i], input[1][i], input[2][i], input[3][i]},inputSecondLayer);
        for(int j=0;j<neuronFirstLevel;j++){
            printf("Input second layer from neuron %d : %d\n",j,inputSecondLayer[j]);
        }

        printf("\n\n-------------------Second layer-----------------------\n\n");
        simulate(secondLevel,neuronSecondLevel,neuronFirstLevel,weightsSecondLevel,inputSecondLayer, inputThirdLayer);
        for(int j=0;j<neuronSecondLevel;j++){
            printf("Output from neuron %d : %d, timestep : %d\n",j,inputThirdLayer[j],i);
        }
    }

    return 0;
}