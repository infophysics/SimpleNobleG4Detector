import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

def LoadData(File):
    DF = pd.read_csv(File)
    DF = DF[ DF['Include'] == 1 ]
    Yw = (DF['YLow'] + DF['YHigh']) / 2.0
    Yw[ Yw == 0 ] = 0.05*DF['Yield'][ Yw == 0 ]
    DF = DF[ ['Cite', 'Energy', 'Field', 'Yield', 'Prediction', 'Residuals'] ]
    DF['YSigma'] = Yw
    return DF

def FieldPlot(DMData, Model):
    Fields = DMData['Field'].to_numpy()
    Cite = DMData['Cite'].to_numpy()
    Deviation = 100 * ((DMData['Prediction'] - DMData['Yield']) / DMData['Yield'])
    Dw = 100 * (DMData['YSigma'] / DMData['Yield'])

    Figure = plt.figure(constrained_layout=True, figsize=(20,7))
    Ax = Figure.add_subplot(1,1,1)

    for c in np.unique(Cite):
        CiteMask = (Cite == c)
        Ax.errorbar(Fields[CiteMask],
                    Deviation[CiteMask],
                    yerr=Dw[CiteMask],
                    label=c,
                    fmt='o',
                    markersize=3)

    Ax.set_xlabel('Field [V/cm]')
    Ax.set_ylabel('Deviation [%]')
    Ax.set_xlim(0.1, 100000)
    Ax.set_xscale('log')
    Ax.set_ylim(-25, 25)
    Ax.set_title('Percent Deviation of Fit Results from Data')
    Ax.grid(linestyle='--')
    Ax.legend()

    Figure.savefig(f'Model{Model}.png')

def main():
    InDir = '/home/mueller/Projects/SimpleNobleG4Detector/PythonScripts/'
    DMDataName = 'FitResults2.csv'
    DMData = LoadData(InDir+DMDataName)
    FieldPlot(DMData, 2)

if __name__ == '__main__':
    main()
