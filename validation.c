#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include "neuramorph.h"

typedef struct TrainArg {

  const char* pathDataset;
  int seed;
  int nbSampleEval;
  bool oneHot;
  float weakUnitThreshold;
  int depth;
  int maxLvlDiv;
  int nbMaxInputsUnit;
  int nbMaxUnitDepth;
  int order;
  FILE* streamInfo;

} TrainArg;

void TrainArgPrint(const TrainArg* arg) {

  fprintf(
    arg->streamInfo,
    "pathDataset: %s\n",
    arg->pathDataset);
  fprintf(
    arg->streamInfo,
    "nbSampleEval: %d\n",
    arg->nbSampleEval);
  fprintf(
    arg->streamInfo,
    "oneHot: %d\n",
    arg->oneHot);
  fprintf(
    arg->streamInfo,
    "seed: %d\n",
    arg->seed);
  fprintf(
    arg->streamInfo,
    "weakUnitThreshold: %f\n",
    arg->weakUnitThreshold);
  fprintf(
    arg->streamInfo,
    "depth: %d\n",
    arg->depth);
  fprintf(
    arg->streamInfo,
    "maxLvlDiv: %d\n",
    arg->maxLvlDiv);
  fprintf(
    arg->streamInfo,
    "nbMaxInputsUnit: %d\n",
    arg->nbMaxInputsUnit);
  fprintf(
    arg->streamInfo,
    "nbMaxUnitDepth: %d\n",
    arg->nbMaxUnitDepth);
  fprintf(
    arg->streamInfo,
    "order: %d\n",
    arg->order);

}

void Train(const TrainArg* arg) {

  fprintf(
    arg->streamInfo,
    "=============================================================\n");

  srand(arg->seed);
  GDataSetVecFloat dataset =
    GDataSetVecFloatCreateStaticFromFile(arg->pathDataset);
  GDSShuffle(&dataset);
  fprintf(
    arg->streamInfo,
    "%s\n",
    GDSDesc(&dataset));
  fprintf(
    arg->streamInfo,
    "nbTotalSample: %ld\n",
    GDSGetSize(&dataset));
  fprintf(
    arg->streamInfo,
    "nbInputs: %d\n",
    GDSGetNbInputs(&dataset));
  fprintf(
    arg->streamInfo,
    "nbOutputs: %d\n",
    GDSGetNbOutputs(&dataset));
  TrainArgPrint(arg);
  VecShort2D split = VecShortCreateStatic2D();
  VecSet(
    &split,
    0,
    GDSGetSize(&dataset) - arg->nbSampleEval);
  VecSet(
    &split,
    1,
    arg->nbSampleEval);
  GDSSplit(
    &dataset,
    (VecShort*)&split);
  NeuraMorph* nm =
    NeuraMorphCreate(
      GDSGetNbInputs(&dataset),
      GDSGetNbOutputs(&dataset));
  NMSetFlagOneHot(
    nm,
    arg->oneHot);
  NeuraMorphTrainer trainer =
    NeuraMorphTrainerCreateStatic(
      nm,
      &dataset);

  NMTrainerSetWeakThreshold(
    &trainer,
    arg->weakUnitThreshold);
  NMTrainerSetDepth(
    &trainer,
    arg->depth);
  NMTrainerSetMaxLvlDiv(
    &trainer,
    arg->maxLvlDiv);
  NMTrainerSetNbMaxInputsUnit(
    &trainer,
    arg->nbMaxInputsUnit);
  NMTrainerSetNbMaxUnitDepth(
    &trainer,
    arg->nbMaxUnitDepth);
  NMTrainerSetOrder(
    &trainer,
    arg->order);
  NMTrainerRun(&trainer);
  NMTrainerSetStreamInfo(
    &trainer,
    arg->streamInfo);
  NMTrainerEval(&trainer);
  fprintf(
    arg->streamInfo,
    "Bias (min/avg/sigma/max): ");
  VecPrint(
    NMTrainerResEval(&trainer),
    arg->streamInfo);
  float percCorrect =
    (float)NMTrainerGetNbCorrect(&trainer) /
    (float)GDSGetSizeCat(
      NMTrainerDataset(&trainer),
      NMTrainerGetICatEval(&trainer));
  if (arg->oneHot) {

    fprintf(
      arg->streamInfo,
      " Accuracy %f%%",
      percCorrect * 100.0);

  }

  fprintf(
    arg->streamInfo,
    "\n");
  NMTrainerSetICatEval(
    &trainer,
    0);
  NMTrainerSetStreamInfo(
    &trainer,
    NULL);
  NMTrainerEval(&trainer);
  fprintf(
    arg->streamInfo,
    "Bias training (min/avg/sigma/max): ");
  VecPrint(
    NMTrainerResEval(&trainer),
    arg->streamInfo);
  percCorrect =
    (float)NMTrainerGetNbCorrect(&trainer) /
    (float)GDSGetSizeCat(
      NMTrainerDataset(&trainer),
      NMTrainerGetICatTraining(&trainer));
  if (arg->oneHot) {

    fprintf(
      arg->streamInfo,
      " Accuracy %f%%",
      percCorrect * 100.0);

  }

  fprintf(
    arg->streamInfo,
    "\n");

  NeuraMorphTrainerFreeStatic(&trainer);
  NeuraMorphFree(&nm);
  GDataSetVecFloatFreeStatic(&dataset);

}

void Iris() {

  TrainArg arg = {
    .pathDataset = "./Datasets/iris.json",
    .seed = 0,
    .nbSampleEval = 25,
    .oneHot = true,
    .weakUnitThreshold = 0.99,
    .depth = 1,
    .maxLvlDiv = 0,
    .nbMaxInputsUnit = 3,
    .nbMaxUnitDepth = 100,
    .order = 2,
    .streamInfo = stdout
  };
  Train(&arg);

}

void WisconsinDiagnosticBreastCancerDataset() {

  TrainArg arg = {
    .pathDataset = "./Datasets/wdbc.json",
    .seed = 0,
    .nbSampleEval = 50,
    .oneHot = true,
    .weakUnitThreshold = 0.99,
    .depth = 1,
    .maxLvlDiv = 0,
    .nbMaxInputsUnit = 3,
    .nbMaxUnitDepth = 100,
    .order = 2,
    .streamInfo = stdout
  };
  Train(&arg);

  // https://www.kaggle.com/uciml/breast-cancer-wisconsin-data/discussion/62297
  // 99%
}

void Arrythmia() {

  TrainArg arg = {
    .pathDataset = "./Datasets/arrhythmia.json",
    .seed = 0,
    .nbSampleEval = 25,
    .oneHot = true,
    .weakUnitThreshold = 0.95,
    .depth = 1,
    .maxLvlDiv = 0,
    .nbMaxInputsUnit = 2,
    .nbMaxUnitDepth = 100,
    .order = 2,
    .streamInfo = stdout
  };
  Train(&arg);

  // https://www.hindawi.com/journals/cmmm/2018/7310496/
  // 81.11% when used with 80/20 data split and 92.07% using 90/10 data split
}

void Abalone() {

  TrainArg arg = {
    .pathDataset = "./Datasets/abalone.json",
    .seed = 0,
    .nbSampleEval = 100,
    .oneHot = false,
    .weakUnitThreshold = 0.95,
    .depth = 1,
    .maxLvlDiv = 0,
    .nbMaxInputsUnit = 3,
    .nbMaxUnitDepth = 100,
    .order = 2,
    .streamInfo = stdout
  };
  Train(&arg);

}

int main() {

  Iris();
  WisconsinDiagnosticBreastCancerDataset();
  Arrythmia();
  Abalone();

  // Return success code
  return 0;

}
