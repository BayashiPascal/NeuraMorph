#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include "neuramorph.h"

void UnitTestNeuraMorphUnitCreateFree() {

  VecLong* iIn = VecLongCreate(3);
  VecSet(
    iIn,
    0,
    0);
  VecSet(
    iIn,
    1,
    1);
  VecSet(
    iIn,
    2,
    2);
  VecLong* iOut = VecLongCreate(2);
  VecSet(
    iOut,
    0,
    0);
  VecSet(
    iOut,
    1,
    1);
  NeuraMorphUnit* unit =
    NeuraMorphUnitCreate(
      iIn,
      iOut);
  if (
    VecGetDim(unit->coeffs[0]) != 10 ||
    VecGetDim(unit->outputs) != 2 ||
    VecGetDim(unit->lowFilters) != 4 ||
    VecGetDim(unit->highFilters) != 4 ||
    VecGetDim(unit->scaledInputs) != 4) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NeuraMorphUnitCreate failed (1)");
    PBErrCatch(NeuraMorphErr);

  }

  bool isSame =
    VecIsEqual(
      unit->iInputs,
      iIn);
  if (isSame == false) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NeuraMorphUnitCreate failed (2)");
    PBErrCatch(NeuraMorphErr);

  }

  isSame =
    VecIsEqual(
      unit->iOutputs,
      iOut);
  if (isSame == false) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NeuraMorphUnitCreate failed (3)");
    PBErrCatch(NeuraMorphErr);

  }

  NeuraMorphUnitFree(&unit);
  if (unit != NULL) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NeuraMorphUnitFree failed");
    PBErrCatch(NeuraMorphErr);

  }

  VecFree(&iIn);
  VecFree(&iOut);
  printf("UnitTestNeuraMorphUnitCreateFree OK\n");

}

void UnitTestNeuraMorphUnitGetSet() {

  VecLong* iIn = VecLongCreate(3);
  VecLong* iOut = VecLongCreate(2);
  NeuraMorphUnit* unit =
    NeuraMorphUnitCreate(
      iIn,
      iOut);

  if (NMUnitIInputs(unit) != unit->iInputs) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NMUnitIInputs failed");
    PBErrCatch(NeuraMorphErr);

  }

  if (NMUnitIOutputs(unit) != unit->iOutputs) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NMUnitIOutputs failed");
    PBErrCatch(NeuraMorphErr);

  }

  if (NMUnitOutputs(unit) != unit->outputs) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NMUnitOutputs failed");
    PBErrCatch(NeuraMorphErr);

  }

  NeuraMorphUnitFree(&unit);
  VecFree(&iIn);
  VecFree(&iOut);
  printf("UnitTestNeuraMorphUnitGetSet OK\n");

}

void UnitTestNeuraMorphUnitEvaluate() {

  VecLong* iIn = VecLongCreate(3);
  VecLong* iOut = VecLongCreate(2);
  NeuraMorphUnit* unit =
    NeuraMorphUnitCreate(
      iIn,
      iOut);

  for (
    long iInput = 3;
    iInput--;) {

    VecSet(
      unit->lowFilters,
      iInput + 1,
      0.0);
    VecSet(
      unit->highFilters,
      iInput + 1,
      2.0);

  }

  // iOutput == 0 -> 1.0+x+y+z+x^2+xy+xz+y^2+yz+z^2
  // iOutput == 1 -> x^2-xy+2xz+3y^2-4yz+5z^2
  float coeffs[2][10] = {

    { 1.0, 1.0,  1.0, 1.0,  1.0,  1.0, 1.0,  1.0,  1.0, 1.0},
    { 0.0, 0.0,  1.0, 0.0, -1.0,  3.0, 0.0,  2.0, -4.0, 5.0}

  };
  for (
    long iOutput = 2;
    iOutput--;) {

    for (
      long iCoeff = 10;
      iCoeff--;) {

      VecSet(
        unit->coeffs[iOutput],
        iCoeff,
        coeffs[iOutput][iCoeff]);

    }

  }

  VecFloat* inputs = VecFloatCreate(3);
  VecSet(
    inputs,
    0,
    1.0);
  VecSet(
    inputs,
    1,
    3.0);
  VecSet(
    inputs,
    2,
    1.5);

  NMUnitEvaluate(
    unit,
    inputs);

  float check[2];
  float x = 2.0 * (1.0 - 0.0) / (2.0 - 0.0) - 1.0;
  float y = 0.0; //2.0 * (3.0 - 0.0) / (2.0 - 0.0) - 1.0;
  float z = 2.0 * (1.5 - 0.0) / (2.0 - 0.0) - 1.0;
  check[0] = 1.0 + x + y + z + x * x + x * y + x * z + y * y + y * z + z * z;
  check[1] =
    x * x - x * y + 2.0 * x * z + 3.0 * y * y - 4.0 * y * z + 5.0 * z * z;
  for (
    long iOutput = 2;
    iOutput--;) {

    float v =
      VecGet(
        unit->outputs,
        iOutput);
    bool same =
      ISEQUALF(
        v,
        check[iOutput]);
    if (same == false) {

      NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
      sprintf(
        NeuraMorphErr->_msg,
        "NMUnitEvaluate failed");
      PBErrCatch(NeuraMorphErr);

    }

  }

  NeuraMorphUnitFree(&unit);
  VecFree(&iIn);
  VecFree(&iOut);
  VecFree(&inputs);
  printf("UnitTestNeuraMorphUnitEvaluate OK\n");

}

void UnitTestNeuraMorphUnit() {

  UnitTestNeuraMorphUnitCreateFree();
  UnitTestNeuraMorphUnitGetSet();
  UnitTestNeuraMorphUnitEvaluate();
  printf("UnitTestNeuraMorphUnit OK\n");

}

void UnitTestAll() {

  UnitTestNeuraMorphUnit();
  printf("UnitTestAll OK\n");

}

int main() {

  UnitTestAll();

  // Return success code
  return 0;

}
