/* PMSIS includes */ 
#include "pmsis.h" 
#include <stdio.h>
#include "parallelLIF.h"
#include <math.h>
#include <time.h>

Neuron firstLevel[neuronFirstLevel]; 
Neuron secondLevel[neuronSecondLevel];

int weightsFirstLevel[neuronFirstLevel][neuronFirstLevel];
int weightsSecondLevel[neuronSecondLevel][neuronFirstLevel];

int inputFirstLayer[neuronFirstLevel];
int inputSecondLayer[neuronFirstLevel];
int inputThirdLayer[neuronSecondLevel];

int input[neuronFirstLevel][timestep] = {
        {1, 0},
        {0, 0},
        {0, 0},
        {0, 1},
        {0, 0},
        {0, 0},
        {0, 0}
    };

 

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


void simulateFirstLayer(LayerInstanziation* layer,int core_id,int iteration,int num_inputs) {
    if(core_id+iteration*8<layer->neuronNumber){
            for (int j = 0; j < num_inputs; j++) {
                //printf("%d\n",layer->input[j]);
                if (layer->input[j] == 1) {
                    //printf("ECCOMI");
                    layer->neuronLayer[core_id+iteration*8].potential += weightsFirstLevel[core_id+iteration*8][j];
                }
                //To see better the evolution of neuron, put update_neuron here.
                //update_neuron(&neurons[i], i, inputNextLayer);
            }
            update_neuron(&(layer->neuronLayer[core_id+iteration*8]), core_id+iteration*8, layer->output);
    }
}


void simulateSecondLayer(LayerInstanziation* layer,int core_id,int iteration,int num_inputs) {
    if(core_id+iteration*8<layer->neuronNumber){
            for (int j = 0; j < num_inputs; j++) {
                if (layer->input[j] == 1) {
                    //printf("ECCOMI");
                    layer->neuronLayer[core_id+iteration*8].potential += weightsSecondLevel[core_id+iteration*8][j];
                }
                //To see better the evolution of neuron, put update_neuron here.
                //update_neuron(&neurons[i], i, inputNextLayer);
            }
            update_neuron(&(layer->neuronLayer[core_id+iteration*8]), core_id+iteration*8, layer->output);
    }
}


void initializeNeuron(int core_id, double threshold, double resetValue, int num_inputs, double tau,int iteration,LayerInstanziation* layer) {
        if(core_id+iteration*8< layer->neuronNumber){
            layer->neuronLayer[core_id+iteration*8].potential = -65.0;
            layer->neuronLayer[core_id+iteration*8].threshold = threshold;
            layer->neuronLayer[core_id+iteration*8].spiked = false;
            layer->neuronLayer[core_id+iteration*8].reset = resetValue;
            layer->neuronLayer[core_id+iteration*8].num_inputs = num_inputs;
            layer->neuronLayer[core_id+iteration*8].tau = tau; // Set the time constant
            printf("Neuron number %d instanziate by core %d\n",core_id+iteration*8,core_id);
            printf("Potential : %f\nThresold : %f\n",layer->neuronLayer[core_id].potential,layer->neuronLayer[core_id].threshold);
        }
}



void init_output( LayerInstanziation* layer,int core_id,int iteration) {
    if(core_id+iteration*8<layer->neuronNumber){
        layer->output[core_id+iteration*8]=0;
        //printf("Output fissato a %d da core %d\n",layer->output[core_id],core_id);
    }
}



void initialize_weights(LayerInstanziation* layer, int core_id, int iteration,int input,int weights[][input]){
    if(core_id+iteration*8<layer->neuronNumber){
        for(int i=0;i<input;i++){
            int randomInRange = core_id+3;
            /*int random_value = pi_rand();  // PULP function to generate a number on 32 bits.

            //Random value between -5 and 5
            int randomInRange = (random_value % 11) - 5;
            */
            weights[core_id+iteration*8][i]=randomInRange;
            //printf("Weights posizione %d %d fissato a %d\n",core_id,i,randomInRange);
        }
    }
}
 
 /* Task executed by cluster cores. */ 
void cluster_neuronInstanziation(LayerInstanziation* layer) 
{ 
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();
    uint32_t iteration = 0;
    while(layer->neuronNumber>iteration*8){
        //standard value for LIF  
        initializeNeuron(core_id,-50.0, -65.0, neuronFirstLevel, 10.0,iteration,layer);
        iteration++;
    }
} 


void cluster_outputInstanziation(LayerInstanziation* layer) 
{ 
    uint32_t iteration =0;
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id(); 
    while(layer->neuronNumber>iteration*8){ 
        init_output(layer,core_id,iteration);
        iteration++;
    }
} 

void cluster_weightsInstanziationFirstLayer(LayerInstanziation* layer) 
{ 
    uint32_t iteration=0;
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();  
    while(layer->neuronNumber>iteration*8){
        initialize_weights(layer,core_id,iteration,neuronFirstLevel,weightsFirstLevel);
        iteration++;
    }
} 


void cluster_weightsInstanziationSecondLayer(LayerInstanziation* layer) 
{ 
    uint32_t iteration=0;
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();  
    while(layer->neuronNumber>iteration*8){
        initialize_weights(layer,core_id,iteration,neuronFirstLevel,weightsSecondLevel);
        iteration++;
    }
} 


void cluster_simulationFirstLayer(LayerInstanziation* layer) 
{ 
    uint32_t iteration=0;
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();  
    while(layer->neuronNumber>iteration*8){
        //printf("%d\n",layer->input[0]);
        simulateFirstLayer(layer,core_id,iteration,neuronFirstLevel);
        iteration++;
    }
} 

void cluster_simulationSecondLayer(LayerInstanziation* layer) 
{ 
    uint32_t iteration=0;
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();  
    while(layer->neuronNumber>iteration*8){
        simulateSecondLayer(layer,core_id,iteration,neuronFirstLevel);
        iteration++;
    }
} 




 /* Cluster main entry, executed by core 0. */ 
 void cluster_delegate(LayerInstanziation* layer) 
 { 
    /* Task dispatch to cluster cores. */ 
    //pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_neuronInstanziation,(neuronNumber,neuronLevel));
    pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_neuronInstanziation,layer);
 } 

 void cluster_delegate2(LayerInstanziation* layer) 
 { 
    /* Task dispatch to cluster cores. */ 
    pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_outputInstanziation, layer);
 } 

 void cluster_delegate3(LayerInstanziation* layer) 
 { 
    /* Task dispatch to cluster cores. */ 
    pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_weightsInstanziationFirstLayer, layer);
 } 

  void cluster_delegate4(LayerInstanziation* layer) 
 { 
    /* Task dispatch to cluster cores. */ 
    pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_weightsInstanziationSecondLayer, layer);
 } 

   void cluster_delegate5(LayerInstanziation* layer) 
 { 
    /* Task dispatch to cluster cores. */ 
    pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_simulationFirstLayer, layer);
 } 

   void cluster_delegate6(LayerInstanziation* layer) 
 { 
    /* Task dispatch to cluster cores. */ 
    pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_simulationSecondLayer, layer);
 } 


 
 
 void neuronInstanziation(void) 
 { 
    struct pi_device cluster_dev; 
    struct pi_cluster_conf cl_conf; 

    LayerInstanziation secondLayer;
    secondLayer.neuronNumber=neuronSecondLevel;
    secondLayer.neuronLayer=secondLevel;
    secondLayer.output=inputThirdLayer;
    secondLayer.input=inputSecondLayer;

    LayerInstanziation firstLayer;
    firstLayer.neuronNumber=neuronFirstLevel;
    firstLayer.neuronLayer=firstLevel;
    firstLayer.output=inputSecondLayer;

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
 
 
 /* Program Entry. */ 
 int main(void) 
 { 
    printf("\n\n\t *** Neuron instanziation ***\n\n"); 
    return pmsis_kickoff((void *) neuronInstanziation); 
 }
