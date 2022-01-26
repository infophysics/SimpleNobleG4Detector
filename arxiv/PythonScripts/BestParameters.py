import numpy as np
import uproot
import sys

def main(File):
    ROOTFile = uproot.open(File)
    Keys = ROOTFile.keys()
    print(Keys)

    # Dataset information.
    UniqueKeys = dict()
    for k in Keys:
        u, i = k.split(";")
        if u in UniqueKeys.keys() and UniqueKeys[u] < i: UniqueKeys[u] = i
        else: UniqueKeys[u] = i
    ZippedKeys = [ k + ';' + UniqueKeys[k] for k in UniqueKeys.keys() ]
    print(f'Found {len(Keys)} datasets:')
    
    for k in ZippedKeys:
        print(f'\t{k.split(";")}')
        print(f'\t{k.split(";")[0]}')

    # Fit statistics.
    print('\n*** Fit Statistics ***')
    NPar = len(ROOTFile[ZippedKeys[0]].keys()) - 1
    NPoints = len(ROOTFile[ZippedKeys[0]]['chi2'].array())
    GridDim = [ len(np.unique(ROOTFile[ZippedKeys[0]][f'p{i}'])) for i in range(NPar) ]
    print(f'\tNumber of parameters: {NPar}')
    print(f'\tGrid points searched: {NPoints} {GridDim}')

    # Global results.
    FitResults = dict()
    for k in ZippedKeys:
        FitResults[k.split(';')[0]] = [ ROOTFile[k][sk].array() for sk in ROOTFile[k].keys() ]

    GlobalChiSquares = np.sum( [ FitResults[x][-1] for x in FitResults.keys() ], axis=0 )
    FitResults['Global'] = FitResults[list(FitResults.keys())[0]][0:NPar]
    FitResults['Global'].append( GlobalChiSquares )

    #Mask = FitResults[list(FitResults.keys())[0]][2] == 1.0
    #FitResults['ICARUS_3Ton_500'] = [ x[Mask] for x in FitResults[ list(FitResults.keys())[0] ][0:NPar] ]
    #FitResults['ICARUS_3Ton_500'].append( np.sum( [FitResults['ICARUS_3Ton_Muons_500'][-1][Mask], FitResults['ICARUS_3Ton_Protons_500'][-1][Mask] ], axis=0 ) )

    print('\n*** Fit Results ***')
    SummarizeResults(FitResults, 'Global', NPar)

    for k in ZippedKeys:
        SummarizeResults(FitResults, k.split(';')[0], NPar)

    #SummarizeResults(FitResults, 'ICARUS_3Ton_500', NPar)
    

def SummarizeResults(FitResults, Tag, NPar):
    Zi = np.argmin(FitResults[Tag][-1])
    ZMin = round(np.min(FitResults[Tag][-1]), 2)
    BestPars = [ round(x[Zi],4) for x in FitResults[Tag][0:NPar] ]
    RedChi2 = round( ZMin / ( NPoints(Tag) - NPar ), 2)
    
    print(f'--- {Tag} Results ---')
    print(f'\tBest Parameters: {BestPars}')
    print(f'\tChi^2 at Minimum: {ZMin}')
    print(f'\tReduced Chi^2: {RedChi2}')
    

def NPoints(Tag):
    NDict = {'Aprile_207Bi_ceK3_976keV': 25,
             'ARIS_241Am_gamma_59keV': 3,
             'Bondar_241Am_gamma_59keV': 3,
	     'Scalettar_113Sn_ceK2_364keV': 87,
             'Scalettar_113Sn_ceK2_364keV_Part1': 25,
             'Scalettar_113Sn_ceK2_364keV_Part2': 25,
             'Scalettar_113Sn_ceK2_364keV_Part3': 25,
             'Scalettar_113Sn_ceK2_364keV_Part4': 12,
	     'ArgoNeuT_Deuterons': 29,
	     'ArgoNeuT_Protons': 75,
	     'ICARUS_3Ton_Muons': 75,
	     'ICARUS_3Ton_Muons_500': 25,
	     'ICARUS_3Ton_Protons': 75,
	     'ICARUS_3Ton_Protons_500': 35,
	     'ICARUS_T600_Muons': 15}
    Total = sum( [ NDict[k] for k in NDict.keys() ] )
    NDict['ICARUS_3Ton_500'] = NDict['ICARUS_3Ton_Protons_500'] + NDict['ICARUS_3Ton_Muons_500']
    NDict['Global'] = Total
    return(NDict[Tag])
    
if __name__ == '__main__':
    NArg = len(sys.argv)
    if NArg == 2:
        File = sys.argv[1]
        main(File)
    else:
        print('Invalid arguments.')
