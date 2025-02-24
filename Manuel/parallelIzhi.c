/** 
 * @file parallelIzhi.c
 * @brief Implementation of a Spiking Neural Network using the Izhikevich model on GAP8.
 * 
 * This program simulates a two-layer Spiking Neural Network using parallel execution
 * on GAP8 cores. It includes neuron instantiation, weight initialization, and 
 * simulation functions for both layers.
 */
#include "pmsis.h" 
#include <stdio.h>
#include "parallelIzhi.h"
#include <math.h>
#include <time.h>
#include <GapBuiltins.h>
/** @brief Array of neurons in the first layer */
Neuron firstLevel[neuronFirstLevel]; 
/** @brief Array of neurons in the second layer */
Neuron secondLevel[neuronSecondLevel];

/**
* Weights matrix for connections within the first level.
* Each row corresponds to a neuron in the first level and each column to an input neuron.
*/
int weightsFirstLevel[neuronFirstLevel][neuronFirstLevel];


/** 
 * Weights matrix for connections from the first level to the second level.
 * Each row corresponds to a neuron in the second level and each column to an input neuron from the first level.
 */
 int weightsSecondLevel[neuronSecondLevel][neuronFirstLevel];

 /** @brief Input array for the first layer */
 int inputFirstLayer[neuronFirstLevel];
 
 /** @brief Input array for the second layer */
 int inputSecondLayer[neuronFirstLevel];
 
 /** @brief Input array for the third layer */
 int inputThirdLayer[neuronSecondLevel];
 
 /** @brief Sample input sequence for neurons */
 int input[neuronFirstLevel][timestep] = {
         {1, 0},
         {0, 0},
         {0, 0},
         {0, 1},
         {0, 0},
         {0, 0},
         {0, 0}
     };

 /**
 * @brief Updates a single neuron's state based on the Izhikevich model.
 * 
 * This function computes the membrane potential and recovery variable for 
 * a neuron using the Izhikevich differential equations. It also determines 
 * whether the neuron has spiked.
 * 
 * @param n Pointer to the neuron to update.
 * @param numberNeuron Index of the neuron in the layer.
 * @param inputNextLayer Pointer to the input array of the next layer.
 * @param current The input current applied to the neuron.
 */
void update_neuron(Neuron* n, int numberNeuron, int* inputNextLayer, double current) {

    //Computation of the Izhikevich differential equations
    double v_old = n->potential; 
    n->potential += 0.04 * v_old * v_old + 5 * v_old + 140 - n->u + current;
    n->u += n->a * (n->b * v_old - n->u);

    if (n->potential >= 30) { // 30mV threshold voltage for Izhikevich
        n->spiked = 1;
        inputNextLayer[numberNeuron] = 1; 
        n->potential = n->c;              // Potential reset
        n->u += n->d;                     //Update of the recovery value
    } else {
        n->spiked = 0;
    }

    // Debugging output
    printf("Neuron -> %d, potential: %.2f, recovery: %.2f, spiked: %d\n",
           numberNeuron, n->potential, n->u, n->spiked);
}


/**
 * @brief Simulates the update of neurons in the first layer.
 *
 * This function iterates over the neurons of the first layer assigned to the current core,
 * computes the input current based on incoming spikes and corresponding weights, and updates
 * the neuron's state by calling update_neuron.
 *
 * @param layer Pointer to the layer instantiation containing neurons, inputs, and outputs.
 * @param core_id Identifier for the current processing core.
 * @param iteration Current iteration number in the simulation.
 * @param num_inputs Number of input connections to be processed.
 */

void simulateFirstLayer(LayerInstanziation* layer,int core_id,int iteration,int num_inputs) {
    int iterationNumber=gap_muls(iteration,8);
    int neuronNumber=gap_addnormu(core_id,iterationNumber,0);
    if(neuronNumber<layer->neuronNumber){
            double input_current=0.0;
            for (int j = 0; j < num_inputs; j++) {
                //printf("%d\n",layer->input[j]);
                if (layer->input[j] == 1) {
                    //printf("ECCOMI");
                    input_current=input_current+weightsFirstLevel[neuronNumber][j];
                }
                //To see better the evolution of neuron, put update_neuron here.
                //update_neuron(&neurons[i], i, inputNextLayer);
            }
            update_neuron(&(layer->neuronLayer[neuronNumber]), neuronNumber, layer->output,input_current);
    }
}

/**
 * @brief Simulates the update of neurons in the second layer.
 *
 * Similar to simulateFirstLayer, this function processes neurons in the second layer assigned to the
 * current core. It calculates the input current from incoming spikes and their associated weights and
 * then updates the neuron's state.
 *
 * @param layer Pointer to the layer instantiation containing neurons, inputs, number of inputs neuron number and outputs.
 * @param core_id Identifier for the current processing core.
 * @param iteration Current iteration number in the simulation.
 * @param num_inputs Number of input connections to be processed.
 */

void simulateSecondLayer(LayerInstanziation* layer,int core_id,int iteration,int num_inputs) {
    int iterationNumber=gap_muls(iteration,8);
    int neuronNumber=gap_addnormu(core_id,iterationNumber,0);
    if(neuronNumber<layer->neuronNumber){
            double input_current=0.0;
            for (int j = 0; j < num_inputs; j++) {
                if (layer->input[j] == 1) {
                    //printf("ECCOMI");
                    input_current=input_current+weightsSecondLevel[neuronNumber][j];
                }
                //To see better the evolution of neuron, put update_neuron here.
                //update_neuron(&neurons[i], i, inputNextLayer);
            }
            update_neuron(&(layer->neuronLayer[neuronNumber]),neuronNumber, layer->output,input_current);
    }
}

/**
 * @brief Initializes a neuron in the given layer.
 *
 * This function sets the initial state of a neuron by initializing its membrane potential,
 * recovery variable, model parameters (a, b, c, d), and the number of inputs. The neuron is
 * selected based on the current core and iteration.
 *
 * @param core_id Identifier of the processing core initializing the neuron.
 * @param a Parameter 'a' of the Izhikevich neuron model.
 * @param b Parameter 'b' of the Izhikevich neuron model.
 * @param c Reset value for the membrane potential after a spike.
 * @param d Increment to the recovery variable after a spike.
 * @param initialPotential Initial membrane potential for the neuron.
 * @param iteration Current iteration number of the simulation.
 * @param layer Pointer to the layer instantiation containing the neuron array.
 * @param num_inputs Number of input connections for the neuron.
 */

void initializeNeuron(int core_id, double a, double b, double c, double d, double initialPotential, int iteration, LayerInstanziation* layer,int num_inputs) {
        int iterationNumber=gap_muls(iteration,8);
        int neuron_index=gap_addnormu(core_id,iterationNumber,0);
        if(core_id+iteration*8< layer->neuronNumber){
            layer->neuronLayer[neuron_index].potential = initialPotential; // initial potential (v)
            layer->neuronLayer[neuron_index].u = b * initialPotential;     // recovery variable (u)
            layer->neuronLayer[neuron_index].a = a;                       // 'a' parameter'
            layer->neuronLayer[neuron_index].b = b;                       // 'b' parameter'
            layer->neuronLayer[neuron_index].c = c;                       // potential reset
            layer->neuronLayer[neuron_index].d = d;                       // increment of u after the spike
            layer->neuronLayer[neuron_index].spiked = false;              // no spike initially
            layer->neuronLayer[neuron_index].num_inputs = num_inputs;             
            // Debugging
            printf("Neuron number %d instanziate by core %d\n", neuron_index, core_id);
            printf("Potential: %f, Recovery: %f, Parameters (a, b, c, d): (%f, %f, %f, %f)\n",
                    layer->neuronLayer[neuron_index].potential, layer->neuronLayer[neuron_index].u,
                    layer->neuronLayer[neuron_index].a, layer->neuronLayer[neuron_index].b,
                    layer->neuronLayer[neuron_index].c, layer->neuronLayer[neuron_index].d);
        }
}


/**
 * @brief Initializes the output for a neuron in the layer.
 *
 * This function sets the output value of a neuron to zero for a given core and iteration.
 *
 * @param layer Pointer to the layer instantiation containing the output array.
 * @param core_id Identifier of the processing core.
 * @param iteration Current iteration number of the simulation.
 */

void init_output( LayerInstanziation* layer,int core_id,int iteration) {
    int iterationNumber=gap_muls(iteration,8);
    int neuronNumber=gap_addnormu(core_id,iterationNumber,0);
    if(neuronNumber<layer->neuronNumber){
        layer->output[neuronNumber]=0;
        //printf("Output fissato a %d da core %d\n",layer->output[core_id],core_id);
    }
}

/**
 * @brief Initializes the weights for the connections of a neuron.
 *
 * This function assigns a weight value for each input connection of a neuron, using a simple
 * function based on the core identifier. The weights matrix is updated for the neuron selected
 * based on the core and iteration.
 *
 * @param layer Pointer to the layer instantiation containing the neuron array.
 * @param core_id Identifier of the processing core.
 * @param iteration Current iteration number of the simulation.
 * @param input Number of input connections for the neuron.
 * @param weights 2D array representing the weights for the neuron's connections.
 */


void initialize_weights(LayerInstanziation* layer, int core_id, int iteration,int input,int weights[][input]){
    int iterationNumber=gap_muls(iteration,8);
    int neuronNumber=gap_addnormu(core_id,iterationNumber,0);
    if(neuronNumber<layer->neuronNumber){
        for(int i=0;i<input;i++){
            int randomInRange = core_id+3;
            /*int random_value = pi_rand();  // PULP function to generate a number on 32 bits.

            //Random value between -5 and 5
            int randomInRange = (random_value % 11) - 5;
            */
            weights[neuronNumber][i]=randomInRange;
            //printf("Weights posizione %d %d fissato a %d\n",core_id,i,randomInRange);
        }
    }
}
 
/**
 * @brief Cluster-level instantiation of neurons.
 *
 * This function is executed by the cluster cores and initializes neurons in the provided layer.
 * It iterates over the neurons (in chunks of 8) and calls the initializeNeuron function with standard
 * Izhikevich parameters.
 *
 * @param layer Pointer to the layer instantiation containing the neurons to be initialized.
 */

void cluster_neuronInstanziation(LayerInstanziation* layer) 
{ 
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();
    uint32_t iteration = 0;
    while(layer->neuronNumber>iteration*8){
        //standard value for Izhikevich  
        initializeNeuron(core_id, 0.02, 0.2, -65.0, 8.0, -65.0, iteration, layer, layer->num_inputs);
        iteration++;
    }
} 

/**
 * @brief Cluster-level instantiation of neuron outputs.
 *
 * This function is executed by the cluster cores and initializes the output array of each neuron in the
 * provided layer to zero. It processes neurons in chunks based on the iteration index.
 *
 * @param layer Pointer to the layer instantiation containing the outputs to be initialized.
 */
void cluster_outputInstanziation(LayerInstanziation* layer) 
{ 
    uint32_t iteration =0;
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id(); 
    while(layer->neuronNumber>iteration*8){ 
        init_output(layer,core_id,iteration);
        iteration++;
    }
} 
/**
 * @brief Cluster-level instantiation of weights for the first layer.
 *
 * This function is executed by the cluster cores and initializes the weights for the neurons in the first layer.
 * It calls the initialize_weights function for each neuron (in chunks of 8) to set up the synaptic weights.
 *
 * @param layer Pointer to the layer instantiation containing the neurons and weights to be initialized.
 */

void cluster_weightsInstanziationFirstLayer(LayerInstanziation* layer) 
{ 
    uint32_t iteration=0;
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();  
    while(layer->neuronNumber>iteration*8){
        initialize_weights(layer,core_id,iteration,layer->num_inputs,weightsFirstLevel);
        iteration++;
    }
} 

/**
 * @brief Cluster-level instantiation of weights for the second layer.
 *
 * This function is executed by the cluster cores and initializes the weights for the neurons in the second layer.
 * It iterates over the neurons (in chunks of 8) and calls the initialize_weights function to set up the synaptic weights.
 *
 * @param layer Pointer to the layer instantiation containing the neurons and weights to be initialized.
 */

void cluster_weightsInstanziationSecondLayer(LayerInstanziation* layer) 
{ 
    uint32_t iteration=0;
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();  
    while(layer->neuronNumber>iteration*8){
        initialize_weights(layer,core_id,iteration,layer->num_inputs,weightsSecondLevel);
        iteration++;
    }
} 

/**
 * @brief Cluster-level simulation of the first layer.
 *
 * This function is executed by the cluster cores and simulates the activity of the first layer of neurons.
 * It iterates over the neurons in chunks of 8 and updates their state based on the inputs and synaptic weights.
 *
 * @param layer Pointer to the layer instantiation containing neurons, inputs, and outputs.
 */

void cluster_simulationFirstLayer(LayerInstanziation* layer) 
{ 
    uint32_t iteration=0;
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();  
    while(layer->neuronNumber>iteration*8){
        //printf("%d\n",layer->input[0]);
        simulateFirstLayer(layer,core_id,iteration,layer->num_inputs);
        iteration++;
    }
} 
/**
 * @brief Cluster-level simulation of the second layer.
 *
 * This function is executed by the cluster cores and simulates the activity of the second layer of neurons.
 * It iterates over the neurons in chunks of 8 and updates their state based on the inputs and synaptic weights.
 *
 * @param layer Pointer to the layer instantiation containing neurons, inputs, and outputs.
 */
void cluster_simulationSecondLayer(LayerInstanziation* layer) 
{ 
    uint32_t iteration=0;
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();  
    while(layer->neuronNumber>iteration*8){
        simulateSecondLayer(layer,core_id,iteration,layer->num_inputs);
        iteration++;
    }
} 


/**
 * @brief Main cluster entry point for neuron instantiation.
 *
 * This function is executed by core 0 and dispatches the neuron instantiation task to all cluster cores.
 * It calls the cluster_neuronInstanziation function on each core to initialize the neurons in the layer.
 *
 * @param layer Pointer to the layer instantiation containing the neurons to be initialized.
 */
  void cluster_delegate(LayerInstanziation* layer) 
 { 
    /* Task dispatch to cluster cores. */ 
    //pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_neuronInstanziation,(neuronNumber,neuronLevel));
    pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_neuronInstanziation,layer);
 } 


 /**
 * @brief Main cluster entry point for output initialization.
 *
 * This function is executed by core 0 and dispatches the output initialization task to all cluster cores.
 * It calls the cluster_outputInstanziation function on each core to initialize the output array of the neurons.
 *
 * @param layer Pointer to the layer instantiation containing the outputs to be initialized.
 */
 void cluster_delegate2(LayerInstanziation* layer) 
 { 
    /* Task dispatch to cluster cores. */ 
    pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_outputInstanziation, layer);
 } 


/**
 * @brief Main cluster entry point for first layer weights instantiation.
 *
 * This function is executed by core 0 and dispatches the weight initialization task for the first layer 
 * to all cluster cores. It calls the cluster_weightsInstanziationFirstLayer function on each core.
 *
 * @param layer Pointer to the layer instantiation containing the neurons and weights to be initialized.
 */
 void cluster_delegate3(LayerInstanziation* layer) 
 { 
    /* Task dispatch to cluster cores. */ 
    pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_weightsInstanziationFirstLayer, layer);
 } 


  /**
 * @brief Main cluster entry point for second layer weights instantiation.
 *
 * This function is executed by core 0 and dispatches the weight initialization task for the second layer 
 * to all cluster cores. It calls the cluster_weightsInstanziationSecondLayer function on each core.
 *
 * @param layer Pointer to the layer instantiation containing the neurons and weights to be initialized.
 */
  void cluster_delegate4(LayerInstanziation* layer) 
 { 
    /* Task dispatch to cluster cores. */ 
    pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_weightsInstanziationSecondLayer, layer);
 } 

 
 /**
 * @brief Main cluster entry point for simulating the first layer.
 *
 * This function is executed by core 0 and dispatches the first layer simulation task to all cluster cores.
 * It calls the cluster_simulationFirstLayer function on each core to update the state of the neurons in the first layer.
 *
 * @param layer Pointer to the layer instantiation containing the neurons, inputs, and outputs.
 */
   void cluster_delegate5(LayerInstanziation* layer) 
 { 
    /* Task dispatch to cluster cores. */ 
    pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_simulationFirstLayer, layer);
 } 


 /**
 * @brief Main cluster entry point for simulating the second layer.
 *
 * This function is executed by core 0 and dispatches the second layer simulation task to all cluster cores.
 * It calls the cluster_simulationSecondLayer function on each core to update the state of the neurons in the second layer.
 *
 * @param layer Pointer to the layer instantiation containing the neurons, inputs, and outputs.
 */
  void cluster_delegate6(LayerInstanziation* layer) 
{  
    /* Task dispatch to cluster cores. */ 
    pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_simulationSecondLayer, layer);
}  


/**
 * @brief Initializes neurons and starts the simulation.
 *
 * This function configures and opens the cluster, instantiates neurons for the first and second layers,
 * and sends tasks to the cluster cores for neuron and weight instantiation. It then enters the simulation loop,
 * where for each timestep, it initializes neuron outputs, assigns inputs to the first layer, and dispatches the
 * simulation tasks for both layers.
 *
 * @note This function is intended to be executed on a cluster by core 0.
 */
 
 void neuronInstanziation(void) 
 { 
    struct pi_device cluster_dev; 
    struct pi_cluster_conf cl_conf; 

    LayerInstanziation secondLayer;
    secondLayer.neuronNumber=neuronSecondLevel;
    secondLayer.neuronLayer=secondLevel;
    secondLayer.output=inputThirdLayer;
    secondLayer.input=inputSecondLayer;
    secondLayer.num_inputs=neuronFirstLevel;

    LayerInstanziation firstLayer;
    firstLayer.neuronNumber=neuronFirstLevel;
    firstLayer.neuronLayer=firstLevel;
    firstLayer.output=inputSecondLayer;
    firstLayer.num_inputs=neuronFirstLevel;

    /* Init cluster configuration structure. */ 
    pi_cluster_conf_init(&cl_conf); 
    cl_conf.id = 0;                /* Set cluster ID. */ 
    /* Configure & open cluster. */ 
    pi_open_from_conf(&cluster_dev, &cl_conf); 
    if (pi_cluster_open(&cluster_dev)) { 
        printf("Cluster open failed !\n"); 
        pmsis_exit(-1); 
    } 
    /* Prepare cluster task and send it to cluster. */ 
    struct pi_cluster_task cl_task; 
    //We can pass only one argument, so the instanziation shoulb be done for every layer?

    //The next three function are called in order to instanziate the first layer of neuron, it can be optimized using a struct
    printf("-------------------FIRST LAYER----------------------\n");
    pi_cluster_send_task_to_cl(&cluster_dev, pi_cluster_task(&cl_task, cluster_delegate, &firstLayer));
    pi_cluster_send_task_to_cl(&cluster_dev, pi_cluster_task(&cl_task, cluster_delegate3, &firstLayer));


    printf("-------------------SECOND LAYER----------------------\n");
    pi_cluster_send_task_to_cl(&cluster_dev, pi_cluster_task(&cl_task, cluster_delegate, &secondLayer));
    pi_cluster_send_task_to_cl(&cluster_dev, pi_cluster_task(&cl_task, cluster_delegate4, &secondLayer));
    printf("End. Your neuron instanziation:\n"); 
    printf("\n\n------------------------Start of the simulation-----------------------\n\n");
    for(int i = 0;i<timestep;i++){
        printf("\n\n------------------------Timestep %d-----------------------\n\n",i);
        //initialize output of the neuron layers
        pi_cluster_send_task_to_cl(&cluster_dev, pi_cluster_task(&cl_task, cluster_delegate2, &firstLayer));
        pi_cluster_send_task_to_cl(&cluster_dev, pi_cluster_task(&cl_task, cluster_delegate2, &secondLayer));
        for(int j=0;j<neuronFirstLevel;j++){
            inputFirstLayer[j]=input[j][i];
            //right assiignment, the input of the first layer is correctly assigned.
        }
        firstLayer.input=inputFirstLayer;
        //Assignement between vector pointers
        printf("\n\n------------------------First layer----------------------\n\n");
        pi_cluster_send_task_to_cl(&cluster_dev, pi_cluster_task(&cl_task, cluster_delegate5, &firstLayer));
        printf("\n\n------------------------Second layer-----------------------\n\n");
        pi_cluster_send_task_to_cl(&cluster_dev, pi_cluster_task(&cl_task, cluster_delegate6, &secondLayer));

    }
    pi_cluster_close(&cluster_dev); 
    pmsis_exit(0); 
 } 
 
 
/**
 * @brief Main entry point of the program.
 *
 * This function prints the startup message and kicks off the execution by calling the
 * PMSIS kickoff function with neuronInstanziation as the entry function.
 *
 * @return int Returns the status code from pmsis_kickoff.
 */
 int main(void) 
 { 
    printf("\n\n\t *** Neuron instanziation ***\n\n"); 
    return pmsis_kickoff((void *) neuronInstanziation); 
 }
