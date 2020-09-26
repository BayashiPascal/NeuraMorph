#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include "neuramorph.h"
#include "pbdataanalysis.h"

#define MNIST_IMGSIZE 28
#define NB_INPUT MNIST_IMGSIZE * MNIST_IMGSIZE
#define NB_OUTPUT 10

typedef struct Sample {
  float _props[794];
} Sample;

typedef struct DataSet {
  // Number of sample
  int _nbSample;
  // Samples
  Sample* _samples;
} DataSet;

typedef struct MNISTImg {
  unsigned char _cat;
  unsigned char _pixels[MNIST_IMGSIZE * MNIST_IMGSIZE];
} MNISTImg;

typedef struct MNIST {
  int _nbImg;
  MNISTImg* _imgs;
} MNIST;

MNIST* MNISTLoad(char* fnLbl, char* fnImg) {
  FILE* fLbl = fopen(fnLbl, "rb");
  if (!fLbl) {
    printf("Couldn't open %s\n", fnLbl);
    return NULL;
  }
  FILE* fImg = fopen(fnImg, "rb");
  if (!fImg) {
    printf("Couldn't open %s\n", fnImg);
    fclose(fLbl);
    return NULL;
  }
  MNIST* mnist = PBErrMalloc(&thePBErr, sizeof(MNIST));
  int buff;
  int ret;
  // Magic number
  for (int i = 4; i--;)
    ret = fread((char*)(&buff) + i, 1, 1, fLbl);
  if (buff != 2049) {
    printf("Magic number for %s is invalid (%d==2049)\n", fnLbl, buff);
    fclose(fLbl);
    fclose(fImg);
    return NULL;
  }
  for (int i = 4; i--;)
    ret = fread((char*)(&buff) + i, 1, 1, fImg);
  if (buff != 2051) {
    printf("Magic number for %s is invalid (%d==2051)\n", fnLbl, buff);
    fclose(fLbl);
    fclose(fImg);
    return NULL;
  }
  // Number of items
  for (int i = 4; i--;)
    ret = fread((char*)(&(mnist->_nbImg)) + i, 1, 1, fLbl);
  for (int i = 4; i--;)
    ret = fread((char*)(&buff) + i, 1, 1, fImg);
  if (buff != mnist->_nbImg) {
    printf("Nb of items doesn't match (%d==%d)\n", buff, mnist->_nbImg);
    fclose(fLbl);
    fclose(fImg);
    return NULL;
  }
  // Number of rows and columns
  for (int i = 4; i--;)
    ret = fread((char*)(&buff) + i, 1, 1, fImg);
  if (buff != 28) {
    printf("Unexpected image size (rows) (%d==%d)\n", 
      buff, 28);
    fclose(fLbl);
    fclose(fImg);
    return NULL;
  }
  for (int i = 4; i--;)
    ret = fread((char*)(&buff) + i, 1, 1, fImg);
  if (buff != 28) {
    printf("Unexpected image size (columns) (%d==%d)\n", 
      buff, 28);
    fclose(fLbl);
    fclose(fImg);
    return NULL;
  }
  // Images
  printf("Loading %d images...\n", mnist->_nbImg);
  mnist->_imgs = 
    PBErrMalloc(&thePBErr, sizeof(MNISTImg) * mnist->_nbImg);
  for (int iImg = 0; iImg < mnist->_nbImg; ++iImg) {
    MNISTImg* img = mnist->_imgs + iImg;
    // Label
    ret = fread(&(img->_cat), 1, 1, fLbl);
    // Pixels
    for (int iPixel = 0; iPixel < MNIST_IMGSIZE * MNIST_IMGSIZE; 
      ++iPixel) {
      ret = fread(img->_pixels + iPixel, 1, 1, fImg);
    }
  }
  printf("Loaded MNIST successfully.\n");
  fflush(stdout);
  fclose(fImg);
  fclose(fLbl);
  (void)ret;
  return mnist;
}

void GenerateMNIST() {

  MNIST* mnist = MNISTLoad("./Datasets/train-labels.idx1-ubyte", "./Datasets/train-images.idx3-ubyte");
  DataSet dataset;
  DataSet* that = &dataset;
  that->_nbSample = 60000;
  that->_samples = 
    PBErrMalloc(NeuraMorphErr, sizeof(Sample) * that->_nbSample);
  for (int iSample = 0; iSample < that->_nbSample; ++iSample) {
    for (int iProp = 0; iProp < 784; ++iProp) {
      that->_samples[iSample]._props[iProp] =
        mnist->_imgs[iSample]._pixels[iProp];
    }
    for (int iProp = 0; iProp < 10; ++iProp) {
      if (mnist->_imgs[iSample]._cat == iProp) {
        that->_samples[iSample]._props[784 + iProp] = 1.0;
      } else {
        that->_samples[iSample]._props[784 + iProp] = -1.0;
      }
    }
  }

  FILE* fp = fopen("./Datasets/mnist.json", "w");
  fprintf(fp,
"{\n \
\"dataSet\": \"MNIST\",\n \
\"dataSetType\": \"0\",\n \
\"desc\": \"http://yann.lecun.com/exdb/mnist/\",\n \
  \"nbInputs\": \"784\",\n \
  \"nbOutputs\": \"10\",\n \
\"dim\": {\n \
  \"_dim\":\"1\",\n \
  \"_val\":[\"794\"]\n \
},\n \
\"nbSample\": \"%d\",\n \
\"samples\": [\n", that->_nbSample);
  for (int iSample = 0;
    iSample < that->_nbSample;
    ++iSample) {
      fprintf(fp,
"    {\n \
    \"_dim\":\"794\",\n \
    \"_val\":[");
    for (int iProp = 0; iProp < 794; ++iProp) {
      fprintf(fp,"\"%f\"",that->_samples[iSample]._props[iProp]);
      if (iProp < 793) {
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

  GenerateMNIST();
  return 0;

}
