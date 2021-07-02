import numpy as np
import matplotlib.pyplot as plt
from matplotlib.collections import LineCollection
from matplotlib import cm as cm
from matplotlib import colors as colors
import matplotlib.gridspec as gridspec
import uproot
import pandas as pd

def LoadDF(File):
    #Recalculate = False
    #if Recalculate:
    #    Steps = uproot.open(File)['Kinematics']
    #    DF = Steps.pandas.df(['N', 'dE', 'dx', 'x0', 'y0', 'z0', 'x1', 'y1', 'z1', 'id', 'pdg'])
    #    print(DF.head(10))    
    #    
    #    DFs = list()
    #    for n in range(250): #np.unique(DF.N.to_numpy()):
    #        print(n)
    #        DFs.append(Segmentize(DF[DF.N == n], n))
    #
    #    NewDF = pd.concat(DFs)
    #    NewDF.to_csv('SegmentedTracks.csv', index=False)
    #    print(NewDF.head(25))
    #else:
    #    NewDF = pd.read_csv('SegmentedTracks_10keV.csv')

    Tracks = uproot.open(File)['Tracking']
    NewDF = Tracks.pandas.df(['N', 'dE', 'dx', 'B'])

    PlotSegment(NewDF, 0)

    NewDF['dQ'] = dQ(NewDF.dE.to_numpy(), NewDF.dx.to_numpy())
    NewDF['dQ2'] = dQ2(NewDF.dE.to_numpy(), NewDF.dx.to_numpy())
    print(NewDF.head(25))
    dQdx = np.empty((len(np.unique(NewDF.N.to_numpy())), 25), dtype=float)
    dQdx2 = np.empty((len(np.unique(NewDF.N.to_numpy())), 25), dtype=float)
    for n in np.unique(NewDF.N.to_numpy()):
        Mask = (NewDF.N.to_numpy() == n ) & ( NewDF.B.to_numpy() < 25)
        Groups = NewDF[Mask].groupby('B').sum()
        dQdxTemp = Groups.dQ.to_numpy() / 5.0
        dQdx2Temp = Groups.dQ2.to_numpy() / 5.0
        if len(dQdxTemp) == 25:
            dQdx[n,:] = dQdxTemp
            dQdx2[n,:] = dQdx2Temp

    dQdxEff = np.empty((len(np.unique(NewDF.N.to_numpy())), 25), dtype=float)
    for n in np.unique(NewDF.N.to_numpy()):
        Mask = (NewDF.N.to_numpy() == n ) & ( NewDF.B.to_numpy() < 25)
        Groups = NewDF[Mask].groupby('B').sum()
        dQdxEffTemp = dQ(Groups.dE.to_numpy(), 5.0) / 5.0
        if len(dQdxEffTemp) == 25:
            dQdxEff[n,:] = dQdxEffTemp
        else: print('Length requirement not met.')

    Histogram(dQdx)
    Scatter2([dQdx,dQdx2], 'Single Particle Recombination', 'single')
    Scatter(dQdxEff, 'Coated Particle Recombination', 'coated')

def Scatter(dQdx, Title, suf):
    ICARUS = pd.read_csv('/home/mueller/Downloads/700Vcm.csv')
    
    Figure = plt.figure(figsize=(8,6), constrained_layout=True)
    Ax = Figure.add_subplot(1,1,1)
    Means = np.mean(dQdx, axis=0)
    Std = np.std(dQdx, axis=0)
    X = [ i*0.5 + 0.25 for i in range(25) ]
    Ax.errorbar(X, Means, yerr=Std/np.sqrt(len(dQdx)), fmt='go', label='Geant4 + ICARUS Recomb.')
    Ax.scatter(ICARUS.X.to_numpy(), ICARUS.Y.to_numpy(), label='ICARUS 3-Ton (500 V/cm)')
    Ax.set_xlabel('Residual Range [cm]')
    Ax.set_ylabel('dQ/dx [10^3 e-/cm]')
    Ax.set_ylim(0,25)
    Figure.suptitle(Title)
    Ax.legend()
    Figure.savefig(f'dQdxVsRR_{suf}')

def Scatter2(dQdx, Title, suf):
    ICARUS = pd.read_csv('/home/mueller/Downloads/700Vcm.csv')
    
    Figure = plt.figure(figsize=(8,6), constrained_layout=True)
    Ax = Figure.add_subplot(1,1,1)
    Means1 = np.mean(dQdx[0], axis=0)
    Means2 = np.mean(dQdx[1], axis=0)
    Std1 = np.std(dQdx[0], axis=0)
    Std2 = np.std(dQdx[1], axis=0)
    X = [ i*0.5 + 0.25 for i in range(25) ]
    Ax.errorbar(X, Means1, yerr=Std1/np.sqrt(len(dQdx[0])), fmt='go', label='Geant4 + ICARUS Recomb.')
    Ax.errorbar(X, Means2, yerr=Std2/np.sqrt(len(dQdx[1])), fmt='ro', label='Geant4 + ICARUS Recomb. (MC Corrected)')
    Ax.scatter(ICARUS.X.to_numpy(), ICARUS.Y.to_numpy(), label='ICARUS 3-Ton (500 V/cm)')
    Ax.set_xlabel('Residual Range [cm]')
    Ax.set_ylabel('dQ/dx [10^3 e-/cm]')
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
    dEdx = dE/(dx)
    A = 33 # e-/keV 500 V/cm
    k = 0.130
    #A = 0.87
    #k = (0.1225/1.3954)
    return ( (A*dEdx / (1 + k*dEdx)) * (dx) )# / (23.6/1000)

def dQ2(dE, dx):
    dEdx = dE/dx
    A = 0.87
    k = (0.1225/1.3954)
    return ( (A*dEdx / (1 + k*dEdx)) * dx ) / (23.6/1000)

def Segmentize(Data, n=0):
    PrimaryMask = Data['id'].to_numpy() == 1
    NotPrimaryMask = np.invert(PrimaryMask)
    BinSize = 0.5 # cm
    NBins = 24
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
    
    for i in range(len(Data[NotPrimaryMask])):
        Row = Data[NotPrimaryMask].iloc[i]
        #Row = Row.x1, Row.y1, Row.z1
        d = [ DCA(BinPoints[n][0], BinPoints[n][1], Row) for n in range(len(BinPoints)) ]
        #print(min(d), min(d) < SecThreshold)
        if min(d) < SecThreshold: SecTMPBins[i] = np.argmin(d)
        else: SecTMPBins[i] = -1
        #print(min(d) < SecThreshold, BinAssignment[np.invert(PrimaryMask)][i])

    BinAssignment[NotPrimaryMask] = SecTMPBins

    Mask = ( BinAssignment != -1 ) & ( Data.dE.to_numpy() != 0 ) & ( Data.dx.to_numpy() != 0 )
    DataDict = { 'N': Data[Mask].N.to_numpy(),
                 'dE': Data[Mask].dE.to_numpy(),
                 'dx': Data[Mask].dx.to_numpy(),
                 'B': BinAssignment[Mask] }

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
    return 1000 if (np.isnan(d) or not gClamp) else d

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
    File = '/home/mueller/Projects/SimpleNobleG4Detector/build/Muon_5MeV.root'
    LoadDF(File)

if __name__ == '__main__':
    main()
