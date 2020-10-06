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
  const char* type;
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
  FILE* fpDoc;
  int nbDisplay;
  bool pcaFlag;
  float accPred;
  float accPredTrain;
  float time;
  float precAcc;

} TrainArg;

void TrainArgPrint(
  const TrainArg* arg,
  FILE* streamInfo) {

  fprintf(
    streamInfo,
    "label: %s\n",
    arg->label);
  fprintf(
    streamInfo,
    "type: %s\n",
    arg->type);
  fprintf(
    streamInfo,
    "pathDataset: %s\n",
    arg->pathDataset);
  fprintf(
    streamInfo,
    "percSampleEval: %d\n",
    arg->percSampleEval);
  fprintf(
    streamInfo,
    "oneHot: %d\n",
    arg->oneHot);
  fprintf(
    streamInfo,
    "allHot: %d\n",
    arg->allHot);
  fprintf(
    streamInfo,
    "seed: %d\n",
    arg->seed);
  fprintf(
    streamInfo,
    "weakUnitThreshold: %f\n",
    arg->weakUnitThreshold);
  fprintf(
    streamInfo,
    "depth: %d\n",
    arg->depth);
  fprintf(
    streamInfo,
    "maxLvlDiv: %d\n",
    arg->maxLvlDiv);
  fprintf(
    streamInfo,
    "nbMaxInputsUnit: %d\n",
    arg->nbMaxInputsUnit);
  fprintf(
    streamInfo,
    "nbMaxUnitDepth: %d\n",
    arg->nbMaxUnitDepth);
  fprintf(
    streamInfo,
    "order: %d\n",
    arg->order);
  fprintf(
    streamInfo,
    "nbDisplay: %d\n",
    arg->nbDisplay);
  fprintf(
    streamInfo,
    "pca: %d\n",
    arg->pcaFlag);
  fprintf(
    streamInfo,
    "precAcc: %f\n",
    arg->precAcc);

}

void Train(TrainArg* arg) {

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
  long nbSample = MIN(GDSGetSize(&dataset), 10000L);
  long nbSampleEval = nbSample / arg->percSampleEval;
  fprintf(
    arg->streamInfo,
    "%s\n",
    GDSDesc(&dataset));
  fprintf(
    arg->streamInfo,
    "nbTotalSample: %ld\n",
    nbSample);
  fprintf(
    arg->streamInfo,
    "nbInputs: %d\n",
    GDSGetNbInputs(&dataset));
  fprintf(
    arg->streamInfo,
    "nbOutputs: %d\n",
    GDSGetNbOutputs(&dataset));
  TrainArgPrint(
    arg,
    arg->streamInfo);

  fprintf(
    arg->fpDoc,
    "Training parameters:\\\\\n" \
    "\\begin{center}\n" \
    "\\begin{tabular}{|l|l|}\n");
  fprintf(
    arg->fpDoc,
    "\\hline\n" \
    "Nb inputs/outputs&%d/%d\\\\\n",
    GDSGetNbInputs(&dataset),
    GDSGetNbOutputs(&dataset));
  fprintf(
    arg->fpDoc,
    "\\hline\n" \
    "Nb samples&%ld\\\\\n",
    nbSample);
  fprintf(
    arg->fpDoc,
    "\\hline\n" \
    "Percentage of training samples&%d\\%%\\\\\n",
    100 - arg->percSampleEval);

  VecLong2D split = VecLongCreateStatic2D();
  VecSet(
    &split,
    0,
    nbSample - nbSampleEval);
  VecSet(
    &split,
    1,
    nbSampleEval);
  GDSSplit(
    &dataset,
    &split);
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

  NMTrainerSetStreamInfo(
    &trainer,
    arg->streamInfo);
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
  NMTrainerSetPrecAcc(
    &trainer,
    arg->precAcc);
  double clockStart = clock();
  NMTrainerRun(&trainer);

  if (trainer.failed) {
    arg->accPred = -10.0;
    NeuraMorphTrainerFreeStatic(&trainer);
    NeuraMorphFree(&nm);
    GDataSetVecFloatFreeStatic(&dataset);
    return;
  }

  double timeUsed = 
    ((double)(clock() - clockStart)) / CLOCKS_PER_SEC;
  fprintf(
    arg->streamInfo,
    "Time NMTrainerRun: %fs\n",
    timeUsed);
  arg->time = timeUsed;

  fprintf(
    arg->fpDoc,
    "\\hline\n" \
    "weakUnitThreshold&%f\\\\\n",
    arg->weakUnitThreshold);
  fprintf(
    arg->fpDoc,
    "\\hline\n" \
    "precAcc&%f\\\\\n",
    arg->precAcc);
  fprintf(
    arg->fpDoc,
    "\\hline\n" \
    "depth&%d\\\\\n",
    arg->depth);
  fprintf(
    arg->fpDoc,
    "\\hline\n" \
    "maxLvlDiv&%d\\\\\n",
    arg->maxLvlDiv);
  fprintf(
    arg->fpDoc,
    "\\hline\n" \
    "nbMaxInputsUnit&%d\\\\\n",
    arg->nbMaxInputsUnit);
  fprintf(
    arg->fpDoc,
    "\\hline\n" \
    "nbMaxUnitDepth&%d\\\\\n",
    arg->nbMaxUnitDepth);
  fprintf(
    arg->fpDoc,
    "\\hline\n" \
    "order&%d\\\\\n",
    arg->order);
  fprintf(
    arg->fpDoc,
    "\\hline\n" \
    "pca&%d\\\\\n",
    arg->pcaFlag);
  fprintf(
    arg->fpDoc,
    "\\hline\n" \
    "oneHot&%d\\\\\n",
    arg->oneHot);
  fprintf(
    arg->fpDoc,
    "\\hline\n" \
    "allHot&%d\\\\\n",
    arg->allHot);

  fprintf(
    arg->fpDoc,
    "\\hline\n" \
    "Time training&%.1fs\\\\\n",
    timeUsed);
  fprintf(
    arg->fpDoc,
    "\\hline\n" \
    "\\end{tabular}\\\\\n");

  fprintf(
    arg->fpDoc,
    "\\end{center}\\newline\n");

  int nbOutDisplay = GDSGetNbOutputs(&dataset);
  if (nbOutDisplay > 1) {

    ++nbOutDisplay;

  }

  NMTrainerEval(&trainer);
  fprintf(
    arg->streamInfo,
    "Bias prediction (min/avg/sigma/max) and accuracy:\n");

  fprintf(
    arg->fpDoc,
    "Results:\n" \
    "\\begin{center}\n" \
    "\\begin{tabular}{|l|l|}\n");
  fprintf(
    arg->fpDoc,
    "\\hline\n" \
    "\\makecell{Bias prediction (min/avg/sigma/max)\\\\and accuracy}&" \
    "\\makecell{");
  for (
    int iOut = 0;
    iOut < nbOutDisplay;
    ++iOut) {

    if (iOut < GDSGetNbOutputs(&dataset)) {

      fprintf(
        arg->streamInfo,
        "c.%02d ",
        iOut);
      fprintf(
        arg->fpDoc,
        "c.%02d ",
        iOut);

    } else {

      fprintf(
        arg->streamInfo,
        ".... ");
      fprintf(
        arg->fpDoc,
        ".... ");

    }

    VecPrint(
      NMTrainerResEval(&trainer)[iOut],
      arg->streamInfo);
    VecPrint(
      NMTrainerResEval(&trainer)[iOut],
      arg->fpDoc);

    float percCorrect =
      (float)NMTrainerGetNbCorrect(&trainer)[iOut] /
      (float)GDSGetSizeCat(
        NMTrainerDataset(&trainer),
        NMTrainerGetICatEval(&trainer)) * 100.0;
    if (
      arg->oneHot == false &&
      iOut == GDSGetNbOutputs(&dataset)) {

      percCorrect /= (float)GDSGetNbOutputs(&dataset);

    }
    fprintf(
      arg->streamInfo,
      " %.2f%%",
      percCorrect);
    fprintf(
      arg->fpDoc,
      " %.2f\\%%",
      percCorrect);
    arg->accPred = percCorrect;

    fprintf(
      arg->streamInfo,
      "\n");

    fprintf(
      arg->fpDoc,
      "\\\\\n");

  }

  fprintf(
    arg->fpDoc,
    "}\\\\\n\n");

 NMTrainerSetICatEval(
    &trainer,
    0);
  FILE* fp =
    fopen(
      "/dev/null",
      "w");
  NMTrainerSetStreamInfo(
    &trainer,
    fp);
  fclose(fp);
  NMTrainerEval(&trainer);
  fprintf(
    arg->streamInfo,
    "Bias training (min/avg/sigma/max) and accuracy:\n");
  fprintf(
    arg->fpDoc,
    "\\hline\n" \
    "\\makecell{Bias training (min/avg/sigma/max)\\\\and accuracy}&" \
    "\\makecell{");
  for (
    int iOut = 0;
    iOut < nbOutDisplay;
    ++iOut) {

    if (iOut < GDSGetNbOutputs(&dataset)) {

      fprintf(
        arg->streamInfo,
        "c.%02d ",
        iOut);
      fprintf(
        arg->fpDoc,
        "c.%02d ",
        iOut);

    } else {

      fprintf(
        arg->streamInfo,
        ".... ");
      fprintf(
        arg->fpDoc,
        ".... ");

    }

    VecPrint(
      NMTrainerResEval(&trainer)[iOut],
      arg->streamInfo);
    VecPrint(
      NMTrainerResEval(&trainer)[iOut],
      arg->fpDoc);

    float percCorrect =
      (float)NMTrainerGetNbCorrect(&trainer)[iOut] /
      (float)GDSGetSizeCat(
        NMTrainerDataset(&trainer),
        NMTrainerGetICatTraining(&trainer)) * 100.0;
    if (
      arg->oneHot == false &&
      iOut == GDSGetNbOutputs(&dataset)) {

      percCorrect /= (float)GDSGetNbOutputs(&dataset);

    }
    fprintf(
      arg->streamInfo,
      " %.2f%%",
      percCorrect);
    fprintf(
      arg->fpDoc,
      " %.2f\\%%",
      percCorrect);
    arg->accPredTrain = percCorrect;

    fprintf(
      arg->streamInfo,
      "\n");
    fprintf(
      arg->fpDoc,
      "\\\\\n");

  }
  fprintf(
    arg->fpDoc,
    "}\\\\\n");
  fprintf(
    arg->fpDoc,
    "\\hline\n" \
    "\\end{tabular}\\\n" \
    "\\end{center}\n");

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
      "s.%d\n",
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


void RGBHSV() {

  TrainArg arg = {
    .label = "RGB to HSV",
    .type = "Regression",
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
    .precAcc = PBMATH_EPSILON,
    .pcaFlag = true,
    .streamInfo = stdout
  };
  Train(&arg);

}

void DiabeteRisk() {

  TrainArg arg = {
    .label = "Diabete",
    .type = "Classification",
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
    .precAcc = PBMATH_EPSILON,
    .pcaFlag = true,
    .streamInfo = stdout
  };
  Train(&arg);

}

void HCV() {

  TrainArg arg = {
    .label = "Hepatitis",
    .type = "Classification",
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
    .precAcc = PBMATH_EPSILON,
    .pcaFlag = true,
    .streamInfo = stdout
  };
  Train(&arg);

}

void Amphibian() {

  TrainArg arg = {
    .label = "Amphibian",
    .type = "Classification",
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
    .precAcc = PBMATH_EPSILON,
    .pcaFlag = true,
    .streamInfo = stdout
  };
  Train(&arg);

}

void MNIST() {

  TrainArg arg = {
    .label = "MNIST",
    .type = "Classification",
    .pathDataset = "./Datasets/mnist.json",
    .seed = 0,
    .percSampleEval = 10,
    .oneHot = true,
    .allHot = false,
    .weakUnitThreshold = 0.95,
    .depth = 2,
    .maxLvlDiv = 0,
    .nbMaxInputsUnit = 2,
    .nbMaxUnitDepth = 10,
    .order = 2,
    .nbDisplay = 5,
    .precAcc = PBMATH_EPSILON,
    .pcaFlag = false,
    .streamInfo = stdout
  };
  Train(&arg);

}

void WisconsinDiagnosticBreastCancerDataset() {

  FILE* fpDoc =
    fopen(
      "./Validation/wdbc.tex",
      "w");
  TrainArg arg = {
    .label = "Breast cancer",
    .type = "Classification",
    .pathDataset = "./Datasets/wdbc.json",
    .seed = 0,
    .percSampleEval = 10,
    .oneHot = true,
    .allHot = false,
    .weakUnitThreshold = 0.9,
    .depth = 5,
    .maxLvlDiv = 1,
    .nbMaxInputsUnit = 2,
    .nbMaxUnitDepth = 21,
    .order = 2,
    .nbDisplay = 5,
    .precAcc = PBMATH_EPSILON,
    .pcaFlag = true,
    .streamInfo = stdout,
    .fpDoc = fpDoc
  };
  Train(&arg);
  fclose(fpDoc);

}

void Iris() {

  FILE* fpDoc =
    fopen(
      "./Validation/iris.tex",
      "w");
  TrainArg arg = {
    .label = "Iris",
    .type = "Classification",
    .pathDataset = "./Datasets/iris.json",
    .seed = 0,
    .percSampleEval = 10,
    .oneHot = true,
    .allHot = false,
    .weakUnitThreshold = 0.975,
    .depth = 2,
    .maxLvlDiv = 1,
    .nbMaxInputsUnit = 2,
    .nbMaxUnitDepth = 11,
    .order = 1,
    .nbDisplay = 5,
    .precAcc = PBMATH_EPSILON,
    .pcaFlag = true,
    .streamInfo = stdout,
    .fpDoc = fpDoc
  };
  Train(&arg);
  fclose(fpDoc);

}

void Annealing() {

  FILE* fpDoc =
    fopen(
      "./Validation/annealing.tex",
      "w");
  TrainArg arg = {
    .label = "Annealing",
    .type = "Classification",
    .pathDataset = "./Datasets/annealing.json",
    .seed = 0,
    .percSampleEval = 10,
    .oneHot = true,
    .allHot = false,
    .weakUnitThreshold = 0.8,
    .depth = 4,
    .maxLvlDiv = 3,
    .nbMaxInputsUnit = 2,
    .nbMaxUnitDepth = 3,
    .order = 2,
    .nbDisplay = 5,
    .precAcc = PBMATH_EPSILON,
    .pcaFlag = true,
    .streamInfo = stdout,
    .fpDoc = fpDoc
  };
  Train(&arg);
  fclose(fpDoc);

}

void Arrythmia() {

  FILE* fpDoc =
    fopen(
      "./Validation/arrhythmia.tex",
      "w");
  TrainArg arg = {
    .label = "Arrythmia",
    .type = "Classification",
    .pathDataset = "./Datasets/arrhythmia.json",
    .seed = 0,
    .percSampleEval = 10,
    .oneHot = true,
    .allHot = false,
    .weakUnitThreshold = 0.9,
    .depth = 4,
    .maxLvlDiv = 0,
    .nbMaxInputsUnit = 2,
    .nbMaxUnitDepth = 1,
    .order = 2,
    .nbDisplay = 5,
    .precAcc = PBMATH_EPSILON,
    .pcaFlag = true,
    .streamInfo = stdout,
    .fpDoc = fpDoc
  };
  Train(&arg);
  fclose(fpDoc);

}

void SolarFlare() {

  FILE* fpDoc =
    fopen(
      "./Validation/solar-flare.tex",
      "w");
  TrainArg arg = {
    .label = "Solar Flare",
    .type = "Regression",
    .pathDataset = "./Datasets/solar-flare.json",
    .seed = 0,
    .percSampleEval = 10,
    .oneHot = false,
    .allHot = false,
    .weakUnitThreshold = 0.8,
    .depth = 2,
    .maxLvlDiv = 2,
    .nbMaxInputsUnit = 2,
    .nbMaxUnitDepth = 3,
    .order = 2,
    .nbDisplay = 5,
    .precAcc = 0.5,
    .pcaFlag = true,
    .streamInfo = stdout,
    .fpDoc = fpDoc
  };
  Train(&arg);
  fclose(fpDoc);

  fpDoc =
    fopen(
      "./Validation/solar-flare2.tex",
      "w");
  TrainArg arg2 = arg;
  arg2.precAcc = 1.0;
  Train(&arg2);
  fclose(fpDoc);

}

void AgaricusLepiota() {

  FILE* fpDoc =
    fopen(
      "./Validation/agaricus-lepiota.tex",
      "w");
  TrainArg arg = {
    .label = "AgaricusLepiota",
    .type = "Classification",
    .pathDataset = "./Datasets/agaricus-lepiota.json",
    .seed = 0,
    .percSampleEval = 10,
    .oneHot = true,
    .allHot = false,
    .weakUnitThreshold = 0.8,
    .depth = 6,
    .maxLvlDiv = 1,
    .nbMaxInputsUnit = 2,
    .nbMaxUnitDepth = 3,
    .order = 1,
    .nbDisplay = 5,
    .precAcc = PBMATH_EPSILON,
    .pcaFlag = false,
    .streamInfo = stdout,
    .fpDoc = fpDoc
  };
  Train(&arg);
  fclose(fpDoc);

}

void Abalone() {

  FILE* fpDoc =
    fopen(
      "./Validation/abalone.tex",
      "w");
  TrainArg arg = {
    .label = "Abalone",
    .type = "Regression",
    .pathDataset = "./Datasets/abalone.json",
    .seed = 0,
    .percSampleEval = 10,
    .oneHot = false,
    .allHot = false,
    .weakUnitThreshold = 0.8,
    .depth = 2,
    .maxLvlDiv = 0,
    .nbMaxInputsUnit = 2,
    .nbMaxUnitDepth = 3,
    .order = 1,
    .nbDisplay = 5,
    .precAcc = 0.5,
    .pcaFlag = true,
    .streamInfo = stdout,
    .fpDoc = fpDoc
  };
  Train(&arg);
  fclose(fpDoc);

  fpDoc =
    fopen(
      "./Validation/abalone2.tex",
      "w");
  TrainArg arg2 = arg;
  arg2.precAcc = 1.0;
  Train(&arg2);
  fclose(fpDoc);

}

void TicTacToe() {

  FILE* fpDoc =
    fopen(
      "./Validation/tic-tac-toe.tex",
      "w");
  TrainArg arg = {
    .label = "TicTacToe",
    .type = "Classification",
    .pathDataset = "./Datasets/tic-tac-toe.json",
    .seed = 0,
    .percSampleEval = 10,
    .oneHot = true,
    .allHot = false,
    .weakUnitThreshold = 0.8,
    .depth = 7,
    .maxLvlDiv = 0,
    .nbMaxInputsUnit = 2,
    .nbMaxUnitDepth = 3,
    .order = 2,
    .nbDisplay = 5,
    .precAcc = PBMATH_EPSILON,
    .pcaFlag = true,
    .streamInfo = stdout,
    .fpDoc = fpDoc
  };
  Train(&arg);
  fclose(fpDoc);

}

void PenDigits() {

  FILE* fpDoc =
    fopen(
      "./Validation/pendigits.tex",
      "w");
  TrainArg arg = {
    .label = "Pen Digits",
    .type = "Classification",
    .pathDataset = "./Datasets/pendigits.json",
    .seed = 0,
    .percSampleEval = 10,
    .oneHot = true,
    .allHot = false,
    .weakUnitThreshold = 0.8,
    .depth = 4,
    .maxLvlDiv = 2,
    .nbMaxInputsUnit = 2,
    .nbMaxUnitDepth = 3,
    .order = 2,
    .nbDisplay = 5,
    .precAcc = PBMATH_EPSILON,
    .pcaFlag = true,
    .streamInfo = stdout,
    .fpDoc = fpDoc
  };
  Train(&arg);
  fclose(fpDoc);

}

void ImageSegmentation() {

  FILE* fpDoc =
    fopen(
      "./Validation/segmentation.tex",
      "w");
  TrainArg arg = {
    .label = "Image Segmentation",
    .type = "Classification",
    .pathDataset = "./Datasets/segmentation.json",
    .seed = 0,
    .percSampleEval = 10,
    .oneHot = true,
    .allHot = false,
    .weakUnitThreshold = 0.8,
    .depth = 1,
    .maxLvlDiv = 3,
    .nbMaxInputsUnit = 2,
    .nbMaxUnitDepth = 3,
    .order = 2,
    .nbDisplay = 5,
    .precAcc = PBMATH_EPSILON,
    .pcaFlag = true,
    .streamInfo = stdout,
    .fpDoc = fpDoc
  };
  Train(&arg);
  fclose(fpDoc);

}

void Dermatology() {

  FILE* fpDoc =
    fopen(
      "./Validation/dermatology.tex",
      "w");
  TrainArg arg = {
    .label = "Dermatology Data Set",
    .type = "Classification",
    .pathDataset = "./Datasets/dermatology.json",
    .seed = 0,
    .percSampleEval = 10,
    .oneHot = true,
    .allHot = false,
    .weakUnitThreshold = 0.8,
    .depth = 1,
    .maxLvlDiv = 2,
    .nbMaxInputsUnit = 2,
    .nbMaxUnitDepth = 3,
    .order = 2,
    .nbDisplay = 5,
    .precAcc = PBMATH_EPSILON,
    .pcaFlag = true,
    .streamInfo = stdout,
    .fpDoc = fpDoc
  };
  Train(&arg);
  fclose(fpDoc);

}

void PageBlocks() {

  FILE* fpDoc =
    fopen(
      "./Validation/page-blocks.tex",
      "w");
  TrainArg arg = {
    .label = "Page Blocks Data Set",
    .type = "Classification",
    .pathDataset = "./Datasets/page-blocks.json",
    .seed = 0,
    .percSampleEval = 10,
    .oneHot = true,
    .allHot = false,
    .weakUnitThreshold = 0.8,
    .depth = 1,
    .maxLvlDiv = 2,
    .nbMaxInputsUnit = 2,
    .nbMaxUnitDepth = 3,
    .order = 3,
    .nbDisplay = 5,
    .precAcc = PBMATH_EPSILON,
    .pcaFlag = true,
    .streamInfo = stdout,
    .fpDoc = fpDoc
  };
  Train(&arg);
  fclose(fpDoc);

}

void BalanceScale() {

  FILE* fpDoc =
    fopen(
      "./Validation/balance-scale.tex",
      "w");
  TrainArg arg = {
    .label = "Balance Scale Data Set",
    .type = "Classification",
    .pathDataset = "./Datasets/balance-scale.json",
    .seed = 0,
    .percSampleEval = 10,
    .oneHot = true,
    .allHot = false,
    .weakUnitThreshold = 0.8,
    .depth = 4,
    .maxLvlDiv = 3,
    .nbMaxInputsUnit = 2,
    .nbMaxUnitDepth = 3,
    .order = 1,
    .nbDisplay = 5,
    .precAcc = PBMATH_EPSILON,
    .pcaFlag = true,
    .streamInfo = stdout,
    .fpDoc = fpDoc
  };
  Train(&arg);
  fclose(fpDoc);

}

void Vote() {

  FILE* fpDoc =
    fopen(
      "./Validation/vote.tex",
      "w");
  TrainArg arg = {
    .label = "Vote Data Set",
    .type = "Classification",
    .pathDataset = "./Datasets/vote.json",
    .seed = 0,
    .percSampleEval = 10,
    .oneHot = true,
    .allHot = false,
    .weakUnitThreshold = 0.8,
    .depth = 1,
    .maxLvlDiv = 0,
    .nbMaxInputsUnit = 2,
    .nbMaxUnitDepth = 3,
    .order = 1,
    .nbDisplay = 5,
    .precAcc = PBMATH_EPSILON,
    .pcaFlag = true,
    .streamInfo = stdout,
    .fpDoc = fpDoc
  };
  Train(&arg);
  fclose(fpDoc);

}

void Search() {

  FILE* fp =
    fopen(
      "/dev/null",
      "w");
  TrainArg bestArg = {
    .label = "Annealing",
    .type = "Classification",
    .pathDataset = "./Datasets/annealing.json",
    .seed = 0,
    .percSampleEval = 10,
    .oneHot = true,
    .allHot = false,
    .weakUnitThreshold = 0.9,
    .depth = 4,
    .maxLvlDiv = 4,
    .nbMaxInputsUnit = 2,
    .nbMaxUnitDepth = 3,
    .order = 2,
    .nbDisplay = 5,
    .precAcc = PBMATH_EPSILON,
    .pcaFlag = true,
    .streamInfo = fp,
    .fpDoc = fp
  };
  
  TrainArg arg = bestArg;
  float bestAcc = -1.0;
  float bestAccTrain = -1.0;
  float bestTime = -1.0;
  float minBest = -1.0;
  for (
    int depth = 1;
    depth < 11;
    ++depth) {

    printf(
      "\nSearch depth %d\n",
      depth);

    for (
      int maxLvlDiv = 0;
      maxLvlDiv < 4;
      ++maxLvlDiv) {

      for (
        int nbMaxUnitDepth = 3;
        nbMaxUnitDepth < 11;
        nbMaxUnitDepth += 10) {

        for (
          int order = 1;
          order < 4;
          ++order) {

          for (
            float weakUnitThreshold = 0.8;
            weakUnitThreshold < 0.999;
            weakUnitThreshold += 10.015) {

            arg.weakUnitThreshold = weakUnitThreshold;  
            arg.order = order;  
            arg.nbMaxUnitDepth = nbMaxUnitDepth;  
            arg.maxLvlDiv = maxLvlDiv;  
            arg.depth = depth;  
            Train(&arg);

            // Potential best
            if (
              bestAcc < 0.0 ||
              minBest < arg.accPred ||
              (ISEQUALF(bestAcc, arg.accPred) &&
              bestAccTrain < arg.accPredTrain) ||
              (ISEQUALF(bestAcc, arg.accPred) &&
              ISEQUALF(bestAccTrain, arg.accPredTrain) &&
              bestTime > arg.time)) {

              printf("Potential best. Check influence of random seed\n");
              TrainArg argCheck = arg;
              float best = arg.accPred;
              minBest = best;
              float maxBest = best;
              for (
                int seed = 1;
                seed < 5;
                ++seed) {

                argCheck.seed = seed;
                Train(&argCheck);
                best += argCheck.accPred;
                minBest =
                  MIN(
                    minBest,
                    argCheck.accPred);
                maxBest =
                  MAX(
                    maxBest,
                    argCheck.accPred);

              }

              arg.accPred = best / 5.0;
              printf(
                "Seed variation: %f %f %f\n",
                minBest,
                arg.accPred,
                maxBest);

              // Real best
              if (
                bestAcc < 0.0 ||
                bestAcc < arg.accPred ||
                (ISEQUALF(bestAcc, arg.accPred) &&
                bestAccTrain < arg.accPredTrain) ||
                (ISEQUALF(bestAcc, arg.accPred) &&
                ISEQUALF(bestAccTrain, arg.accPredTrain) &&
                bestTime > arg.time)) {

                bestArg = arg;
                bestAcc = arg.accPred;
                bestAccTrain = arg.accPredTrain;
                bestTime = arg.time;
                printf(
                  "best: predAcc:%f trainAcc:%f trainTime:%f\n",
                  bestAcc, bestAccTrain, bestTime);
                TrainArgPrint(
                  &bestArg,
                  stdout);
                fflush(stdout);

              }

            }

          }

        }

      }

    }

  }

  fclose(fp);

}

int main() {

  //Search();
  //WisconsinDiagnosticBreastCancerDataset();
  //Iris();
  Annealing();
  //Arrythmia();
  //AgaricusLepiota();
  //SolarFlare();
  //Abalone();
  //TicTacToe();
  //PenDigits();
  //ImageSegmentation();
  //Dermatology();
  //PageBlocks();
  //BalanceScale();
  //Vote();


/*
  RGBHSV();
  DiabeteRisk();
  HCV();
  Amphibian();
  DocFooterTab();

  DocHeaderTab();
  //MNIST();
*/

  // Return success code
  return 0;

}

