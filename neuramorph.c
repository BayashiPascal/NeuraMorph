// ============ NEURAMORPH.C ================

// ================= Include =================

#include "neuramorph.h"
#if BUILDMODE == 0
#include "neuramorph-inline.c"
#endif

// ----- NeuraMorphUnit

// ================ Functions declaration ====================

// Return the number of coefficients of a NeuraMorphUnit having 'nbIn' inputs
long NMUnitGetNBCoeff(long nbIn);

// Get the coefficient for the pair of inputs 'iInputA', 'iInputB' in the
// NeuraMorphUnit 'that' for the output 'iOutput'
float NMUnitGetCoeff(
  const NeuraMorphUnit* that,
                   long iInputA,
                   long iInputB,
                   long iOutput);

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
  long nbIn = VecGetDim(iInputs) + 1;
  long nbOut = VecGetDim(iOutputs);

  // Init properties
  that->iInputs = VecClone(iInputs);
  that->iOutputs = VecClone(iOutputs);
  that->lowFilters = VecFloatCreate(nbIn);
  that->highFilters = VecFloatCreate(nbIn);
  that->lowOutputs = NULL;
  that->highOutputs = NULL;
  that->outputs = VecFloatCreate(nbOut);
  that->coeffs =
    PBErrMalloc(
      NeuraMorphErr,
      sizeof(VecFloat*) * nbOut);
  long nbCoeff = NMUnitGetNBCoeff(nbIn);
  for (
    long iOut = nbOut;
    iOut--;
    that->coeffs[iOut] = VecFloatCreate(nbCoeff));

  // 'nbIn + 1' for the constant
  that->activeInputs =
    PBErrMalloc(
      NeuraMorphErr,
      sizeof(bool) * nbIn);
  that->scaledInputs = VecFloatCreate(nbIn);

  // Set the input value, filters and active flag for the constant
  VecSet(
    that->scaledInputs,
    0,
    1.0);
  that->activeInputs[0] = true;

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
  long nbOut = VecGetDim((*that)->iOutputs);
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
  for (
    long iOut = nbOut;
    iOut--;
    VecFree((*that)->coeffs + iOut));
  free((*that)->coeffs);
  free((*that)->activeInputs);
  VecFree(&((*that)->scaledInputs));
  free(*that);
  *that = NULL;

}

// Return the number of coefficients of a NeuraMorphUnit having 'nbIn' inputs
long NMUnitGetNBCoeff(long nbIn) {

#if BUILDMODE == 0

  if (nbIn <= 0) {

    NeuraMorphErr->_type = PBErrTypeInvalidArg;
    sprintf(
      NeuraMorphErr->_msg,
      "'nbIn' is invalid (%ld>0)",
      nbIn);
    PBErrCatch(NeuraMorphErr);

  }

#endif

  // Declare a variable to memorise the result
  long nb = 0;

  // Calculate the number of values in the triangular matrix of size
  // nbIn
  for (
    long i = nbIn;
    i >= 0;
    nb += (i--));

  // Return the result
  return nb;

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
  VecSetNull(that->outputs);

  // Update the active flags  and scaled inputs (skip the constant)
  for (
    long iInput = 1;
    iInput < VecGetDim(that->scaledInputs);
    ++iInput) {

    // Get the input value and its low/high filters
    float val =
      VecGet(
        inputs,
        iInput - 1);
    float low =
      VecGet(
        that->lowFilters,
        iInput);
    float high =
      VecGet(
        that->highFilters,
        iInput);

    // If the value is inside the filter
    if (
      low <= val &&
      val <= high &&
      (high - low) > PBMATH_EPSILON) {

      // Set this value as active
      that->activeInputs[iInput] = true;

      // Scale the value according to the filter
      float scaled = 2.0 * (val - low) / (high - low) - 1.0;
      VecSet(
        that->scaledInputs,
        iInput,
        scaled);

    // Else the value is outside the filter
    } else {

      // Set this value as inactive
      that->activeInputs[iInput] = false;

    }

  }

  // Loop on the pair of active inputs
  for (
    long iInputA = 0;
    iInputA < VecGetDim(that->scaledInputs);
    ++iInputA) {

    if (that->activeInputs[iInputA] == true) {

      for (
        long iInputB = 0;
        iInputB <= iInputA;
        ++iInputB) {

        if (that->activeInputs[iInputB] == true) {

          // Loop on the outputs
          for (
            long iOutput = 0;
            iOutput < VecGetDim(that->outputs);
            ++iOutput) {

            // Calculate the components for this output and pair of inputs
            float comp =
              VecGet(
                that->scaledInputs,
                iInputA) *
              VecGet(
                that->scaledInputs,
                iInputB) *
              NMUnitGetCoeff(
                that,
                iInputA,
                iInputB,
                iOutput);

            // Add the component to the output
            float cur =
              VecGet(
                that->outputs,
                iOutput);
            VecSet(
              that->outputs,
              iOutput,
              cur + comp);

          }

        }

      }

    }

  }

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

// Get the coefficient for the pair of inputs 'iInputA', 'iInputB' in the
// NeuraMorphUnit 'that' for the output 'iOutput'
float NMUnitGetCoeff(
  const NeuraMorphUnit* that,
                   long iInputA,
                   long iInputB,
                   long iOutput) {

#if BUILDMODE == 0

  if (that == NULL) {

    NeuraMorphErr->_type = PBErrTypeNullPointer;
    sprintf(
      NeuraMorphErr->_msg,
      "'that' is null");
    PBErrCatch(NeuraMorphErr);

  }

  if (
    iInputA < 0 ||
    iInputA >= VecGetDim(that->scaledInputs)) {

    NeuraMorphErr->_type = PBErrTypeInvalidArg;
    sprintf(
      NeuraMorphErr->_msg,
      "'iInputA' is invalid (0<=%ld<%ld)",
      iInputA,
      VecGetDim(that->scaledInputs));
    PBErrCatch(NeuraMorphErr);

  }

  if (
    iInputB < 0 ||
    iInputB >= VecGetDim(that->scaledInputs)) {

    NeuraMorphErr->_type = PBErrTypeInvalidArg;
    sprintf(
      NeuraMorphErr->_msg,
      "'iInputB' is invalid (0<=%ld<%ld)",
      iInputB,
      VecGetDim(that->scaledInputs));
    PBErrCatch(NeuraMorphErr);

  }

  if (iInputA < iInputB) {

    NeuraMorphErr->_type = PBErrTypeInvalidArg;
    sprintf(
      NeuraMorphErr->_msg,
      "The pair of indices is invalid (%ld>=%ld)",
      iInputA,
      iInputB);
    PBErrCatch(NeuraMorphErr);

  }

  if (
    iOutput < 0 ||
    iOutput >= VecGetDim(that->outputs)) {

    NeuraMorphErr->_type = PBErrTypeInvalidArg;
    sprintf(
      NeuraMorphErr->_msg,
      "'iInputB' is invalid (0<=%ld<%ld)",
      iInputB,
      VecGetDim(that->outputs));
    PBErrCatch(NeuraMorphErr);

  }

#endif

  // Calculate the index of the coefficient
  long iCoeff = 0;
  for (
    long shift = 0;
    shift < iInputA;
    iCoeff += (shift++) + 1);
  iCoeff += iInputB;

  // Return the coefficient
  float coeff =
    VecGet(
      that->coeffs[iOutput],
      iCoeff);
  return coeff;

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
  that->units = GSetCreateStatic();

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

  }

  while (GSetNbElem(&((*that)->units)) > 0) {

    NeuraMorphUnit* unit = GSetPop(&((*that)->units));
    NeuraMorphUnitFree(&unit);

  }

  free(*that);
  *that = NULL;

}
