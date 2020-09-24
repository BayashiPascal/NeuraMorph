#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include "neuramorph.h"
#include "pbdataanalysis.h"

typedef struct TrainArg {

  const char* label;
  const char* pathDataset;
  int seed;
  int percSampleEval;
  bool oneHot;
  bool allHot;
  float weakUnitThreshold;
  int depth;
  int maxLvlDiv;
  int nbMaxInputsUnit;
  int nbMaxUnitDepth;
  int order;
  FILE* streamInfo;
  int nbDisplay;
  bool pcaFlag;

} TrainArg;

void TrainArgPrint(const TrainArg* arg) {

  fprintf(
    arg->streamInfo,
    "label: %s\n",
    arg->label);
  fprintf(
    arg->streamInfo,
    "pathDataset: %s\n",
    arg->pathDataset);
  fprintf(
    arg->streamInfo,
    "percSampleEval: %d\n",
    arg->percSampleEval);
  fprintf(
    arg->streamInfo,
    "oneHot: %d\n",
    arg->oneHot);
  fprintf(
    arg->streamInfo,
    "allHot: %d\n",
    arg->allHot);
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
  fprintf(
    arg->streamInfo,
    "nbDisplay: %d\n",
    arg->nbDisplay);
  fprintf(
    arg->streamInfo,
    "pca: %d\n",
    arg->pcaFlag);

}

void Train(const TrainArg* arg) {

  fprintf(
    arg->streamInfo,
    "=============================================================\n");

  srand(arg->seed);
  GDataSetVecFloat datasetOrig =
    GDataSetVecFloatCreateStaticFromFile(arg->pathDataset);
  GDataSetVecFloat dataset = datasetOrig;
  if (arg->pcaFlag) {
    PrincipalComponentAnalysis pca =
      PrincipalComponentAnalysisCreateStatic();
    PCASearch(
        &pca,
        &datasetOrig);
    dataset =
      PCAConvert(
        &pca,
        &datasetOrig,
        PCAGetNbComponents(&pca));
    GDataSetVecFloatFreeStatic(&datasetOrig);
    PrincipalComponentAnalysisFreeStatic(&pca);
  }
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
  short nbSampleEval = GDSGetSize(&dataset) / arg->percSampleEval;
  VecSet(
    &split,
    0,
    GDSGetSize(&dataset) - nbSampleEval);
  VecSet(
    &split,
    1,
    nbSampleEval);
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
  NMSetFlagAllHot(
    nm,
    arg->allHot);
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
  double clockStart = clock();
  NMTrainerRun(&trainer);
  double timeUsed = 
    ((double)(clock() - clockStart)) / CLOCKS_PER_SEC;
  fprintf(
    arg->streamInfo,
    "Time NMTrainerRun: %fs\n",
    timeUsed);
  NMTrainerSetStreamInfo(
    &trainer,
    arg->streamInfo);
  NMTrainerEval(&trainer);
  fprintf(
    arg->streamInfo,
    "Bias prediction (min/avg/sigma/max) and accuracy:\n");
  for (
    int iOut = 0;
    iOut < GDSGetNbOutputs(&dataset);
    ++iOut) {

    fprintf(
      arg->streamInfo,
      "#%d ",
      iOut);
    VecPrint(
      NMTrainerResEval(&trainer)[iOut],
      arg->streamInfo);
    float percCorrect =
      (float)NMTrainerGetNbCorrect(&trainer)[iOut] /
      (float)GDSGetSizeCat(
        NMTrainerDataset(&trainer),
        NMTrainerGetICatEval(&trainer));
    if (arg->oneHot || arg->allHot) {

      fprintf(
        arg->streamInfo,
        " %f%%",
        percCorrect * 100.0);

    }

    fprintf(
      arg->streamInfo,
      "\n");

  }

  NMTrainerSetICatEval(
    &trainer,
    0);
  NMTrainerSetStreamInfo(
    &trainer,
    NULL);
  NMTrainerEval(&trainer);
  fprintf(
    arg->streamInfo,
    "Bias training (min/avg/sigma/max) and accuracy:\n");
  for (
    int iOut = 0;
    iOut < GDSGetNbOutputs(&dataset);
    ++iOut) {

    fprintf(
      arg->streamInfo,
      "#%d ",
      iOut);
    VecPrint(
      NMTrainerResEval(&trainer)[iOut],
      arg->streamInfo);
    float percCorrect =
      (float)NMTrainerGetNbCorrect(&trainer)[iOut] /
      (float)GDSGetSizeCat(
        NMTrainerDataset(&trainer),
        NMTrainerGetICatTraining(&trainer));
    if (arg->oneHot || arg->allHot) {

      fprintf(
        arg->streamInfo,
        " %f%%",
        percCorrect * 100.0);

    }

    fprintf(
      arg->streamInfo,
      "\n");

  }

  fprintf(
    arg->streamInfo,
    "Samples of prediction:\n");
  GDSReset(
    NMTrainerDataset(&trainer),
    NMTrainerGetICatEval(&trainer));
  bool flagStep = true;
  for (
    int iDisplay = 0;
    iDisplay < arg->nbDisplay && flagStep;
    ++iDisplay) {

    VecFloat* inputs =
      GDSGetSampleInputs(
        NMTrainerDataset(&trainer),
        NMTrainerGetICatEval(&trainer));
    VecFloat* outputs =
      GDSGetSampleOutputs(
        NMTrainerDataset(&trainer),
        NMTrainerGetICatEval(&trainer));
    NMEvaluate(
      NMTrainerNeuraMorph(&trainer),
      inputs);
    fprintf(
      arg->streamInfo,
      "#%d\n",
      iDisplay);
    /*fprintf(
      arg->streamInfo,
      "Input: ");
    VecPrintln(
      inputs,
      arg->streamInfo);*/
    fprintf(
      arg->streamInfo,
      "Truth: ");
    VecPrintln(
      outputs,
      arg->streamInfo);
    fprintf(
      arg->streamInfo,
      "Pred : ");
    VecPrintln(
      NMOutputs(NMTrainerNeuraMorph(&trainer)),
      arg->streamInfo);
    VecFree(&inputs);
    VecFree(&outputs);
    flagStep =
      GDSStepSample(
        NMTrainerDataset(&trainer),
        NMTrainerGetICatEval(&trainer));

  }

  NeuraMorphTrainerFreeStatic(&trainer);
  NeuraMorphFree(&nm);
  GDataSetVecFloatFreeStatic(&dataset);

}

void Iris() {

  TrainArg arg = {
    .label = "Iris",
    .pathDataset = "./Datasets/iris.json",
    .seed = 0,
    .percSampleEval = 10,
    .oneHot = true,
    .allHot = false,
    .weakUnitThreshold = 0.95,
    .depth = 6,
    .maxLvlDiv = 0,
    .nbMaxInputsUnit = 2,
    .nbMaxUnitDepth = 10,
    .order = 2,
    .nbDisplay = 5,
    .pcaFlag = true,
    .streamInfo = stdout
  };
  Train(&arg);

}

void WisconsinDiagnosticBreastCancerDataset() {

  TrainArg arg = {
    .label = "Breast cancer",
    .pathDataset = "./Datasets/wdbc.json",
    .seed = 0,
    .percSampleEval = 10,
    .oneHot = true,
    .allHot = false,
    .weakUnitThreshold = 0.95,
    .depth = 4,
    .maxLvlDiv = 2,
    .nbMaxInputsUnit = 2,
    .nbMaxUnitDepth = 10,
    .order = 2,
    .nbDisplay = 5,
    .pcaFlag = true,
    .streamInfo = stdout
  };
  Train(&arg);

  // https://www.kaggle.com/uciml/breast-cancer-wisconsin-data/discussion/62297
  // 99%
}

void Arrythmia() {

  TrainArg arg = {
    .label = "Arrythmia",
    .pathDataset = "./Datasets/arrhythmia.json",
    .seed = 0,
    .percSampleEval = 10,
    .oneHot = true,
    .allHot = false,
    .weakUnitThreshold = 0.95,
    .depth = 3,
    .maxLvlDiv = 2,
    .nbMaxInputsUnit = 2,
    .nbMaxUnitDepth = 10,
    .order = 2,
    .nbDisplay = 5,
    .pcaFlag = true,
    .streamInfo = stdout
  };
  Train(&arg);

  // https://www.hindawi.com/journals/cmmm/2018/7310496/
  // 81.11% when used with 80/20 data split and 92.07% using 90/10 data split
}

void Abalone() {

  TrainArg arg = {
    .label = "Abalone",
    .pathDataset = "./Datasets/abalone.json",
    .seed = 0,
    .percSampleEval = 10,
    .oneHot = false,
    .allHot = false,
    .weakUnitThreshold = 0.95,
    .depth = 9,
    .maxLvlDiv = 2,
    .nbMaxInputsUnit = 2,
    .nbMaxUnitDepth = 10,
    .order = 2,
    .nbDisplay = 5,
    .pcaFlag = true,
    .streamInfo = stdout
  };
  Train(&arg);

}

void RGBHSV() {

  TrainArg arg = {
    .label = "RGB to HSV",
    .pathDataset = "./Datasets/rgbhsv.json",
    .seed = 0,
    .percSampleEval = 10,
    .oneHot = false,
    .allHot = false,
    .weakUnitThreshold = 0.5,
    .depth = 5,
    .maxLvlDiv = 2,
    .nbMaxInputsUnit = 2,
    .nbMaxUnitDepth = 10,
    .order = 2,
    .nbDisplay = 5,
    .pcaFlag = true,
    .streamInfo = stdout
  };
  Train(&arg);

}

void DiabeteRisk() {

  TrainArg arg = {
    .label = "Diabete",
    .pathDataset = "./Datasets/diabeteRisk.json",
    .seed = 0,
    .percSampleEval = 10,
    .oneHot = true,
    .allHot = false,
    .weakUnitThreshold = 0.95,
    .depth = 5,
    .maxLvlDiv = 2,
    .nbMaxInputsUnit = 2,
    .nbMaxUnitDepth = 10,
    .order = 2,
    .nbDisplay = 5,
    .pcaFlag = true,
    .streamInfo = stdout
  };
  Train(&arg);

}

void HCV() {

  TrainArg arg = {
    .label = "Hepatitis",
    .pathDataset = "./Datasets/hcv.json",
    .seed = 0,
    .percSampleEval = 10,
    .oneHot = true,
    .allHot = false,
    .weakUnitThreshold = 0.95,
    .depth = 5,
    .maxLvlDiv = 1,
    .nbMaxInputsUnit = 2,
    .nbMaxUnitDepth = 10,
    .order = 2,
    .nbDisplay = 5,
    .pcaFlag = true,
    .streamInfo = stdout
  };
  Train(&arg);

}

void Amphibian() {

  TrainArg arg = {
    .label = "Amphibian",
    .pathDataset = "./Datasets/amphibian.json",
    .seed = 1,
    .percSampleEval = 10,
    .oneHot = false,
    .allHot = true,
    .weakUnitThreshold = 0.95,
    .depth = 4,
    .maxLvlDiv = 0,
    .nbMaxInputsUnit = 2,
    .nbMaxUnitDepth = 10,
    .order = 2,
    .nbDisplay = 5,
    .pcaFlag = true,
    .streamInfo = stdout
  };
  Train(&arg);

}

int main() {

  RGBHSV();
  DiabeteRisk();
  HCV();
  Amphibian();
  Iris();
  Abalone();
  WisconsinDiagnosticBreastCancerDataset();
  Arrythmia();

  // Return success code
  return 0;

}

