import numpy as np
import matplotlib.pyplot as plt
import uproot

class RecoHistogram:
    def __init__(self, File, Key):
        H2 = uproot.open(File)[Key]
        self.xbins = H2.edges[0]
        self.ybins = H2.edges[1]
        self.xcent = self.BinCenters(self.xbins)
        self.ycent = self.BinCenters(self.ybins)
        self.vals = H2.values
        self.reco_calculated = False

    def BinCenters(self, bins):
        return ((bins[0:-1] + bins[1:]) / 2.0).flatten()

    def PlotdEdx(self):
        Figure = plt.figure()
        Ax = Figure.add_subplot(1,1,1)
        xv, yv = np.meshgrid(self.xcent, self.ycent)
        Ax.hist2d(xv.ravel(), yv.ravel(), weights=self.vals.ravel(), bins=(self.xbins, self.ybins))
        Figure.savefig('testplot.png')
        
    def PlotReco(self):
        if self.reco_calculated:
            Figure = plt.figure()
            Ax = Figure.add_subplot(1,1,1)
            xv, yv = np.meshgrid(self.recocent, self.ycent)
            Ax.hist2d(xv.ravel(), yv.ravel(), weights=self.vals.ravel(), bins=(self.recobins, self.ybins))
            Figure.savefig('testplot_reco.png')
        else: print('No recombination data found. Call CalcRecombination() with the desired recombination function.')

    def SetField(self, Field):
        self.field = Field

    def CalcRecombination(self, RecoFunc):
        self.recocent = RecoFunc(self.xcent, self.field)
        self.reco_calculated = True

def Recombination(dEdx, Field):
    Eps = 0.07*pow(Field, -0.85);
    return (1 - ( Eps * dEdx ) / ( 1 + Eps * dEdx ));

def CalcQ(R, dE):
    return (R*dE) / (23.6 / 1000000)

def CalcL(R, dE):
    return (dE / (19.5 / 1000000)) - CalcQ(R, dE)
        
def main():
    InDir = '/home/mueller/Projects/SimpleNobleG4Detector/'
    File = 'testH2.root'

    R = RecoHistogram(InDir+File, 'dEdx')
    R.PlotdEdx()
    R.SetField(0.5)
    R.CalcRecombination(Recombination)
    R.PlotReco()

if __name__ == '__main__':
    main()
