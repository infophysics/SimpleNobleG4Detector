import numpy as np
import matplotlib.pyplot as plt
from matplotlib.collections import LineCollection
from matplotlib import cm as cm
from matplotlib import colors as colors
import uproot
import pandas as pd

def LoadDF(File):
    Steps = uproot.open(File)['Kinematics']
    DF = Steps.pandas.df(['N', 'dE', 'dx', 'x0', 'y0', 'z0', 'x1', 'y1', 'z1', 'id', 'pdg'])
    print(DF.head(10))
    #Voxelize(DF[DF.N == 0][:10], 0.2)
    #for i in range(5): PlotEvent(DF[DF.N == i], i)
    Segmentize2(DF[DF.N == 1])

    DF[DF.N == 0].to_csv('dump.csv', index=False)

def Segmentize2(Data):
    PrimaryMask = Data['id'].to_numpy() == 1

    CumL = list()
    DX = Data[PrimaryMask].dx.to_numpy()
    SumL = 0
    for i in range(len(Data[PrimaryMask])):
        #Row = Data[PrimaryMask].iloc[i]
        #P1 = Row.x0, Row.y0, Row.z0
        #d = Distance(P1, Row)
        SumL += 10*DX[i]
        CumL.append(SumL)
    TotalLength = SumL
    print(TotalLength)
    print(np.sum(Data[PrimaryMask].dE.to_numpy()))
    print(np.sum(Data.dE.to_numpy()))
    print(Data[np.invert(PrimaryMask)].head(10))
    PlotEvent(Data[PrimaryMask], 0)
    
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
    return np.sqrt( v[0]**2 + v[1]**2 + v[2]**2 )

def Distance(P1, Row):
    P2 = Row.x1, Row.y1, Row.z1
    return np.sqrt( (P1[0]-P2[0])**2 + (P1[1]-P2[1])**2 + (P1[2]-P2[2])**2 )



def Segmentize(Data):
    PrimaryMask = Data['id'].to_numpy() == 1
    print(np.sum(PrimaryMask))
    print(np.sum(np.invert(PrimaryMask)))

    r1 = Data[PrimaryMask].iloc[-1]
    P1 = r1.x1, r1.y1, r1.z1
    print(P1)
    LengthMask = [ Distance(P1, Data.iloc[i]) < 25 for i in range(len(Data)) ]

    dEdx = list()
    I1 = len(Data[LengthMask & PrimaryMask]) - 1

    Figure = plt.figure()
    Ax = Figure.add_subplot(1,1,1)
    Scalar = 6/2.36

    for b in range(5):
        P1 = Data[LengthMask & PrimaryMask].iloc[I1]
        P1 = P1.x1, P1.y1, P1.z1
        I0 = next( i for i in range(I1, -1, -1) if Distance(P1, Data[LengthMask & PrimaryMask].iloc[i]) > 0.5 )
        P0 = Data[LengthMask & PrimaryMask].iloc[I0]
        P0 = P0.x1, P0.y1, P0.z1
        BinMask = [ DCA(P0, P1, Data.iloc[i]) < 0.2 for i in range(len(Data)) ]
        print(np.sum(BinMask))
        dEdx.append(Scalar * np.sum(Data[BinMask].dE.to_numpy()) / 0.5)
        Ax.scatter([P1[2]], [P1[0]], s=5, label=f'Segment {b}')
        I1 = I0
        collection, R = CreateCollection(Data[BinMask], np.repeat(b, len(Data[BinMask])), VMin=0, VMax=5)
        Ax.add_collection(collection)
    #print(Fi)
    print(dEdx)
    Ax.set_xlim(R[0]-10, R[1]+10)
    Ax.set_ylim(R[2]-10, R[3]+10)
    Figure.legend()
    Figure.savefig('Segments.png', dpi=600)

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
    Ax.set_xlim(Range[0]-10, Range[1]+10)
    Ax.set_ylim(Range[2]-10, Range[3]+10)

    if suf: Figure.savefig(f'EVD{N}.png', dpi=600)
    else: Figure.savefig(f'EVD{N}_{suf}.png', dpi=600)


    

    
def Vox(x0, y0, z0, x1, y1, z1, EDep, Res):
    print('***********************')
    #return f'({int((x1-x0)/(2*Res))},{int((y1-y0)/(2*Res))},{int((z1-z0)/(2*Res))})'
    Vox0 = int(x0/Res), int(y0/Res), int(z0/Res)
    Vox1 = int(x1/Res), int(y1/Res), int(z1/Res)
    if Vox0==Vox1:
        print('Single voxel.')
        return Vox0
    else:
        xc, yc, zc = x0, y0, z0
        vxc, vyc, vzc = int(xc/Res), int(yc/Res), int(zc/Res)
        l = np.sqrt( (x1-x0)**2 + (y1-y0)**2 + (z1-z0)**2 )
        mx, my, mz = (x1-x0)/l, (y1-y0)/l, (z1-z0)/l
        print('Slopes', mx, my, mz)
        tx, ty, tz = 1000, 1000, 1000
        if xc != x1:
            tx = ( (vxc+1)*Res - xc ) / mx if mx > 0 else ( (vxc-1)*Res - xc ) / mx
        if yc != y1:
            ty = ( (vyc+1)*Res - yc ) / my if my > 0 else ( (vyc-1)*Res - yc ) / my
        if zc != z1:
            tz = ( (vzc+1)*Res - zc ) / mz if mz > 0 else ( (vzc-1)*Res - zc ) / mz
        print('VoxC', Vox0)
        print('Vox1', Vox1)
        print('ParT', tx, ty, tz)

        xp, yp, zp = xc, yc, zc
        if tz < tx and tz < ty:
            xc, yc, zc = xc + mx*tz, yc + my*tz, zc + mz*tz
            if mz > 0: vzc += 1
            else: vzc -= 1
        elif ty < tx and ty < tz:
            xc, yc, zc = xc + mx*ty, yc + my*ty, zc + mz*ty
            if my > 0: vyc += 1
            else: vyc -= 1
        elif tx < ty and tx < tz:
            xc, yc, zc = xc + mx*tx, yc + my*tx, zc + mz*tx
            if mx > 0: vxc += 1
            else: vxc -= 1
            
        print('New Vox', (vxc, vyc, vzc))
        print('New Pos', (xc, yc, zc))
        LVox = np.sqrt( (xc-xp)**2 + (yc-yp)**2 + (zc-zp)**2 ) / l
        LRangeOut = np.sqrt( (xp-x1)**2 + (yp-y1)**2 + (zp-z1)**2) / l
        
        print('Length in Voxel', min(LVox, LRangeOut))
        print('Energy in Voxel', EDep*min(LVox, LRangeOut))
        
        return int((x1-x0)/(2*Res)), int((y1-y0)/(2*Res)), int((z1-z0)/(2*Res))
    
def Voxelize(Event, Res):
    Voxs = np.vectorize(Vox)(Event['x0'],
                             Event['y0'],
                             Event['z0'],
                             Event['x1'],
                             Event['y1'],
                             Event['z1'],
                             Event['dE'],
                             Res)
    print(Voxs)
    #print(Event.head())
    #VoxData = pd.DataFrame(Event.groupby(['x', 'y', 'z']).dE.sum())
    #print(VoxData.head())
    
    
def main():
    File = '/home/mueller/Projects/SimpleNobleG4Detector/NobleG4_build/Muon_5MeV.root'
    LoadDF(File)

if __name__ == '__main__':
    main()
