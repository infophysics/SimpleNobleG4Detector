import numpy as np
import uproot
import pandas as pd
import matplotlib.pyplot as plt

def Recombination(dEdx, Field):
    Eps = 0.07*pow(Field, -0.85);
    return (1 - ( Eps * dEdx ) / ( 1 + Eps * dEdx ));

def CalcQ(R, dE):
    return (R*dE) / (23.6 / 1000000)

def CalcL(R, dE):
    return (dE / (19.5 / 1000000)) - CalcQ(R, dE)

def CalcYields(G4Sample, Field, Type):
    G4Dir = '/home/mueller/Projects/SimpleNobleG4Detector/Macros/G4Output/'
    G4T = uproot.open(f'{G4Dir}{G4Sample}.root')['Kinematics']
    Rs = Recombination(G4T.array('dEdx'), Field/1000)
    Qns = CalcQ(Rs, G4T.array('dE')) if Type == 'Q' else CalcL(Rs, G4T.array('dE'))
    NEvents = np.max(G4T.array('N')) + 1
    MeanE = 1000 * np.sum(G4T.array('dE')) / ( NEvents )
    MeanQn = ( np.sum(Qns) / NEvents ) / ( MeanE )
    return round(MeanQn,2)

def PlotSource(DMData, G4Sources):
    Figure = plt.figure(constrained_layout=False)
    Ax0 = Figure.add_subplot(2,1,1)
    Ax1 = Figure.add_subplot(2,1,2, sharex=Ax0)
    Axs = [Ax0, Ax1]
    Colors = ['C0','C1','C2','C3','C4','C5',]
    for i in range(2):
        SourceMask = [ str(x) == str(G4Sources[i]) for x in DMData['G4Sample'] ]
        print(sum(SourceMask))
        Fs = DMData['Field'].to_numpy()[SourceMask]
        Ys = DMData['Yield'].to_numpy()[SourceMask]
        Yw = (DMData['YLow'].to_numpy()[SourceMask] + DMData['YHigh'].to_numpy()[SourceMask]) / 2.0
        Ps = DMData['Prediction'].to_numpy()[SourceMask]

        YData = Ps/Ys - 1
        
        Axs[i].errorbar(Fs, YData, yerr=Yw/Ys,  label=G4Sources[i], fmt='o', markersize=3, c=Colors[i])
        Axs[i].set_ylim(-0.25, 0.25)
        Axs[i].set_xlim(0.1, 100000)
        Axs[i].set_xscale('log')
        Axs[i].grid(linestyle='--')
        Axs[i].set_ylabel('Deviation from Data [%]')
        Axs[i].set_xlabel('Field [V/cm]')
    
    Figure.legend()
    Figure.savefig(f'{G4Sources[0]}_Predictions.png')
    
def main():
    DMDir = '/home/mueller/Projects/ArNEST/NobleG4/'
    
    DMData = pd.read_csv(f'{DMDir}DMData.csv')
    NaNMask = [ str(x) != 'nan' for x in DMData['G4Sample'] ]
    DMData = DMData[NaNMask]

    DMData['Prediction'] = DMData.apply( lambda row: CalcYields(row['G4Sample'], row['Field'], row['Type']), axis=1)
    #DMData[['Cite', 'G4Sample', 'Energy', 'Field', 'Type', 'Yield', 'Prediction']].to_csv('PredictedDM.csv', index=False)
    
    G4Sources = ['113Sn_ceK2_364keV', '207Bi_ceK3_976keV']
    PlotSource(DMData, G4Sources)


if __name__ == '__main__':
    main()
