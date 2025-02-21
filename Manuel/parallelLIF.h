// neuron.h

#ifndef NEURON_H
#define NEURON_H

#include <stdbool.h>

// Neuron structure declaration

// Constants for the number of neurons at each level
#define neuronFirstLevel 10
#define neuronSecondLevel 8
#define neuronThirdLevel 3
/*#define num_neuron3rdLevel 12
#define num_neuron4thLevel 10
#define num_neuron5thLevel 8
#define num_neuron6thLevel 6
#define num_neuron7thLevel 4
*/

#define timestep 2

typedef struct {
    double potential;   // Membrane potential
    double threshold;   // Threshold for spike
    int spiked;         // Spike flag
    double reset;       // Reset value after spike
    int num_inputs;     // Number of inputs
    double tau;         // Time constant for decay (leak)
} Neuron;

typedef struct {
    int neuronNumber;
    int num_inputs;
    Neuron* neuronLayer;
    int* output;
    int* input;
} LayerInstanziation;

// Function prototypes
/*
void update_neuron(Neuron* n, int numberNeuron, int* inputNextLayer);
void initializeWeights(int weights[][16], int rows, int columns);
void initilizeNeuron(Neuron *n, int num_neuron, double threshold, double resetValue, int num_inputs,double tau);
void init_output(int *input_to_the_Layer, int num_neuron_on_the_Level);
void verbose_output_of_layer(int num_neurono_of_the_Level, int* input8thLayer, int t);
void simulate(Neuron* neurons, int num_neurons, int num_inputs, int weights[][num_inputs], int* input, int* inputNextLayer);
void initialize_weights(int neuron, int input, int weights[][input]);
*/

#endif // NEURON_H
