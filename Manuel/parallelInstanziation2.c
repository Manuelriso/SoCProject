/* PMSIS includes */ 
#include "pmsis.h" 
#include <stdio.h>
#include "parallelInstanziation.h"
#include <math.h>
#include <time.h>

Neuron firstLevel[neuronFirstLevel]; 
Neuron secondLevel[neuronSecondLevel];

int weightsFirstLevel[neuronFirstLevel][neuronFirstLevel];
int weightsSecondLevel[neuronSecondLevel][neuronFirstLevel];

int inputSecondLayer[neuronFirstLevel];
int inputThirdLayer[neuronSecondLevel];

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


void initializeNeuron(int core_id, double threshold, double resetValue, int num_inputs, double tau,int iteration,LayerInstanziation* layer) {
        if(core_id+iteration*8< layer->neuronNumber){
            layer->neuronLayer[core_id+iteration*8].potential = 0.0;
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
        printf("Output fissato a %d da core %d\n",layer->output[core_id],core_id);
    }
}



void initialize_weights(LayerInstanziation* layer, int core_id, int iteration,int input,int weights[][input]){
    if(core_id+iteration*8<layer->neuronNumber){
        for(int i=0;i<input;i++){
            // Generate random number between min and max
            int randomInRange = core_id+3;
            weights[core_id+iteration*8][i]=randomInRange;
            printf("Weights posizione %d %d fissato a %d\n",core_id,i,randomInRange);
        }
    }
}
 
 /* Task executed by cluster cores. */ 
void cluster_neuronInstanziation(LayerInstanziation* layer) 
{ 
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();
    uint32_t iteration = 0;
    while(layer->neuronNumber>iteration*8){  
        initializeNeuron(core_id,10.0, 2.0, neuronFirstLevel, 10.0,iteration,layer);
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
 
 
 void neuronInstanziation(void) 
 { 
    struct pi_device cluster_dev; 
    struct pi_cluster_conf cl_conf; 

    LayerInstanziation secondLayer;
    secondLayer.neuronNumber=neuronSecondLevel;
    secondLayer.neuronLayer=secondLevel;
    secondLayer.output=inputThirdLayer;

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
    pi_cluster_send_task_to_cl(&cluster_dev, pi_cluster_task(&cl_task, cluster_delegate2, &firstLayer));
    pi_cluster_send_task_to_cl(&cluster_dev, pi_cluster_task(&cl_task, cluster_delegate3, &firstLayer));

    printf("-------------------SECOND LAYER----------------------\n");
    pi_cluster_send_task_to_cl(&cluster_dev, pi_cluster_task(&cl_task, cluster_delegate, &secondLayer));
    pi_cluster_send_task_to_cl(&cluster_dev, pi_cluster_task(&cl_task, cluster_delegate2, &secondLayer));
    pi_cluster_send_task_to_cl(&cluster_dev, pi_cluster_task(&cl_task, cluster_delegate4, &secondLayer));
    pi_cluster_close(&cluster_dev); 
    printf("End. Your neuron instanziation:\n"); 

    printf("------------------------Start of the simulation-----------------------");
    for(int i = 0;i<timestep;i++){
        
    }
    pmsis_exit(0); 
 } 
 
 
 /* Program Entry. */ 
 int main(void) 
 { 
    printf("\n\n\t *** Neuron instanziation ***\n\n"); 
    return pmsis_kickoff((void *) neuronInstanziation); 
 }
