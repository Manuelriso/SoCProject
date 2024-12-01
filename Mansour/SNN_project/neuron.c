#include <stdio.h>
#include <stdlib.h>  // For rand(), srand()
#include <time.h>    // For time()
#include "neuron.h"  // Include the header file

void update_neuron(Neuron* n, int numberNeuron, int* inputNextLayer) {
    if (n->potential >= n->threshold) {
        n->spiked = true;
        inputNextLayer[numberNeuron] = 1;
        n->potential = n->reset;
    } else {
        n->spiked = false;
    }
    printf("Neuron -> %d, potential: %.2f, threshold: %.2f, spiked: %d\n",
           numberNeuron, n->potential, n->threshold, n->spiked);
}

void initializeWeights(int weights[][16], int rows, int columns) {
    srand(time(NULL));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            weights[i][j] = (rand() % 11) - 5;
        }
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

void simulate(Neuron* neurons, int num_neurons, int num_inputs, int weights[][num_inputs], int* input, int* inputNextLayer) {
    for (int i = 0; i < num_neurons; i++) {
        for (int j = 0; j < num_inputs; j++) {
            if (input[j] == 1) {
                neurons[i].potential += weights[i][j];
            }
            update_neuron(&neurons[i], i, inputNextLayer);
        }
    }
}

void init_output(int* input_to_the_Layer, int num_neuron_on_the_Level) {
    for (int j = 0; j < num_neuron_on_the_Level; j++) {
        input_to_the_Layer[j] = 0;
    }
}

void verbose_output_of_layer(int num_neurono_of_the_Level, int* input8thLayer, int t) {
    for (int j = 0; j < num_neurono_of_the_Level; j++) {
        printf("Output from neuron %d: %d, timestep: %d\n", j, input8thLayer[j], t);
    }
}




int main(int argc, char*argv[]){

    //Definition of the two neurla levels
    Neuron firstLevel[num_neuronFirstLevel];
    Neuron secondLevel[num_neuronSecondLevel];
    Neuron neuronsLvl3[num_neuron3rdLevel];
    Neuron neuronsLvl4[num_neuron4thLevel];
    Neuron neuronsLvl5[num_neuron5thLevel];
    Neuron neuronsLvl6[num_neuron6thLevel];
    Neuron neuronsLvl7[num_neuron7thLevel];




    // Define weight matrices according to the defined sizes
    int weightsInputsToFirst[num_neuronFirstLevel][num_neuronFirstLevel];
    int weightsFirstToSecond[num_neuronSecondLevel][num_neuronFirstLevel];
    int weightsSecondToThird[num_neuron3rdLevel][num_neuronSecondLevel];
    int weightsThirdToFourth[num_neuron4thLevel][num_neuron3rdLevel];
    int weightsFourthToFifth[num_neuron5thLevel][num_neuron4thLevel];
    int weightsFifthToSixth[num_neuron6thLevel][num_neuron5thLevel];
    int weightsSixthToSeventh[num_neuron7thLevel][num_neuron6thLevel];

    // Initialize weight matrices
    initializeWeights(weightsInputsToFirst, num_neuronSecondLevel, num_neuronFirstLevel);
    initializeWeights(weightsFirstToSecond, num_neuronSecondLevel, num_neuronFirstLevel);
    initializeWeights(weightsSecondToThird, num_neuron3rdLevel, num_neuronSecondLevel);
    initializeWeights(weightsThirdToFourth, num_neuron4thLevel, num_neuron3rdLevel);
    initializeWeights(weightsFourthToFifth, num_neuron5thLevel, num_neuron4thLevel);
    initializeWeights(weightsFifthToSixth, num_neuron6thLevel, num_neuron5thLevel);
    initializeWeights(weightsSixthToSeventh, num_neuron7thLevel, num_neuron6thLevel);

    ///weight that connect primary inputs with first level neurons. Each row define connection to a neuron
   /*
   / int weightsFirstLevel[num_neuronFirstLevel][4] = {{3, 2, 4, 1},
                                                {4,5,6,4},
                                                {3,10,1,-5},
                                                {4,1,2,-4}};
    
    ///weight that connect outputs of the first layer with second level neurons. Each row define connection to a neuron
    int weightsSecondLevel[num_neuronSecondLevel][num_neuronFirstLevel] = {{0, -3, 3, 8},
                                                                    {0,5,0,0}}; */
    
    //definition of vectors that store outputs of each layer of neurons
    int inputSecondLayer[num_neuronFirstLevel];
    int inputThirdLayer[num_neuronSecondLevel];
    int input4thLayer[num_neuron3rdLevel];
    int input5thLayer[num_neuron4thLevel];
    int input6thLayer[num_neuron5thLevel];
    int input7thLayer[num_neuron6thLevel];
    int input8thLayer[num_neuron7thLevel]; // or the final result of the network because we have 7 layers

    //Initialization of every neuron in the network
    initilizeNeuron(firstLevel,num_neuronFirstLevel,6.0,2.0,num_neuronFirstLevel);
    initilizeNeuron(secondLevel,num_neuronSecondLevel,6.0,2.0,num_neuronFirstLevel);
    initilizeNeuron(neuronsLvl3,num_neuron3rdLevel,6.0,2.0,num_neuronSecondLevel);
    initilizeNeuron(neuronsLvl4,num_neuron4thLevel,6.0,2.0,num_neuron3rdLevel);
    initilizeNeuron(neuronsLvl5,num_neuron5thLevel,6.0,2.0,num_neuron4thLevel);
    initilizeNeuron(neuronsLvl6,num_neuron6thLevel,6.0,2.0,num_neuron5thLevel);
    initilizeNeuron(neuronsLvl7,num_neuron7thLevel,6.0,2.0,num_neuron6thLevel);
    


    //Printf to control everything is okay
    printf("First layer\n");
    for (int i = 0; i < num_neuronFirstLevel; i++) {
        printf("Neuron %d -> potential: %.2f, threshold: %.2f\n",
               i, firstLevel[i].potential, firstLevel[i].threshold);
    }

    printf("Second layer\n");
    for (int i = 0; i < num_neuronSecondLevel; i++) {
        printf("Neuron %d -> potential: %.2f, threshold: %.2f\n",
               i, secondLevel[i].potential, secondLevel[i].threshold);
    }

    printf("3rd layer\n");
    for (int i = 0; i < num_neuron3rdLevel; i++) {
        printf("Neuron %d -> potential: %.2f, threshold: %.2f\n",
               i, neuronsLvl3[i].potential, neuronsLvl3[i].threshold);
    }

    printf("4th layer\n");
    for (int i = 0; i < num_neuron4thLevel; i++) {
        printf("Neuron %d -> potential: %.2f, threshold: %.2f\n",
               i, neuronsLvl4[i].potential, neuronsLvl4[i].threshold);
    }
    printf("5th layer\n");
    for (int i = 0; i < num_neuron5thLevel; i++) {
        printf("Neuron %d -> potential: %.2f, threshold: %.2f\n",
               i, neuronsLvl5[i].potential, neuronsLvl5[i].threshold);
    }
    printf("6th layer\n");
    for (int i = 0; i < num_neuron6thLevel; i++) {
        printf("Neuron %d -> potential: %.2f, threshold: %.2f\n",
               i, neuronsLvl6[i].potential, neuronsLvl6[i].threshold);
    }
    printf("7th layer\n");
    for (int i = 0; i < num_neuron7thLevel; i++) {
        printf("Neuron %d -> potential: %.2f, threshold: %.2f\n",
               i, neuronsLvl7[i].potential, neuronsLvl7[i].threshold);
    }

    //definition of primary inputs, according to # of timesteps
    int input[16][timestep] = {
        {1, 0},
        {0, 0},
        {0, 0},
        {1, 0},
        {1, 0},
        {0, 1},
        {0, 1},
        {1, 1},
        {0, 1},
        {1, 1},
        {0, 0},
        {0, 1},
        {1, 1},
        {0, 1},
        {1, 0},
        {1, 0},
    };

    //Simulation of the network
    for (int t = 0; t < timestep; t++) {
        init_output(inputSecondLayer, num_neuronSecondLevel);
        init_output(inputThirdLayer, num_neuron3rdLevel);
        init_output(input4thLayer, num_neuron4thLevel);
        init_output(input5thLayer, num_neuron5thLevel);
        init_output(input6thLayer, num_neuron6thLevel);
        init_output(input7thLayer, num_neuron6thLevel);
        init_output(input8thLayer, num_neuron7thLevel); 


        printf("\n\n-------------------First layer-----------------------\n\n");
        simulate(firstLevel, num_neuronFirstLevel, num_neuronFirstLevel, weightsInputsToFirst, input[t], inputSecondLayer);
        verbose_output_of_layer(num_neuronFirstLevel,inputSecondLayer,t);


        printf("\n\n-------------------Second layer-----------------------\n\n");
        simulate(secondLevel, num_neuronSecondLevel, num_neuronFirstLevel, weightsFirstToSecond, inputSecondLayer, inputThirdLayer);
        verbose_output_of_layer(num_neuronSecondLevel,inputThirdLayer,t);
        
        printf("\n\n-------------------Third layer-----------------------\n\n");
        simulate(neuronsLvl3, num_neuron3rdLevel, num_neuronSecondLevel, weightsSecondToThird, inputThirdLayer, input4thLayer);
        verbose_output_of_layer(num_neuron3rdLevel,input4thLayer,t);
        
        printf("\n\n-------------------Fourth layer-----------------------\n\n");
        simulate(neuronsLvl4, num_neuron4thLevel, num_neuron3rdLevel, weightsThirdToFourth, input4thLayer, input5thLayer);
        verbose_output_of_layer(num_neuron4thLevel,input5thLayer,t);
        
        printf("\n\n-------------------Fifth layer-----------------------\n\n");
        simulate(neuronsLvl5, num_neuron5thLevel, num_neuron4thLevel, weightsFifthToSixth, input5thLayer, input6thLayer);
        verbose_output_of_layer(num_neuron5thLevel,input6thLayer,t);


        printf("\n\n-------------------Sixth layer-----------------------\n\n");
        simulate(neuronsLvl6, num_neuron6thLevel, num_neuron5thLevel, weightsSixthToSeventh, input6thLayer, input7thLayer);
        verbose_output_of_layer(num_neuron6thLevel,input7thLayer,t);

        printf("\n\n-------------------Seventh layer-----------------------\n\n");
        simulate(neuronsLvl7, num_neuron7thLevel, num_neuron6thLevel, weightsSixthToSeventh, input7thLayer, input8thLayer);  // only the result
        verbose_output_of_layer(num_neuron7thLevel,input8thLayer,t);

    }

    return 0;
}
