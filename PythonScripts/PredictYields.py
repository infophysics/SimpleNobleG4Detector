import numpy as np
import uproot
import pandas as pd

def Recombination(dEdx, Field):
    Eps = 0.07*pow(Field, -0.85);
    return (1 - ( Eps * dEdx ) / ( 1 + Eps * dEdx ));

def CalcQ(R, dE):
    return (R*dE) / (23.6 / 1000000)

def CalcL(R, dE):
    return (dE / (19.5 / 1000000)) - CalcQ(R, dE)

def CalcYields(G4Sample, Field, Type):
    G4Dir = '/home/mueller/Projects/SimpleNobleG4Detector/SourceMacros/G4Output/'
    G4T = uproot.open(f'{G4Dir}{G4Sample}.root')['Kinematics']
    Rs = Recombination(G4T.array('dEdx'), Field/1000)
    Qns = CalcQ(Rs, G4T.array('dE')) if Type == 'Q' else CalcL(Rs, G4T.array('dE'))
    NEvents = np.max(G4T.array('N')) + 1
    MeanE = 1000 * np.sum(G4T.array('dE')) / ( NEvents )
    MeanQn = ( np.sum(Qns) / NEvents ) / ( MeanE )
    return round(MeanQn,2)

def main():
    DMDir = '/home/mueller/Projects/ArNEST/NobleG4/'
    
    DMData = pd.read_csv(f'{DMDir}DMData.csv')
    NaNMask = [ str(x) != 'nan' for x in DMData['G4Sample'] ]
    DMData = DMData[NaNMask]

    DMData['Prediction'] = DMData.apply( lambda row: CalcYields(row['G4Sample'], row['Field'], row['Type']), axis=1)
    DMData[['Cite', 'G4Sample', 'Energy', 'Field', 'Type', 'Yield', 'Prediction']].to_csv('PredictedDM.csv', index=False)
    
    #G4Samples = np.unique(DMData['G4Sample'].to_numpy())
    #for S in G4Samples: CalcYields(S, 500)
    #SampleMasks = [ DMData['G4Sample'] == S for S in G4Samples ]

if __name__ == '__main__':
    main()
