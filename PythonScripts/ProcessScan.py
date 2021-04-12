import numpy as np
import uproot
import matplotlib.pyplot as plt
import glob

def Plot(Sets):
    Figure = plt.figure(constrained_layout=True)
    Ax = Figure.add_subplot(1,1,1)

    for s in Sets:
        Ax.errorbar(s[3], s[1], yerr=s[2], fmt='o', label=f'{s[0]} V/cm', markersize=3)


    Ax.set_xlabel('Primary Energy [keV]')
    Ax.set_ylabel('Charge Yield [e-/keV]')
    Ax.legend()
    Figure.savefig('ElectronScan.png')

def LoadFile(File):
    # Load single file.
    T = uproot.open(File)['EventKinematics']
    electrons = T.array('e')
    energies = 1000*T.array('E')
    QY = electrons / energies
    return np.mean(QY), np.std(QY), np.mean(energies)

def ProcessField(Field):
    Files = glob.glob(f'/home/mueller/Projects/SimpleNobleG4Detector/SourceMacros/G4Scan/{Field}Vcm/*.root')
    Qs = list()
    Qw = list()
    Es = list()
    for f in Files:
        qs, qw, es = LoadFile(f)
        Qs.append(qs)
        Qw.append(qw)
        Es.append(es)
    print(sorted([round(x,2) for x in Es], key=float))
    return Field, Qs, Qw, Es

def main():
    
    Sets = list()
    Sets.append(ProcessField(250))
    Sets.append(ProcessField(500))
    Sets.append(ProcessField(750))
    Plot(Sets)


if __name__ == '__main__':
    main()
