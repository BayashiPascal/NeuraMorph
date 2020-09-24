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
  float _props[21];
} Sample;

typedef struct DataSet {
  // Number of sample
  int _nbSample;
  // Samples
  Sample* _samples;
} DataSet;

void GenerateAmphibian() {

  FILE* f = fopen("./Datasets/amphibian.csv", "r");
  if (f == NULL) {
    printf("Couldn't open the data set file\n");
    return;
  }
  int ret = 0;
  DataSet dataset;
  DataSet* that = &dataset;
  that->_nbSample = 189;
  that->_samples = 
    PBErrMalloc(NeuraMorphErr, sizeof(Sample) * that->_nbSample);
  for (int iSample = 0; iSample < that->_nbSample; ++iSample) {
    ret = fscanf(f, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", 
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
      that->_samples[iSample]._props + 11,
      that->_samples[iSample]._props + 12,
      that->_samples[iSample]._props + 13,
      that->_samples[iSample]._props + 14,
      that->_samples[iSample]._props + 15,
      that->_samples[iSample]._props + 16,
      that->_samples[iSample]._props + 17,
      that->_samples[iSample]._props + 18,
      that->_samples[iSample]._props + 19,
      that->_samples[iSample]._props + 20);
    if (ret == EOF) {
      printf("Couldn't read the dataset\n");
      fclose(f);
      return;
    }
    for (int o = 14; o < 21; ++o) {
      if (ISEQUALF(that->_samples[iSample]._props[o], 0.0)) {
        that->_samples[iSample]._props[o] = -1.0;
      }
    }
  }
  fclose(f);


  FILE* fp = fopen("./Datasets/amphibian.json", "w");
  fprintf(fp,
"{\n \
\"dataSet\": \"Amphibians dataset\",\n \
\"dataSetType\": \"0\",\n \
\"desc\": \"https://archive.ics.uci.edu/ml/datasets/Amphibians\",\n \
  \"nbInputs\": \"14\",\n \
  \"nbOutputs\": \"7\",\n \
\"dim\": {\n \
  \"_dim\":\"1\",\n \
  \"_val\":[\"21\"]\n \
},\n \
\"nbSample\": \"%d\",\n \
\"samples\": [\n", that->_nbSample);
  for (int iSample = 0;
    iSample < that->_nbSample;
    ++iSample) {
      fprintf(fp,
"    {\n \
    \"_dim\":\"21\",\n \
    \"_val\":[\"%f\", \"%f\", \"%f\", \"%f\", \"%f\", \"%f\", \
    \"%f\", \"%f\", \"%f\", \"%f\", \"%f\", \"%f\", \"%f\", \"%f\", \"%f\", \"%f\", \"%f\", \"%f\", \"%f\", \"%f\", \"%f\"]\n \
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
    that->_samples[iSample]._props[16],
    that->_samples[iSample]._props[17],
    that->_samples[iSample]._props[18],
    that->_samples[iSample]._props[19],
    that->_samples[iSample]._props[20]);

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

  GenerateAmphibian();
  return 0;

}
