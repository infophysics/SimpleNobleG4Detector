import numpy as np
import uproot
import pandas as pd
import matplotlib.pyplot as plt
import glob
from array import array
from ROOT import TGraph

def Plot(Sets, Particle):
    Figure = plt.figure(constrained_layout=True)
    Ax = Figure.add_subplot(1,1,1)
    Colors = ['C0', 'C1', 'C2', 'C3', 'C4']

    for i, s in enumerate(Sets):
        #m, c, b = Ax.errorbar(s[3], s[1], yerr=s[2], fmt='o', label=f'{s[0]} V/cm', markersize=3)
        #[bar.set_alpha(0.5) for bar in b]
        GraphCenter = TGraph(len(s[3]), array('d', s[3]), array('d', s[1]))
        GraphUpper =  TGraph(len(s[3]), array('d', s[3]), array('d', s[1]+s[2]))
        GraphLower = TGraph(len(s[3]), array('d', s[3]), array('d', s[1]-s[2]))
        x = np.linspace(np.min(s[3]), np.max(s[3]), 5000)
        yc = np.vectorize(GraphCenter.Eval)(x)
        yu = np.vectorize(GraphUpper.Eval)(x)
        yl = np.vectorize(GraphLower.Eval)(x)
        Ax.plot(x, yc, linestyle='-', c=Colors[i], label=f'{s[0]} V/cm')
        Ax.fill_between(x, yl, yu, alpha=0.5)
        #Ax.plot(x, yu, linestyle='--', c=Colors[i])
        #Ax.plot(x, yl, linestyle='--', c=Colors[i])

    Ax.set_title(f'Charge Yield for Primary {Particle.capitalize()}')
    Ax.set_xlabel('Primary Energy [keV]')
    Ax.set_ylabel('Charge Yield [e-/keV]')
    Ax.legend()
    Figure.savefig(f'{Particle}_scan.png')

def LoadFile(File):
    # Load single file.
    T = uproot.open(File)['EventKinematics']
    electrons = T.array('e')
    energies = 1000*T.array('E')
    NaNMask = np.invert(np.isnan(electrons)) & np.invert(np.isnan(energies))
    QY = electrons[NaNMask] / energies[NaNMask]
    QYNaNMask = np.invert(np.isnan(QY))
    return np.mean(QY[QYNaNMask]), np.std(QY[QYNaNMask]), np.mean(energies[NaNMask])

def ProcessField(Field, Particle):
    Files = glob.glob(f'/home/mueller/Projects/SimpleNobleG4Detector/Macros/Scans/{Field}V/*{Particle}*.root')
    Qs = list()
    Qw = list()
    Es = list()
    for f in Files:
        qs, qw, es = LoadFile(f)
        Qs.append(qs)
        Qw.append(qw)
        Es.append(es)
    return Field, np.array(Qs), np.array(Qw), np.array(Es)

def SaveSets(Sets, Particle):
    SetDict = {'fEnergy': list(),
               'fCharge': list(),
               'fWidth' : list(),
               'fField' : list()}
    for s in Sets:
        SetDict['fEnergy'].extend(s[3])
        SetDict['fCharge'].extend(s[1])
        SetDict['fWidth'].extend(s[2])
        SetDict['fField'].extend(np.repeat(s[0], len(s[3])))

    pd.DataFrame(SetDict).round(4).to_csv(f'{Particle}_scan.csv', index=False)

def LoadSets(Particle):
    Frame = pd.read_csv(f'{Particle}_scan.csv')
    Sets = list()
    for f in np.unique(Frame['fField']):
        FieldMask = (Frame['fField'] == f)
        Field = f
        Es = Frame['fEnergy'].to_numpy()[FieldMask]
        Qs = Frame['fCharge'].to_numpy()[FieldMask]
        Qw = Frame['fWidth'].to_numpy()[FieldMask]
        Sets.append((Field, Qs, Qw, Es))
    return Sets
    
def main():

    Particles = ['alpha']# ['electron', 'photon', 'alpha']
    for p in Particles:
        Sets = list()
        Sets.append(ProcessField(250, p))
        Sets.append(ProcessField(500, p))
        #Sets.append(ProcessField(750, p))
        #Sets.append(ProcessField(1000, p))
        SaveSets(Sets, p)
        #Sets = LoadSets(p)
        Plot(Sets, p)


if __name__ == '__main__':
    main()
