#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include "neuramorph.h"
#include "pbdataanalysis.h"

typedef struct Sample {
  float _props[17];
} Sample;

typedef struct DataSet {
  // Number of sample
  int _nbSample;
  // Samples
  Sample* _samples;
} DataSet;

void GenerateHCV() {

  FILE* f = fopen("./Datasets/hcvdat0.csv", "r");
  if (f == NULL) {
    printf("Couldn't open the data set file\n");
    return;
  }
  int ret = 0;
  DataSet dataset;
  DataSet* that = &dataset;
  that->_nbSample = 589;
  that->_samples = 
    PBErrMalloc(NeuraMorphErr, sizeof(Sample) * that->_nbSample);
  float cat;
  for (int iSample = 0; iSample < that->_nbSample; ++iSample) {
    ret = fscanf(f, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", 
      &cat,
      that->_samples[iSample]._props + 0,
      that->_samples[iSample]._props + 1,
      that->_samples[iSample]._props + 2,
      that->_samples[iSample]._props + 3,
      that->_samples[iSample]._props + 4,
      that->_samples[iSample]._props + 5,
      that->_samples[iSample]._props + 6,
      that->_samples[iSample]._props + 7,
      that->_samples[iSample]._props + 8,
      that->_samples[iSample]._props + 9,
      that->_samples[iSample]._props + 10,
      that->_samples[iSample]._props + 11);
    if (ret == EOF) {
      printf("Couldn't read the dataset\n");
      fclose(f);
      return;
    }
    if (ISEQUALF(cat, 0.0)) {
      that->_samples[iSample]._props[12] = 1.0;
      that->_samples[iSample]._props[13] = -1.0;
      that->_samples[iSample]._props[14] = -1.0;
      that->_samples[iSample]._props[15] = -1.0;
      that->_samples[iSample]._props[16] = -1.0;
    } else if (ISEQUALF(cat, 1.0)) {
      that->_samples[iSample]._props[12] = -1.0;
      that->_samples[iSample]._props[13] = 1.0;
      that->_samples[iSample]._props[14] = -1.0;
      that->_samples[iSample]._props[15] = -1.0;
      that->_samples[iSample]._props[16] = -1.0;
    } else if (ISEQUALF(cat, 2.0)) {
      that->_samples[iSample]._props[12] = -1.0;
      that->_samples[iSample]._props[13] = -1.0;
      that->_samples[iSample]._props[14] = 1.0;
      that->_samples[iSample]._props[15] = -1.0;
      that->_samples[iSample]._props[16] = -1.0;
    } else if (ISEQUALF(cat, 3.0)) {
      that->_samples[iSample]._props[12] = -1.0;
      that->_samples[iSample]._props[13] = -1.0;
      that->_samples[iSample]._props[14] = -1.0;
      that->_samples[iSample]._props[15] = 1.0;
      that->_samples[iSample]._props[16] = -1.0;
    } else if (ISEQUALF(cat, 4.0)) {
      that->_samples[iSample]._props[12] = -1.0;
      that->_samples[iSample]._props[13] = -1.0;
      that->_samples[iSample]._props[14] = -1.0;
      that->_samples[iSample]._props[15] = -1.0;
      that->_samples[iSample]._props[16] = 1.0;
    }
  }
  fclose(f);


  FILE* fp = fopen("./Datasets/hcv.json", "w");
  fprintf(fp,
"{\n \
\"dataSet\": \"HCV that\",\n \
\"dataSetType\": \"0\",\n \
\"desc\": \"https://archive.ics.uci.edu/ml/datasets/HCV+data\",\n \
  \"nbInputs\": \"12\",\n \
  \"nbOutputs\": \"5\",\n \
\"dim\": {\n \
  \"_dim\":\"1\",\n \
  \"_val\":[\"17\"]\n \
},\n \
\"nbSample\": \"%d\",\n \
\"samples\": [\n", that->_nbSample);
  for (int iSample = 0;
    iSample < that->_nbSample;
    ++iSample) {
      fprintf(fp,
"    {\n \
    \"_dim\":\"17\",\n \
    \"_val\":[\"%f\", \"%f\", \"%f\", \"%f\", \"%f\", \"%f\", \
    \"%f\", \"%f\", \"%f\", \"%f\", \"%f\", \"%f\", \"%f\", \"%f\", \"%f\", \"%f\", \"%f\"]\n \
 }",
    that->_samples[iSample]._props[0],
    that->_samples[iSample]._props[1],
    that->_samples[iSample]._props[2],
    that->_samples[iSample]._props[3],
    that->_samples[iSample]._props[4],
    that->_samples[iSample]._props[5],
    that->_samples[iSample]._props[6],
    that->_samples[iSample]._props[7],
    that->_samples[iSample]._props[8],
    that->_samples[iSample]._props[9],
    that->_samples[iSample]._props[10],
    that->_samples[iSample]._props[11],
    that->_samples[iSample]._props[12],
    that->_samples[iSample]._props[13],
    that->_samples[iSample]._props[14],
    that->_samples[iSample]._props[15],
    that->_samples[iSample]._props[16]);

    if (iSample < that->_nbSample - 1)
      fprintf(fp, ",");
    fprintf(fp, "\n");
  }
  fprintf(fp,
"  ]\n" \
"}\n");
  fclose(fp);


}

int main() {

  GenerateHCV();
  return 0;

}
