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
  double clockStart = clock();
  NMTrainerRun(&trainer);
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
  if (
    nbOutDisplay > 1 &&
    arg->allHot == false) {

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
    if (arg->oneHot || arg->allHot) {

      float percCorrect =
        (float)NMTrainerGetNbCorrect(&trainer)[iOut] /
        (float)GDSGetSizeCat(
          NMTrainerDataset(&trainer),
          NMTrainerGetICatEval(&trainer)) * 100.0;
      fprintf(
        arg->streamInfo,
        " %.2f%%",
        percCorrect);
      fprintf(
        arg->fpDoc,
        " %.2f\\%%",
        percCorrect);
      arg->accPred = percCorrect;

    }

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
    if (arg->oneHot || arg->allHot) {

      float percCorrect =
        (float)NMTrainerGetNbCorrect(&trainer)[iOut] /
        (float)GDSGetSizeCat(
          NMTrainerDataset(&trainer),
          NMTrainerGetICatTraining(&trainer)) * 100.0;
      fprintf(
        arg->streamInfo,
        " %.2f%%",
        percCorrect);
      fprintf(
        arg->fpDoc,
        " %.2f\\%%",
        percCorrect);
      arg->accPredTrain = percCorrect;

    }

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

void Arrythmia() {

  TrainArg arg = {
    .label = "Arrythmia",
    .type = "Classification",
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
    .type = "Regression",
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
    .pcaFlag = false,
    .streamInfo = stdout
  };
  Train(&arg);

}

void Annealing() {

  TrainArg arg = {
    .label = "Annealing",
    .type = "Classification",
    .pathDataset = "./Datasets/annealing.json",
    .seed = 0,
    .percSampleEval = 10,
    .oneHot = true,
    .allHot = false,
    .weakUnitThreshold = 0.95,
    .depth = 6,
    .maxLvlDiv = 1,
    .nbMaxInputsUnit = 2,
    .nbMaxUnitDepth = 20,
    .order = 2,
    .nbDisplay = 5,
    .pcaFlag = true,
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
    .label = "Iris",
    .type = "Classification",
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
    .streamInfo = fp,
    .fpDoc = fp
  };
  
  TrainArg arg = bestArg;
  float bestAcc = -1.0;
  float bestAccTrain = -1.0;
  float bestTime = -1.0;
  for (
    int depth = 2;
    depth < 4;
    ++depth) {

    printf(
      "Search depth %d\n",
      depth);

    for (
      int maxLvlDiv = 0;
      maxLvlDiv < 3;
      ++maxLvlDiv) {

      for (
        int nbMaxUnitDepth = 1;
        nbMaxUnitDepth < 22;
        nbMaxUnitDepth += 10) {

        for (
          int order = 1;
          order < 4;
          ++order) {

          for (
            float weakUnitThreshold = 0.9;
            weakUnitThreshold < 0.999;
            weakUnitThreshold += 0.015) {

            arg.weakUnitThreshold = weakUnitThreshold;  
            arg.order = order;  
            arg.nbMaxUnitDepth = nbMaxUnitDepth;  
            arg.maxLvlDiv = maxLvlDiv;  
            arg.depth = depth;  
            Train(&arg);

            // Potential best
            if (
              bestAcc < 0.0 ||
              bestAcc < arg.accPred ||
              (ISEQUALF(bestAcc, arg.accPred) &&
              bestAccTrain < arg.accPredTrain) ||
              (ISEQUALF(bestAcc, arg.accPred) &&
              ISEQUALF(bestAccTrain, arg.accPredTrain) &&
              bestTime > arg.time)) {

              printf("Potential best. Check influence of random seed\n");
              TrainArg argCheck = arg;
              float best = arg.accPred;
              float minBest = best;
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
                  "best: %f %f %f\n",
                  bestAcc, bestAccTrain, bestTime);
                TrainArgPrint(
                  &bestArg,
                  stdout);

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
  Iris();


/*
  RGBHSV();
  DiabeteRisk();
  HCV();
  Amphibian();
  Abalone();
  DocFooterTab();

  DocHeaderTab();
  Arrythmia();
  //MNIST();
  Annealing();
*/

  // Return success code
  return 0;

}

