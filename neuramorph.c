// ============ NEURAMORPH.C ================

// ================= Include =================

#include "neuramorph.h"
#if BUILDMODE == 0
#include "neuramorph-inline.c"
#endif

// ----- NeuraMorphUnitBody

// ================ Functions implementation ====================

// Create a new NeuraMorphUnitBody for 'nbInputs' inputs
NeuraMorphUnitBody* NeuraMorphUnitBodyCreate(long nbInputs) {

#if BUILDMODE == 0

  if (nbInputs <= 0) {

    NeuraMorphErr->_type = PBErrTypeInvalidArg;
    sprintf(
      NeuraMorphErr->_msg,
      "'nbInputs' is invalid (%d>0)",
      nbInputs);
    PBErrCatch(NeuraMorphErr);

  }

#endif

  // Allocate memory for the NeuraMorphUnitBody
  NeuraMorphUnitBody* that =
    PBErrMalloc(
      NeuraMorphErr,
      sizeof(NeuraMorphUnitBody));

  that->lowFilters = VecFloatCreate(nbIn);
  that->highFilters = VecFloatCreate(nbIn);
  that->transfers = NULL;
  that->value = 0.0;

  // Return the new NeuraMorphUnitBody
  return that;

}

// Free the memory used by the NeuraMorphUnitBody 'that'
void NeuraMorphUnitBodyFree(NeuraMorphUnitBody** that) {

  // Check the input
  if (that == NULL || *that == NULL) {

    return;

  }

  // Free memory
  BBodyFree(&((*that)->transfer));
  VecFree(&((*that)->lowFilters));
  VecFree(&((*that)->highFilters));
  free(*that);
  *that = NULL;

}

// Check if the NeuraMorphUnitBody 'that' includes the 'inputs'
bool NMUnitBodyCheckInputs(
  const NeuraMorphUnitBody* that,
            const VecFloat* inputs) {

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

  if (VecGetDim(inputs) != VecGetDim(that->lowFilters)) {

    NeuraMorphErr->_type = PBErrTypeInvalidArg;
    sprintf(
      NeuraMorphErr->_msg,
      "'inputs' has invalid dimension (%ld!=%ld)",
      VecGetDim(inputs),
      VecGetDim(that->lowFilters));
    PBErrCatch(NeuraMorphErr);

  }

#endif

  // Loop on input values
  for (
    long iInput = VecGetDim(inputs);
    iInput--;) {

    // Get the value and lower and higher bounds
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

    // If the input value is out of bound
    if (
      val < low - PBMATH_EPSILON ||
      val > high + PBMATH_EPSILON) {

      // The body doesn't include the inputs
      return false;

    }

  }

  // If we reach here, the body includes the inputs
  return true;

}

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
  that->bodies = GSetCreateStatic();
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
  if ((*that)->lowOutputs != NULL) {

    VecFree(&((*that)->lowOutputs));

  }

  if ((*that)->highOutputs != NULL) {

    VecFree(&((*that)->highOutputs));

  }

  VecFree(&((*that)->outputs));
  while (GSetNbElem(NeuraMorphUnitBodies(*that)) > 0) {

    NeuraMorphUnitBody* body = GSetPop(NeuraMorphUnitBodies(*that));
    NeuraMorphUnitBodyFree(&(body->transfer));

  }

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

  if (inputs == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'inputs' is null");
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

  // Get the NeuraMorphUnitBody for the inputs in argument
  NeuraMorphUnitBody* body =
    NMUnitBody(
      that,
      inputs);

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
        body->lowFilters,
        iInput);
    float high =
      VecGet(
        body->highFilters,
        iInput);

    // Set the value in the unit inputs
    float valInput = 0.5;
    if (high - low > PBMATH_EPSILON) {

      valInput = (val - low) / (high - low);

    }

    VecSet(
      that->unitInputs,
      iInput,
      valInput);

  }

  // Apply the transfer function
  that->outputs =
    BBodyGet(
      body->transfer,
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

// Get the NeuraMorphUnitBody of the NeuraMorphUnit 'that' for the
// 'inputs', i.e. the first one whose filters include 'inputs'
NeuraMorphUnitBody* NMUnitBody(
  const NeuraMorphUnit* that,
        const VecFloat* inputs) {

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

#endif

  // Declare the variable to memorize the result body
  NeuraMorphUnitBody* body = NULL;

  // Loop on the bodies
  bool flagStep = true;
  GSetIterForward iter = GSetIterForwardCreateStatic(NMUnitBodies(that));
  do {

    // Get the body
    body = GSetIterGet(&iter);

    // If the body includes the inputs
    bool flagInclude =
      NMUnitBodyCheckInputs(
        body,
        inputs);
    if (flagInclude == true) {

      // Stop here
      flagStep = false;

    // Else, the body doesn't include the inputs
    } else {

      // Step to the next body
      flagStep = GSetIterStep(&iter);

    }

  } while (flagStep);

  // Return the result body
  return body;

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
    " val(%04.6f)",
    NMUnitGetValue(that));
fprintf(
  stream,
  " nbSample(%ld)",
  that->nbTrainingSample);

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
// If 'flagSubdiv' is true, search down through subdivisions of the
// inputs space
void NMTrainerTrainUnit(
  NeuraMorphTrainer* that,
               GSet* trainedUnits,
      const VecLong* iInputs,
      const VecLong* iOutputs,
                bool flagSubDiv);

// Precompute the values of the NeuraMorph for each sample of the
// GDataset for the NeuraMorphTrainer 'that'
void NMTrainerPrecomputeValues(NeuraMorphTrainer* that);

// Free the precomputed values of the NeuraMorphTrainer 'that'
void NMTrainerFreePrecomputed(NeuraMorphTrainer* that);

// Train recursively the bodies of the 'unit' on the 'samples'
// Return true if the training succeeded
bool NMTrainerTrainBody(
         NeuraMorphTrainer* that,
            NeuraMorphUnit* unit,
        NeuraMorphUnitBody* body,
  const NeuraMorphUnitBody* parentBody,
                        int curLvl,
                        int maxLvl,
                      GSet* samples);

// Calculate the transfer function for the 'body' using the
// precomputer 'pods'
void NMTrainerSearchBodyTransfer(
   NeuraMorphTrainer* that,
      NeuraMorphUnit* unit,
  NeuraMorphUnitBody* body,
                GSet* pods);

// Evaluate the NeuraMorphUnit 'that' on the samples in the 'pods'
// This is the average of ||truth-pred||
float NMUnitGetValueSamples(
    NeuraMorphUnit* that,
    GSet* pods);

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
  that.preComp = GSetCreateStatic();
  that.lowInputs = NULL;
  that.highInputs = NULL;
  that.streamInfo = NULL;
  that.resEval = VecFloatCreate(4);
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
  VecFree(&(that->resEval));
  while (GSetNbElem(NMTrainerPrecomp(that)) > 0) {

    NMPodInputOutput* pod = GSetPop(NMTrainerPrecomp(that));
    VecFree(&(pod->input));
    VecFree(&(pod->output));
    free(pod);

  }

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
VecPrint(iInputs,stderr);fprintf(stderr, " %f    \r", NMUnitGetValue(GSetTail(&trainedUnits)));
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

    if (a >= iMinInput) {

      noveltyCond = true;

    }

  }

  return noveltyCond;

}

// Train a new NeuraMorphUnit with the interface defined by 'iInputs'
// and 'iOutputs', and add it to the set, sorted on its value
// If 'flagSubdiv' is true, search down through subdivisions of the
// inputs space
void NMTrainerTrainUnit(
  NeuraMorphTrainer* that,
               GSet* trainedUnits,
      const VecLong* iInputs,
      const VecLong* iOutputs,
                bool flagSubDiv) {


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

  // Create the initial body including all the samples
  NeuraMorphUnitBody* body = NeuraMorphUnitBodyCreate(nbInputs);

  // Initialise the low and high filters of the body
  for (
    long iInput = nbInputs;
    iInput++;) {

    long jInput =
      VecGet(
        iInputs,
        iInput);
    float low =
      VecGet(
        that->lowInputs,
        jInput);
    float high =
      VecGet(
        that->highInputs,
        jInput);
    VecSet(
      body->lowFilters,
      jInput,
      low);
    VecSet(
      body->highFilters,
      jInput,
      high);

  }

  // Get the number of samples
  long nbSample =
    GDSGetSizeCat(
      NMTrainerDataset(that),
      NMTrainerGetICatTraining(that));

  // Get the max level of subdivision
  int nbMaxLvlSubDiv =
    flagSubDiv ? NMTrainerGetMaxLvlDiv(that) : 0;

  NeuraMorph* unit =
    NeuraMorphUnitCreate(
      iInputs,
      iOutputs);

  // Start the training of the bodies of the unit
  bool flagSuccess =
    NMTrainerTrainBody(
      that,
      unit,
      body,
      NULL,
      0,
      nbMaxLvlSubDiv,
      NMTrainerPrecomp(that));

  // If the training was succesfull
  if (flagSuccess == true) {

    // Evalutate the resulting unit
    float val =
      NMUnitGetValueSamples(
        unit,
        NMTrainerPrecomp(that));
    NMUnitSetValue(
      unit,
      val);

    // Add the new unit to the trained units
    GSetAddSort(
      trainedUnits,
      unit,
      NMUnitGetValue(unit));

  }

}

// Evaluate the NeuraMorphUnit 'that' on the samples in the 'pods'
// This is the average of ||truth-pred||
float NMUnitGetValueSamples(
    NeuraMorphUnit* that,
    GSet* pods) {

#if BUILDMODE == 0

  if (that == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'that' is null");
    PBErrCatch(NeuraMorphErr);

  }

  if (pods == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'pods' is null");
    PBErrCatch(NeuraMorphErr);

  }

#endif

  // Declare a variable to calculate the average bias
  float bias = 0.0;

  // Loop on the pods
  bool flagStep = true;
  GSetIterForward iter = GSetIterForwardCreateStatic(samples);
  do {

    // Get the pod
    NMPodInputOutput* pod = GSetIterGet(&iter);

    // Declare the inputs and outputs of the body
    VecFloat* inputs = VecCreate(nbInputs);
    VecFloat* outputs = VecCreate(nbOutputs);

    // Loop on the inputs of the body
    for (
      long iInput = nbInputs;
      iInput--;) {

      // Get the index of the input
      long jInput =
        VecGet(
          NMUnitIInputs(unit),
          iInput);

      // Get the value and filters for this input
      float low =
        VecGet(
          body->lowFilters,
          iInput);
      float high =
        VecGet(
          body->highFilters,
          iInput);
      float val =
        VecGet(
          pod->inputs,
          jInput);

      // Set the input value
      float valInput = 0.5;
      if (high - low > PBMATH_EPSILON) {

        valInput = (val - low) / (high - low);

      }

      VecSet(
        inputs,
        iInput,
        valInput);

    }

    // Loop on the outputs of the body
    for (
      long iOutput = nbOutputs;
      iOutput--;) {

      // Get the index of the output
      long jOutput =
        VecGet(
          NMUnitIOutputs(unit),
          iOutput);
      jOutput -= NMGetNbHidden(NMTrainerNeuraMorph(that));

      // Get the value of this output
      float val =
        VecGet(
          pod->outputs,
          jOutput);

      // Set the output value
      VecSet(
        outputs,
        iOutput,
        val);

    }

    // Evaluate the unit
    NMUnitEvaluate(
      that,
      inputs);

    // Update the bias
    bias +=
      VecDist(
      outputs,
      NMUnitOutputs(that));

    // Free memory
    VecFree(&inputs);
    VecFree(&outputs);

    // Step to the next sample
    flagStep = GSetIterStep(&iter);

  } while (flagStep);

  // Calculate the value
  float value = -1.0 * bias / (float)GSetNbElem(pods);

  // Return the value
  return value;

}

// Train recursively the bodies of the 'unit' on the 'samples'
// Return true if the training succeeded
bool NMTrainerTrainBody(
         NeuraMorphTrainer* that,
            NeuraMorphUnit* unit,
        NeuraMorphUnitBody* body,
  const NeuraMorphUnitBody* parentBody,
                        int curLvl,
                        int maxLvl,
                      GSet* samples) {

#if BUILDMODE == 0

  if (that == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'that' is null");
    PBErrCatch(NeuraMorphErr);

  }

  if (unit == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'unit' is null");
    PBErrCatch(NeuraMorphErr);

  }

  if (body == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'body' is null");
    PBErrCatch(NeuraMorphErr);

  }

  if (samples == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'samples' is null");
    PBErrCatch(NeuraMorphErr);

  }

#endif

  // Get the number of inputs
  long nbInputs = NMUnitGetNbInputs(unit);

  // Declare a variable to memorize the success of training
  bool flagSuccess = true;

  // Create a GSet to filter the samples with the filters of the body
  GSet filteredSamples = GSetCreate();

  // Loop on the samples
  bool flagStep = true;
  GSetIterForward iter = GSetIterForwardCreateStatic(samples);
  do {

    // Get the pod
    NMPodInputOutput* pod = GSetIterGet(&iter);

    // Get the inputs of the pod
    VecFloat* inputs = pod->inputs;

    // Declare a flag to filter the sample
    bool flagFilter = true;

    // Loop on the inputs of the body
    for (
      long iInput = nbInputs;
      iInput--;) {

      // Get the index of the input
      long jInput =
        VecGet(
          iInputs,
          iInput);

      // Get the value and filters for this input
      float low =
        VecGet(
          body->lowFilters,
          iInput);
      float high =
        VecGet(
          body->highFilters,
          iInput);
      float val =
        VecGet(
          inputs,
          jInput);

      // Check the filter
      if (
        val < low - PBMATH_EPSILON ||
        val > high + PBMATH_EPSILON) {

        flagFilter = false;

      }

    }

    // If the input matches the filter
    if (flagFilter) {

      //  Add the pod to the filtered samples
      GSetAppend(
        &filteredSamples,
        pod);

    }

    // Step to the next sample
    flagStep = GSetIterStep(&iter);

  } while (flagStep);

  // If there is enough samples to train the body
  long nbMinSample =
    powi(
      NMTrainerGetOrder(that) + 2,
      NMUnitGetNbInputs(unit) + 1);
  if (GSetNbElem(&filteredSamples) >= nbMinSample) {

    // Calculate the transfer function for the body
    NMTrainerSearchBodyTransfer(
      that,
      unit,
      body,
      filteredSamples);

    // If we could calculate the transfer function
    if (body->transfer != NULL) {

      // If this body is better than its parent
      if (
        parentBody == NULL ||
        body->value > parentBody->value) {

        // Add the body to the unit
        GSetPush(
          NMUnitBodies(unit),
          body);

        // If we are not at the maximum subdivision level
        if (curLvl < maxLvl) {

          // Get the number of bodies at sub level
          long nbBodies =
            powi(
              2,
              NMUnitGetNbInputs(unit));

          // Allocate memory for bodies at sub level
          NMUnitBody** bodies =
            PBErrMalloc(
              NeuraMorphErr,
              nbBodies * sizeof(NMUnitBody*));

          // Set the filters of bodies at sub level
          VecShort* subDiv = VecShortCreate(NMUnitGetNbInputs(unit));
          VecShort* boundsDiv = VecShortCreate(NMUnitGetNbInputs(unit));
          VecSetAll(
            boundsDiv,
            2);
          bool flagStepDiv = true;
          do {

            // Loop on the filter values
            for (
              long iFilter = nbInputs;
              iFilter--;) {

              // Get the low and high filter of the current body
              float low =
                VecGet(
                  body->lowFilters,
                  iFilter);
              float high =
                VecGet(
                  body->highFilters,
                  iFilter);

              // Get the subdivision indice
              long iDiv =
                VecGet(
                  subDiv,
                  iFilter);

              // Set the low and high filter of the subbody
              // according to the division
              VecSet(
                bodies[iBody]->lowFilters,
                iFilter,
                low + ((float)iDiv) * 0.5 * (high - low));
              VecSet(
                bodies[iBody]->highFilters,
                iFilter,
                high - ((float)(1 - iDiv)) * 0.5 * (high - low));

            }

            // Step to the next div
            flagStepDiv =
              VecStep(
                subDiv,
                boundsDiv);

          } while (flagStepDiv);
 
          // Free memory
          VecFree(&subDiv);
          VecFree(&boundsDiv);

          // Train the bodies
          for (
            long iBody = nbBodies;
            iBody--;) {

            bool flagSuccessSubBody =
              NMTrainerTrainBody(
                that,
                unit,
                bodies[iBody],
                body,
                curLvl + 1,
                maxLvl,
                filteredSamples);

            // If the training failed
            if (flagSuccessSubBody == false) {

              // Free memory
              NeuraMorphUnitBodyFree(bodies + iBody);

            }

          }

          // Free memory
          free(bodies);

        }

      // Else this body is not better than its parent
      } else {

        // Update the success flag
        flagSuccess = false;

      }

    // Else, we could not calculate the transfer function
    } else {

      // Update the success flag
      flagSuccess = false;

    }

  // Else, there is not enough samples to train the body
  } else {

    // Update the flag
    flagSuccess = false;

  }

  // Free memory
  GSetFlush(&filteredSamples);

  // Return the flag for success of training
  return flagSuccess;

}

// Calculate the transfer function for the 'body' using the
// precomputer 'pods'
void NMTrainerSearchBodyTransfer(
   NeuraMorphTrainer* that,
      NeuraMorphUnit* unit,
  NeuraMorphUnitBody* body,
                GSet* pods) {

#if BUILDMODE == 0

  if (that == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'that' is null");
    PBErrCatch(NeuraMorphErr);

  }

  if (unit == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'unit' is null");
    PBErrCatch(NeuraMorphErr);

  }

  if (body == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'body' is null");
    PBErrCatch(NeuraMorphErr);

  }

  if (pods == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'pods' is null");
    PBErrCatch(NeuraMorphErr);

  }

#endif

  // Get the number of inputs
  long nbInputs = NMUnitGetNbInputs(unit);
  long nbOutputs = NMUnitGetNbOutputs(unit);

  // Declare the sets in argument of BBodyFromPointCloud
  GSet trainingInputs = GSetCreateStatic();
  GSet trainingOutputs = GSetCreateStatic();

  // Loop on the pods
  bool flagStep = true;
  GSetIterForward iter = GSetIterForwardCreateStatic(pods);
  do {

    // Get the pod
    NMPodInputOutput* pod = GSetIterGet(&iter);

    // Declare the inputs and outputs of the body
    VecFloat* inputs = VecCreate(nbInputs);
    VecFloat* outputs = VecCreate(nbOutputs);

    // Loop on the inputs of the body
    for (
      long iInput = nbInputs;
      iInput--;) {

      // Get the index of the input
      long jInput =
        VecGet(
          NMUnitIInputs(unit),
          iInput);

      // Get the value and filters for this input
      float low =
        VecGet(
          body->lowFilters,
          iInput);
      float high =
        VecGet(
          body->highFilters,
          iInput);
      float val =
        VecGet(
          pod->inputs,
          jInput);

      // Set the input value
      float valInput = 0.5;
      if (high - low > PBMATH_EPSILON) {

        valInput = (val - low) / (high - low);

      }

      VecSet(
        inputs,
        iInput,
        valInput);

    }

    // Loop on the outputs of the body
    for (
      long iOutput = nbOutputs;
      iOutput--;) {

      // Get the index of the output
      long jOutput =
        VecGet(
          NMUnitIOutputs(unit),
          iOutput);
      jOutput -= NMGetNbHidden(NMTrainerNeuraMorph(that));

      // Get the value of this output
      float val =
        VecGet(
          pod->outputs,
          jOutput);

      // Set the output value
      VecSet(
        outputs,
        iOutput,
        val);

    }

    // Add the inputs and outputs to the arguments of BBodyFromPointCloud
    GSetAppend(
      &trainingInputs,
      inputs);
    GSetAppend(
      &trainingOutputs,
      outputs);

    // Step to the next sample
    flagStep = GSetIterStep(&iter);

  } while (flagStep);

  // Calculate the transfer function
  float bias = 0.0;
  body->transfer =
    BBodyFromPointCloud(
      NMTrainerGetOrder(that),
      &trainingInputs,
      &trainingOutputs,
      &bias);

  // If we could calculate the transfer function
  if (unit->transfer != NULL) {

    // Set the value of the body
    body->value =  -1.0 * bias;

  }

  // Free memory
  while (GSetNbElem(&trainingInputs) > 0) {

    VecFloat* v = GSetPop(&trainingInputs);
    VecFree(&v);
    v = GSetPop(&trainingOutputs);
    VecFree(&v);

  }

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

  // Free memory
  while (GSetNbElem(NMTrainerPrecomp(that)) > 0) {

    NMPodInputOutput* pod = GSetPop(NMTrainerPrecomp(that));
    VecFree(&(pod->input));
    VecFree(&(pod->output));
    free(pod);

  }

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

    // Run the NeuraMorph on the sample
    NMEvaluate(
      NMTrainerNeuraMorph(that),
      inputs);

    // Create a pod for the sample
    NMPodInputOutput* pod =
      PBErrMalloc(
        NeuraMorphErr,
        sizeof(NMPodInputOutput));

    // Add the pod to the precomputed values
    GSetAppend(
      NMTrainerPrecomp(that),
      pod);

    // Get a clone of the sample's outputs
    pod->output =
      GDSGetSampleOutputs(
        NMTrainerDataset(that),
        NMTrainerGetICatTraining(that));

    // Allocate memory for the precomputed input
    pod->input = VecFloatCreate(sizeInp);

    // Copy the inputs and hidden values into the precomputed vector
    for (
      long i = NMGetNbInput(NMTrainerNeuraMorph(that));
      i--;) {

      float val =
        VecGet(
          NMInputs(NMTrainerNeuraMorph(that)),
          i);
      VecSet(
        pod->input,
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
        pod->input,
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

  // Declare variables to calculate the result of evaluation
  float minBias = 0.0;
  float avgBias = 0.0;
  float maxBias = 0.0;
  float sigmaBias = 0.0;
  that->nbCorrect = 0;
  long nbSample =
    GDSGetSizeCat(
      NMTrainerDataset(that),
      NMTrainerGetICatEval(that));
  float* biases =
    PBErrMalloc(
      NeuraMorphErr,
      sizeof(float) * nbSample);

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
    biases[iSample] = bias;
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

  // Calculate the mean and standard deviation
  avgBias /= (float)nbSample;
  for (
    iSample = nbSample;
    iSample--;) {

    sigmaBias +=
     powi(
       biases[iSample] - avgBias,
       2);

  }
  sigmaBias /= (float)(nbSample - 1);
  sigmaBias = sqrt(sigmaBias);

  // Memorize the result of evaluation
  VecSet(
    that->resEval,
    0,
    minBias);
  VecSet(
    that->resEval,
    1,
    avgBias);
  VecSet(
    that->resEval,
    2,
    sigmaBias);
  VecSet(
    that->resEval,
    3,
    maxBias);

  // Free memory
  free(biases);

}
