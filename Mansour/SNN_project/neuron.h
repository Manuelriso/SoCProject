// neuron.h

#ifndef NEURON_H
#define NEURON_H

#include <stdbool.h>

// Neuron structure declaration
typedef struct {
    double potential;   // Membrane potential
    double threshold;   // Threshold over which we activate the neuron
    bool spiked;        // Flag for the spike
    double reset;
    int num_inputs;
} Neuron;

// Constants for the number of neurons at each level
#define num_neuronFirstLevel 16
#define num_neuronSecondLevel 14
#define num_neuron3rdLevel 12
#define num_neuron4thLevel 10
#define num_neuron5thLevel 8
#define num_neuron6thLevel 6
#define num_neuron7thLevel 4

#define timestep 2

// Function prototypes
void update_neuron(Neuron* n, int numberNeuron, int* inputNextLayer);
void initializeWeights(int weights[][16], int rows, int columns);
void initilizeNeuron(Neuron *n, int num_neuron, double threshold, double resetValue, int num_inputs);
void init_output(int *input_to_the_Layer, int num_neuron_on_the_Level);
void verbose_output_of_layer(int num_neurono_of_the_Level, int* input8thLayer, int t);

#endif // NEURON_H
