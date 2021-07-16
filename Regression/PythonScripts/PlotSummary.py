import numpy as np
import uproot
import pandas as pd
import matplotlib.pyplot as plt
import yaml

def main():
    cfg="""
    Datasets:
      - Name: 'ICARUS_Muons'
        Prefix: 'Global'
        FigTitle: 'ICARUS 3-Ton Muons'
        Data: [ '/home/mueller/Dropbox/Projects/NobleG4Scripts/Fitting/data/ICARUS_3Ton_muons_absolute.csv' ]
        Summary: '/home/mueller/Projects/SimpleNobleG4Detector/Regression/build/ICARUS_Muons_Summary_Global_Jul14.root'
        Fields: [0.5, 0.35, 0.2]
        Range: [0, 20]
      - Name: 'ICARUS_Protons'
        Prefix: 'Global'
        FigTitle: 'ICARUS 3-Ton Protons'
        Data: [ '/home/mueller/Dropbox/Projects/NobleG4Scripts/Fitting/data/ICARUS_3Ton_protons_absolute.csv' ]
        Summary: '/home/mueller/Projects/SimpleNobleG4Detector/Regression/build/ICARUS_Protons_Summary_Global_Jul14.root'
        Fields: [0.5, 0.35, 0.2]
        Range: [0, 40]
      - Name: 'ArgoNeuT_Protons'
        Prefix: 'Global'
        FigTitle: 'ArgoNeuT Protons'
        Data: [ '/home/mueller/Dropbox/Projects/NobleG4Scripts/Fitting/data/ArgoNeuT_protons_80.csv',
                '/home/mueller/Dropbox/Projects/NobleG4Scripts/Fitting/data/ArgoNeuT_protons_60.csv',
                '/home/mueller/Dropbox/Projects/NobleG4Scripts/Fitting/data/ArgoNeuT_protons_50.csv',
                '/home/mueller/Dropbox/Projects/NobleG4Scripts/Fitting/data/ArgoNeuT_protons_40.csv' ]
        DataLabels: [ "phi = 80", "phi = 60", "phi = 50", "phi = 40" ]
        Summary: '/home/mueller/Projects/SimpleNobleG4Detector/Regression/build/ArgoNeuT_Protons_Summary_Global_Jul14.root'
        Fields: [0.481]
        Range: [0, 40]
      - Name: 'T600_Muons'
        Prefix: 'Global'
        FigTitle: 'ICARUS 3-Ton Muons'
        Data: [ '/home/mueller/Dropbox/Projects/NobleG4Scripts/Fitting/data/ICARUS_T600_protons.csv' ]
        Summary: '/home/mueller/Projects/SimpleNobleG4Detector/Regression/build/T600_Muons_Summary_Global_Jul14.root'
        Fields: [0.5]
        Range: [0, 20]
      - Name: 'ArgoNeuT_Deuterons'
        Prefix: 'Global'
        FigTitle: 'ArgoNeuT Deuterons'
        Data: [ '/home/mueller/Dropbox/Projects/NobleG4Scripts/Fitting/data/ArgoNeuT_deuterons.csv' ]
        Summary: '/home/mueller/Projects/SimpleNobleG4Detector/Regression/build/ArgoNeuT_Protons_Summary_Global_Jul14.root'
        Fields: [0.481]
        Range: [0, 40]
      - Name: 'ICARUS_Muons'
        Prefix: 'Best'
        FigTitle: 'ICARUS 3-Ton Muons'
        Data: [ '/home/mueller/Dropbox/Projects/NobleG4Scripts/Fitting/data/ICARUS_3Ton_muons_absolute.csv' ]
        Summary: '/home/mueller/Projects/SimpleNobleG4Detector/Regression/build/ICARUS_Muons_Summary_Best.root'
        Fields: [0.5, 0.35, 0.2]
        Range: [0, 20]
      - Name: 'ICARUS_Protons'
        Prefix: 'Best'
        FigTitle: 'ICARUS 3-Ton Protons'
        Data: [ '/home/mueller/Dropbox/Projects/NobleG4Scripts/Fitting/data/ICARUS_3Ton_protons_absolute.csv' ]
        Summary: '/home/mueller/Projects/SimpleNobleG4Detector/Regression/build/ICARUS_Protons_Summary_Best.root'
        Fields: [0.5, 0.35, 0.2]
        Range: [0, 40]
      - Name: 'ArgoNeuT_Protons'
        Prefix: 'Best'
        FigTitle: 'ArgoNeuT Protons'
        Data: [ '/home/mueller/Dropbox/Projects/NobleG4Scripts/Fitting/data/ArgoNeuT_protons_80.csv',
                '/home/mueller/Dropbox/Projects/NobleG4Scripts/Fitting/data/ArgoNeuT_protons_60.csv',
                '/home/mueller/Dropbox/Projects/NobleG4Scripts/Fitting/data/ArgoNeuT_protons_50.csv',
                '/home/mueller/Dropbox/Projects/NobleG4Scripts/Fitting/data/ArgoNeuT_protons_40.csv' ]
        DataLabels: [ "phi = 80", "phi = 60", "phi = 50", "phi = 40" ]
        Summary: '/home/mueller/Projects/SimpleNobleG4Detector/Regression/build/ArgoNeuT_Protons_Summary_Best.root'
        Fields: [0.481]
        Range: [0, 40]
      - Name: 'T600_Muons'
        Prefix: 'Best'
        FigTitle: 'ICARUS 3-Ton Muons'
        Data: [ '/home/mueller/Dropbox/Projects/NobleG4Scripts/Fitting/data/ICARUS_T600_protons.csv' ]
        Summary: '/home/mueller/Projects/SimpleNobleG4Detector/Regression/build/T600_Muons_Summary_Best.root'
        Fields: [0.5]
        Range: [0, 20]
      - Name: 'ArgoNeuT_Deuterons'
        Prefix: 'Best'
        FigTitle: 'ArgoNeuT Deuterons'
        Data: [ '/home/mueller/Dropbox/Projects/NobleG4Scripts/Fitting/data/ArgoNeuT_deuterons.csv' ]
        Summary: '/home/mueller/Projects/SimpleNobleG4Detector/Regression/build/ArgoNeuT_Deuterons_Summary_Best.root'
        Fields: [0.481]
        Range: [0, 40]
    Contours:
      - Name: 'Birks'
        Data: ['/home/mueller/Projects/SimpleNobleG4Detector/Regression/build/RegressionSummary4.root',
               '/home/mueller/Projects/SimpleNobleG4Detector/Regression/build/RegressionSummary5.root']
        Parameters: [0, 1]
        Range: [ [0.89, 0.94], [0.050, 0.055] ]
    SingleContours:
      - Name: 'Combined'
        FigTitle: 'Combined'
        Data: '/home/mueller/Projects/SimpleNobleG4Detector/Regression/build/RegressionSummary_Jul14.root'
        Tag: ['3Ton_Muons', '3Ton_Protons', 'T600_Muons', 'ArgoNeuT_Protons', 'ArgoNeuT_Deuterons']
        N: 240
        Parameters: [0, 1]
        Range: [ [0.87, 0.923], [0.0405, 0.062] ]
      - Name: '3Ton_Muons'
        FigTitle: 'ICARUS 3-Ton Muons'
        Data: '/home/mueller/Projects/SimpleNobleG4Detector/Regression/build/RegressionSummary_Jul14.root'
        Tag: '3Ton_Muons'
        N: 73
        Parameters: [0, 1]
        Range: [ [0.80, 1.00], [0.03, 0.07] ]
      - Name: '3Ton_Protons'
        FigTitle: 'ICARUS 3-Ton Protons'
        Data: '/home/mueller/Projects/SimpleNobleG4Detector/Regression/build/RegressionSummary_Jul14.root'
        Tag: '3Ton_Protons'
        N: 73
        Parameters: [0, 1]
        Range: [ [0.87, 0.923], [0.0405, 0.062] ]
      - Name: 'T600_Muons'
        FigTitle: 'ICARUS T600 Muons'
        Data: '/home/mueller/Projects/SimpleNobleG4Detector/Regression/build/RegressionSummary_Jul14.root'
        Tag: 'T600_Muons'
        N: 13
        Parameters: [0, 1]
        Range: [ [0.87, 0.923], [0.0405, 0.062] ]
      - Name: 'ArgoNeuT_Protons'
        FigTitle: 'ArgoNeuT Protons'
        Data: '/home/mueller/Projects/SimpleNobleG4Detector/Regression/build/RegressionSummary_Jul14.root'
        Tag: 'ArgoNeuT_Protons'
        N: 75
        Parameters: [0, 1]
        Range: [ [0.87, 0.923], [0.0405, 0.062] ]
      - Name: 'ArgoNeuT_Deuterons'
        FigTitle: 'ArgoNeuT Deuterons'
        Data: '/home/mueller/Projects/SimpleNobleG4Detector/Regression/build/RegressionSummary_Jul14.root'
        Tag: 'ArgoNeuT_Deuterons'
        N: 27
        Parameters: [0, 1]
        Range: [ [0.87, 0.923], [0.0405, 0.062] ]

    """
    plt.rcParams['text.usetex'] = True
    CFG = yaml.load(cfg, Loader=yaml.FullLoader)

    f1 = (r'$R(\frac{dE}{dx}, F) = \frac{A}{1+\frac{k_B}{F}\frac{dE}{dx}}$', [r'A', r'$k_B$'])
    
    for Dataset in CFG['Datasets']:
        Summaries = [ uproot.open(Dataset['Summary'])[f'Field{i}'] for i in range(len(Dataset['Fields'])) ]
        DFs = [ LoadData(Dataset['Data'], Dataset['Fields'][i]) for i in range(len(Dataset['Fields'])) ]
        PlotField(DFs, Summaries, Dataset)

    #for Contour in CFG['SingleContours']:
    #    PlotSingleContour(Contour, f1)
    
    #for Contour in CFG['Contours']:
    #    PlotContour(Contour, f1)

def PlotSingleContour(CFG, Function):
    if isinstance(CFG['Tag'], list):
        Data = [ uproot.open(CFG['Data'])[CFG['Tag'][i]] for i in range(len(CFG['Tag'])) ]
        X = np.array(Data[0][f'p{CFG["Parameters"][0]}'].array())
        Y = np.array(Data[0][f'p{CFG["Parameters"][1]}'].array())
        Z = np.sum( np.array([ np.array(Data[i]['chi2'].array()) for i in range(len(Data)) ]), axis=0 )

    else:
        Data = uproot.open(CFG['Data'])[CFG['Tag']]
        X = Data[f'p{CFG["Parameters"][0]}'].array()
        Y = Data[f'p{CFG["Parameters"][1]}'].array()
        Z = Data['chi2'].array()
    Range = [ [0.80, 1.00], [0.03, 0.07] ]

    Figure = plt.figure(figsize=(8,6), constrained_layout=True)
    Ax = Figure.add_subplot(1,1,1)
    Ax.tricontour(X, Y, Z-np.min(Z), levels=[2.30, 4.61])
    Ax.scatter([X[np.argmin(Z)]], [Y[np.argmin(Z)]])
    Ax.set_xlabel(Function[1][0], fontsize=16)
    Ax.set_ylabel(Function[1][1], fontsize=16)
    #Ax.set_xlim(CFG['Range'][0][0], CFG['Range'][0][1])
    #Ax.set_ylim(CFG['Range'][1][0], CFG['Range'][1][1])
    Ax.set_xlim(Range[0][0], Range[0][1])
    Ax.set_ylim(Range[1][0], Range[1][1])
    Ax.set_title(CFG['FigTitle'])

    #print( round(X[np.argmin(Z)], 5), round(Y[np.argmin(Z)], 5) )
    zi = np.argmin(Z)
    zmin = f'{Z[zi]:.5f}'
    xmin = f'{X[zi]:.5f}'
    ymin = f'{Y[zi]:.5f}'
    zmin_red = f'{Z[zi]/CFG["N"]:.5f}'
    print( f'Best parameters: ({xmin}, {ymin})\n\tChi^2: {zmin}\n\tReduced Chi^2: {zmin_red}' )

    TextPos = [0.8, 0.9]
    Ax.text(TextPos[0],
            TextPos[1],
            Function[0],
            horizontalalignment='center',
            fontsize=24,
            transform=Ax.transAxes,
            fontweight=1000,
            zorder=3)

    Figure.savefig('Contour_' + CFG['Name'])
    
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
    Prefix = CFG['Prefix']
    Title = CFG['Name']
    FigTitle = CFG['FigTitle']
    Range = CFG['Range']
    Figure = plt.figure(figsize=(8,6), constrained_layout=True)
    Ax = Figure.add_subplot()

    if len(Data[0]) == 1 :
        for fi, F in enumerate(Fields):
            Ax.errorbar(Data[fi][0][0].to_numpy(), Data[fi][0][4].to_numpy(), yerr=Data[fi][0][5].to_numpy(), fmt='o', label=f'{F} kV/cm', markersize=4, fillstyle='none')
            Ax.scatter(Summary[fi]['dEdx'].array(), Summary[fi]['R'].array(), s=1)
    elif len(Data) == 1:
        for l, L in enumerate(CFG['DataLabels']):
            Ax.errorbar(Data[0][l][0].to_numpy(), Data[0][l][4].to_numpy(), yerr=Data[0][l][5].to_numpy(), fmt='o', label=f'{L} ({Fields[0]} kV/cm)', markersize=4, fillstyle='none')
        Ax.scatter(Summary[0]['dEdx'].array(), Summary[0]['R'].array(), s=1)
        

    Ax.set_xlim(Range[0], Range[1])
    Ax.set_ylim(0, 1)
    Ax.set_xlabel('dE/dx [MeV/cm]')
    Ax.set_ylabel('Recombination')
    Ax.set_title(FigTitle)
    Ax.legend()
    Figure.savefig(Prefix + 'Results_' + Title, dpi=300)
    
def LoadData(Files, Field):
    print(Files)
    DFs = [ pd.read_csv(f, header=None) for f in Files ]
    DF = [ d[ np.abs(d[2] - Field) < 0.01 ] for d in DFs]
    return DF
    

if __name__ == '__main__':
    main()
