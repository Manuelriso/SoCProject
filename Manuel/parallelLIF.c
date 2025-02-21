/* PMSIS includes */ 
#include "pmsis.h" 
#include <stdio.h>
#include "parallelLIF.h"
#include <math.h>
#include <time.h>
#include <GapBuiltins.h>


//Instanziation of the different layers of our network

Neuron firstLevel[neuronFirstLevel]; 
Neuron secondLevel[neuronSecondLevel];
Neuron thirdLevel[neuronThirdLevel];


/*Instanziation of the weights of the fully connected network. For every neuron, we have n input
and m output, where n is the number of neuron of the previous layer, m is the number of neuron of the 
previous layer*/
int weightsFirstLevel[neuronFirstLevel][neuronFirstLevel];
int weightsSecondLevel[neuronSecondLevel][neuronFirstLevel];
int weightsThirdLevel[neuronThirdLevel][neuronSecondLevel];


//Here we define the input/output for every neuron in eache layer.
int inputFirstLayer[neuronFirstLevel];
int inputSecondLayer[neuronFirstLevel];
int inputThirdLayer[neuronSecondLevel];
int inputFourthLayer[neuronThirdLevel];


//Here we define the train of input of the network
int input[neuronFirstLevel][timestep] = {
        {1, 0},
        {0, 0},
        {0, 0},
        {0, 1},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 1},
        {1, 0},
        {0, 0},
    };


/**
* @brief Update of the neuron.
*
* The update neuron function is called every time we are simulating the entire network.
In this case, we will update the spike of the neuron, checking if, after a timestep, 
the potential has increased over the threshold.
We will generate some spikes, and we have also the exponential decreasing of the potential during the time.
*
* @param n Is the pointer to a neuron array 
* @param numberNeuron Is the number of the neuron in a specific layer
* @param inputNextLayer It's the pointer to the vector representing the "output" of a layer.For every layer, the output of each neuron is equal to 1 if we reach the threshold voltage
*/

 void update_neuron(Neuron* n, int numberNeuron, int* inputNextLayer) {
    // Apply decay to the membrane potential
    n->potential=n->reset+(n->potential-n->reset)*exp(-1.0 / n->tau);
    //n->potential *= exp(-1.0 / n->tau); // Assuming timestep size of 1
    
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




/**
* @brief Simulation of the first layer.
*
* Here we are simulating the first layer.
We'd like to simulate every layer with the same function, 
but we have some problem with the passing argument of the weights, and so we do it manually.
We'd like to insert it in the LayerInstanziation struct, but for now, we're not able to do it.
However, what we do is to increase the potential of each neuron if a specif input of the neuron is equal to 1.
j represent the number of input that each neuron will receive from the previous layer.
For example, weightsFirstLevel[2][3] is the weight of the connection that connect 
the second neuron of this layer with the third neuron of the previous layer.
The same is done for all other layers.
*
* @param layer It is the layer that we are simulating
* @param core_id The core (0-7) on which is executed the function
* @param iteration It is the number of time that the function has been executed on the parallel cores
* @param num_inputs is the number of inputs that each neuron receives(it can be 0 or 1)
*/


void simulateFirstLayer(LayerInstanziation* layer,int core_id,int iteration,int num_inputs) {
    int iterationNumber=gap_muls(iteration,8);
    int neuronNumber=gap_addnormu(core_id,iterationNumber,0);
    if(neuronNumber<layer->neuronNumber){
            for (int j = 0; j < num_inputs; j++) {
                //printf("%d\n",layer->input[j]);
                if (layer->input[j] == 1) {
                    //printf("ECCOMI");
                    //layer->neuronLayer[core_id+iteration*8].potential = gap_add2((layer->neuronLayer[core_id+iteration*8].potential),(weightsFirstLevel[core_id+iteration*8][j]));
                    layer->neuronLayer[neuronNumber].potential += weightsFirstLevel[neuronNumber][j];
                }
                //To see better the evolution of neuron, put update_neuron here.
                //update_neuron(&neurons[i], i, inputNextLayer);
            }
            update_neuron(&(layer->neuronLayer[neuronNumber]), neuronNumber, layer->output);
    }
}



/**
* @brief Simulation of the second layer.
*
* Here we are simulating the second layer.
We'd like to simulate every layer with the same function, 
but we have some problem with the passing argument of the weights, and so we do it manually.
We'd like to insert it in the LayerInstanziation struct, but for now, we're not able to do it.
However, what we do is to increase the potential of each neuron if a specif input of the neuron is equal to 1.
j represent the number of input that each neuron will receive from the previous layer.
For example, weightsSecondLevel[2][3] is the weight of the connection that connect 
the second neuron of this layer with the third neuron of the previous layer.
The same is done for all other layers.
*
* @param layer It is the layer that we are simulating
* @param core_id The core (0-7) on which is executed the function
* @param iteration It is the number of time that the function has been executed on the parallel cores
* @param num_inputs is the number of inputs that each neuron receives(it can be 0 or 1)
*/


void simulateSecondLayer(LayerInstanziation* layer,int core_id,int iteration,int num_inputs) {
    int iterationNumber=gap_muls(iteration,8);
    int neuronNumber=gap_addnormu(core_id,iterationNumber,0);
    if(neuronNumber<layer->neuronNumber){
            for (int j = 0; j < num_inputs; j++) {
                if (layer->input[j] == 1) {
                    //printf("ECCOMI");
                    layer->neuronLayer[neuronNumber].potential += weightsSecondLevel[neuronNumber][j];
                }
                //To see better the evolution of neuron, put update_neuron here.
                //update_neuron(&neurons[i], i, inputNextLayer);
            }
            update_neuron(&(layer->neuronLayer[neuronNumber]), neuronNumber, layer->output);
    }
}




/**
* @brief Simulation of the third layer.
*
* Here we are simulating the third layer.
We'd like to simulate every layer with the same function, 
but we have some problem with the passing argument of the weights, and so we do it manually.
We'd like to insert it in the LayerInstanziation struct, but for now, we're not able to do it.
However, what we do is to increase the potential of each neuron if a specif input of the neuron is equal to 1.
j represent the number of input that each neuron will receive from the previous layer.
For example, weightsThirdLevel[2][3] is the weight of the connection that connect 
the second neuron of this layer with the third neuron of the previous layer.
The same is done for all other layers.
*
* @param layer It is the layer that we are simulating
* @param core_id The core (0-7) on which is executed the function
* @param iteration It is the number of time that the function has been executed on the parallel cores
* @param num_inputs is the number of inputs that each neuron receives(it can be 0 or 1)
*/

void simulateThirdLayer(LayerInstanziation* layer,int core_id,int iteration,int num_inputs) {
    int iterationNumber=gap_muls(iteration,8);
    int neuronNumber=gap_addnormu(core_id,iterationNumber,0);
    if(neuronNumber<layer->neuronNumber){
            for (int j = 0; j < num_inputs; j++) {
                if (layer->input[j] == 1) {
                    //printf("ECCOMI");
                    layer->neuronLayer[neuronNumber].potential += weightsThirdLevel[neuronNumber][j];
                }
                //To see better the evolution of neuron, put update_neuron here.
                //update_neuron(&neurons[i], i, inputNextLayer);
            }
            update_neuron(&(layer->neuronLayer[neuronNumber]), neuronNumber, layer->output);
    }
}


/*
void simulateLayer(LayerInstanziation* layer,int core_id,int iteration,int num_inputs) {
    int iterationNumber=gap_muls(iteration,8);
    int neuronNumber=gap_addnormu(core_id,iterationNumber,0);
    if(neuronNumber<layer->neuronNumber){
            for (int j = 0; j < num_inputs; j++) {
                if (layer->input[j] == 1) {
                    //printf("ECCOMI");
                    layer->neuronLayer[neuronNumber].potential += layer->weights[neuronNumber][j];
                }
                //To see better the evolution of neuron, put update_neuron here.
                //update_neuron(&neurons[i], i, inputNextLayer);
            }
            update_neuron(&(layer->neuronLayer[neuronNumber]), neuronNumber, layer->output);
    }
}
*/




/**
* @brief Output instanziation of a layer.
*
* This is the inizialization of every neuron.
We perform the inizialization with standard value of the LIF literature, 
and we do it for every neuron and for every layer of the network.
We use some GAP SDK function in order to optimize the computation of the neuron's index to be updated. 
We perform 8 inizialization at a time, one on every core.
*
* @param core_id The core (0-7) on which is executed the function
* @param threshold Thresold voltage of each neuron for the LIF model
* @param resetValue Reset voltage of each neuron for the LIF model
* @param num_inputs Number of inputs of each neuron
* @param tau A discharge component for the LIF model
* @param iteration It is the number of time that the function has been executed on the parallel cores
* @param layer It is the layer that we are instanziating
*/

void initializeNeuron(int core_id, double threshold, double resetValue, int num_inputs, double tau,int iteration,LayerInstanziation* layer) {
        int iterationNumber=gap_muls(iteration,8);
        int neuronNumber=gap_addnormu(core_id,iterationNumber,0);
        if(neuronNumber< layer->neuronNumber){
            //We use GAP efficient mul function
            layer->neuronLayer[neuronNumber].potential = -65.0;
            layer->neuronLayer[neuronNumber].threshold = threshold;
            layer->neuronLayer[neuronNumber].spiked = false;
            layer->neuronLayer[neuronNumber].reset = resetValue;
            layer->neuronLayer[neuronNumber].num_inputs = num_inputs;
            layer->neuronLayer[neuronNumber].tau = tau; // Set the time constant
            printf("Neuron number %d instanziate by core %d\n",neuronNumber,core_id);
            printf("Potential : %f\nThresold : %f\n",layer->neuronLayer[neuronNumber].potential,layer->neuronLayer[neuronNumber].threshold);
        }
}



/**
* @brief Output instanziation of a layer.
*
* In this function we're initializing all outputs of a layer of the network,
 that will be ever equal to 0 initially.
We use some GAP_SDK function to compute the index of the neuron
*
* @param layer The layer to which initialize all outputs.
* @param core_id The core (0-7) on which is executed the function
* @param iteration It is the number of time that the function has been executed on the parallel cores
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
* @brief Weights instanziation of a layer.
*
* Here we initialize all weights. We're considering a fully connected nettwork, so for every neuron, 
we will have N inputs and M outputs connection.
N is the number of neuron of the previous layer, M is the number of neuron of the next layer.
In this case, the matrix weights has dimension p x N, where p is the number of neuron of the layer
*
* @param layer The layer to which initialize all weights with the previous layer of the network.
* @param core_id The core (0-7) on which is executed the function
* @param iteration It is the number of time that the function has been executed on the parallel cores
* @param input It's the number of input connections of each neuron, equal to the number of neuron of the previous layer(fully connected network)
* @param weights Matrix of the weights representing that layer.
*/

void initialize_weights(LayerInstanziation* layer, int core_id, int iteration,int input,int weights[][input]){
    int iterationNumber=gap_muls(iteration,8);
    int neuronNumber=gap_addnormu(core_id,iterationNumber,0);
    if(neuronNumber<layer->neuronNumber){
        for(int i=0;i<input;i++){
            int randomInRange = core_id;
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
* @brief Weights instanziation of the third layer.
*
* This function takes one layer and will run the instanziation of every neuron of the layer.
This function will be executed on the eight parallel cores, and will stop only when all the neuron 
are instanziated correctly. 
We call for every neuron the initializeNeuron function.
*
* @param layer The layer to simulate, in this case the third one.
*/
  
void cluster_neuronInstanziation(LayerInstanziation* layer) 
{ 
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();
    uint32_t iteration = 0;
    int iterationNumber=gap_muls(iteration,8);
    while(layer->neuronNumber>iterationNumber){
        //standard value for LIF  
        initializeNeuron(core_id,-50.0, -65.0, layer->num_inputs, 10.0,iteration,layer);
        iteration++;
        iterationNumber=gap_muls(iteration,8);
    }
} 




/**
* @brief Output instanziation of the layer.
*
* This function takes one layer and will run the instanziation of the output for every neuron of the layer.
We're going to instanziate all outputs of each neuron to 0.
This function will be executed on the eight parallel cores, and will stop only when all the neuron and their output
are instanziated correctly. 
We call for every neuron the init_output function.
*
* @param layer The layer to simulate, in this case the third one.
*/

void cluster_outputInstanziation(LayerInstanziation* layer) 
{ 
    uint32_t iteration =0;
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id(); 
    uint32_t iterationNumber=gap_muls(iteration,8);
    while(layer->neuronNumber>iterationNumber){ 
        init_output(layer,core_id,iteration);
        iteration++;
        iterationNumber=gap_muls(iteration,8);
    }
} 



/**
* @brief Weights instanziation of the first layer.
*
* This function takes the first layer and will run the instanziation of the weights for every neuron of the layer.
We're simulating a fully connected network, so for every neuron, we need to define the weights of the connection
with every neuron of the previous layer. In the case of the first layer, we need to define the weights that
connect every neuron with the primary inputs.
This function will be executed on the eight parallel cores, and will stop only when all the neuron and their weights
are instanziated correctly.
We call for every neuron the initialize_weights function.
*
* @param layer The layer to simulate, in this case the first one.
*/

void cluster_weightsInstanziationFirstLayer(LayerInstanziation* layer) 
{ 
    uint32_t iteration=0;
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id(); 
    int iterationNumber=gap_muls(iteration,8); 
    while(layer->neuronNumber>iterationNumber){
        initialize_weights(layer,core_id,iteration,layer->num_inputs,weightsFirstLevel);
        iteration++;
        iterationNumber=gap_muls(iteration,8);
    }
} 



/**
* @brief Weights instanziation of the second layer.
*
* This function takes the second layer and will run the instanziation of the weights for every neuron of the layer.
We're simulating a fully connected network, so for every neuron, we need to define the weights of the connection
with every neuron of the previous layer
This function will be executed on the eight parallel cores, and will stop only when all the neuron and their weights
are instanziated correctly. 
We call for every neuron the initialize_weights function.
*
* @param layer The layer to simulate, in this case the second one.
*/


void cluster_weightsInstanziationSecondLayer(LayerInstanziation* layer) 
{ 
    uint32_t iteration=0;
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id(); 
    int iterationNumber=gap_muls(iteration,8); 
    while(layer->neuronNumber>iterationNumber){
        initialize_weights(layer,core_id,iteration,layer->num_inputs,weightsSecondLevel);
        iteration++;
        iterationNumber=gap_muls(iteration,8);
    }
} 


/**
* @brief Weights instanziation of the third layer.
*
* This function takes the third layer and will run the instanziation of the weights for every neuron of the layer.
We're simulating a fully connected network, so for every neuron, we need to define the weights of the connection
with every neuron of the previous layer
This function will be executed on the eight parallel cores, and will stop only when all the neuron and their weights
are instanziated correctly. 
We call for every neuron the initialize_weights function.
*
* @param layer The layer to simulate, in this case the third one.
*/

void cluster_weightsInstanziationThirdLayer(LayerInstanziation* layer) 
{ 
    uint32_t iteration=0;
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id(); 
    int iterationNumber=gap_muls(iteration,8); 
    while(layer->neuronNumber>iterationNumber){
        initialize_weights(layer,core_id,iteration,layer->num_inputs,weightsThirdLevel);
        iteration++;
        iterationNumber=gap_muls(iteration,8);
    }
} 



/**
* @brief Simulation of the first layer.
*
* This function takes the first layer and will run the simulation for every neuron of the layer.
This function will be executed on the eight parallel cores, and will stop only when all the neurons 
of the layer are simulated correctly.
We call for every neuron the simulateFirstLayer function.
*
* @param layer The layer to simulate, in this case the first one.
*/


void cluster_simulationFirstLayer(LayerInstanziation* layer) 
{ 
    uint32_t iteration=0;
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();  
    int iterationNumber=gap_muls(iteration,8);
    while(layer->neuronNumber>iterationNumber){
        //printf("%d\n",layer->input[0]);
        simulateFirstLayer(layer,core_id,iteration,layer->num_inputs);
        iteration++;
        iterationNumber=gap_muls(iteration,8);
    }
} 



/**
* @brief Simulation of the second layer.
*
* This function takes the second layer and will run the simulation for every neuron of the layer.
This function will be executed on the eight parallel cores, and will stop only when all the neurons 
of the layer are simulated correctly.
We call for every neuron the simulateSecondLayer function.
*
* @param layer The layer to simulate, in this case the second one.
*/

void cluster_simulationSecondLayer(LayerInstanziation* layer) 
{ 
    uint32_t iteration=0;
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id(); 
    int iterationNumber=gap_muls(iteration,8); 
    while(layer->neuronNumber>iterationNumber){
        simulateSecondLayer(layer,core_id,iteration,layer->num_inputs);
        iteration++;
        iterationNumber=gap_muls(iteration,8);
    }
} 




 
/**
* @brief Simulation of the third layer.
*
* This function takes the third layer and will run the simulation for every neuron of the layer.
This function will be executed on the eight parallel cores, and will stop only when all the neurons 
of the layer are simulated correctly. 
We call for every neuron the simulateThirdLayer function.
*
* @param layer The layer to simulate, in this case the third one.
*/

void cluster_simulationThirdLayer(LayerInstanziation* layer) 
{ 
    uint32_t iteration=0;
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id(); 
    int iterationNumber=gap_muls(iteration,8); 
    while(layer->neuronNumber>iterationNumber){
        simulateThirdLayer(layer,core_id,iteration,layer->num_inputs);
        iteration++;
        iterationNumber=gap_muls(iteration,8);
    }
} 



 
/**
* @brief cluster delegation of the neuron instanziation.
*
* This function takes one layer, and calling the cluster_neuronInstanziation function, it will
send to the core cluster the request to initialize all the neurons of the specified layer.
*
* @param layer The layer of the network to initialize.
*/

 void cluster_delegate(LayerInstanziation* layer) 
 { 
    /* Task dispatch to cluster cores. */ 
    pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_neuronInstanziation,layer);
 } 




 /**
* @brief cluster delegation of the output instanziation.
*
* This function takes one layer, and calling the cluster_outputInstanziation function, it will
send to the core cluster the request to initialize all the output of the specified layer.
*
* @param layer The layer of the network to initialize.
*/

 void cluster_delegate2(LayerInstanziation* layer) 
 { 
    /* Task dispatch to cluster cores. */ 
    pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_outputInstanziation, layer);
 } 




 /**
* @brief cluster delegation of the first layer's weights instanziation.
*
* This function takes the first layer, and calling the cluster_weightsInstanziationFirstLayer function, it will
send to the core cluster the request to initialize all the weights of the first layer.
*
* @param layer The layer of the network to initialize.
*/

 void cluster_delegate3(LayerInstanziation* layer) 
 { 
    /* Task dispatch to cluster cores. */ 
    pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_weightsInstanziationFirstLayer, layer);
 } 


 /**
* @brief cluster delegation of the second layer's weights instanziation.
*
* This function takes the second layer, and calling the cluster_weightsInstanziationSecondLayer function, it will
send to the core cluster the request to initialize all the weights of the second layer.
*
* @param layer The layer of the network to initialize.
*/

  void cluster_delegate4(LayerInstanziation* layer) 
 { 
    /* Task dispatch to cluster cores. */ 
    pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_weightsInstanziationSecondLayer, layer);
 } 




 /**
* @brief cluster delegation of the third layer's weights instanziation.
*
* This function takes the third layer, and calling the cluster_weightsInstanziationThirdLayer function, it will
send to the core cluster the request to initialize all the weights of the third layer.
*
* @param layer The layer of the network to initialize.
*/

   void cluster_delegate5(LayerInstanziation* layer) 
 { 
    /* Task dispatch to cluster cores. */ 
    pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_weightsInstanziationThirdLayer, layer);
 } 




 /**
* @brief cluster delegation of the first layer simulation.
*
* This function takes the first layer, and calling the cluster_simulationFirstLayer function, it will
send to the core cluster the request to simulate all the neurons of the specified layer.
*
* @param layer The layer of the network to simulate.
*/

   void cluster_delegate6(LayerInstanziation* layer) 
 { 
    /* Task dispatch to cluster cores. */ 
    pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_simulationFirstLayer, layer);
 } 





 /**
* @brief cluster delegation of the second layer simulation.
*
* This function takes the second layer, and calling the cluster_simulationSecondLayer function, it will
send to the core cluster the request to simulate all the neurons of the specified layer.
*
* @param layer The layer of the network to simulate.
*/

   void cluster_delegate7(LayerInstanziation* layer) 
 { 
    /* Task dispatch to cluster cores. */ 
    pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_simulationSecondLayer, layer);
 } 




 
 /**
* @brief cluster delegation of the third layer simulation.
*
* This function takes the third layer, and calling the cluster_simulationThirdLayer function, it will
send to the core cluster the request to simulate all the neurons of the specified layer.
*
* @param layer The layer of the network to simulate.
*/

    void cluster_delegate8(LayerInstanziation* layer) 
 { 
    /* Task dispatch to cluster cores. */ 
    pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_simulationThirdLayer, layer);
 } 


 
 
/**
* @brief Instanziation and simulation of the entire network.
*
* This function has no input parameters, it initializes the cluster and cores, we initialize all layers of the network
we execute all the basic procedure to run the spiking neural network.
We call, in order, all the cluster delegate used to run all functionalities.
*
*/
 
 void neuronInstanziation(void) 
 { 
    struct pi_device cluster_dev; 
    struct pi_cluster_conf cl_conf; 

    /*We created a struct, called LayerInstanziation, useful in our cluster function,
    because they can accept only one argument. We pass input and output pointer, 
    number of neurons in the layer and so on*/

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

    LayerInstanziation thirdLayer;
    thirdLayer.neuronNumber=neuronThirdLevel;
    thirdLayer.neuronLayer=thirdLevel;
    thirdLayer.output=inputFourthLayer;
    thirdLayer.input=inputThirdLayer;
    thirdLayer.num_inputs=neuronSecondLevel;


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

    printf("-------------------THIRD LAYER----------------------\n");
    pi_cluster_send_task_to_cl(&cluster_dev, pi_cluster_task(&cl_task, cluster_delegate, &thirdLayer));
    pi_cluster_send_task_to_cl(&cluster_dev, pi_cluster_task(&cl_task, cluster_delegate5, &thirdLayer));
    printf("End. Your neuron instanziation:\n"); 
    printf("\n\n------------------------Start of the simulation-----------------------\n\n");
    for(int i = 0;i<timestep;i++){
        printf("\n\n------------------------Timestep %d-----------------------\n\n",i);
        //initialize output of the neuron layers
        pi_cluster_send_task_to_cl(&cluster_dev, pi_cluster_task(&cl_task, cluster_delegate2, &firstLayer));
        pi_cluster_send_task_to_cl(&cluster_dev, pi_cluster_task(&cl_task, cluster_delegate2, &secondLayer));
        pi_cluster_send_task_to_cl(&cluster_dev, pi_cluster_task(&cl_task, cluster_delegate2, &thirdLayer));
        for(int j=0;j<neuronFirstLevel;j++){
            inputFirstLayer[j]=input[j][i];
            //right assiignment, the input of the first layer is correctly assigned.
        }
        firstLayer.input=inputFirstLayer;
        //Assignement between vector pointers
        printf("\n\n------------------------First layer----------------------\n\n");
        pi_cluster_send_task_to_cl(&cluster_dev, pi_cluster_task(&cl_task, cluster_delegate6, &firstLayer));
        printf("\n\n------------------------Second layer-----------------------\n\n");
        pi_cluster_send_task_to_cl(&cluster_dev, pi_cluster_task(&cl_task, cluster_delegate7, &secondLayer));
        printf("\n\n------------------------Third layer-----------------------\n\n");
        pi_cluster_send_task_to_cl(&cluster_dev, pi_cluster_task(&cl_task, cluster_delegate8, &thirdLayer));

    }
    pi_cluster_close(&cluster_dev); 
    pmsis_exit(0); 
 } 
 
 


 /* Program Entry. */ 
 int main(void) 
 { 
    printf("\n\n\t *** Neuron instanziation ***\n\n"); 
    return pmsis_kickoff((void *) neuronInstanziation); 
 }
