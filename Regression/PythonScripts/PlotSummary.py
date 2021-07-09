import numpy as np
import uproot
import pandas as pd
import matplotlib.pyplot as plt
import yaml

def main():
    cfg="""
    Datasets:
      - Name: 'ICARUS_Muons'
        FigTitle: 'ICARUS Muons'
        Data: [ '/home/mueller/Dropbox/Projects/ArNEST/G4/ICARUS_3Ton_muons.csv' ]
        Summary: '/home/mueller/Projects/SimpleNobleG4Detector/Regression/build/ICARUS_Muons_Summary.root'
        Fields: [0.5, 0.35, 0.2]
        Range: [0, 20]
      - Name: 'ICARUS_Protons'
        FigTitle: 'ICARUS Protons'
        Data: [ '/home/mueller/Dropbox/Projects/ArNEST/G4/ICARUS_3Ton_protons.csv' ]
        Summary: '/home/mueller/Projects/SimpleNobleG4Detector/Regression/build/ICARUS_Protons_Summary.root'
        Fields: [0.5, 0.35, 0.2]
        Range: [0, 40]
      - Name: 'ArgoNeuT_Protons'
        FigTitle: 'ArgoNeuT Protons'
        Data: [ '/home/mueller/Dropbox/Projects/ArNEST/G4/ArgoNeuT_protons_80.csv',
                '/home/mueller/Dropbox/Projects/ArNEST/G4/ArgoNeuT_protons_60.csv',
                '/home/mueller/Dropbox/Projects/ArNEST/G4/ArgoNeuT_protons_50.csv',
                '/home/mueller/Dropbox/Projects/ArNEST/G4/ArgoNeuT_protons_40.csv' ]
        DataLabels: [ "phi = 80", "phi = 60", "phi = 50", "phi = 40" ]
        Summary: '/home/mueller/Projects/SimpleNobleG4Detector/Regression/build/ArgoNeuT_Protons_80_Summary.root'
        Fields: [0.481]
        Range: [0, 40]
    Contours:
      - Name: 'Birks'
        Data: ['/home/mueller/Projects/SimpleNobleG4Detector/Regression/build/RegressionSummary4.root',
               '/home/mueller/Projects/SimpleNobleG4Detector/Regression/build/RegressionSummary5.root']
        Parameters: [0, 1]
        Range: [ [0.89, 0.94], [0.050, 0.055] ]
    """
    plt.rcParams['text.usetex'] = True
    CFG = yaml.load(cfg, Loader=yaml.FullLoader)

    f1 = (r'$R(\frac{dE}{dx}, F) = \frac{A}{1+\frac{k_B}{F}\frac{dE}{dx}}$', [r'A', r'$k_B$'])
    
    for Dataset in CFG['Datasets']:
        Summaries = [ uproot.open(Dataset['Summary'])[f'Field{i}'] for i in range(len(Dataset['Fields'])) ]
        DFs = [ LoadData(Dataset['Data'], Dataset['Fields'][i]) for i in range(len(Dataset['Fields'])) ]
        PlotField(DFs, Summaries, Dataset)

    for Contour in CFG['Contours']:
        PlotContour(Contour, f1)
    
def PlotContour(CFG, Function):
    Data = [ uproot.open(d)['RegTree']  for d in CFG['Data'] ]
    X = np.concatenate([ d.array(f'p{CFG["Parameters"][0]}') for d in Data ])
    Y = np.concatenate([ d.array(f'p{CFG["Parameters"][1]}') for d in Data ])
    Z = np.concatenate([ d.array('chi2') for d in Data ])

    Figure = plt.figure(figsize=(8,6), constrained_layout=True)
    Ax = Figure.add_subplot(1,1,1)
    Ax.tricontour(X, Y, Z-np.min(Z), levels=[2.30, 4.61])
    Ax.scatter([X[np.argmin(Z)]], [Y[np.argmin(Z)]])
    Ax.set_xlabel(Function[1][0], fontsize=16)
    Ax.set_ylabel(Function[1][1], fontsize=16)
    Ax.set_xlim(CFG['Range'][0][0], CFG['Range'][0][1])
    Ax.set_ylim(CFG['Range'][1][0], CFG['Range'][1][1])

    print( X[np.argmin(Z)], Y[np.argmin(Z)] )

    TextPos = [0.8, 0.9]
    Ax.text(TextPos[0],
            TextPos[1],
            Function[0],
            horizontalalignment='center',
            fontsize=24,
            transform=Ax.transAxes,
            fontweight=1000,
            zorder=3)
    
    Figure.savefig(CFG['Name'])
    

def PlotField(Data, Summary, CFG):
    Fields = CFG['Fields']
    Title = CFG['Name']
    FigTitle = CFG['FigTitle']
    Range = CFG['Range']
    Figure = plt.figure(figsize=(8,6), constrained_layout=True)
    Ax = Figure.add_subplot()

    if len(Data[0]) == 1 :
        for fi, F in enumerate(Fields):
            Ax.errorbar(Data[fi][0][0].to_numpy(), Data[fi][0][4].to_numpy(), yerr=Data[fi][0][4].to_numpy() * Data[fi][0][5].to_numpy(), fmt='o', label=f'{F} kV/cm', markersize=4, fillstyle='none')
            Ax.scatter(Summary[fi].array('dEdx'), Summary[fi].array('R'), s=1)
    elif len(Data) == 1:
        for l, L in enumerate(CFG['DataLabels']):
            Ax.errorbar(Data[0][l][0].to_numpy(), Data[0][l][4].to_numpy(), yerr=Data[0][l][5].to_numpy(), fmt='o', label=f'{L} ({Fields[0]} kV/cm)', markersize=4, fillstyle='none')
        Ax.scatter(Summary[0].array('dEdx'), Summary[0].array('R'), s=1)
        

    Ax.set_xlim(Range[0], Range[1])
    Ax.set_ylim(0, 1)
    Ax.set_xlabel('dE/dx [MeV/cm]')
    Ax.set_ylabel('Recombination')
    Ax.set_title(FigTitle)
    Ax.legend()
    Figure.savefig(Title, dpi=300)
    
def LoadData(Files, Field):
    print(Files)
    DFs = [ pd.read_csv(f, header=None) for f in Files ]
    DF = [ d[ np.abs(d[2] - Field) < 0.01 ] for d in DFs]
    return DF
    

if __name__ == '__main__':
    main()
