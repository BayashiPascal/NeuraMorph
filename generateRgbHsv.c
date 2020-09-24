#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include "neuramorph.h"
#include "pbdataanalysis.h"

typedef struct {
    double r;       // a fraction between 0 and 1
    double g;       // a fraction between 0 and 1
    double b;       // a fraction between 0 and 1
} Rgb;

typedef struct {
    double h;       // angle in degrees divided by 360
    double s;       // a fraction between 0 and 1
    double v;       // a fraction between 0 and 1
} Hsv;
// https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
Hsv rgb2hsv(Rgb in)
{
    Hsv         out;
    double      min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min  < in.b ? min  : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max  > in.b ? max  : in.b;

    out.v = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
        out.s = 0;
        out.h = 0; // undefined, maybe nan?
        return out;
    }
    if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0              
        // s = 0, h is undefined
        out.s = 0.0;
        out.h = 0.0;                            // its now undefined
        return out;
    }
    if( in.r >= max )                           // > is bogus, just keeps compilor happy
        out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
    else
    if( in.g >= max )
        out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
    else
        out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if( out.h < 0.0 )
        out.h += 360.0;
    out.h /= 360.0;

    return out;
}

void MakeRGB() {

  int nbSample = 1000;
  FILE* fp =
    fopen(
      "./Datasets/rgbhsv.json",
      "w");
  fprintf(
    fp,
    "{\n \
  \"dataSet\": \"RGB HSV dataset\",\n \
  \"dataSetType\": \"0\",\n \
  \"desc\": \"RGB HSV dataset\",\n \
  \"nbInputs\": \"3\",\n \
  \"nbOutputs\": \"3\",\n \
  \"dim\": {\n \
    \"_dim\":\"1\",\n \
    \"_val\":[\"6\"]\n \
  },\n \
  \"nbSample\": \"%d\",\n \
  \"samples\": [\n",
    nbSample);
  for (
    int iSample = 0;
    iSample < nbSample;
    ++iSample) {

    Rgb rgb;
    rgb.r = rnd();
    rgb.g = rnd();
    rgb.b = rnd();

    Hsv hsv = rgb2hsv(rgb);
       
      fprintf(fp,
"    {\n \
    \"_dim\":\"6\",\n \
    \"_val\":[\"%f\", \"%f\", \"%f\", \"%f\", \"%f\", \"%f\"]\n \
 }",
    rgb.r,
    rgb.g,
    rgb.b,
    hsv.h,
    hsv.s,
    hsv.v);

    if (iSample < nbSample - 1)
      fprintf(fp, ",");
    fprintf(fp, "\n");
  }
  fprintf(fp,
"  ]\n" \
"}\n");
  fclose(fp);

}

int main() {

  MakeRGB();
  return 0;

}

