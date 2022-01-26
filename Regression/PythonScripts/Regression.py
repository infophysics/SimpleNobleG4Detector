import numpy as np
import uproot
import pandas as pd
from ROOT import TGraph
from ROOT import TSpline3
from array import array
import matplotlib.pyplot as plt


def dQModel(dE, dx, F, P):
    dEdx = dE/dx
    return ( (P[0]*dEdx / (1 + ( P[1] / F) * dEdx)) * dx ) / (23.6/1000)

def BuildTGraph(File, F, P):
    NBins = 25
    Tracks = uproot.open(File)['Tracking']
    NewDF = Tracks.pandas.df(['N', 'dE', 'dx', 'B'])
    NewDF['dQ'] = dQModel(NewDF.dE.to_numpy(), NewDF.dx.to_numpy(), F, P)

    dQdx = np.empty((len(np.unique(NewDF.N.to_numpy())), NBins), dtype=float)
    dEdx = np.empty((len(np.unique(NewDF.N.to_numpy())), NBins), dtype=float)
    for n in np.unique(NewDF.N.to_numpy()):
        Mask = (NewDF.N.to_numpy() == n ) & ( NewDF.B.to_numpy() < NBins)
        Groups = NewDF[Mask].groupby('B').sum()
        dQdxTemp = Groups.dQ.to_numpy() / 0.5
        dEdxTemp = Groups.dE.to_numpy() / 0.5
        if len(dQdxTemp) == NBins:
            dQdx[n,:] = dQdxTemp
            dEdx[n,:] = dEdxTemp

    X = [ i*0.5 + 0.25 for i in range(NBins) ]
    #Graph = TGraph(len(X), array('d', X), array('d', np.mean(dQdx, axis=0)))
    Graph = TGraph(len(X), array('d', np.mean(dEdx, axis=0)), array('d', np.mean(dQdx, axis=0)))
    GVec = np.vectorize(TSpline3("graph", Graph).Eval)
    return np.mean(dEdx, axis=0), np.mean(dQdx, axis=0), GVec
    #return GVec

def main():
    File = '/home/ncarrara/physics/SimpleNobleG4Detector/build/Muon_10keV_NoDecay.root'
    P = [0.87, (0.1225/1.36)*0.5]
    #TG_ICARUS_Muons_500Vcm = BuildTGraph(File, 0.5, P)
    Data_ICARUS_Muons_500Vcm = pd.read_csv('/home/ncarrara/physics/SimpleNobleG4Detector/Data/500Vcm.csv')

    dEdx, dQdx, GVec = BuildTGraph(File, 0.5, P)

    Data_ICARUS_dQdx = Data_ICARUS_Muons_500Vcm.Y.to_numpy()
    print(pd.DataFrame( { 'dQdx': Data_ICARUS_dQdx, 'dEdx': GVec(Data_ICARUS_dQdx) } ).head(30) )
    
    #Compare(dEdx, dQdx)

def Compare(dEdx, dQdx):
    Figure = plt.figure()
    Ax = Figure.add_subplot(111)
    Ax.scatter(dEdx, dQdx)
    Ax.set_xlim(0, 15)
    Ax.set_ylim(0, 250)

    print( pd.DataFrame( {'dEdx': dEdx, 'dQdx': dQdx} ).head() )
    

    Figure.savefig('dQdx_vs_dEdx')

if __name__ == '__main__':
    main()
