with open('./Datasets/agaricus-lepiota.data') as f:
  res = ""
  row = f.readline()
  data = []
  while row:
    props = row.split(',')
    corrProps = props[1:] + [props[0]]
    data.append(corrProps)
    row = f.readline()
nbSamples = len(data)
nbProps = len(data[0])
distinctValProps = []
for iProp in range(nbProps):
  distinctValProps.append(set([d[iProp] for d in data]))
iFloatProps = []
nbInProps = 22
nbInCleanProps = 0
nbOutCleanProps = 0
dataClean = []
for iProp in range(nbProps):
  if len(distinctValProps[iProp]) > 1:
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
        dataClean[iSample].append(str(float(sample[iProp])))
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
  ' "dataSet": "Mushroom Data Set",\n' + \
  ' "dataSetType": "0",\n' + \
  ' "desc": "https://archive.ics.uci.edu/ml/datasets/mushroom",\n' + \
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
