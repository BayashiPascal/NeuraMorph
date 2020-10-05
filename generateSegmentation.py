path_data = './Datasets/segmentation.data'
iFloatProps = [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18]
nbInProps = 19
invalidProps = []

with open(path_data) as f:
  res = ""
  row = f.readline()
  row = f.readline()
  row = f.readline()
  row = f.readline()
  row = f.readline()
  row = f.readline()
  data = []
  while row:
    props = row.split(',')
    props = props[1:] + [props[0]]
    data.append(props)
    row = f.readline()
nbSamples = len(data)
nbProps = len(data[0])
distinctValProps = []
for iProp in range(nbProps):
  distinctValProps.append(set([d[iProp] for d in data]))
#print(str(distinctValProps))
#quit()
nbInCleanProps = 0
nbOutCleanProps = 0
dataClean = []
for iProp in range(nbProps):
  if len(distinctValProps[iProp]) > 1 and \
      iProp not in invalidProps:

    if iProp in iFloatProps:
      if iProp < nbInProps:
        nbInCleanProps += 1
      else:
        nbOutCleanProps += 1
    else:
      if len(distinctValProps[iProp]) > 2:
        nb = len(distinctValProps[iProp])
      else:
        nb = 1
      if iProp < nbInProps:
        nbInCleanProps += nb
      else:
        nbOutCleanProps += nb

    for iSample, sample in enumerate(data):
      if len(dataClean) == iSample:
        dataClean.append([])
      if iProp in iFloatProps:
        try:
          dataClean[iSample].append(str(float(sample[iProp])))
        except ValueError:
          print(
            "Invalid float value on sample #" + str(iSample) +
            " column #" + str(iProp))
          #quit()
      else:
        if len(distinctValProps[iProp]) > 2:
          for catProp in distinctValProps[iProp]:
            if sample[iProp] == catProp:
              dataClean[iSample].append("1.0")
            else:
              dataClean[iSample].append("-1.0")
        else:
            if sample[iProp] == list(distinctValProps[iProp])[0]:
              dataClean[iSample].append("1.0")
            else:
              dataClean[iSample].append("-1.0")
gdsSamples = []
for row in dataClean:
  gdsSample = \
    '{"_dim":"' + str(nbInCleanProps + nbOutCleanProps) + \
    '","_val":["' + '","'.join(row) + '"]}'
  gdsSamples.append(gdsSample)
gds = \
  '{\n' + \
  ' "dataSet": "Image Segmentation Data Set",\n' + \
  ' "dataSetType": "0",\n' + \
  ' "desc": "http://archive.ics.uci.edu/ml/datasets/image+segmentation",\n' + \
  '   "nbInputs": "' + str(nbInCleanProps) + '",\n' + \
  '   "nbOutputs": "' + str(nbOutCleanProps) + '",\n' + \
  ' "dim": {\n' + \
  '   "_dim":"1",\n' + \
  '   "_val":["' + str(nbInCleanProps + nbOutCleanProps) + '"]\n' + \
  ' },\n' + \
  ' "nbSample": "' + str(nbSamples) + '",\n' + \
  ' "samples": [\n'
gds += ',\n'.join(gdsSamples)
gds += ']}\n'

print(gds)
