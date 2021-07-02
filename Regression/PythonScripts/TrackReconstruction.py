import numpy as np
import matplotlib.pyplot as plt
from matplotlib.collections import LineCollection
from matplotlib import cm as cm
from matplotlib import colors as colors
import matplotlib.gridspec as gridspec
import uproot
import pandas as pd
from array import array
from ROOT import TGraph, TSpline3

def main():
    #File = '/home/mueller/Projects/SimpleNobleG4Detector/build/Muon_10keV_AllProcesses.root'
    #LoadDF(File, 'AllProcesses')
    File = '/home/mueller/Projects/SimpleNobleG4Detector/build/Muon_10keV_NoDecay.root'
    LoadDF(File, 'NoDecay')
    #File = '/home/mueller/Projects/SimpleNobleG4Detector/PythonScripts/SegmentedTracks_10keV.csv'
    #LoadDF(File, 'Python', 24, True)

def dQModel(dE, dx, F, P):
    dEdx = dE/dx
    #A = 33 # e-/keV 500 V/cm
    #k = 0.130
    #A = 0.87
    #k = (0.1225/1.3954)
    return ( (P[0]*dEdx / (1 + ( P[1] / F) * dEdx)) * dx ) / (23.6/1000)
    
def BuildTGraph(File, F, P):
    Tracks = uproot.open(File)['Tracking']
    NewDF = Tracks.pandas.df(['N', 'dE', 'dx', 'B'])
    P = [0.87, (0.1225/1.36)*0.5]
    NewDF['dQ'] = dQModel(NewDF.dE.to_numpy(), NewDF.dx.to_numpy(), F, P)
    for n in np.unique(NewDF.N.to_numpy()):
        Mask = (NewDF.N.to_numpy() == n ) & ( NewDF.B.to_numpy() < NBins)
        Groups = NewDF[Mask].groupby('B').sum()
        dQdxTemp = Groups.dQ.to_numpy() / 5.0
        dQdx2Temp = Groups.dQ2.to_numpy() / 5.0
        if len(dQdxTemp) == NBins:
            count += 1
            dQdx[n,:] = dQdxTemp
            dQdx2[n,:] = dQdx2Temp

    X = [ i*0.5 + 0.25 for i in range(NBins) ]
    Graph = TGraph(len(X), array('d', X), array('d', np.mean(dQdx, axis=0)))
    GVec = np.vectorize(TSpline3("graph", Graph).Eval)
    return GVec
    
def LoadDF(File, Suff='', NBins=25, CSV=False):
    if not CSV:
        Tracks = uproot.open(File)['Tracking']
        NewDF = Tracks.pandas.df(['N', 'dE', 'dx', 'B'])
    else:
        NewDF = pd.read_csv(File)

    NewDF['dQ'] = dQ(NewDF.dE.to_numpy(), NewDF.dx.to_numpy())
    NewDF['dQ2'] = dQ2(NewDF.dE.to_numpy(), NewDF.dx.to_numpy())
    print(NewDF.head(25))
    dQdx = np.empty((len(np.unique(NewDF.N.to_numpy())), NBins), dtype=float)
    dQdx2 = np.empty((len(np.unique(NewDF.N.to_numpy())), NBins), dtype=float)
    count = 0
    for n in np.unique(NewDF.N.to_numpy()):
        Mask = (NewDF.N.to_numpy() == n ) & ( NewDF.B.to_numpy() < NBins)
        Groups = NewDF[Mask].groupby('B').sum()
        dQdxTemp = Groups.dQ.to_numpy() / 5.0
        dQdx2Temp = Groups.dQ2.to_numpy() / 5.0
        if len(dQdxTemp) == NBins:
            count += 1
            dQdx[n,:] = dQdxTemp
            dQdx2[n,:] = dQdx2Temp
    print(f'Accepted events: {count}')

    #dQdxEff = np.empty((len(np.unique(NewDF.N.to_numpy())), 25), dtype=float)
    #for n in np.unique(NewDF.N.to_numpy()):
    #    Mask = (NewDF.N.to_numpy() == n ) & ( NewDF.B.to_numpy() < 25)
    #    Groups = NewDF[Mask].groupby('B').sum()
    #    dQdxEffTemp = dQ(Groups.dE.to_numpy(), 5.0) / 5.0
    #    if len(dQdxEffTemp) == 25:
    #        dQdxEff[n,:] = dQdxEffTemp
    #    else: print('Length requirement not met.')

    #Scatter(dQdx, 'Single Particle Recombination', Suff)
    #Histogram(dQdx)
    Scatter2([dQdx,dQdx2], 'Single Particle Recombination', NBins, Suff)
    #Scatter(dQdxEff, 'Coated Particle Recombination', 'coated')

def Scatter(dQdx, Title, NBins, suf):
    ICARUS = pd.read_csv('/home/mueller/Downloads/700Vcm.csv')
    
    Figure = plt.figure(figsize=(8,6), constrained_layout=True)
    Ax = Figure.add_subplot(1,1,1)
    Means = np.mean(dQdx, axis=0)
    Std = np.std(dQdx, axis=0)
    X = [ i*0.5 + 0.25 for i in range(NBins) ]
    Ax.errorbar(X, Means, yerr=Std/np.sqrt(len(dQdx)), fmt='go', label='Geant4 + ICARUS Recomb.')
    Ax.scatter(ICARUS.X.to_numpy(), ICARUS.Y.to_numpy(), label='ICARUS 3-Ton (500 V/cm)')
    Ax.set_xlabel('Residual Range [cm]')
    Ax.set_ylabel('dQ/dx [10^3 e-/cm]')
    Ax.set_xlim(0,13)
    Ax.set_ylim(0,25)
    Figure.suptitle(Title)
    Ax.legend()
    Figure.savefig(f'dQdxVsRR_{suf}')

def Scatter2(dQdx, Title, NBins, suf):
    ICARUS = pd.read_csv('/home/mueller/Downloads/700Vcm.csv')
    
    Figure = plt.figure(figsize=(8,6), constrained_layout=True)
    Ax = Figure.add_subplot(1,1,1)
    Means1 = np.mean(dQdx[0], axis=0)
    Means2 = np.mean(dQdx[1], axis=0)
    Std1 = np.std(dQdx[0], axis=0)
    Std2 = np.std(dQdx[1], axis=0)
    X = [ i*0.5 + 0.25 for i in range(NBins) ]
    Graph = TGraph(len(X), array('d', X), array('d', Means2))
    GVec = np.vectorize(TSpline3("graph", Graph).Eval)
    Ax.errorbar(X, Means1, yerr=Std1/np.sqrt(len(dQdx[0])), fmt='go', label='Geant4 + ICARUS Recomb.')
    Ax.errorbar(X, Means2, yerr=Std2/np.sqrt(len(dQdx[1])), fmt='ro', label='Geant4 + ICARUS Recomb. (MC Corrected)')
    Ax.scatter(ICARUS.X.to_numpy(), ICARUS.Y.to_numpy(), label='ICARUS 3-Ton (500 V/cm)')
    xlin = np.linspace(0.25, 13, 1000)
    Ax.plot(xlin, GVec(xlin))
    Ax.set_xlabel('Residual Range [cm]')
    Ax.set_ylabel('dQ/dx [10^3 e-/cm]')
    Ax.set_xlim(0,13)
    Ax.set_ylim(0,25)
    Figure.suptitle(Title)
    Ax.legend()
    Figure.savefig(f'dQdxVsRR_{suf}')
    
def Histogram(dQdx, Group=0):
    Figure = plt.figure(figsize=(8,8),constrained_layout=True)
    GS = gridspec.GridSpec(ncols=2, nrows=2, figure=Figure)
    Axs = [ Figure.add_subplot(GS[0,0]),
            Figure.add_subplot(GS[0,1]),
            Figure.add_subplot(GS[1,0]),
            Figure.add_subplot(GS[1,1]) ]
    for i in range(4):
        Axs[i].hist(dQdx[:,4*Group+i], bins=100, histtype='step', linewidth=2)
        Axs[i].set_xlabel('dQ/dx [10^3 e-/cm]')
        Axs[i].set_ylabel('Entries')
        Axs[i].set_xlim(0,30)
        Axs[i].set_title(f'Bin: {4*Group+i}')
    Figure.savefig(f'dQdxHist_Group{Group}')
    
def dQ(dE, dx):
    dEdx = dE/dx
    A = 33 # e-/keV 500 V/cm
    k = 0.130
    #A = 0.87
    #k = (0.1225/1.3954)
    return ( (A*dEdx / (1 + k*dEdx)) * dx )# / (23.6/1000)

def dQ2(dE, dx):
    dEdx = dE/dx
    A = 0.87
    k = (0.1225/1.3954)
    return ( (A*dEdx / (1 + k*dEdx)) * dx ) / (23.6/1000)

if __name__ == '__main__':
    main()
