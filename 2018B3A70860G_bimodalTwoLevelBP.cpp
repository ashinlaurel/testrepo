/*#####################################################################################################################
Note: Please don’t upload the assignments, template file/solution and lab. manual on GitHub or others public repository.
Kindly remove them, if you have uploaded the previous assignments. 
It violates the BITS’s Intellectual Property Rights (IPR).
***********************************************************************************************************************/

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
using namespace std;

//Parameters for BTB
#define N_ENTRY_BTB 2 //0. BIA and 1. BTA 
#define N_ROWS_BTB 8 //2^n

#define N_BIT 2 //bit used in the predictors
//branch's actual outcome stored into the file.
char BOUTCOME_FILE[100];

#define INIT_STATE 0 //initial state of the predictors
//Parameters for Bimodal branch predictor
#define N_BITS_PC_BIMODAL 3

//Parameters for Two-level branch predictor
#define N_BITS_PC_2LEVEL 3
#define N_BITS_LBP 2
#define initStateLBP 0 //Starting state in the saturation BP

int** BTB = NULL; //[0] for BIA val, [1] for BTA 
int BTBLastIndx = -1;
int* Bimodal = NULL;
int** LHT=NULL;
FILE* fp = NULL;

int getPrediction(int n, int currState) {
    int maxState, pivotState, prediction; 
    //WRITE YOUR CODE HERE
    
    maxState= (int)pow(2,n)-1;
    pivotState=maxState/2;

    if(currState > pivotState){
        prediction =1;
    }else{
        prediction =0;
    }

    return prediction;
}
void updateNbitPredictor(int n, int* currState, int actualOutcome) {
    //actualOutcome can have values either 0 (untaken) or 1 (taken)
    int maxState, pivotState;
    // WRITE YOUR CODE HERE
    maxState= (int)pow(2,n)-1;
    pivotState=maxState/2;
    
    if(!actualOutcome) {
        if(*currState!=INIT_STATE)
            *currState=*currState-1;
    }
    else {
        if(*currState!=maxState)
            *currState=*currState+1;
    }
    
}

void initBTB(void) {

    BTB = new int* [N_ROWS_BTB];
    for (int i = 0; i < N_ROWS_BTB; i++) {
        BTB[i] = new int[N_ENTRY_BTB];
        BTB[i][0] = 0; // assigning the PC value as 0
        BTB[i][1] = -1; //assigning the BTA as -1
        
    }
}
int searchIndxBTB(int val) {
    int i;
    for (i = 0; i < N_ROWS_BTB; i++) {
        if (BTB[i][0] == val) {
            break;
        }
    }
    if (i == (N_ROWS_BTB)) {
        //if entry not found
        return -1;
    }
    else
        return i;
}

void initBTBHistory(int initState) {//initialize the branch predictor state with 'initState'
    for (int i = 0; i < N_ROWS_BTB; i++) {
        BTB[i][2] = initState; // assigning initial history
    }
}

void initBimodal(void){
    Bimodal = (int*)malloc(pow(2, N_BITS_PC_BIMODAL) * sizeof(int));
    for (int i=0; i < pow(2, N_BITS_PC_BIMODAL); i++){
        Bimodal[i] = INIT_STATE;
    }
}
int getTableIndx(int val){
    return (val & 0b0000000000000000000000000000111);    
}
void bimodalBranchPredictor() {
    char outcome[4];
    int pc, indxBTB, intOutcome, predictOutcome;
    int** predictStat = NULL;
      
    
    fp=fopen(BOUTCOME_FILE,"r");    
      
    //i-th predictor's prediction statistics: predictStat [i][0] for total decissions and 
    //predictStat[i][1] for correct decissions

    predictStat = (int**)malloc(N_ROWS_BTB * sizeof(int*));
    for (int i = 0; i < N_ROWS_BTB; i++) {
        predictStat[i] = (int*)malloc(2 * sizeof(int));
        predictStat[i][0] = 0;
        predictStat[i][1] = 0;
    }
    //WRITE YOUR CODE HERE
	initBimodal();
	initBTB();
	initBTBHistory(INIT_STATE);
    while (fscanf(fp, "%x %s", &pc, &outcome) != EOF)
	{
		indxBTB = searchIndxBTB(pc);
		if (indxBTB == -1)
		{
			BTBLastIndx++;
			BTB[BTBLastIndx][0] = pc;
			BTB[BTBLastIndx][1] = -1;
			indxBTB = BTBLastIndx;
		}
		predictStat[indxBTB][0]++;
		predictOutcome = getPrediction(N_BIT, Bimodal[getTableIndx(pc)]);
		if (outcome[0] == 'T') {
			intOutcome = 1;
		}
		else {
			intOutcome = 0;
		}
		if ((intOutcome == predictOutcome))
			predictStat[indxBTB][1]++;
		updateNbitPredictor(N_BIT, &Bimodal[getTableIndx(pc)], intOutcome);
	}

    fclose(fp);
    cout << "############################################################################" << endl;
    cout << "Bimodal Branch Predictor's Statistics\n";
    for (int i = 0; i < (BTBLastIndx+1); i++) {
        cout << "========================================================================" << endl;
        cout << i + 1 << " predictor's ("<<BTB[i][0]<<") total prediction: " << predictStat[i][0] << endl;
        cout << i + 1 << " predictor's ("<<BTB[i][0]<<") correct prediction: " << predictStat[i][1] << endl;
        cout << i + 1 << " predictor's ("<<BTB[i][0]<<") accuracy: " << float(predictStat[i][1]) / predictStat[i][0] << endl;

    }
    cout << "############################################################################" << endl;
    float avgAccuracy = 0;
    for (int i= 0; i < (BTBLastIndx+1); i++) {
        avgAccuracy += float(predictStat[i][1]) / predictStat[i][0];
    }
    avgAccuracy = avgAccuracy / (BTBLastIndx+1);
    cout << "Bimodal BP's Average accuracy= " << avgAccuracy << endl;
}


void initLHT(void) {

    int noEntry=pow(2,N_BITS_PC_2LEVEL);
    int noInfo=2; //[0] for histroy and [1] for BPs
    LHT = new int* [noEntry];
    for (int i = 0; i < noEntry; i++) {
        LHT[i] = new int[noInfo];
        LHT[i][0] = 0; // assigning the history as 0s
        LHT[i][1] = initStateLBP; //assigning the initial value, initStateLBP, to each predictors.
        
    }
}
void updateLHT(int indx, int outcome){
    int temp;
    //WRITE YOUR CODE HERE
    temp=LHT[indx][0];
	temp=temp >> 1;
	LHT[indx][0]=temp | (int)(pow(2,N_BITS_PC_2LEVEL-1)*outcome);

}

void twoLevelBranchPredictor(){

    char outcome[4];
    int pc, indxBTB, indxLHT, getCurState, intOutcome, predictOutcome;    
    int** predictStat = NULL;
    int* currState;
    

    fp=fopen(BOUTCOME_FILE,"r");

     //i-th predictor's prediction statistics: predictStat [i][0] for total decissions and 
    //predictStat[i][1] for correct decissions

    predictStat = (int**)malloc(pow(2, N_BITS_PC_2LEVEL) * sizeof(int*));
    for (int i = 0; i < pow(2, N_BITS_PC_2LEVEL); i++) {
        predictStat[i] = (int*)malloc(2 * sizeof(int));
        predictStat[i][0] = 0;
        predictStat[i][1] = 0;
    }
    //WRITE YOUR CODE HERE

    BTBLastIndx=-1;
    initLHT();
	initBTB();
	

    while (fscanf(fp, "%x %s", &pc, &outcome) != EOF)
	{
		indxBTB = searchIndxBTB(pc);
 
		if (indxBTB == -1)
		{
			BTBLastIndx++;
			BTB[BTBLastIndx][0] = pc;
			BTB[BTBLastIndx][1] = -1;
			BTB[BTBLastIndx][2] = INIT_STATE;
			indxBTB = BTBLastIndx;
		}
		
        indxLHT=getTableIndx(pc);
		predictStat[indxBTB][0]++;
		predictOutcome = getPrediction(N_BITS_LBP, LHT[LHT[indxLHT][0]][1]);
		
        if (outcome[0] == 'T')  {
			intOutcome = 1;
		}
		else    {
			intOutcome = 0;
		}
 
		if(intOutcome == predictOutcome)
			predictStat[indxBTB][1]++;
 
		updateNbitPredictor(N_BITS_LBP, &LHT[LHT[indxLHT][0]][1], intOutcome);
		updateLHT(indxLHT,intOutcome);
	}

    fclose(fp);
    cout << "############################################################################" << endl;
    cout << "2-level Branch Predictor's Statistics\n";
    for (int i = 0; i < (BTBLastIndx+1); i++) {
        cout << "========================================================================" << endl;
        cout << i + 1 << " predictor's ("<<BTB[i][0]<<") total prediction: " << predictStat[i][0] << endl;
        cout << i + 1 << " predictor's ("<<BTB[i][0]<<") correct prediction: " << predictStat[i][1] << endl;
        cout << i + 1 << " predictor's ("<<BTB[i][0]<<") accuracy: " << float(predictStat[i][1]) / predictStat[i][0] << endl;

    }
    cout << "##############################################################################" << endl;
    float avgAccuracy = 0;
    for (int i= 0; i < (BTBLastIndx+1); i++) {
        avgAccuracy += float(predictStat[i][1]) / predictStat[i][0];
    }
    avgAccuracy = avgAccuracy / (BTBLastIndx+1);
    cout << "2-level BP's Average accuracy= " << avgAccuracy << endl;
}


int main(int argc, char **argv)
{
    strcpy(BOUTCOME_FILE, argv[1]);
    bimodalBranchPredictor(); 
    twoLevelBranchPredictor();

}
