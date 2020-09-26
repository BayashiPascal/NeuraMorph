/*
with open('./Datasets/anneal.data') as f:
  res = ""
  row = f.readline()
  while row:
    props = row.split(',')
    clean_props = []
    for prop in props:
      try:
        float(prop)
        clean_props.append(prop)
      except ValueError:
        clean_props.append(str(ord(prop)))
    res += ','.join(clean_props)
    row = f.readline()
print(res)
*/


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
  float _props[44];
} Sample;

typedef struct DataSet {
  // Number of sample
  int _nbSample;
  // Samples
  Sample* _samples;
} DataSet;

void GenerateAnnealing() {

  FILE* f = fopen("./Datasets/anneal.data", "r");
  if (f == NULL) {
    printf("Couldn't open the data set file\n");
    return;
  }
  int ret = 0;
  DataSet dataset;
  DataSet* that = &dataset;
  that->_nbSample = 798;
  that->_samples = 
    PBErrMalloc(NeuraMorphErr, sizeof(Sample) * that->_nbSample);
  for (int iSample = 0; iSample < that->_nbSample; ++iSample) {
    ret = fscanf(f, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", 
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
      that->_samples[iSample]._props + 20,
      that->_samples[iSample]._props + 21,
      that->_samples[iSample]._props + 22,
      that->_samples[iSample]._props + 23,
      that->_samples[iSample]._props + 24,
      that->_samples[iSample]._props + 25,
      that->_samples[iSample]._props + 26,
      that->_samples[iSample]._props + 27,
      that->_samples[iSample]._props + 28,
      that->_samples[iSample]._props + 29,
      that->_samples[iSample]._props + 30,
      that->_samples[iSample]._props + 31,
      that->_samples[iSample]._props + 32,
      that->_samples[iSample]._props + 33,
      that->_samples[iSample]._props + 34,
      that->_samples[iSample]._props + 35,
      that->_samples[iSample]._props + 36,
      that->_samples[iSample]._props + 37,
      that->_samples[iSample]._props + 38);
    if (ret == EOF) {
      printf("Couldn't read the dataset\n");
      fclose(f);
      return;
    }
    float c = that->_samples[iSample]._props[38];
    for (int o = 38; o < 44; ++o) {
      if (ISEQUALF(c, o - 37)) {
        that->_samples[iSample]._props[o] = 1.0;
      } else {
        that->_samples[iSample]._props[o] = -1.0;
      }
    }
  }
  fclose(f);


  FILE* fp = fopen("./Datasets/annealing.json", "w");
  fprintf(fp,
"{\n \
\"dataSet\": \"Annealing dataset\",\n \
\"dataSetType\": \"0\",\n \
\"desc\": \"https://archive.ics.uci.edu/ml/datasets/Annealing\",\n \
  \"nbInputs\": \"38\",\n \
  \"nbOutputs\": \"6\",\n \
\"dim\": {\n \
  \"_dim\":\"1\",\n \
  \"_val\":[\"44\"]\n \
},\n \
\"nbSample\": \"%d\",\n \
\"samples\": [\n", that->_nbSample);
  for (int iSample = 0;
    iSample < that->_nbSample;
    ++iSample) {
      fprintf(fp,
"    {\n \
    \"_dim\":\"44\",\n \
    \"_val\":[");
    for (int iProp = 0; iProp < 44; ++iProp) {
      fprintf(fp,"\"%f\"",that->_samples[iSample]._props[iProp]);
      if (iProp < 43) {
        fprintf(fp,",");
      }
    }
    fprintf(fp,"]\n }");

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

  GenerateAnnealing();
  return 0;

}
