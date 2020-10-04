with open('./Datasets/arrhythmia.data') as f:
  res = ""
  row = f.readline()
  data = []
  while row:
    props = row.split(',')
    data.append(props)
    row = f.readline()
nbSamples = len(data)
nbProps = len(data[0])
distinctValProps = []
for iProp in range(nbProps):
  distinctValProps.append(set([d[iProp] for d in data]))
iFloatProps = []
nbInProps = 279
iNominalProps = [1]
for i in [21,33,45,57,69,81,93,105,117,129,141,153]:
  for j in range(6):
    iNominalProps.append(i+j)
for iProp in range(nbInProps):
  if iProp not in iNominalProps:
    iFloatProps.append(iProp)
nbInCleanProps = 0
nbOutCleanProps = 0
dataClean = []
invalidProps = [10,11,12,13,14]
for iProp in range(nbProps):
  if len(distinctValProps[iProp]) > 1 and \
      iProp not in invalidProps:
    if iProp in iFloatProps:
      if iProp < nbInProps:
        nbInCleanProps += 1
      else:
        nbOutCleanProps += 1
    else:
      if iProp < nbInProps:
        nbInCleanProps += len(distinctValProps[iProp])
      else:
        nbOutCleanProps += len(distinctValProps[iProp])
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
        for catProp in distinctValProps[iProp]:
          if sample[iProp] == catProp:
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
  ' "dataSet": "Arrhythmia dataset",\n' + \
  ' "dataSetType": "0",\n' + \
  ' "desc": "https://archive.ics.uci.edu/ml/datasets/arrhythmia",\n' + \
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
