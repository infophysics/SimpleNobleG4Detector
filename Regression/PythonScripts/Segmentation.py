import numpy as np
import matplotlib.pyplot as plt
from matplotlib.collections import LineCollection
from matplotlib import cm as cm
from matplotlib import colors as colors
import matplotlib.gridspec as gridspec
import uproot
import pandas as pd

def LoadDF(File):
    Steps = uproot.open(File)['Tracking']
    DF = Steps.pandas.df(['N', 'dE', 'dx', 'x0', 'y0', 'z0', 'x1', 'y1', 'z1', 'id', 'B', 'rho'])
    print(DF.head(10))    
        
    DFs = list()
    for n in range(10): #np.unique(DF.N.to_numpy()):
        print(n)
        DFs.append(Segmentize(DF[DF.N == n], n))
    
    NewDF = pd.concat(DFs)
    NewDF.to_csv('SegmentedTracks.csv', index=False)
    print(np.sum( NewDF.B1 == NewDF.B2 ))
    print(len(DF), len(NewDF))
    print(NewDF.head(25))

def Segmentize(Data, n=0):
    PrimaryMask = Data['id'].to_numpy() == 1
    NotPrimaryMask = np.invert(PrimaryMask)
    BinSize = 0.5 # cm
    NBins = 25
    SecThreshold = 0.2 # cm

    CumL = list()
    DX = Data[PrimaryMask].dx.to_numpy()
    SumL = 0
    for i in range(len(Data[PrimaryMask])):
        #Row = Data[PrimaryMask].iloc[i]
        #P1 = Row.x0, Row.y0, Row.z0
        #d = Distance(P1, Row)
        SumL += DX[i]
        CumL.append(SumL)
    TotalLength = SumL

    BinAssignment = np.zeros(len(PrimaryMask), dtype=int)
    Distances = np.zeros(len(PrimaryMask), dtype=float)
    Gammas = np.zeros(len(PrimaryMask), dtype=float)
    TMPBins = np.zeros(len(Data[PrimaryMask]), dtype=int)
    CBin = 0
    Row = Data[PrimaryMask].iloc[-1]
    StartPoint = Row.x1, Row.y1, Row.z1
    EndPoint = Row.x0, Row.y0, Row.z0
    BinPoints = list()
    for i in range(len(Data[PrimaryMask]) - 1, -1, -1):
        b = int( (TotalLength - CumL[i]) / BinSize )
        if b < NBins and b != CBin:
            BinPoints.append( (StartPoint, EndPoint) )
            StartPoint = EndPoint
            CBin = b
        if b < NBins:
            Row = Data[PrimaryMask].iloc[i]
            EndPoint = Row.x0, Row.y0, Row.z0
            TMPBins[i] = b
        else: TMPBins[i] = -1

    #for j in range(len(BinPoints)): print(BinPoints[j])

    BinAssignment[PrimaryMask] = TMPBins
    SecTMPBins = np.zeros(len(Data[NotPrimaryMask]), dtype=int)
    DTMPBins = np.zeros(len(Data[NotPrimaryMask]), dtype=float)
    gTMPBins = np.zeros(len(Data[NotPrimaryMask]), dtype=float)
    
    for i in range(len(Data[NotPrimaryMask])):
        Row = Data[NotPrimaryMask].iloc[i]
        #Row = Row.x1, Row.y1, Row.z1
        d = [ DCA(BinPoints[n][0], BinPoints[n][1], Row)[0] for n in range(len(BinPoints)) ]
        g = [ DCA(BinPoints[n][0], BinPoints[n][1], Row)[1] for n in range(len(BinPoints)) ]
        #print(min(d), min(d) < SecThreshold)
        if min(d) < SecThreshold: SecTMPBins[i] = np.argmin(d)
        else: SecTMPBins[i] = -1
        if SecTMPBins[i] == -1: DTMPBins[i] = min(d)
        gTMPBins[i] = g[np.argmin(d)]
        #print(min(d) < SecThreshold, BinAssignment[np.invert(PrimaryMask)][i])

    BinAssignment[NotPrimaryMask] = SecTMPBins
    Distances[NotPrimaryMask] = DTMPBins
    Gammas[NotPrimaryMask] = gTMPBins

    Mask = ( BinAssignment != -2 )# & ( Data.dE.to_numpy() != 0 ) & ( Data.dx.to_numpy() != 0 )
    DataDict = { 'N': Data[Mask].N.to_numpy(),
                 'dE': Data[Mask].dE.to_numpy(),
                 'dx': Data[Mask].dx.to_numpy(),
                 'id': Data[Mask]['id'].to_numpy(),
                 'rho': Data[Mask].rho.to_numpy(),
                 'd': Distances[Mask],
                 'g': Gammas[Mask],
                 'B1': Data[Mask].B.to_numpy(),
                 'B2': BinAssignment[Mask] }

    Draw = False
    if Draw:
        Figure = plt.figure(constrained_layout=True)
        Ax1 = Figure.add_subplot(2,1,1)
        Ax2 = Figure.add_subplot(2,1,2)
        Mask = ( BinAssignment != -1 )
        collection, R = CreateCollection(Data[Mask], Data[Mask].dE.to_numpy()/Data[Mask].dx.to_numpy(), VMin=0, VMax=30)
        Ax1.add_collection(collection)
        collection, R = CreateCollection(Data[Mask], BinAssignment[Mask], VMin=-1, VMax=12)
        Ax2.add_collection(collection)
        plt.setp(Ax1.get_xticklabels(), visible=False)
        Ax1.set_xlim(R[0]-5, R[1]+5)
        Ax1.set_ylim(R[2]-2, R[3]+2)
        Ax2.set_xlim(R[0]-5, R[1]+5)
        Ax2.set_ylim(R[2]-2, R[3]+2)
        #Figure.legend()
        Ax1.set_xlabel('Z Position [mm]')
        Ax1.set_ylabel('X Position [mm]')
        Ax2.set_xlabel('Z Position [mm]')
        Ax2.set_ylabel('X Position [mm]')
        Figure.savefig(f'Segments_{n}.png', dpi=600)
        
    #print(TotalLength)
    return pd.DataFrame(DataDict)

    #print(np.sum(Data[PrimaryMask].dE.to_numpy()))
    #print(np.sum(Data.dE.to_numpy()))
    #print(Data[np.invert(PrimaryMask)].head(10))

    #print(len(Data[PrimaryMask]))
    #print(np.unique(TMPBins, return_counts=True))
    #print(TMPBins)

    #print(len(Data[np.invert(PrimaryMask)]))
    #print(np.unique(BinAssignment, return_counts=True))
    
    #PlotEvent(Data[PrimaryMask], 0, suf='Tracking')
    
def DCA(P1, P2, P):
    P = P.x1, P.y1, P.z1
    a = ( (P[0]-P1[0])*(P2[0]-P1[0]) +
          (P[1]-P1[1])*(P2[1]-P1[1]) +
          (P[2]-P1[2])*(P2[2]-P1[2]) )
    b = ( (P2[0]-P1[0])**2 +
          (P2[1]-P1[1])**2 +
          (P2[2]-P1[2])**2 )
    g = a/b
    v = [ P1[0]+g*(P2[0]-P1[0]) - P[0],
          P1[1]+g*(P2[1]-P1[1]) - P[1],
          P1[2]+g*(P2[2]-P1[2]) - P[2] ]
    d = np.sqrt( v[0]**2 + v[1]**2 + v[2]**2 )
    gClamp = ( g >= 0 and g <= 1.0 )
    return (1000 if (np.isnan(d) or not gClamp) else d), g

def Distance(P1, Row):
    P2 = Row.x1, Row.y1, Row.z1
    return np.sqrt( (P1[0]-P2[0])**2 + (P1[1]-P2[1])**2 + (P1[2]-P2[2])**2 )

def CreateCollection(Data, ColorVar, VMin=0, VMax=5):
    x0 = Data.x0.to_numpy()
    x1 = Data.x1.to_numpy()
    z0 = Data.z0.to_numpy()
    z1 = Data.z1.to_numpy()

    m = plt.cm.ScalarMappable(cmap=cm.viridis, norm=colors.Normalize(vmin=VMin, vmax=VMax, clip=True))

    Lines = list()
    for i in range(len(x0)):
        Lines.append( [(z0[i], x0[i]), (z1[i], x1[i])] )

    Colors = m.to_rgba(ColorVar)
    Collection = LineCollection(Lines, colors=Colors)

    Range = np.min(z0), np.max(z0), np.min(x0), np.max(x0)
    
    return Collection, Range
        
def PlotEvent(Data, N, suf=''):

    ColorVar = Data.dE.to_numpy()/Data.dx.to_numpy()
    Collection, Range = CreateCollection(Data, ColorVar)

    Figure = plt.figure()
    Ax = Figure.add_subplot(1,1,1)
    Ax.add_collection(Collection)
    Ax.set_xlim(Range[0]-5, Range[1]+5)
    Ax.set_ylim(Range[2]-2, Range[3]+2)

    if not suf: Figure.savefig(f'EVD{N}.png', dpi=600)
    else: Figure.savefig(f'EVD{N}_{suf}.png', dpi=600)

def main():
    File = '/home/mueller/Projects/SimpleNobleG4Detector/build/Muon_10keV_NoDecay.root'
    LoadDF(File)

if __name__ == '__main__':
    main()
