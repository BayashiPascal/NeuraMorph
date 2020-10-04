path_data = './Datasets/tic-tac-toe.data'
iFloatProps = []
nbInProps = 9
invalidProps = []

with open(path_data) as f:
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
#print(str(distinctValProps))
#quit()
nbInCleanProps = 9
nbOutCleanProps = 1
dataClean = []
for iProp in range(nbProps):
  if len(distinctValProps[iProp]) > 1 and \
      iProp not in invalidProps:

    for iSample, sample in enumerate(data):
      if len(dataClean) == iSample:
        dataClean.append([])
      if len(distinctValProps[iProp]) == 2:
        if sample[iProp] == "positive\n":
          dataClean[iSample].append("1.0")
        else:
          dataClean[iSample].append("-1.0")
      else:
          if sample[iProp] == 'x':
            dataClean[iSample].append("1.0")
          elif sample[iProp] == 'o':
            dataClean[iSample].append("-1.0")
          else:
            dataClean[iSample].append("0.0")
gdsSamples = []
for row in dataClean:
  gdsSample = \
    '{"_dim":"' + str(nbInCleanProps + nbOutCleanProps) + \
    '","_val":["' + '","'.join(row) + '"]}'
  gdsSamples.append(gdsSample)
gds = \
  '{\n' + \
  ' "dataSet": "Tic Tac Toe Data Set",\n' + \
  ' "dataSetType": "0",\n' + \
  ' "desc": "https://archive.ics.uci.edu/ml/datasets/Tic-Tac-Toe+Endgame",\n' + \
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
