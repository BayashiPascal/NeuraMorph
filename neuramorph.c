// ============ NEURAMORPH.C ================

// ================= Include =================

#include "neuramorph.h"
#if BUILDMODE == 0
#include "neuramorph-inline.c"
#endif

// ----- NeuraMorphUnit

// ================ Functions declaration ====================

// Update the low and high of the hiddens of the NeuraMorph 'that' with
// the low and high of its units
void NMUpdateLowHighHiddens(NeuraMorph* that);

// ================ Functions implementation ====================

// Create a new NeuraMorphUnit between the input 'iInputs' and the
// outputs 'iOutputs'
NeuraMorphUnit* NeuraMorphUnitCreate(
  const VecLong* iInputs,
  const VecLong* iOutputs) {

#if BUILDMODE == 0

  if (iInputs == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'iInputs' is null");
    PBErrCatch(NeuraMorphErr);

  }

  if (iOutputs == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'iOutputs' is null");
    PBErrCatch(NeuraMorphErr);

  }

#endif

  // Allocate memory for the NeuraMorphUnit
  NeuraMorphUnit* that =
    PBErrMalloc(
      NeuraMorphErr,
      sizeof(NeuraMorphUnit));

  // Get the number of inputs (including the constant) and outputs
  long nbIn = VecGetDim(iInputs);
  long nbOut = VecGetDim(iOutputs);

  // Init properties
  that->iInputs = VecClone(iInputs);
  that->iOutputs = VecClone(iOutputs);
  that->lowFilters = VecFloatCreate(nbIn);
  that->highFilters = VecFloatCreate(nbIn);
  that->lowOutputs = NULL;
  that->highOutputs = NULL;
  that->outputs = VecFloatCreate(nbOut);
  VecShort2D dim = VecShortCreateStatic2D();
  VecSet(
    &dim,
    0,
    nbIn);
  VecSet(
    &dim,
    1,
    nbOut);
  that->transfer = NULL;
  that->unitInputs = VecFloatCreate(nbIn);
  that->value = 0.0;

  // Return the new NeuraMorphUnit
  return that;

}

// Free the memory used by the NeuraMorphUnit 'that'
void NeuraMorphUnitFree(NeuraMorphUnit** that) {

  // Check the input
  if (that == NULL || *that == NULL) {

    return;

  }

  // Free memory
  VecFree(&((*that)->iInputs));
  VecFree(&((*that)->iOutputs));
  VecFree(&((*that)->lowFilters));
  VecFree(&((*that)->highFilters));
  if ((*that)->lowOutputs != NULL) {

    VecFree(&((*that)->lowOutputs));

  }

  if ((*that)->highOutputs != NULL) {

    VecFree(&((*that)->highOutputs));

  }

  VecFree(&((*that)->outputs));
  BBodyFree(&((*that)->transfer));
  VecFree(&((*that)->unitInputs));
  free(*that);
  *that = NULL;

}

// Calculate the outputs for the 'inputs' with the NeuraMorphUnit 'that'
// Update 'that->outputs'
void NMUnitEvaluate(
  NeuraMorphUnit* that,
  const VecFloat* inputs) {

#if BUILDMODE == 0

  if (that == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'that' is null");
    PBErrCatch(NeuraMorphErr);

  }

  if (VecGetDim(inputs) != VecGetDim(that->iInputs)) {

    NeuraMorphErr->_type = PBErrTypeInvalidArg;
    sprintf(
      NeuraMorphErr->_msg,
      "'inputs' has invalid dimension (%ld!=%ld)",
      VecGetDim(inputs),
      VecGetDim(that->iInputs));
    PBErrCatch(NeuraMorphErr);

  }

#endif

  // Reset the outputs
  VecFree(&(that->outputs));

  // Update the scaled inputs
  for (
    long iInput = 0;
    iInput < VecGetDim(that->unitInputs);
    ++iInput) {

    // Get the input value and its low/high filters
    float val =
      VecGet(
        inputs,
        iInput);
    float low =
      VecGet(
        that->lowFilters,
        iInput);
    float high =
      VecGet(
        that->highFilters,
        iInput);

    // Set the value in the unit inputs
    VecSet(
      that->unitInputs,
      iInput,
      (val - low) / (high - low));

  }

  // Apply the transfer function
  that->outputs =
    BBodyGet(
      that->transfer,
      that->unitInputs);

  // If the low and high values for outputs don't exist yet
  if (that->lowOutputs == NULL) {

    // Create the low and high values by cloning the current output
    that->lowOutputs = VecClone(that->outputs);
    that->highOutputs = VecClone(that->outputs);

  // Else, the low and high values for outputs exist
  } else {

    // Loop on the outputs
    for (
      long iOutput = 0;
      iOutput < VecGetDim(that->outputs);
      ++iOutput) {

      // Update the low and high values for this output
      float val =
        VecGet(
          that->outputs,
          iOutput);

      float curLow =
        VecGet(
          that->lowOutputs,
          iOutput);
      if (curLow > val) {

        VecSet(
          that->lowOutputs,
          iOutput,
          val);

      }

      float curHigh =
        VecGet(
          that->highOutputs,
          iOutput);
      if (curHigh < val) {

        VecSet(
          that->highOutputs,
          iOutput,
          val);

      }

    }

  }

}

// Print the NeuraMorphUnit 'that' on the 'stream'
void NMUnitPrint(
  const NeuraMorphUnit* that,
                  FILE* stream) {

#if BUILDMODE == 0

  if (that == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'that' is null");
    PBErrCatch(NeuraMorphErr);

  }

  if (stream == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'stream' is null");
    PBErrCatch(NeuraMorphErr);

  }

#endif

  VecPrint(
    NMUnitIInputs(that),
    stream);
  fprintf(
    stream,
    " -> ");
  VecPrint(
    NMUnitIOutputs(that),
    stream);
  fprintf(
    stream,
    " (%04.6f)",
    NMUnitGetValue(that));

}

// ----- NeuraMorph

// ================ Functions implementation ====================

// Create a new NeuraMorph with 'nbInput' inputs and 'nbOutput' outputs
NeuraMorph* NeuraMorphCreate(
  long nbInput,
  long nbOutput) {

  // Allocate memory for the NeuraMorph
  NeuraMorph* that =
    PBErrMalloc(
      NeuraMorphErr,
      sizeof(NeuraMorph));

  // Init properties
  that->nbInput = nbInput;
  that->nbOutput = nbOutput;
  that->inputs = VecFloatCreate(nbInput);
  that->outputs = VecFloatCreate(nbOutput);
  that->hiddens = NULL;
  that->lowHiddens = NULL;
  that->highHiddens = NULL;
  that->units = GSetCreateStatic();
  that->flagOneHot = false;

  // Return the NeuraMorph
  return that;

}

// Free the memory used by the NeuraMorph 'that'
void NeuraMorphFree(NeuraMorph** that) {

  // Check the input
  if (that == NULL || *that == NULL) {

    return;

  }

  // Free memory
  VecFree(&((*that)->inputs));
  VecFree(&((*that)->outputs));
  if ((*that)->hiddens != NULL) {

    VecFree(&((*that)->hiddens));
    VecFree(&((*that)->lowHiddens));
    VecFree(&((*that)->highHiddens));

  }

  while (GSetNbElem(&((*that)->units)) > 0) {

    NeuraMorphUnit* unit = GSetPop(&((*that)->units));
    NeuraMorphUnitFree(&unit);

  }

  free(*that);
  *that = NULL;

}

// Add one NeuraMorphUnit with input and output indices 'iInputs'
// and 'iOutputs' to the NeuraMorph 'that'
// Return the created NeuraMorphUnit
NeuraMorphUnit* NMAddUnit(
     NeuraMorph* that,
  const VecLong* iInputs,
  const VecLong* iOutputs) {

#if BUILDMODE == 0

  if (that == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'that' is null");
    PBErrCatch(NeuraMorphErr);

  }

  if (iInputs == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'iInputs' is null");
    PBErrCatch(NeuraMorphErr);

  }

  if (iOutputs == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'iOutputs' is null");
    PBErrCatch(NeuraMorphErr);

  }

#endif

  // Create the NeuraMorphUnit
  NeuraMorphUnit* unit =
    NeuraMorphUnitCreate(
      iInputs,
      iOutputs);

  // Append the new NeuraorphUnit to the set of NeuraMorphUnit
  GSetAppend(
    &(that->units),
    unit);

  // Return the new unit
  return unit;

}

// Remove the NeuraMorphUnit 'unit' from the NeuraMorph 'that'
// The NeuraMorphUnit is not freed
void NMRemoveUnit(
      NeuraMorph* that,
  NeuraMorphUnit* unit) {

#if BUILDMODE == 0

  if (that == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'that' is null");
    PBErrCatch(NeuraMorphErr);

  }

#endif

  // Remove the NeuraorphUnit from the set of NeuraMorphUnit
  GSetRemoveAll(
    &(that->units),
    unit);

}

// Burry the NeuraMorphUnits in the 'units' set into the
// NeuraMorph 'that'
// 'units' is empty after calling this function
// The NeuraMorphUnits iOutputs must point toward the NeuraMorph
// outputs
// NeuraMorphUnits' iOutputs are redirected toward new hidden values
// 'that->hiddens' is resized as necessary
void NMBurryUnits(
  NeuraMorph* that,
        GSet* units) {

#if BUILDMODE == 0

  if (that == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'that' is null");
    PBErrCatch(NeuraMorphErr);

  }

#endif

  // Declare a variable to memorize the number of hidden values
  // to add
  long nbHiddenValues = 0;

  // While there are units to burry
  while (GSetNbElem(units) > 0) {

    // Get the unit
    NeuraMorphUnit* unit = GSetPop(units);

    // Loop on the iOutputs of the unit
    for (
      long iOutput = 0;
      iOutput < VecGetDim(NMUnitIOutputs(unit));
      ++iOutput) {

      long indice =
        VecGet(
          NMUnitIOutputs(unit),
          iOutput);
      VecSet(
        unit->iOutputs,
        iOutput,
        indice + nbHiddenValues);

    }

    // Append the unit to the set of NeuraMorphUnit
    GSetAppend(
      &(that->units),
      unit);

    // Update the number of new hidden values
    nbHiddenValues += VecGetDim(NMUnitIOutputs(unit));

  }

  // If there is already hidden values
  if (that->hiddens != NULL) {

    // Add the previous number of hidden values
    nbHiddenValues += VecGetDim(that->hiddens);

    // Free memory
    VecFree(&(that->hiddens));
    VecFree(&(that->lowHiddens));
    VecFree(&(that->highHiddens));

  }

  // If there are hidden values after burrying
  if (nbHiddenValues > 0) {

    // Resize the hiddens value vector
    that->hiddens = VecFloatCreate(nbHiddenValues);
    that->lowHiddens = VecFloatCreate(nbHiddenValues);
    that->highHiddens = VecFloatCreate(nbHiddenValues);

    // Update the low and high of the hiddens with the low and high
    // of the units
    NMUpdateLowHighHiddens(that);

  }

}

// Get a new vector with indices of the outputs in the NeuraMorph 'that'
VecLong* NMGetVecIOutputs(const NeuraMorph* that) {

#if BUILDMODE == 0

  if (that == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'that' is null");
    PBErrCatch(NeuraMorphErr);

  }

#endif

  // Allocate memory for the result
  VecLong* iOutputs = VecLongCreate(NMGetNbOutput(that));

  // Loop on indices
  for (
    long iOutput = 0;
    iOutput < NMGetNbOutput(that);
    ++iOutput) {

    // Set the indice of this output
    VecSet(
      iOutputs,
      iOutput,
      iOutput + NMGetNbHidden(that));

  }

  // Return the result
  return iOutputs;

}

// Update the low and high of the hiddens of the NeuraMorph 'that' with
// the low and high of its units
void NMUpdateLowHighHiddens(NeuraMorph* that) {

#if BUILDMODE == 0

  if (that == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'that' is null");
    PBErrCatch(NeuraMorphErr);

  }

#endif

  // Loop on the units
  GSetIterForward iter =
    GSetIterForwardCreateStatic(&(that->units));
  do {

    // Get the unit
    NeuraMorphUnit* unit = GSetIterGet(&iter);

    // Loop on the iOutputs of the unit
    for (
      long iOutput = 0;
      iOutput < VecGetDim(NMUnitIOutputs(unit));
      ++iOutput) {

      // Get the indice
      long indice =
        VecGet(
          NMUnitIOutputs(unit),
          iOutput);

      // If the indice points to a hidden value
      if (indice < NMGetNbHidden(that)) {

        // If the low and high exist
        if (
          unit->lowOutputs != NULL &&
          unit->highOutputs != NULL) {

          // Update the low and high
          float low =
            VecGet(
              unit->lowOutputs,
              iOutput);
          float high =
            VecGet(
              unit->highOutputs,
              iOutput);
          VecSet(
            that->lowHiddens,
            indice,
            low);
          VecSet(
            that->highHiddens,
            indice,
            high);

        }

      }

    }

  } while (GSetIterStep(&iter));

}

// Evaluate the NeuraMorph 'that' on the 'inputs' values
void NMEvaluate(
  NeuraMorph* that,
    VecFloat* inputs) {

#if BUILDMODE == 0

  if (that == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'that' is null");
    PBErrCatch(NeuraMorphErr);

  }

  if (inputs == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'inputs' is null");
    PBErrCatch(NeuraMorphErr);

  }

  if (VecGetDim(inputs) != VecGetDim(that->inputs)) {

    NeuraMorphErr->_type = PBErrTypeInvalidArg;
    sprintf(
      NeuraMorphErr->_msg,
      "'inputs' has invalid size (%ld==%ld)",
      VecGetDim(inputs),
      VecGetDim(that->inputs));
    PBErrCatch(NeuraMorphErr);

  }

#endif

  // Copy the inputs into the internal inputs
  VecCopy(
    that->inputs,
    inputs);

  // Reset the internal outputs
  VecSetNull(that->outputs);

  // If there are no units
  if (GSetNbElem(&(that->units)) == 0) {

    // Nothing else to do
    return;

  }

  // Loop on the units
  GSetIterForward iter = GSetIterForwardCreateStatic(&(that->units));
  do {

    // Get the unit
    NeuraMorphUnit* unit = GSetIterGet(&iter);

    // Allocate memory for inputs sent to the unit
    VecFloat* unitInputs = VecFloatCreate(NMUnitGetNbInputs(unit));

    // Loop on the input indices of the unit
    for (
      long iInput = 0;
      iInput < NMUnitGetNbInputs(unit);
      ++iInput) {

      // Get the input indice
      long indiceInput =
        VecGet(
          NMUnitIInputs(unit),
          iInput);

      // Declare a variable to memorize the input value
      float val = 0.0;

      // If this indice points toward an input
      if (indiceInput < NMGetNbInput(that)) {

        // Get the input value of the NeuraMorph for this indice
        val =
          VecGet(
            NMInputs(that),
            indiceInput);

      // Else, the indice points toward a hidden value
      } else {

        // Get the hidden value of the NeuraMorph for this indice
        val =
          VecGet(
            that->hiddens,
            indiceInput - NMGetNbInput(that));

      }

      // Set the input value for the unit for this indice
      VecSet(
        unitInputs,
        iInput,
        val);

    }

    // Evaluate the unit
    NMUnitEvaluate(
      unit,
      unitInputs);

    // Free the memory used by the unit input
    VecFree(&unitInputs);

    // Loop on the output indices of the unit
    for (
      long iOutput = 0;
      iOutput < NMUnitGetNbOutputs(unit);
      ++iOutput) {

      // Get the output value of the unit for this indice
      float val =
        VecGet(
          NMUnitOutputs(unit),
          iOutput);

      // Get the output indice
      long indiceOutput =
        VecGet(
          NMUnitIOutputs(unit),
          iOutput);

      // If the indice points toward a hidden
      if (indiceOutput < NMGetNbHidden(that)) {

        // Set the hidden value of the NeuraMorph for this indice
        VecSet(
          that->hiddens,
          indiceOutput,
          val);

      // Else, the indice points toward an output
      } else {

        // Set the output value of the NeuraMorph for this indice
        VecSet(
          that->outputs,
          indiceOutput - NMGetNbHidden(that),
          val);

      }

    }

  } while (GSetIterStep(&iter));

  // If the NeuraMorph is a one hot encoder
  if (NMGetFlagOneHot(that) == true) {

    // Get the one hot
    long oneHot = VecGetIMaxVal(that->outputs);

    // Convert the output values
    VecSetAll(
      that->outputs,
      -1.0);
    VecSet(
      that->outputs,
      oneHot,
      1.0);

  }

}

// ----- NeuraMorphTrainer

// ================ Functions declaration ====================

// Return true if the vector 'v' is a valid indices configuration
// i.e. v[i]<v[j] for all i<j
bool NMTrainerIsValidInputConfig(
  const VecLong* v,
            long iMinInput);

// Train a new NeuraMorphUnit with the interface defined by 'iInputs'
// and 'iOutputs', and add it to the set, sorted on its value
// If 'lastUnit' is true, the NeuraMorphUnit will be the last one in
// its NeuraMorph
void NMTrainerTrainUnit(
  NeuraMorphTrainer* that,
               GSet* trainedUnits,
      const VecLong* iInputs,
      const VecLong* iOutputs,
                bool lastUnit);

// Precompute the values of the NeuraMorph for each sample of the
// GDataset for the NeuraMorphTrainer 'that'
void NMTrainerPrecomputeValues(NeuraMorphTrainer* that);

// Free the precomputed values of the NeuraMorphTrainer 'that'
void NMTrainerFreePrecomputed(NeuraMorphTrainer* that);

// ================ Functions implemetation ====================

// Create a static NeuraMorphTrainer for the NeuraMorph 'neuraMorph' and the
// GDataSet 'dataset'
// Default depth: 2
// Default iCatTraining: 0
// Default weakUnitThreshold: 0.9
NeuraMorphTrainer NeuraMorphTrainerCreateStatic(
        NeuraMorph* neuraMorph,
  GDataSetVecFloat* dataset) {

#if BUILDMODE == 0

  if (neuraMorph == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'neuraMorph' is null");
    PBErrCatch(NeuraMorphErr);

  }

  if (dataset == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'dataset' is null");
    PBErrCatch(NeuraMorphErr);

  }

#endif

  // Declare the new NeuraMorphTrainer
  NeuraMorphTrainer that;

  // Init properties
  that.neuraMorph = neuraMorph;
  that.dataset = dataset;
  that.depth = 2;
  that.order = 1;
  that.nbMaxUnitDepth = 2;
  that.maxLvlDiv = 2;
  that.nbMaxInputsUnit =
    MAX(
      GDSGetNbOutputs(dataset),
      2);
  that.iCatTraining = 0;
  that.iCatEval = 1;
  that.weakUnitThreshold = 0.9;
  that.preCompInp = NULL;
  that.lowInputs = NULL;
  that.highInputs = NULL;
  that.streamInfo = NULL;
  that.resEval = VecFloatCreateStatic3D();
  that.nbCorrect = 0;

  // Return the NeuraMorphTrainer
  return that;

}

// Free the memory used by the static NeuraMorphTrainer 'that'
void NeuraMorphTrainerFreeStatic(NeuraMorphTrainer* that) {

#if BUILDMODE == 0

  if (that == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'that' is null");
    PBErrCatch(NeuraMorphErr);

  }

#endif

  VecFree(&(that->lowInputs));
  VecFree(&(that->highInputs));

}

// Run the training process for the NeuraMorphTrainer 'that'
void NMTrainerRun(NeuraMorphTrainer* that) {

#if BUILDMODE == 0

  if (that == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'that' is null");
    PBErrCatch(NeuraMorphErr);

  }

#endif

  // Declare a variable to memorize the minimum index needed in the
  // inputs of the new unit to ensure we do not train twice the same
  // unit
  long iMinInput = 0;

  // Loop on training depth
  for (
    short iDepth = 1;
    iDepth <= NMTrainerGetDepth(that);
    ++iDepth) {

    printf(
      "Depth %d/%d...\n",
      iDepth,
      NMTrainerGetDepth(that));

    // Get the number of available inputs for the new unit
    long nbAvailInputs =
      NMGetNbInput(NMTrainerNeuraMorph(that)) +
      NMGetNbHidden(NMTrainerNeuraMorph(that));

    printf(
      "Nb available inputs: %ld\n",
      nbAvailInputs);

    // Precompute the values to speed up the training
    NMTrainerPrecomputeValues(that);

    // Get the output indices
    VecLong* iOutputs = NMGetVecIOutputs(NMTrainerNeuraMorph(that));

    // Declare a set to memorize the trained units
    GSet trainedUnits = GSetCreateStatic();

    // Set a flag to memorize if we are at the last depth
    bool isLastDepth = (iDepth == NMTrainerGetDepth(that));

    // Get the number of inputs per unit
    long nbMaxInputsUnit =
      MIN(
        nbAvailInputs,
        NMTrainerGetNbMaxInputsUnit(that));

    // Loop on the number of inputs for the new unit
    for (
      long nbUnitInputs = 1;
      nbUnitInputs <= nbMaxInputsUnit;
      ++nbUnitInputs) {

      printf(
        "Train units with %04ld input(s)\n",
        nbUnitInputs);

      // Loop on the possible input configurations for the new units
      VecLong* iInputs = VecLongCreate(nbUnitInputs);
      VecLong* iInputsBound = VecLongCreate(nbUnitInputs);
      VecSetAll(
        iInputsBound,
        nbAvailInputs);
      bool hasStepped = true;
      do {

        bool isValidInputConfig =
          NMTrainerIsValidInputConfig(
            iInputs,
            iMinInput);
        if (isValidInputConfig == true) {
if(GSetTail(&trainedUnits)){
VecPrint(iInputs,stdout);printf(" %f    \r", NMUnitGetValue(GSetTail(&trainedUnits)));
}
          // Train the unit
          NMTrainerTrainUnit(
            that,
            &trainedUnits,
            iInputs,
            iOutputs,
            (iDepth != 1));
            //isLastDepth);

          }

        // Step to the next input configuration
        hasStepped =
          VecStep(
            iInputs,
            iInputsBound);

      } while (hasStepped);

      // Free memory
      VecFree(&iInputs);
      VecFree(&iInputsBound);

    }

    // If this is the last depth
    if (isLastDepth == true) {

      // Add the best of all units to the NeuraMorph
      NeuraMorphUnit* bestUnit = GSetDrop(&trainedUnits);
      GSetAppend(
        &(NMTrainerNeuraMorph(that)->units),
        bestUnit);

      printf("Add the last unit\n");
      NMUnitPrintln(
        bestUnit,
        stdout);

      // Discard all other units
      while (GSetNbElem(&trainedUnits) > 0) {

        NeuraMorphUnit* unit = GSetPop(&trainedUnits);
        NeuraMorphUnitFree(&unit);

      }

    // Else, this is not the last depth
    } else {

      // Get the value of the weakest and strongest units
      float weakVal = GSetElemGetSortVal(GSetHeadElem(&trainedUnits));
      float strongVal = GSetElemGetSortVal(GSetTailElem(&trainedUnits));

      // Get the threshold to discard the weakest units
      float threshold =
        weakVal + (strongVal - weakVal) *
        NMTrainerGetWeakThreshold(that);

      // Discard the weakest units
      long nbTrainedUnits = GSetNbElem(&trainedUnits);
      while (
        GSetElemGetSortVal(GSetHeadElem(&trainedUnits)) < threshold
        || GSetNbElem(&trainedUnits) > NMTrainerGetNbMaxUnitDepth(that)) {

        NeuraMorphUnit* unit = GSetPop(&trainedUnits);
        NeuraMorphUnitFree(&unit);

      }

      // Displayed the burried units
      printf(
        "Burry %ld out of %ld unit(s)\n",
        GSetNbElem(&trainedUnits),
        nbTrainedUnits);
      GSetIterForward iter = GSetIterForwardCreateStatic(&trainedUnits);
      do {

        NeuraMorphUnit* unit = GSetIterGet(&iter);
        NMUnitPrintln(
          unit,
          stdout);

      } while (GSetIterStep(&iter));

      // Burry the remaining units
      NMBurryUnits(
        NMTrainerNeuraMorph(that),
        &trainedUnits);

    }

    // Update the minimum index of a valid configuration
    iMinInput = nbAvailInputs;

    // Free memory
    VecFree(&iOutputs);
    NMTrainerFreePrecomputed(that);

  }

}

// Return true if the vector 'v' is a valid indices configuration
// i.e. v[i]<v[j] for all i<j and there exists i such as
// v[i]>=iMinInput
bool NMTrainerIsValidInputConfig(
  const VecLong* v,
            long iMinInput) {

#if BUILDMODE == 0

  if (v == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'v' is null");
    PBErrCatch(NeuraMorphErr);

  }

#endif

  bool noveltyCond = false;
  long a =
    VecGet(
      v,
      0);
  if (a >= iMinInput) {

    noveltyCond = true;

  }

  for (
    long i = 1;
    i < VecGetDim(v);
    ++i) {

    long b =
      VecGet(
        v,
        i);
    if (a >= b) {

      return false;

    }

    a = b;

    /*if (a >= iMinInput) {

      noveltyCond = true;

    }*/

  }

  return noveltyCond;

}

// Train a new NeuraMorphUnit with the interface defined by 'iInputs'
// and 'iOutputs', and add it to the set, sorted on its value
// If 'lastUnit' is true, the NeuraMorphUnit will be the last one in
// its NeuraMorph
void NMTrainerTrainUnit(
  NeuraMorphTrainer* that,
               GSet* trainedUnits,
      const VecLong* iInputs,
      const VecLong* iOutputs,
                bool lastUnit) {

#if BUILDMODE == 0

  if (that == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'that' is null");
    PBErrCatch(NeuraMorphErr);

  }

  if (trainedUnits == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'trainedUnits' is null");
    PBErrCatch(NeuraMorphErr);

  }

  if (iInputs == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'iInputs' is null");
    PBErrCatch(NeuraMorphErr);

  }

  if (iOutputs == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'iOutputs' is null");
    PBErrCatch(NeuraMorphErr);

  }

#endif

  // Get the number of inputs
  long nbInputs = VecGetDim(iInputs);

  // Loop on the division levels
  // (None for the last unit)
  VecShort* curDivLvl = VecShortCreate(nbInputs);
  VecShort* divLvlBound = VecShortCreate(nbInputs);
  if (lastUnit == true) {

    VecSetAll(
      divLvlBound,
      0);

  } else {

    VecSetAll(
      divLvlBound,
      NMTrainerGetMaxLvlDiv(that));

  }

  bool flagStepDivLvl = true;
  do {

    // Get the bounds for the number of division for each input
    // at current levels
    VecShort* divBound = VecShortCreate(nbInputs);
    for (
      long iInput = nbInputs;
      iInput--;) {

      short lvl =
        VecGet(
          curDivLvl,
          iInput);
      short bound =
        powi(
          2,
          lvl);
      VecSet(
        divBound,
        iInput,
        bound);

    }

    // Loop on the combination of divisions
    VecShort* curDiv = VecShortCreate(nbInputs);
    bool flagStepDiv = true;
    do {

      // Create the unit
      NeuraMorphUnit* unit =
        NeuraMorphUnitCreate(
          iInputs,
          iOutputs);

      // Loop on the inputs of the unit
      for (
        long iInput = nbInputs;
        iInput--;) {

        // Get the indice of this input in the NeuraMorph
        short jInput =
          VecGet(
            NMUnitIInputs(unit),
            iInput);

        // Declare variables to memorize the lowest and highest
        // values for this input
        float low = 0.0;
        float high = 0.0;

        // If this input is an input in the NeuraMorph
        if (jInput < NMGetNbInput(NMTrainerNeuraMorph(that))) {

          low =
            VecGet(
              that->lowInputs,
              jInput);
          high =
            VecGet(
              that->highInputs,
              jInput);

        // Else, this input is an hidden value in the NeuraMorph
        } else {

          low =
            VecGet(
              NMLowHiddens(NMTrainerNeuraMorph(that)),
              jInput - NMGetNbInput(NMTrainerNeuraMorph(that)));
          high =
            VecGet(
              NMHighHiddens(NMTrainerNeuraMorph(that)),
              jInput - NMGetNbInput(NMTrainerNeuraMorph(that)));

        }

        // Get the filter values for the current division
        float lowFilter =
          low + (high - low) *
          (float)VecGet(
            curDiv,
            iInput) /
          (float)VecGet(
            divBound,
            iInput);
        float highFilter =
          low + (high - low) *
          (float)(VecGet(
            curDiv,
            iInput) + 1) /
          (float)VecGet(
            divBound,
            iInput);

        // Set the filter values in the unit
        VecSet(
          unit->lowFilters,
          iInput,
          lowFilter);
        VecSet(
          unit->highFilters,
          iInput,
          highFilter);

      }

      // Declare two GSets to extract the filtered samples
      GSetVecFloat trainingInputs = GSetVecFloatCreateStatic();
      GSetVecFloat trainingOutputs = GSetVecFloatCreateStatic();

      // Loop on the samples of the dataset
      long nbSample =
        GDSGetSizeCat(
          NMTrainerDataset(that),
          NMTrainerGetICatTraining(that));
      for (
        long iSample = 0;
        iSample < nbSample;
        ++iSample) {

        // Create the sample's inputs for this unit
        VecFloat* sampleInputs = VecFloatCreate(nbInputs);

        // If all the input values are within the bound of the unit
        bool flag = true;
        for (
          long iInput = nbInputs;
          flag && iInput--;) {

          float low =
            VecGet(
              unit->lowFilters,
              iInput);
          float high =
            VecGet(
              unit->highFilters,
              iInput);

          if (high - low < PBMATH_EPSILON) {

            low -= 1.0;
            high += 1.0;
            VecSet(
              unit->lowFilters,
              iInput,
              low);
            VecSet(
              unit->highFilters,
              iInput,
              high);

          }



          short jInput =
            VecGet(
              iInputs,
              iInput);
          float val =
            VecGet(
              that->preCompInp[iSample],
              jInput);
          if (
            val < low ||
            val > high) {

            flag = false;

          }

          // Simultaneously, scale the inputs values toward the unit
          // input space
          val = (val - low) / (high - low);
          VecSet(
            sampleInputs,
            iInput,
            val);

        }

        if (flag) {

          // Add this sample to the training set for the current unit
          GSetAppend(
            &trainingInputs,
            sampleInputs);
          GSetAppend(
            &trainingOutputs,
            that->preCompOut[iSample]);

        } else {

          // Free memory
          VecFree(&sampleInputs);

        }

      }

      // If we have enough samples to train the unit on the current
      // combination of divisions
      //if (GSetNbElem(&trainingInputs) >= NMUnitGetNbInputs(unit)) {
      long nbMinSample =
        powi(
          NMTrainerGetOrder(that) + 2,
          NMUnitGetNbInputs(unit));
      if (GSetNbElem(&trainingInputs) >= nbMinSample) {

        // Calculate the transfer function
        float bias = 0.0;
        unit->transfer =
          BBodyFromPointCloud(
            NMTrainerGetOrder(that),
            &trainingInputs,
            &trainingOutputs,
            &bias);

        // If we could calculate the transfer function
        if (unit->transfer != NULL) {

          // Set the value of the unit
          float corrRange =
            (float)GSetNbElem(&trainingInputs) /
            (float)GDSGetSizeCat(
              NMTrainerDataset(that),
              NMTrainerGetICatTraining(that));
          NMUnitSetValue(
            unit,
            -1.0 * bias / corrRange);

          // Add the unit to the set of trained units
          GSetAddSort(
            trainedUnits,
            unit,
            NMUnitGetValue(unit));

        // Else, we couldn't calculate the transfer function
        } else {

          // Free memory
          NeuraMorphUnitFree(&unit);

        }

      }

      // Free memory
      while (GSetNbElem(&trainingInputs) > 0) {

        VecFloat* v = GSetPop(&trainingInputs);
        VecFree(&v);

      }

      GSetFlush(&trainingOutputs);

      // Move to the next combination of divisions
      flagStepDiv =
        VecStep(
          curDiv,
          divBound);

    } while (flagStepDiv);

    // Free memory
    VecFree(&curDiv);
    VecFree(&divBound);

    // Move to the next division level
    flagStepDivLvl =
      VecStep(
        curDivLvl,
        divLvlBound);

  } while (flagStepDivLvl);

  // Free memory
  VecFree(&curDivLvl);
  VecFree(&divLvlBound);

}

// Precompute the values of the NeuraMorph for each sample of the
// GDataset for the NeuraMorphTrainer 'that'
void NMTrainerPrecomputeValues(NeuraMorphTrainer* that) {

#if BUILDMODE == 0

  if (that == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'that' is null");
    PBErrCatch(NeuraMorphErr);

  }

#endif

  // Get the number of samples
  long nbSample =
    GDSGetSizeCat(
      NMTrainerDataset(that),
      NMTrainerGetICatTraining(that));

  // Allocate memory
  that->preCompInp =
    PBErrMalloc(
      NeuraMorphErr,
      nbSample * sizeof(VecFloat*));
  that->preCompOut =
    PBErrMalloc(
      NeuraMorphErr,
      nbSample * sizeof(VecFloat*));

  // Reset the low and high values for input
  VecFree(&(that->lowInputs));
  VecFree(&(that->highInputs));

  // Get the size of the precomputed vector
  long sizeInp =
    NMGetNbInput(NMTrainerNeuraMorph(that)) +
    NMGetNbHidden(NMTrainerNeuraMorph(that));

  // Loop on the samples
  long iSample = 0;
  bool flagStep = true;
  GDSReset(
    NMTrainerDataset(that),
    NMTrainerGetICatTraining(that));
  do {

    // Get a clone of the sample's inputs
    VecFloat* inputs =
      GDSGetSampleInputs(
        NMTrainerDataset(that),
        NMTrainerGetICatTraining(that));

    // Update the low and high input values
    if (that->lowInputs == NULL) {

      that->lowInputs = VecClone(inputs);
      that->highInputs = VecClone(inputs);

    } else {

      for (
        long iInput = 0;
        iInput < VecGetDim(inputs);
        ++iInput) {

        float val =
          VecGet(
            inputs,
            iInput);

        float curLow =
          VecGet(
            that->lowInputs,
            iInput);
        if (curLow > val) {

          VecSet(
            that->lowInputs,
            iInput,
            val);

        }

        float curHigh =
          VecGet(
            that->highInputs,
            iInput);
        if (curHigh < val) {

          VecSet(
            that->highInputs,
            iInput,
            val);

        }

      }

    }

    // Get a clone of the sample's outputs
    that->preCompOut[iSample] =
      GDSGetSampleOutputs(
        NMTrainerDataset(that),
        NMTrainerGetICatTraining(that));

    // Run the NeuraMorph on the sample
    NMEvaluate(
      NMTrainerNeuraMorph(that),
      inputs);

    // Allocate memory for the precomputed vector
    that->preCompInp[iSample] = VecFloatCreate(sizeInp);

    // Copy the inputs and hidden values into the precomputed vector
    for (
      long i = NMGetNbInput(NMTrainerNeuraMorph(that));
      i--;) {

      float val =
        VecGet(
          NMInputs(NMTrainerNeuraMorph(that)),
          i);
      VecSet(
        that->preCompInp[iSample],
        i,
        val);

    }

    for (
      long i = NMGetNbHidden(NMTrainerNeuraMorph(that));
      i--;) {

      float val =
        VecGet(
          NMHiddens(NMTrainerNeuraMorph(that)),
          i);
      VecSet(
        that->preCompInp[iSample],
        i + NMGetNbInput(NMTrainerNeuraMorph(that)),
        val);

    }

    // Free memory
    VecFree(&inputs);

    // Move to the next sample
    ++iSample;
    flagStep =
      GDSStepSample(
        NMTrainerDataset(that),
        NMTrainerGetICatTraining(that));

  } while (flagStep);

  // Finally, update the hiddens bound if any
  if (NMGetNbHidden(NMTrainerNeuraMorph(that)) > 0) {

    NMUpdateLowHighHiddens(NMTrainerNeuraMorph(that));

  }

}

// Free the precomputed hidden values of the NeuraMorphTrainer 'that'
void NMTrainerFreePrecomputed(NeuraMorphTrainer* that) {

#if BUILDMODE == 0

  if (that == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'that' is null");
    PBErrCatch(NeuraMorphErr);

  }

#endif

  // If the hidden values are not precomputed
  if (that->preCompInp == NULL) {

    // Stop here
    return;

  }

  // Get the number of samples
  long nbSample =
    GDSGetSizeCat(
      NMTrainerDataset(that),
      NMTrainerGetICatTraining(that));

  // Free memory
  for (
    long iSample = nbSample;
    iSample--;) {

    VecFree(that->preCompInp + iSample);
    VecFree(that->preCompOut + iSample);

  }

  free(that->preCompInp);
  that->preCompInp = NULL;
  free(that->preCompOut);
  that->preCompOut = NULL;

}

// Run the evaluation process for the NeuraMorphTrainer 'that'
void NMTrainerEval(NeuraMorphTrainer* that) {

#if BUILDMODE == 0

  if (that == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'that' is null");
    PBErrCatch(NeuraMorphErr);

  }

#endif

  // Declare a variable to calculate the result of evaluation
  float minBias = 0.0;
  float avgBias = 0.0;
  float maxBias = 0.0;
  that->nbCorrect = 0;

  // Loop on the evaluation samples
  long iSample = 0;
  bool flagStep = true;
  GDSReset(
    NMTrainerDataset(that),
    NMTrainerGetICatEval(that));
  do {

    // Get a clone of the sample's inputs and outputs
    VecFloat* inputs =
      GDSGetSampleInputs(
        NMTrainerDataset(that),
        NMTrainerGetICatEval(that));
    VecFloat* outputs =
      GDSGetSampleOutputs(
        NMTrainerDataset(that),
        NMTrainerGetICatEval(that));

    // Run the NeuraMorph on the sample
    NMEvaluate(
      NMTrainerNeuraMorph(that),
      inputs);

    // Update the result of evaluation
    float bias =
      VecDist(
        outputs,
        NMOutputs(NMTrainerNeuraMorph(that)));
    avgBias += bias;
    if (iSample == 0) {

      minBias = bias;
      maxBias = bias;

    } else {

      minBias =
        MIN(
          bias,
          minBias);
      maxBias =
        MAX(
          bias,
          maxBias);

    }
    if (fabs(bias) < PBMATH_EPSILON) {

      ++(that->nbCorrect);

    }

    // Display the result
    /* if (NMTrainerStreamInfo(that) != NULL) {

      fprintf(
        NMTrainerStreamInfo(that),
        "%02ld ",
        iSample);
      VecPrint(
        inputs,
        NMTrainerStreamInfo(that));
      fprintf(
        NMTrainerStreamInfo(that),
        " -> ");
      VecPrint(
        outputs,
        NMTrainerStreamInfo(that));
      fprintf(
        NMTrainerStreamInfo(that),
        " : ");
      VecPrint(
        NMOutputs(NMTrainerNeuraMorph(that)),
        NMTrainerStreamInfo(that));
      fprintf(
        NMTrainerStreamInfo(that),
        " ");
      fprintf(
        NMTrainerStreamInfo(that),
        "%f\n",
        bias);

    } */

    // Free memory
    VecFree(&inputs);
    VecFree(&outputs);

    // Move to the next sample
    ++iSample;
    flagStep =
      GDSStepSample(
        NMTrainerDataset(that),
        NMTrainerGetICatEval(that));

  } while (flagStep);

  // Memorize the result of evaluation
  avgBias /=
    (float)GDSGetSizeCat(
      NMTrainerDataset(that),
      NMTrainerGetICatEval(that));
  VecSet(
    &(that->resEval),
    0,
    minBias);
  VecSet(
    &(that->resEval),
    1,
    avgBias);
  VecSet(
    &(that->resEval),
    2,
    maxBias);

}
