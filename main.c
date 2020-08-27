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
    VecGetDim(unit->scaledInputs) != 4 ||
    unit->lowOutputs != NULL ||
    unit->highOutputs != NULL) {

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
  VecFloat2D checkHigh = VecFloatCreateStatic2D();
  VecSet(
    &checkHigh,
    0,
    check[0]);
  VecSet(
    &checkHigh,
    1,
    check[1]);
  VecFloat2D checkLow = checkHigh;
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
        "NMUnitEvaluate failed (1)");
      PBErrCatch(NeuraMorphErr);

    }

  }

  bool sameLow =
    VecIsEqual(
      &checkLow,
      unit->lowOutputs);
  bool sameHigh =
    VecIsEqual(
      &checkHigh,
      unit->highOutputs);
  if (
    sameLow == false ||
    sameHigh == false) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NMUnitEvaluate failed (2)");
    PBErrCatch(NeuraMorphErr);

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

void UnitTestNeuraMorphCreateFree() {

  NeuraMorph* nm =
    NeuraMorphCreate(
      3,
      2);
  if (
    nm->nbInput != 3 ||
    nm->nbOutput != 2 ||
    VecGetDim(nm->inputs) != 3 ||
    VecGetDim(nm->outputs) != 2 ||
    nm->hiddens != NULL ||
    GSetNbElem(&(nm->units)) != 0) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NeuraMorphCreate failed");
    PBErrCatch(NeuraMorphErr);

  }

  NeuraMorphFree(&nm);
  if (nm != NULL) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NeuraMorphFree failed");
    PBErrCatch(NeuraMorphErr);

  }

  printf("UnitTestNeuraMorphCreateFree OK\n");

}

void UnitTestNeuraMorphGetSet() {

  NeuraMorph* nm =
    NeuraMorphCreate(
      3,
      2);
  if (NMGetNbInput(nm) != 3) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NMGetNbInput failed");
    PBErrCatch(NeuraMorphErr);

  }

  if (NMGetNbOutput(nm) != 2) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NMGetNbOutput failed");
    PBErrCatch(NeuraMorphErr);

  }

  if (NMGetNbHidden(nm) != 0) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NMGetNbHidden failed");
    PBErrCatch(NeuraMorphErr);

  }

  NMSetNbHidden(
    nm,
    5);
  if (NMGetNbHidden(nm) != 5) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NMSetNbHidden failed");
    PBErrCatch(NeuraMorphErr);

  }

  if (NMInputs(nm) != nm->inputs) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NMInputs failed");
    PBErrCatch(NeuraMorphErr);

  }

  if (NMOutputs(nm) != nm->outputs) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NMOutputs failed");
    PBErrCatch(NeuraMorphErr);

  }

  NeuraMorphFree(&nm);

  printf("UnitTestNeuraMorphGetSet OK\n");

}

void UnitTestNeuraMorphAddRemoveUnit() {

  VecLong3D iInputs = VecLongCreateStatic3D();
  VecSet(
    &iInputs,
    0,
    0);
  VecSet(
    &iInputs,
    1,
    1);
  VecSet(
    &iInputs,
    2,
    2);
  VecLong2D iOutputs = VecLongCreateStatic2D();
  VecSet(
    &iOutputs,
    0,
    0);
  VecSet(
    &iOutputs,
    1,
    1);

  NeuraMorph* nm =
    NeuraMorphCreate(
      3,
      2);

  NeuraMorphUnit* unit =
    NMAddUnit(
      nm,
      (VecLong*)&iInputs,
      (VecLong*)&iOutputs);

  bool isSameA =
    VecIsEqual(
      &iInputs,
      unit->iInputs);
  bool isSameB =
    VecIsEqual(
      &iOutputs,
      unit->iOutputs);
  if (
    GSetNbElem(&(nm->units)) != 1 ||
    GSetHead(&(nm->units)) != unit ||
    isSameA == false ||
    isSameB == false) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NMAddUnit failed");
    PBErrCatch(NeuraMorphErr);

  }

  NeuraMorphFree(&nm);

  nm =
    NeuraMorphCreate(
      3,
      2);

  unit =
    NMAddUnit(
      nm,
      (VecLong*)&iInputs,
      (VecLong*)&iOutputs);

  NMRemoveUnit(
    nm,
    unit);

  if (GSetNbElem(&(nm->units)) != 0) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NMRemoveUnit failed");
    PBErrCatch(NeuraMorphErr);

  }

  NeuraMorphUnitFree(&unit);
  NeuraMorphFree(&nm);

  printf("UnitTestNeuraMorphAddRemoveUnit OK\n");

}

void UnitTestNeuraMorphBurryUnits() {

  VecLong3D iInputs = VecLongCreateStatic3D();
  VecSet(
    &iInputs,
    0,
    0);
  VecSet(
    &iInputs,
    1,
    1);
  VecSet(
    &iInputs,
    2,
    2);
  VecLong2D iOutputs = VecLongCreateStatic2D();
  VecSet(
    &iOutputs,
    0,
    0);
  VecSet(
    &iOutputs,
    1,
    1);

  NeuraMorph* nm =
    NeuraMorphCreate(
      3,
      2);

  NeuraMorphUnit* unitA =
    NeuraMorphUnitCreate(
      (VecLong*)&iInputs,
      (VecLong*)&iOutputs);

  NeuraMorphUnit* unitB =
    NeuraMorphUnitCreate(
      (VecLong*)&iInputs,
      (VecLong*)&iOutputs);

  GSet units = GSetCreateStatic();
  GSetAppend(
    &units,
    unitA);
  GSetAppend(
    &units,
    unitB);

  NMBurryUnits(
    nm,
    &units);

  if (
    GSetNbElem(&units) != 0 ||
    nm->hiddens == NULL ||
    VecGetDim(nm->hiddens) != 4) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NMBurryUnits failed");
    PBErrCatch(NeuraMorphErr);

  }

  VecLong2D checkA = VecLongCreateStatic2D();
  VecSet(
    &checkA,
    0,
    0);
  VecSet(
    &checkA,
    1,
    1);
  VecLong2D checkB = VecLongCreateStatic2D();
  VecSet(
    &checkB,
    0,
    2);
  VecSet(
    &checkB,
    1,
    3);

  bool isSameA =
    VecIsEqual(
      &checkA,
      unitA->iOutputs);
  bool isSameB =
    VecIsEqual(
      &checkB,
      unitB->iOutputs);
  if (
    isSameA == false ||
    isSameB == false) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NMBurryUnits failed");
    PBErrCatch(NeuraMorphErr);

  }

  NeuraMorphFree(&nm);

  printf("UnitTestNeuraMorphBurryUnits OK\n");

}

void UnitTestNeuraMorph() {

  UnitTestNeuraMorphCreateFree();
  UnitTestNeuraMorphGetSet();
  UnitTestNeuraMorphAddRemoveUnit();
  UnitTestNeuraMorphBurryUnits();
  printf("UnitTestNeuraMorph OK\n");

}

void UnitTestAll() {

  UnitTestNeuraMorphUnit();
  UnitTestNeuraMorph();
  printf("UnitTestAll OK\n");

}

int main() {

  UnitTestAll();

  // Return success code
  return 0;

}
