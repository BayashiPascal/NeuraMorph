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
  bool isSame =
    ISEQUALF(
      unit->value,
      0.0);
  if (
    VecGetDim(unit->outputs) != 2 ||
    VecGetDim(unit->lowFilters) != 3 ||
    VecGetDim(unit->highFilters) != 3 ||
    VecGetDim(unit->unitInputs) != 3 ||
    isSame != true ||
    unit->lowOutputs != NULL ||
    unit->highOutputs != NULL) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NeuraMorphUnitCreate failed (1)");
    PBErrCatch(NeuraMorphErr);

  }

  isSame =
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

void UnitTestNeuraMorphUnitGetSetPrint() {

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

  if (NMUnitGetNbInputs(unit) != 3) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NMUnitGetNbInputs failed");
    PBErrCatch(NeuraMorphErr);

  }

  if (NMUnitGetNbOutputs(unit) != 2) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NMUnitGetNbOutputs failed");
    PBErrCatch(NeuraMorphErr);

  }

  bool isSame =
    ISEQUALF(
      NMUnitGetValue(unit),
      0.0);
  if (isSame != true) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NMUnitGetValue failed");
    PBErrCatch(NeuraMorphErr);

  }

  NMUnitSetValue(
    unit,
    0.5);
  isSame =
    ISEQUALF(
      NMUnitGetValue(unit),
      0.5);
  if (isSame != true) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NMUnitSetValue failed");
    PBErrCatch(NeuraMorphErr);

  }

  NMUnitPrintln(
    unit,
    stdout);

  NeuraMorphUnitFree(&unit);
  VecFree(&iIn);
  VecFree(&iOut);
  printf("UnitTestNeuraMorphUnitGetSetPrint OK\n");

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
      iInput,
      0.0);
    VecSet(
      unit->highFilters,
      iInput,
      2.0);

  }

  VecShort2D dim = VecShortCreateStatic2D();
  VecSet(
    &dim,
    0,
    3);
  VecSet(
    &dim,
    1,
    2);
  unit->transfer =
    BBodyCreate(
      1,
      &dim);
  unit->transfer->_ctrl[0]->_val[0] = 1.0;
  unit->transfer->_ctrl[0]->_val[1] = 2.0;

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
  check[0] = -0.0625;
  check[1] = -0.125;
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
  UnitTestNeuraMorphUnitGetSetPrint();
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
    nm->flagOneHot != false ||
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

  if (NMGetFlagOneHot(nm) != false) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NMGetFlagOneHot failed");
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

  NMSetFlagOneHot(
    nm,
    true);
  if (NMGetFlagOneHot(nm) != true) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NMSetFlagOneHot failed");
    PBErrCatch(NeuraMorphErr);

  }

  VecLong* iOuts = NMGetVecIOutputs(nm);
  VecLong2D checkOuts =
    VecLongCreateStatic2D();
  VecSet(
    &checkOuts,
    0,
    5);
  VecSet(
    &checkOuts,
    1,
    6);
  bool isSame =
    VecIsEqual(
      &checkOuts,
      iOuts);
  if (isSame == false) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NMGetVecIOutputs failed");
    PBErrCatch(NeuraMorphErr);

  }

  VecFree(&iOuts);

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

  if (NMHiddens(nm) != nm->hiddens) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NMHiddens failed");
    PBErrCatch(NeuraMorphErr);

  }

  if (NMLowHiddens(nm) != nm->lowHiddens) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NMLowHiddens failed");
    PBErrCatch(NeuraMorphErr);

  }

  if (NMHighHiddens(nm) != nm->highHiddens) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NMHighHiddens failed");
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

void UnitTestNeuraMorphBurryUnitsEvaluate() {

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

  for (
    long iInput = 3;
    iInput--;) {

    VecSet(
      unitA->lowFilters,
      iInput,
      0.0);
    VecSet(
      unitA->highFilters,
      iInput,
      2.0);
    VecSet(
      unitB->lowFilters,
      iInput,
      0.0);
    VecSet(
      unitB->highFilters,
      iInput,
      2.0);

  }

  VecShort2D dim = VecShortCreateStatic2D();
  VecSet(
    &dim,
    0,
    3);
  VecSet(
    &dim,
    1,
    2);
  unitA->transfer =
    BBodyCreate(
      1,
      &dim);
  unitA->transfer->_ctrl[0]->_val[0] = 1.0;
  unitA->transfer->_ctrl[0]->_val[1] = 2.0;
  unitB->transfer =
    BBodyCreate(
      1,
      &dim);
  unitB->transfer->_ctrl[0]->_val[0] = 2.0;
  unitB->transfer->_ctrl[0]->_val[1] = 1.0;

  float x = 1.0;
  float y = 0.5;
  float z = 1.5;
  VecFloat* evalInputs = VecFloatCreate(3);
  VecSet(
    evalInputs,
    0,
    x);
  VecSet(
    evalInputs,
    1,
    y);
  VecSet(
    evalInputs,
    2,
    z);

  NMUnitEvaluate(
    unitA,
    evalInputs);
  NMUnitEvaluate(
    unitB,
    evalInputs);

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
      "NMBurryUnits failed (1)");
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
      "NMBurryUnits failed (2)");
    PBErrCatch(NeuraMorphErr);

  }

  float checkLowAa =
    VecGet(
      unitA->lowOutputs,
      0);
  checkLowAa -=
    VecGet(
      nm->lowHiddens,
      0);
  bool isSameLowAa =
    ISEQUALF(
      checkLowAa,
      0.0);
  float checkLowAb =
    VecGet(
      unitA->lowOutputs,
      1);
  checkLowAb -=
    VecGet(
      nm->lowHiddens,
      1);
  bool isSameLowAb =
    ISEQUALF(
      checkLowAb,
      0.0);
  float checkLowBa =
    VecGet(
      unitB->lowOutputs,
      0);
  checkLowBa -=
    VecGet(
      nm->lowHiddens,
      2);
  bool isSameLowBa =
    ISEQUALF(
      checkLowBa,
      0.0);
  float checkLowBb =
    VecGet(
      unitB->lowOutputs,
      1);
  checkLowBb -=
    VecGet(
      nm->lowHiddens,
      3);
  bool isSameLowBb =
    ISEQUALF(
      checkLowBb,
      0.0);
  float checkHighAa =
    VecGet(
      unitA->lowOutputs,
      0);
  checkHighAa -=
    VecGet(
      nm->lowHiddens,
      0);
  bool isSameHighAa =
    ISEQUALF(
      checkHighAa,
      0.0);
  float checkHighAb =
    VecGet(
      unitA->lowOutputs,
      1);
  checkHighAb -=
    VecGet(
      nm->lowHiddens,
      1);
  bool isSameHighAb =
    ISEQUALF(
      checkHighAb,
      0.0);
  float checkHighBa =
    VecGet(
      unitB->lowOutputs,
      0);
  checkHighBa -=
    VecGet(
      nm->lowHiddens,
      2);
  bool isSameHighBa =
    ISEQUALF(
      checkHighBa,
      0.0);
  float checkHighBb =
    VecGet(
      unitB->lowOutputs,
      1);
  checkHighBb -=
    VecGet(
      nm->lowHiddens,
      3);
  bool isSameHighBb =
    ISEQUALF(
      checkHighBb,
      0.0);
  if (
    isSameLowAa == false ||
    isSameLowAb == false ||
    isSameLowBa == false ||
    isSameLowBb == false ||
    isSameHighAa == false ||
    isSameHighAb == false ||
    isSameHighBa == false ||
    isSameHighBb == false) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NMBurryUnits failed (3)");
    PBErrCatch(NeuraMorphErr);

  }

  VecSet(
    &iInputs,
    0,
    3);
  VecSet(
    &iInputs,
    1,
    4);
  VecSet(
    &iInputs,
    2,
    5);
  VecSet(
    &iOutputs,
    0,
    4);
  VecSet(
    &iOutputs,
    1,
    5);
  NeuraMorphUnit* unitC =
    NMAddUnit(
      nm,
      (VecLong*)&iInputs,
      (VecLong*)&iOutputs);

  for (
    long iInput = 3;
    iInput--;) {

    VecSet(
      unitC->lowFilters,
      iInput,
      0.0);
    VecSet(
      unitC->highFilters,
      iInput,
      20.0);

  }

  unitC->transfer =
    BBodyCreate(
      1,
      &dim);
  unitC->transfer->_ctrl[0]->_val[0] = -1.0;
  unitC->transfer->_ctrl[0]->_val[1] = -2.0;

  NMEvaluate(
    nm,
    evalInputs);
  float checkAout[2];
  checkAout[0] =
    0.09375 -
    VecGet(
      nm->hiddens,
      0);
  checkAout[1] =
    0.1875 -
    VecGet(
      nm->hiddens,
      1);
  float checkBout[2];
  checkBout[0] =
    0.1875 -
    VecGet(
      nm->hiddens,
      2);
  checkBout[1] =
    0.09375 -
    VecGet(
      nm->hiddens,
      3);

  bool isSameAa =
    ISEQUALF(
      checkAout[0],
      0.0);
  bool isSameAb =
    ISEQUALF(
      checkAout[1],
      0.0);
  bool isSameBa =
    ISEQUALF(
      checkBout[0],
      0.0);
  bool isSameBb =
    ISEQUALF(
      checkBout[1],
      0.0);
  if (
    isSameAa == false ||
    isSameAb == false ||
    isSameBa == false ||
    isSameBb == false) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NMEvaluate failed (1)");
    PBErrCatch(NeuraMorphErr);

  }

  x =
    VecGet(
      nm->hiddens,
      0);
  y =
    VecGet(
      nm->hiddens,
      1);
  z =
    VecGet(
      nm->hiddens,
      2);
  float checkCout[2];
  checkCout[0] =
    -0.976738 -
    VecGet(
      unitC->outputs,
      0);
  checkCout[1] =
    -1.953476 -
    VecGet(
      unitC->outputs,
      1);

  bool isSameCa =
    ISEQUALF(
      checkCout[0],
      0.0);
  bool isSameCb =
    ISEQUALF(
      checkCout[1],
      0.0);
  bool isSameCc =
    VecIsEqual(
      unitC->outputs,
      nm->outputs);
  if (
    isSameCa == false ||
    isSameCb == false ||
    isSameCc == false) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NMEvaluate failed (2)");
    PBErrCatch(NeuraMorphErr);

  }

  VecFree(&evalInputs);
  NeuraMorphFree(&nm);

  printf("UnitTestNeuraMorphBurryUnitsEvaluate OK\n");

}

void UnitTestNeuraMorph() {

  UnitTestNeuraMorphCreateFree();
  UnitTestNeuraMorphGetSet();
  UnitTestNeuraMorphAddRemoveUnit();
  UnitTestNeuraMorphBurryUnitsEvaluate();
  printf("UnitTestNeuraMorph OK\n");

}

void UnitTestNeuraMorphTrainerCreateFree() {

  GDataSetVecFloat dataset =
    GDataSetVecFloatCreateStaticFromFile("./Datasets/iris.json");
  NeuraMorph* nm =
    NeuraMorphCreate(
      GDSGetNbInputs(&dataset),
      GDSGetNbOutputs(&dataset));
  NeuraMorphTrainer trainer =
    NeuraMorphTrainerCreateStatic(
      nm,
      &dataset);
  bool isSame =
    ISEQUALF(
      trainer.weakUnitThreshold,
      0.9);
  if (
    trainer.neuraMorph != nm ||
    trainer.depth != 2 ||
    trainer.order != 1 ||
    trainer.nbMaxUnitDepth != 2 ||
    trainer.streamInfo != NULL ||
    trainer.maxLvlDiv != 2 ||
    trainer.nbMaxInputsUnit != GDSGetNbOutputs(&dataset) ||
    isSame != true ||
    trainer.iCatTraining != 0 ||
    trainer.iCatEval != 1 ||
    trainer.dataset != &dataset) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NeuraMorphTrainerCreateStatic failed");
    PBErrCatch(NeuraMorphErr);

  }

  NeuraMorphTrainerFreeStatic(&trainer);
  NeuraMorphFree(&nm);
  GDataSetVecFloatFreeStatic(&dataset);

  printf("UnitTestNeuraMorphTrainerCreateFree OK\n");

}

void UnitTestNeuraMorphTrainerGetSet() {

  GDataSetVecFloat dataset =
    GDataSetVecFloatCreateStaticFromFile("./Datasets/iris.json");
  NeuraMorph* nm =
    NeuraMorphCreate(
      GDSGetNbInputs(&dataset),
      GDSGetNbOutputs(&dataset));
  NeuraMorphTrainer trainer =
    NeuraMorphTrainerCreateStatic(
      nm,
      &dataset);
  if (NMTrainerGetDepth(&trainer) != 2) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NeuraMorphTrainerGetDepth failed");
    PBErrCatch(NeuraMorphErr);

  }

  if (NMTrainerGetOrder(&trainer) != 1) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NeuraMorphTrainerGetOrder failed");
    PBErrCatch(NeuraMorphErr);

  }

  if (NMTrainerStreamInfo(&trainer) != NULL) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NeuraMorphTrainerStreamInfo failed");
    PBErrCatch(NeuraMorphErr);

  }

  if (NMTrainerGetNbMaxUnitDepth(&trainer) != 2) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NeuraMorphTrainerGetNbMaxUnitDepth failed");
    PBErrCatch(NeuraMorphErr);

  }

  if (NMTrainerGetMaxLvlDiv(&trainer) != 2) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NeuraMorphTrainerGetMaxLvlDiv failed");
    PBErrCatch(NeuraMorphErr);

  }

  if (NMTrainerGetNbMaxInputsUnit(&trainer) != GDSGetNbOutputs(&dataset)) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NeuraMorphTrainerGetNbMaxInputsUnit failed");
    PBErrCatch(NeuraMorphErr);

  }

  if (NMTrainerGetICatTraining(&trainer) != 0) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NeuraMorphTrainerGetICatTraining failed");
    PBErrCatch(NeuraMorphErr);

  }

  if (NMTrainerGetICatEval(&trainer) != 1) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NeuraMorphTrainerGetICatEval failed");
    PBErrCatch(NeuraMorphErr);

  }

  bool isSame =
    ISEQUALF(
      NMTrainerGetWeakThreshold(&trainer),
      0.9);
  if (isSame != true) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NeuraMorphTrainerGetWeakThreshold failed");
    PBErrCatch(NeuraMorphErr);

  }

  NMTrainerSetDepth(
    &trainer,
    3);
  if (NMTrainerGetDepth(&trainer) != 3) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NeuraMorphTrainerSetDepth failed");
    PBErrCatch(NeuraMorphErr);

  }

  NMTrainerSetStreamInfo(
    &trainer,
    stdout);
  if (NMTrainerStreamInfo(&trainer) != stdout) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NeuraMorphTrainerSetStreamInfo failed");
    PBErrCatch(NeuraMorphErr);

  }

  NMTrainerSetNbMaxUnitDepth(
    &trainer,
    3);
  if (NMTrainerGetNbMaxUnitDepth(&trainer) != 3) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NeuraMorphTrainerSetNbMaxUnitDepth failed");
    PBErrCatch(NeuraMorphErr);

  }

  NMTrainerSetOrder(
    &trainer,
    3);
  if (NMTrainerGetOrder(&trainer) != 3) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NeuraMorphTrainerSetOrder failed");
    PBErrCatch(NeuraMorphErr);

  }

  NMTrainerSetNbMaxInputsUnit(
    &trainer,
    GDSGetNbOutputs(&dataset) + 1);
  if (NMTrainerGetNbMaxInputsUnit(&trainer) != GDSGetNbOutputs(&dataset) + 1) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NeuraMorphTrainerSetNbMaxInputsUnit failed");
    PBErrCatch(NeuraMorphErr);

  }

  NMTrainerSetICatTraining(
    &trainer,
    3);
  if (NMTrainerGetICatTraining(&trainer) != 3) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NeuraMorphTrainerSetICatTraining failed");
    PBErrCatch(NeuraMorphErr);

  }

  NMTrainerSetICatEval(
    &trainer,
    4);
  if (NMTrainerGetICatEval(&trainer) != 4) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NeuraMorphTrainerSetICatEval failed");
    PBErrCatch(NeuraMorphErr);

  }

  NMTrainerSetMaxLvlDiv(
    &trainer,
    3);
  if (NMTrainerGetMaxLvlDiv(&trainer) != 3) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NeuraMorphTrainerSetMaxLvlDiv failed");
    PBErrCatch(NeuraMorphErr);

  }

  NMTrainerSetWeakThreshold(
    &trainer,
    0.5);
  isSame =
    ISEQUALF(
      NMTrainerGetWeakThreshold(&trainer),
      0.5);
  if (isSame != true) {

    NeuraMorphErr->_type = PBErrTypeUnitTestFailed;
    sprintf(
      NeuraMorphErr->_msg,
      "NeuraMorphTrainerSetWeakThreshold failed");
    PBErrCatch(NeuraMorphErr);

  }

  NeuraMorphTrainerFreeStatic(&trainer);
  NeuraMorphFree(&nm);
  GDataSetVecFloatFreeStatic(&dataset);

  printf("UnitTestNeuraMorphTrainerGetSet OK\n");

}

void UnitTestNeuraMorphTrainerRun() {

  srand(2);
  GDataSetVecFloat dataset =
    GDataSetVecFloatCreateStaticFromFile("./Datasets/iris.json");
  GDSShuffle(&dataset);
  VecShort2D split = VecShortCreateStatic2D();
  VecSet(
    &split,
    0,
    130);
  VecSet(
    &split,
    1,
    20);
  GDSSplit(
    &dataset,
    (VecShort*)&split);
  NeuraMorph* nm =
    NeuraMorphCreate(
      GDSGetNbInputs(&dataset),
      GDSGetNbOutputs(&dataset));
  NMSetFlagOneHot(
    nm,
    true);
  NeuraMorphTrainer trainer =
    NeuraMorphTrainerCreateStatic(
      nm,
      &dataset);

  NMTrainerSetWeakThreshold(
    &trainer,
    0.99);
  NMTrainerSetDepth(
    &trainer,
    3);
  NMTrainerSetMaxLvlDiv(
    &trainer,
    1);
  NMTrainerSetNbMaxInputsUnit(
    &trainer,
    GDSGetNbInputs(&dataset));
  NMTrainerSetOrder(
    &trainer,
    1);
  NMTrainerRun(&trainer);
  NMTrainerSetStreamInfo(
    &trainer,
    stdout);
  NMTrainerEval(&trainer);
  printf("Bias (min/avg/max): ");
  VecPrint(
    NMTrainerResEval(&trainer),
    stdout);
  printf("\n");
  NMTrainerSetICatEval(
    &trainer,
    0);
  NMTrainerSetStreamInfo(
    &trainer,
    NULL);
  NMTrainerEval(&trainer);
  printf("Bias training (min/avg/max): ");
  VecPrint(
    NMTrainerResEval(&trainer),
    stdout);
  printf("\n");

  NeuraMorphTrainerFreeStatic(&trainer);
  NeuraMorphFree(&nm);
  GDataSetVecFloatFreeStatic(&dataset);

  printf("UnitTestNeuraMorphTrainerRun OK\n");

}

void UnitTestNeuraMorphTrainer() {

  UnitTestNeuraMorphTrainerCreateFree();
  UnitTestNeuraMorphTrainerGetSet();
  UnitTestNeuraMorphTrainerRun();
  printf("UnitTestNeuraMorphTrainer OK\n");

}

void UnitTestAll() {

  UnitTestNeuraMorphUnit();
  UnitTestNeuraMorph();
  UnitTestNeuraMorphTrainer();
  printf("UnitTestAll OK\n");

}

int main() {

  UnitTestAll();

  // Return success code
  return 0;

}
