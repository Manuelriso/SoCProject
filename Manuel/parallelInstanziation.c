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
 

 void update_neuron(Neuron* n, int numberNeuron, int* inputNextLayer) {
    // Apply decay to the membrane potential
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


void initializeNeuron(Neuron* n, int core_id, double threshold, double resetValue, int num_inputs, double tau) {
        n[core_id].potential = 0.0;
        n[core_id].threshold = threshold;
        n[core_id].spiked = false;
        n[core_id].reset = resetValue;
        n[core_id].num_inputs = num_inputs;
        n[core_id].tau = tau; // Set the time constant
        printf("Neuron number %d instanziate by core %d\n",core_id,core_id);
        printf("Potential : %f\nThresold : %f\n",n[core_id].potential,n[core_id].threshold);
}



void init_output(int* input_to_the_Layer, int core_id) {
    input_to_the_Layer[core_id]=0;
    printf("Output fissato a %d da core %d\n",input_to_the_Layer[core_id],core_id);
}



void initialize_weights(int neuron, int core_id, int weights[][neuron]){
    for(int i=0;i<neuron;i++){
        // Generate random number between min and max
        int randomInRange = core_id+3;
        weights[core_id][i]=randomInRange;
        printf("Weights posizione %d %d fissato a %d\n",core_id,i,randomInRange);
    }
}
 
 /* Task executed by cluster cores. */ 
void cluster_neuronInstanziation(void *arg) 
{ 
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();  
    initializeNeuron(firstLevel,core_id,10.0, 2.0, neuronFirstLevel, 10.0);
} 


void cluster_outputInstanziation(void *arg) 
{ 
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();  
    init_output(inputSecondLayer,core_id);
} 

void cluster_weightsInstanziation(void *arg) 
{ 
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();  
    initialize_weights(neuronFirstLevel,core_id,weightsFirstLevel);
} 


 /* Cluster main entry, executed by core 0. */ 
 void cluster_delegate(void *arg) 
 { 
    /* Task dispatch to cluster cores. */ 
    pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_neuronInstanziation, arg);
 } 

 void cluster_delegate2(void *arg) 
 { 
    /* Task dispatch to cluster cores. */ 
    pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_outputInstanziation, arg);
 } 

 void cluster_delegate3(void *arg) 
 { 
    /* Task dispatch to cluster cores. */ 
    pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_weightsInstanziation, arg);
 } 
 
 
 void neuronInstanziation(void) 
 { 
    struct pi_device cluster_dev; 
    struct pi_cluster_conf cl_conf; 
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
    struct pi_cluster_task cl_task2;
    struct pi_cluster_task cl_task3;
    pi_cluster_send_task_to_cl(&cluster_dev, pi_cluster_task(&cl_task, cluster_delegate, NULL));
    pi_cluster_send_task_to_cl(&cluster_dev, pi_cluster_task(&cl_task2, cluster_delegate2, NULL));
    pi_cluster_send_task_to_cl(&cluster_dev, pi_cluster_task(&cl_task2, cluster_delegate3, NULL));
    pi_cluster_close(&cluster_dev); 
    printf("End. Your neuron instanziation:\n"); 
    pmsis_exit(0); 
 } 
 
 
 /* Program Entry. */ 
 int main(void) 
 { 
    printf("\n\n\t *** Neuron instanziation ***\n\n"); 
    return pmsis_kickoff((void *) neuronInstanziation); 
 }
