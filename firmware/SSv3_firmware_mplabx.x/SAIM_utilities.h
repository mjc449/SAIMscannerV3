/* 
 * File:   SAIM_utilities.h
 * Author: Marshall
 *
 * Created on August 26, 2018, 6:01 PM
 */

#ifndef SAIM_UTILITIES_H
#define SAIM_UTILITIES_H

//Define the node structure for the linked lists that make up experiments

typedef struct node {
    int* pSeqStart; //Start of the AOI sequence
    int* pSeqEnd; //End of the AOI sequence
    int* pAOTF; //Head of the AOTF profile
    struct node* pNext; //Pointer to the next node in the experiment
    struct node* pLoop; //Pointer to the node to jump to in a looping experiment
} SAIMnode;

//Global variables
static SAIMnode* StartNode = NULL;
static int* StartStep = NULL;
static int* MirrorDetectorRadius = NULL;
static int1 MirrorDetectorTrigger = 0;
const size_t MaxExp = 32;  //Can be changed later if need be
static SAIMnode* ExpHeads[MaxExp];  //Locations of the experiment begin nodes
const size_t MaxSeq = 32;  //Can be changed later if need be
static int* SeqTails[MaxSeq];  //Location of the last angle (HBYTE x) in the sequences
static int SeqArray[MaxSeq][ 2 * 128];

void stop_experiment(void);
int8 get_seq_usb(int8*);
void delete_seq(int);
int8 add_seq_linear(int8*);
int8 create_new_node(int8*);
int8 push_node(int8*);
int8 add_last_node(int8*);
int8 build_loop(int, int);
int8 start_experiment(int8*);
void pause_experiment(void);
void resume_experiment(void);
void restart_experiment(void);
void delete_all_nodes(int);
void pop_node(int);
int8 remove_last_node(int);
void count_steps(int8*);

#endif	/* SAIM_UTILITIES_H */

