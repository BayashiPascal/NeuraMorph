with open('./Datasets/dataset_116_solar-flare_2.csv') as f:
  res = ""
  row = f.readline()
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
iFloatProps = [5,10,11,12]
nbInProps = 10
nbInCleanProps = 0
nbOutCleanProps = 0
dataClean = []
invalidProps = []
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
  ' "dataSet": "Solar Flare Data Set",\n' + \
  ' "dataSetType": "0",\n' + \
  ' "desc": "https://www.openml.org/data/get_csv/3597/dataset_116_solar-flare_2.arff",\n' + \
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
