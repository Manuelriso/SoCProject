/**
 * @file parallelIzhi.h
 * @brief Header file for neuron structures and function prototypes for the Izhikevich neural network simulation.
 */

 #ifndef NEURON_H
 #define NEURON_H
 
 #include <stdbool.h>
 
 // Neuron structure declaration
 
 /**
  * @brief Number of neurons in the first level.
  */
 #define neuronFirstLevel 7
 
 /**
  * @brief Number of neurons in the second level.
  */
 #define neuronSecondLevel 2
 /*#define num_neuron3rdLevel 12
 #define num_neuron4thLevel 10
 #define num_neuron5thLevel 8
 #define num_neuron6thLevel 6
 #define num_neuron7thLevel 4
 */
 
 /**
  * @brief Number of timesteps for the simulation.
  */
 #define timestep 2
 
 
 /**
  * @brief Structure representing an Izhikevich neuron.
  *
  * This structure holds the state and model parameters for a neuron.
  */
 typedef struct {
     double potential;   // Membrane potential (v)
     double u;           // Recovery variable (u)
     double a;           // Parameter 'a' of Izhikevich
     double b;           // Parameter 'b' of Izhikevich
     double c;           // Reset potential
     double d;           // Recovery increment after spike
     int spiked;         // Spike flag
     int num_inputs;     // Number of inputs
 } Neuron;
 
 /**
  * @brief Structure representing a layer of neurons.
  *
  * This structure contains information about a neuron layer, including the number of neurons,
  * a pointer to the array of neurons, and pointers to the input and output arrays.
  */
 typedef struct {
     int neuronNumber;
     int num_inputs;
     Neuron* neuronLayer;
     int* output;
     int* input;
 } LayerInstanziation;
 
 /* Function prototypes*/
 
 /**
  * @brief Updates the state of a neuron using the Izhikevich model.
  *
  * This function computes the new membrane potential and recovery variable for the neuron
  * and sets the spike flag if the threshold is exceeded.
  *
  * @param n Pointer to the neuron to update.
  * @param numberNeuron Index of the neuron.
  * @param inputNextLayer Pointer to the output array for the next layer.
  */
 //void update_neuron(Neuron* n, int numberNeuron, int* inputNextLayer);
 
 /**
  * @brief Initializes the weights matrix.
  *
  * @param weights 2D array representing the weights.
  * @param rows Number of rows in the weights array.
  * @param columns Number of columns in the weights array.
  */
 //void initializeWeights(int weights[][16], int rows, int columns);
 
 /**
  * @brief Initializes a neuron with specific parameters.
  *
  * This function sets the initial state for a neuron including its membrane potential,
  * threshold, reset value, and number of inputs.
  *
  * @param n Pointer to the neuron to initialize.
  * @param num_neuron Neuron index.
  * @param threshold Threshold value for spiking.
  * @param resetValue Reset potential value after a spike.
  * @param num_inputs Number of input connections.
  * @param tau Time constant (if applicable).
  */
 //void initilizeNeuron(Neuron *n, int num_neuron, double threshold, double resetValue, int num_inputs, double tau);
 
 /**
  * @brief Initializes the output array for a neuron layer.
  *
  * This function sets the output values to zero for all neurons in the layer.
  *
  * @param input_to_the_Layer Pointer to the input array.
  * @param num_neuron_on_the_Level Number of neurons on the level.
  */
 //void init_output(int *input_to_the_Layer, int num_neuron_on_the_Level);
 
 /**
  * @brief Provides verbose output for the neuron layer.
  *
  * This function prints detailed information about the neuron's state for debugging purposes.
  *
  * @param num_neurono_of_the_Level Number of neurons on the level.
  * @param input8thLayer Pointer to the input array for the 8th layer.
  * @param t Timestep identifier.
  */
 //void verbose_output_of_layer(int num_neurono_of_the_Level, int* input8thLayer, int t);
 
 /**
  * @brief Simulates the activity of a layer of neurons.
  *
  * This function updates the state of each neuron in the layer based on its inputs and synaptic weights.
  *
  * @param neurons Pointer to the array of neurons.
  * @param num_neurons Number of neurons in the layer.
  * @param num_inputs Number of input connections per neuron.
  * @param weights 2D array representing synaptic weights.
  * @param input Pointer to the input array.
  * @param inputNextLayer Pointer to the output array for the next layer.
  */
 //void simulate(Neuron* neurons, int num_neurons, int num_inputs, int weights[][num_inputs], int* input, int* inputNextLayer);
 
 /**
  * @brief Initializes the weights for a specific neuron.
  *
  * This function assigns initial weight values for all input connections of a neuron.
  *
  * @param neuron Neuron index.
  * @param input Number of input connections.
  * @param weights 2D array of weights.
  */
 //void initialize_weights(int neuron, int input, int weights[][input]);
 
 #endif // NEURON_H