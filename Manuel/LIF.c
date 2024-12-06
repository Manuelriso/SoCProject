#include <stdio.h>
#include "LIF.h"
#include <math.h>
#include <time.h>

void update_neuron(Neuron* n, int numberNeuron, int* inputNextLayer) {
    // Apply decay to the membrane potential
    n->potential *= exp(-1.0 / n->tau); // Assuming timestep size of 1
    
    // Check if the neuron spiked
    if (n->potential >= n->threshold) {
        n->spiked = 1;
        inputNextLayer[numberNeuron] = 1;        
        n->potential = n->reset;  // Reset potential after spike
    } else {
        n->spiked = 0;        
    }

    printf("Neuron -> %d, potential: %.2f, threshold: %.2f, spiked: %d\n",
                numberNeuron, n->potential, n->threshold, n->spiked);
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
            //update_neuron(&neurons[i], i, inputNextLayer);
        }
        update_neuron(&neurons[i], i, inputNextLayer);
    }
}


void initilizeNeuron(Neuron* n, int num_neuron, double threshold, double resetValue, int num_inputs, double tau) {
    for (int i = 0; i < num_neuron; i++) {
        n[i].potential = 0.0;
        n[i].threshold = threshold;
        n[i].spiked = false;
        n[i].reset = resetValue;
        n[i].num_inputs = num_inputs;
        n[i].tau = tau; // Set the time constant
    }
}



void init_output(int* input_to_the_Layer, int num_neuron_on_the_Level) {
    for (int j = 0; j < num_neuron_on_the_Level; j++) {
        input_to_the_Layer[j] = 0;
    }
}

void initialize_weights(int neuron, int input, int weights[][input]){
    srand(NULL);
    for(int i=0;i<neuron;i++){
        for(int j=0;j<input;j++){ 
            int min = -5;
            int max = +10;

            // Generate random number between min and max
            int randomInRange = min + rand() % (max - min + 1);
            weights[j][i]=randomInRange;
        }
    }
}



int main(int argc, char*argv[]){

    //Definition of the two neurla levels
    Neuron firstLevel[neuronFirstLevel];
    Neuron secondLevel[neuronSecondLevel];

    int weightsFirstLevel[neuronFirstLevel][neuronFirstLevel];
    int weightsSecondLevel[neuronSecondLevel][neuronFirstLevel];

    initialize_weights(neuronFirstLevel,neuronFirstLevel,weightsFirstLevel);

    initialize_weights(neuronSecondLevel,neuronFirstLevel,weightsSecondLevel);

    //weight that connect primary inputs with first level neurons. Each row define connection to a neuron
    /*int weightsFirstLevel[neuronFirstLevel][neuronFirstLevel] = {{3, 2, 4, 1,4,6,8,9},
                                                                {4,5,6,6,0,2,0,-3},
                                                                {3,10,1,-5,-3,-2,1,4},
                                                                {0,10,1,-5,-3,-2,1,4},
                                                                {3,10,1,-5,-3,2,1,-4},
                                                                {3,0,1,-5,3,-2,-1,4},
                                                                {3,10,-1,0,3,-2,1,4},
                                                                {4,1,2,-4,2,6,4,7}};
    
    //weight that connect outputs of the first layer with second level neurons. Each row define connection to a neuron
    int weightsSecondLevel[neuronSecondLevel][neuronFirstLevel] = {{0, -3, 3, 8,0,2,4,5},
                                                                    {0,5,0,0,2,8,-3,6}};
    */
    

    //definition of vectors that store outputs of each layer of neurons
    int inputSecondLayer[neuronFirstLevel];
    int inputThirdLayer[neuronSecondLevel];


    //Initialization of every neuron in the network
    initilizeNeuron(firstLevel, neuronFirstLevel, 10.0, 2.0, neuronFirstLevel, 10.0); // Example tau = 10.0
    initilizeNeuron(secondLevel, neuronSecondLevel, 10.0, 2.0, neuronFirstLevel, 10.0);





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
        {1, 1},
        {0, 0},
        {1, 1},
        {0, 1},
        {1, 0}
    };

    //Simulation of the network
    for (int i = 0; i < timestep; i++) {

        //We initialize, in every timestep, all outputs of neurons to 0.
        init_output(inputSecondLayer, neuronFirstLevel);
        init_output(inputThirdLayer, neuronSecondLevel);

        //Simulation of the first layer, we pass all neurons of the layer, number of neurons of the layer, matrix of the weights, primary inputs and the vector to define neuron outputs
        printf("\n\n-------------------First layer-----------------------\n\n");
        //I'm passing to the simulate function the array related to the first colomn of the input,so inputs of the first timestep
        simulate(firstLevel,neuronFirstLevel,neuronFirstLevel,weightsFirstLevel,(int[]){input[0][i], input[1][i], input[2][i], input[3][i]},inputSecondLayer);
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