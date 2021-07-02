import numpy as np
import matplotlib.pyplot as plt
from matplotlib.collections import LineCollection
from matplotlib import cm as cm
from matplotlib import colors as colors
import matplotlib.gridspec as gridspec
import uproot
import pandas as pd

def LoadDF(File):

    Tracks = uproot.open(File)['Tracking']
    NewDF = Tracks.pandas.df(['N', 'dE', 'dx', 'B'])

    NewDF['dQ'] = dQ(NewDF.dE.to_numpy(), NewDF.dx.to_numpy())
    print(NewDF.head(25))
    dQdx = np.zeros((len(np.unique(NewDF.N.to_numpy())), 200), dtype=float)
    dQdx_E = np.zeros((len(np.unique(NewDF.N.to_numpy())), 200), dtype=float)
    count = 0
    for n in np.unique(NewDF.N.to_numpy()):
        Mask = NewDF.N == n
        Groups = NewDF[Mask].groupby('B').sum()
        dQdxTemp = (23.6/1000000) * Groups.dQ.to_numpy() / 0.5#5.0
        #dQdxTemp_E = Groups.dQ.to_numpy() / ( (23.6/1000000) / Groups.dE.to_numpy() )
        if len(dQdxTemp) == 200:
            count += 1
            dQdx[n,:] = dQdxTemp
    dQdx_MIP = np.median(dQdx, axis=0)[-1]
    print(count)
    print(dQdx_MIP)

    #dQdxEff = np.empty((len(np.unique(NewDF.N.to_numpy())), 25), dtype=float)
    #for n in np.unique(NewDF.N.to_numpy()):
    #    Mask = NewDF.N == n
    #    Groups = NewDF[Mask].groupby('B').sum()
    #    dQdxEffTemp = dQ(Groups.dE.to_numpy(), 5.0) / 5.0
    #    if len(dQdxEffTemp) == 25:
    #        dQdxEff[n,:] = dQdxEffTemp
    #    else: print('Length requirement not met.')
    
def dQ(dE, dx):
    dEdx = dE/(dx/10)
    A = 0.800
    k = 0.097/1.3954
    return ( (A / (1 + k*dEdx)) ) * (dE / (23.6/1000000) )

def main():
    File = '/home/mueller/Projects/SimpleNobleG4Detector/build/Muon_5MeV_50um.root'
    LoadDF(File)

if __name__ == '__main__':
    main()
