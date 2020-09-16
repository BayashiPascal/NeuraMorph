// ============ NEURAMORPH.H ================

#ifndef NEURAMORPH_H
#define NEURAMORPH_H

// ================= Include =================

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include "pberr.h"
#include "pbmath.h"
#include "gset.h"
#include "gdataset.h"
#include "bcurve.h"

// ----- NeuraMorphUnit

// ================= Data structure ===================

typedef struct NeuraMorphUnit {

  // Input indices in parent NeuraMorph
  VecLong* iInputs;

  // Output indices in parent NeuraMorph
  VecLong* iOutputs;

  // Lowest and highest values for filtering inputs
  VecFloat* lowFilters;
  VecFloat* highFilters;

  // Lowest and highest values of outputs
  VecFloat* lowOutputs;
  VecFloat* highOutputs;

  // Vector to memorize the output values
  VecFloat* outputs;

  // Transfer function
  BBody* transfer;

  // Working variable to avoid reallocation of memory at each Evaluate()
  VecFloat* unitInputs;

  // Variable to memorize the value of the unit during training
  float value;

long nbTrainingSample;

} NeuraMorphUnit;

// ================ Functions declaration ====================

// Create a new NeuraMorphUnit between the input 'iInputs' and the
// outputs 'iOutputs'
NeuraMorphUnit* NeuraMorphUnitCreate(
  const VecLong* iInputs,
  const VecLong* iOutputs);

// Free the memory used by the NeuraMorphUnit 'that'
void NeuraMorphUnitFree(NeuraMorphUnit** that);

// Get the input indices of the NeuraMorphUnit 'that'
#if BUILDMODE != 0
static inline
#endif
const VecLong* NMUnitIInputs(const NeuraMorphUnit* that);

// Get the output indices of the NeuraMorphUnit 'that'
#if BUILDMODE != 0
static inline
#endif
const VecLong* NMUnitIOutputs(const NeuraMorphUnit* that);

// Get the output values of the NeuraMorphUnit 'that'
#if BUILDMODE != 0
static inline
#endif
const VecFloat* NMUnitOutputs(const NeuraMorphUnit* that);

// Calculate the outputs for the 'inputs' with the NeuraMorphUnit 'that'
// Update 'that->outputs'
void NMUnitEvaluate(
  NeuraMorphUnit* that,
  const VecFloat* inputs);

// Get the number of input values of the NeuraMorphUnit 'that'
#if BUILDMODE != 0
static inline
#endif
long NMUnitGetNbInputs(const NeuraMorphUnit* that);

// Get the number of output values of the NeuraMorphUnit 'that'
#if BUILDMODE != 0
static inline
#endif
long NMUnitGetNbOutputs(const NeuraMorphUnit* that);

// Get the number of coefficients in the transfer function of
// the NeuraMorphUnit 'that'
#if BUILDMODE != 0
static inline
#endif
long NMUnitGetNbCoeffs(const NeuraMorphUnit* that);

// Get the value of the NeuraMorphUnit 'that'
#if BUILDMODE != 0
static inline
#endif
float NMUnitGetValue(const NeuraMorphUnit* that);

// Set the value of the NeuraMorphUnit 'that' to 'val'
#if BUILDMODE != 0
static inline
#endif
void NMUnitSetValue(
  NeuraMorphUnit* that,
            float val);

// Print the NeuraMorphUnit 'that' on the 'stream'
void NMUnitPrint(
  const NeuraMorphUnit* that,
                  FILE* stream);
#define NMUnitPrintln(T, S) \
  do {NMUnitPrint(T, S);fprintf(S, "\n");} while (false)

// ----- NeuraMorph

// ================= Data structure ===================

typedef struct NeuraMorph {

  // Number of inputs and outputs
  long nbInput;
  long nbOutput;

  // Inputs and outputs values
  VecFloat* inputs;
  VecFloat* outputs;

  // Internal values
  VecFloat* hiddens;

  // Lowest and highest values for internal values
  VecFloat* lowHiddens;
  VecFloat* highHiddens;

  // Flag to memorize if the outputs are to be seen as one hot encoding
  bool flagOneHot;

  // GSet of NeuraMorphUnit
  GSet units;

} NeuraMorph;

// ================ Functions declaration ====================

// Create a new NeuraMorph with 'nbInput' inputs and 'nbOutput' outputs
NeuraMorph* NeuraMorphCreate(
  long nbInput,
  long nbOutput);

// Free the memory used by the NeuraMorph 'that'
void NeuraMorphFree(NeuraMorph** that);

// Get the number of input values of the NeuraMorph 'that'
#if BUILDMODE != 0
static inline
#endif
long NMGetNbInput(const NeuraMorph* that);

// Get the number of output values of the NeuraMorph 'that'
#if BUILDMODE != 0
static inline
#endif
long NMGetNbOutput(const NeuraMorph* that);

// Get the input values of the NeuraMorph 'that'
#if BUILDMODE != 0
static inline
#endif
VecFloat* NMInputs(NeuraMorph* that);

// Get the output values of the NeuraMorph 'that'
#if BUILDMODE != 0
static inline
#endif
const VecFloat* NMOutputs(const NeuraMorph* that);

// Get the hidden values of the NeuraMorph 'that'
#if BUILDMODE != 0
static inline
#endif
const VecFloat* NMHiddens(const NeuraMorph* that);

// Get the lowest bound of hidden values of the NeuraMorph 'that'
#if BUILDMODE != 0
static inline
#endif
const VecFloat* NMLowHiddens(const NeuraMorph* that);

// Get the highest bound of hidden values of the NeuraMorph 'that'
#if BUILDMODE != 0
static inline
#endif
const VecFloat* NMHighHiddens(const NeuraMorph* that);

// Get the number of hidden values of the NeuraMorph 'that'
#if BUILDMODE != 0
static inline
#endif
long NMGetNbHidden(const NeuraMorph* that);

// Set the number of hidden values of the NeuraMorph 'that' to 'nb'
#if BUILDMODE != 0
static inline
#endif
void NMSetNbHidden(
  NeuraMorph* that,
         long nb);

// Get the flag for one hot encoding of the NeuraMorph 'that'
#if BUILDMODE != 0
static inline
#endif
bool NMGetFlagOneHot(const NeuraMorph* that);

// Set the flag for one hot encoding of the NeuraMorph 'that' to 'flag'
#if BUILDMODE != 0
static inline
#endif
void NMSetFlagOneHot(
  NeuraMorph* that,
         bool flag);

// Add one NeuraMorphUnit with input and output indices 'iInputs'
// and 'iOutputs' to the NeuraMorph 'that'
// Return the created NeuraMorphUnit
NeuraMorphUnit* NMAddUnit(
     NeuraMorph* that,
  const VecLong* iInputs,
  const VecLong* iOutputs);

// Remove the NeuraMorphUnit 'unit' from the NeuraMorph 'that'
// The NeuraMorphUnit is not freed
void NMRemoveUnit(
      NeuraMorph* that,
  NeuraMorphUnit* unit);

// Burry the NeuraMorphUnits in the 'units' set into the
// NeuraMorph 'that'
// 'units' is empty after calling this function
// The NeuraMorphUnits iOutputs must point toward the NeuraMorph
// outputs
// NeuraMorphUnits' iOutputs are redirected toward new hidden values
// 'that->hiddens' is resized as necessary
void NMBurryUnits(
  NeuraMorph* that,
        GSet* units);

// Get a new vector with indices of the outputs in the NeuraMorph 'that'
VecLong* NMGetVecIOutputs(const NeuraMorph* that);

// Evaluate the NeuraMorph 'that' on the 'inputs' values
void NMEvaluate(
  NeuraMorph* that,
    VecFloat* inputs);

// ----- NeuraMorphTrainer

// ================= Data structure ===================

typedef struct NeuraMorphTrainer {

  // Trained NeuraMorph
  NeuraMorph* neuraMorph;

  // Training dataset
  GDataSetVecFloat* dataset;

  // Index of the dataset's category used for training and evaluation
  unsigned int iCatTraining;
  unsigned int iCatEval;

  // Depth of the training
  short depth;

  // Order of the transfer function of NeuraMorphUnit
  int order;

  // Maximum number of inputs per NeuraMorphUnit
  int nbMaxInputsUnit;

  // Threshold used to discard weakest units during training
  // in [0.0,1.0]
  float weakUnitThreshold;

  // Maximum number of unit kept at each depth
  int nbMaxUnitDepth;

  // Max level of division of values' range
  short maxLvlDiv;

  // Precomputed values to train the NeuraMorphUnit
  VecFloat** preCompInp;
  VecFloat** preCompOut;

  // Lowest and highest values for input values in the training
  // dataset
  VecFloat* lowInputs;
  VecFloat* highInputs;

  // Variable to store the result of the last evaluation
  // min/avg/sigma/max
  VecFloat* resEval;
long nbCorrect;

  // Stream to output info during training and evaluation
  FILE* streamInfo;

} NeuraMorphTrainer;

// ================ Functions declaration ====================

// Create a static NeuraMorphTrainer for the NeuraMorph 'neuraMorph' and the
// GDataSet 'dataset'
// Default depth: 2
// Default iCatTraining: 0
// Default weakUnitThreshold: 0.9
NeuraMorphTrainer NeuraMorphTrainerCreateStatic(
        NeuraMorph* neuraMorph,
  GDataSetVecFloat* dataset);

// Free the memory used by the static NeuraMorphTrainer 'that'
void NeuraMorphTrainerFreeStatic(NeuraMorphTrainer* that);

// Run the training process for the NeuraMorphTrainer 'that'
void NMTrainerRun(NeuraMorphTrainer* that);

// Run the evaluation process for the NeuraMorphTrainer 'that'
void NMTrainerEval(NeuraMorphTrainer* that);

// Get the depth of the NeuraMorphTrainer 'that'
#if BUILDMODE != 0
static inline
#endif
short NMTrainerGetDepth(const NeuraMorphTrainer* that);

// Set the depth of the NeuraMorphTrainer 'that' to 'depth'
#if BUILDMODE != 0
static inline
#endif
void NMTrainerSetDepth(
  NeuraMorphTrainer* that,
               short depth);

// Get the streamInfo of the NeuraMorphTrainer 'that'
#if BUILDMODE != 0
static inline
#endif
FILE* NMTrainerStreamInfo(const NeuraMorphTrainer* that);

// Set the streamInfo of the NeuraMorphTrainer 'that' to 'streamInfo'
#if BUILDMODE != 0
static inline
#endif
void NMTrainerSetStreamInfo(
  NeuraMorphTrainer* that,
               FILE* streamInfo);

// Get the maxLvlDiv of the NeuraMorphTrainer 'that'
#if BUILDMODE != 0
static inline
#endif
short NMTrainerGetMaxLvlDiv(const NeuraMorphTrainer* that);

// Set the maxLvlDiv of the NeuraMorphTrainer 'that' to 'lvl'
#if BUILDMODE != 0
static inline
#endif
void NMTrainerSetMaxLvlDiv(
  NeuraMorphTrainer* that,
               short lvl);

// Get the order of the NeuraMorphTrainer 'that'
#if BUILDMODE != 0
static inline
#endif
int NMTrainerGetOrder(const NeuraMorphTrainer* that);

// Set the order of the NeuraMorphTrainer 'that' to 'order'
#if BUILDMODE != 0
static inline
#endif
void NMTrainerSetOrder(
  NeuraMorphTrainer* that,
                 int order);

// Get the nbMaxUnitDepth of the NeuraMorphTrainer 'that'
#if BUILDMODE != 0
static inline
#endif
int NMTrainerGetNbMaxUnitDepth(const NeuraMorphTrainer* that);

// Set the nbMaxUnitDepth of the NeuraMorphTrainer 'that' to 'nbMaxUnitDepth'
#if BUILDMODE != 0
static inline
#endif
void NMTrainerSetNbMaxUnitDepth(
  NeuraMorphTrainer* that,
                 int nbMaxUnitDepth);

// Get the nbMaxInputsUnit of the NeuraMorphTrainer 'that'
#if BUILDMODE != 0
static inline
#endif
int NMTrainerGetNbMaxInputsUnit(const NeuraMorphTrainer* that);

// Set the nbMaxInputsUnit of the NeuraMorphTrainer 'that' to 'order'
#if BUILDMODE != 0
static inline
#endif
void NMTrainerSetNbMaxInputsUnit(
  NeuraMorphTrainer* that,
                 int nbMaxInputsUnit);

// Get the weakness threshold of the NeuraMorphTrainer 'that'
#if BUILDMODE != 0
static inline
#endif
float NMTrainerGetWeakThreshold(const NeuraMorphTrainer* that);

// Set the weakness threshold of the NeuraMorphTrainer 'that'
// to 'iCat'
#if BUILDMODE != 0
static inline
#endif
void NMTrainerSetWeakThreshold(
  NeuraMorphTrainer* that,
               float weakUnitThreshold);

// Get the index of the training category of the NeuraMorphTrainer 'that'
#if BUILDMODE != 0
static inline
#endif
unsigned int NMTrainerGetICatTraining(const NeuraMorphTrainer* that);

// Set the index of the training category of the NeuraMorphTrainer 'that'
// to 'iCat'
#if BUILDMODE != 0
static inline
#endif
void NMTrainerSetICatTraining(
  NeuraMorphTrainer* that,
        unsigned int iCatTraining);

// Get the index of the evaluation category of the NeuraMorphTrainer 'that'
#if BUILDMODE != 0
static inline
#endif
unsigned int NMTrainerGetICatEval(const NeuraMorphTrainer* that);

// Set the index of the evaluation category of the NeuraMorphTrainer 'that'
// to 'iCat'
#if BUILDMODE != 0
static inline
#endif
void NMTrainerSetICatEval(
  NeuraMorphTrainer* that,
        unsigned int iCatEval);

// Get the NeuraMorph of the NeuraMorphTrainer 'that'
#if BUILDMODE != 0
static inline
#endif
NeuraMorph* NMTrainerNeuraMorph(const NeuraMorphTrainer* that);

// Get the GDataSet of the NeuraMorphTrainer 'that'
#if BUILDMODE != 0
static inline
#endif
GDataSetVecFloat* NMTrainerDataset(const NeuraMorphTrainer* that);

// Get the result of the last evaluation of the NeuraMorphTrainer 'that'
#if BUILDMODE != 0
static inline
#endif
const VecFloat* NMTrainerResEval(const NeuraMorphTrainer* that);

// Get the number of correct output in the last evaluation of the
// NeuraMorphTrainer 'that'
#if BUILDMODE != 0
static inline
#endif
long NMTrainerGetNbCorrect(const NeuraMorphTrainer* that);

// ================ static inliner ====================

#if BUILDMODE != 0
#include "neuramorph-inline.c"
#endif

#endif
