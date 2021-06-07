import numpy as np
import matplotlib.pyplot as plt
import uproot
import pandas as pd

def LoadDF(File):
    Steps = uproot.open(File)['Kinematics']
    DF = Steps.pandas.df(['N', 'dE', 'x0', 'y0', 'z0', 'x1', 'y1', 'z1', 'id', 'pdg'])
    print(DF.head())
    Voxelize(DF[DF.N == 0][:10], 0.2)

def Vox(x0, y0, z0, x1, y1, z1, Res):
    #return f'({int((x1-x0)/(2*Res))},{int((y1-y0)/(2*Res))},{int((z1-z0)/(2*Res))})'
    Vox0 = int(x0/Res), int(y0/Res), int(z0/Res)
    Vox1 = int(x1/Res), int(y1/Res), int(z1/Res)
    if Vox0==Vox1:
        print('Single voxel.')
        return Vox0
    else:
        l = np.sqrt( (x1-x0)**2 + (y1-y0)**2 + (z1-z0)**2 )
        mx, my, mz = (x1-x0)/l, (y1-y0)/l, (z1-z0)/l
        print(mx, my, mz)
        tx = min( x for x in [((Vox0[0] + 1)*Res - x0) / mx, ((Vox0[0] - 1)*Res - x0) / mx ] if x > 0 )
        ty = min( x for x in [((Vox0[1] + 1)*Res - y0) / my, ((Vox0[1] - 1)*Res - y0) / my ] if x > 0 )
        tz = min( x for x in [((Vox0[2] + 1)*Res - z0) / mz, ((Vox0[2] - 1)*Res - z0) / mz ] if x > 0 )
        print(Vox0)
        print(Vox1)
        print(tx, ty, tz)
        return int((x1-x0)/(2*Res)), int((y1-y0)/(2*Res)), int((z1-z0)/(2*Res))
    
def Voxelize(Event, Res):
    Voxs = np.vectorize(Vox)(Event['x0'],
                             Event['y0'],
                             Event['z0'],
                             Event['x1'],
                             Event['y1'],
                             Event['z1'],
                             Res)
    print(Voxs)
    #print(Event.head())
    #VoxData = pd.DataFrame(Event.groupby(['x', 'y', 'z']).dE.sum())
    #print(VoxData.head())
    
    
def main():
    File = '/home/mueller/Projects/SimpleNobleG4Detector/NobleG4_build/Muon_1GeV.root'
    LoadDF(File)

if __name__ == '__main__':
    main()
