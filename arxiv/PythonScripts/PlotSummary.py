import numpy as np
import uproot
import pandas as pd
import matplotlib.pyplot as plt
import yaml

def GetCFG():
    f = open('cfg.yaml')
    cfg = yaml.load(f, Loader=yaml.FullLoader)
    return cfg

def main():
    plt.rcParams['text.usetex'] = True
    CFG = GetCFG()

    f1 = (r'$R(\frac{dE}{dx}, F) = \frac{A}{1+k_B*F^{-c}\frac{dE}{dx}}$', [r'A', r'$k_B$'])

    Suff = CFG['Global']['SummarySuffix']
    for Dataset in CFG['Datasets']:
        Summaries = [ uproot.open(Dataset['Summary'])[f'Field{i}'] for i in range(len(Dataset['Fields'])) ]
        DFs = [ LoadData(Dataset['Data'], Dataset['Fields'][i]) for i in range(len(Dataset['Fields'])) ]
        PlotField(DFs, Summaries, Dataset)

    #for Contour in CFG['SingleContours'][0:1]:
    #    PlotSingleContour(Contour, f1, Suff)
    
    #for Contour in CFG['Contours']:
    #    PlotContour(Contour, f1)

def PlotSingleContour(CFG, Function, suf):
    print(CFG['Tag'])
    Pars = list()
    p0 = CFG['Parameters'][0]
    p1 = CFG['Parameters'][1]
    NPar = len(CFG['Parameters'])
    if isinstance(CFG['Tag'], list):
        Data = [ uproot.open(f'{CFG["Data"]}RegressionSummary_{suf}.root')[CFG['Tag'][i]] for i in range(len(CFG['Tag'])) ]
        #X = np.array(Data[0][f'p{CFG["Parameters"][0]}'].array())
        #Y = np.array(Data[0][f'p{CFG["Parameters"][1]}'].array())
        Pars = [ np.array(Data[0][f'p{CFG["Parameters"][i]}'].array()) for i in range(NPar) ]
        Z = np.sum( np.array([ np.array(Data[i]['chi2'].array()) for i in range(len(Data)) ]), axis=0 )

    else:
        Data = uproot.open(f'{CFG["Data"]}RegressionSummary_{suf}.root')[CFG['Tag']]
        #X = Data[f'p{CFG["Parameters"][0]}'].array()
        #Y = Data[f'p{CFG["Parameters"][1]}'].array()
        Pars = [ np.array(Data[f'p{CFG["Parameters"][i]}'].array()) for i in range(NPar) ]
        Z = Data['chi2'].array()
    Range = [ [0.80, 1.00], [0.03, 0.07] ]

    Mask = Pars[2] == 1.0
    zi = np.argmin(Z[Mask])
    MinPars = [ Pars[i][Mask][zi] for i in range(NPar) ]
    Masks = [ Pars[i] == MinPars[i] if i not in [p0, p1] else np.invert(np.isnan(Pars[i])) for i in range(NPar) ]
    ContourMask = np.all(Masks, axis=0)

    #zi = np.argmin(Z)
    #MinPars = [ Pars[i][zi] for i in range(NPar) ]
    #Masks = [ Pars[i] == MinPars[i] if i not in [p0, p1] else np.invert(np.isnan(Pars[i])) for i in range(NPar) ]
    #ContourMask = np.all(Masks, axis=0)

    Figure = plt.figure(figsize=(8,6), constrained_layout=True)
    Ax = Figure.add_subplot(1,1,1)
    Ax.tricontour(Pars[p0][ContourMask], Pars[p1][ContourMask], Z[ContourMask]-np.min(Z[ContourMask]), levels=[2.30, 4.61])
    Ax.scatter([Pars[p0][Mask][np.argmin(Z[Mask])]], [Pars[p1][Mask][np.argmin(Z[Mask])]])
    Ax.set_xlabel(Function[1][0], fontsize=16)
    Ax.set_ylabel(Function[1][1], fontsize=16)
    #Ax.set_xlim(CFG['Range'][0][0], CFG['Range'][0][1])
    #Ax.set_ylim(CFG['Range'][1][0], CFG['Range'][1][1])
    Ax.set_xlim(Range[0][0], Range[0][1])
    Ax.set_ylim(Range[1][0], Range[1][1])
    Ax.set_title(CFG['FigTitle'])

    #print( round(X[np.argmin(Z)], 5), round(Y[np.argmin(Z)], 5) )
    
    zmin = f'{Z[zi]:.5f}'
    #xmin = f'{Pars[p0][zi]:.5f}'
    #ymin = f'{Pars[p1][zi]:.5f}'
    ParMin = [ f'{Pars[i][zi]:.5f}' for i in range(len(CFG['Parameters'])) ]
    zmin_red = f'{Z[zi]/CFG["N"]:.5f}'
    print( f'Best parameters: {ParMin}\n\tChi^2: {zmin}\n\tReduced Chi^2: {zmin_red}' )

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
    Ax.set_xlabel('dE/dx [MeV/cm]', fontsize=16)
    Ax.set_ylabel('Electron Escape Probability R = 1 - r', fontsize=16)
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
