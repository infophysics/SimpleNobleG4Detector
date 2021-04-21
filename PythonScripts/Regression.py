import numpy as np
import pandas as pd
import uproot
import matplotlib.pyplot as plt
import timeit
from scipy.optimize import curve_fit

def Model0(d, F, p):
    Eps = p[0] * pow(F, p[1])
    R = (1 - ( Eps * d ) / ( 1 + Eps * d ) )
    return R

def Model1(d, F, p):
    R = ( p[0] ) / ( 1 + p[1] * pow(F, -1) * d )
    return R

def Model2(d, F, p):
    R = ( p[0] ) / ( 1 + p[1] * pow(F, p[2]) * d )
    return R

def Model3(d, F, p):
    R = ( p[0] ) / ( 1 + p[1] * pow(F, p[3]) * d + p[2] * pow(F, p[3]) * d**3 )
    return R

def CalcYields(G4Data, Field, p):
    R = RFunc(G4Data[0,:], Field, p)    
    Q = (R * G4Data[1,:]) / (23.6 / 1000000)
    L = (G4Data[1,:] / (19.5 / 1000000)) - Q
    E = np.sum(G4Data[1,:])
    NaNMask = np.invert(np.isnan(Q))
    QTotal = np.sum( Q[NaNMask] * G4Data[2,NaNMask] ) / E
    LTotal = np.sum( L[NaNMask] * G4Data[2,NaNMask] ) / E
    return QTotal, LTotal

def SaveData(SourceDir, Sample):
    # It is possible to decrease the overhead somewhat by reading
    # and slightly parsing the data beforehand, then saving it to
    # a .npy file. This is also useful to scale the data by the
    # reciprocal of the number of events (essentially factoring
    # out the large number of events used in the sample).

    # Load the Geant4 step data for the requested source, then
    # save the data.
    G4Data = uproot.open(f'{SourceDir}{Sample}.root')['Kinematics']
    NEvents = np.max(G4Data.array('N')) + 1

    
    BinnedData = np.array( [G4Data.array('dEdx'),
                            G4Data.array('dE'),
                            np.repeat(1/NEvents, len(G4Data.array('dEdx')))] )
    np.save(f'{SourceDir}{Sample}.npy', BinnedData)

def LoadData(SourceDir, Sample):
    # Load the previously saved data.

    return np.load(f'{SourceDir}{Sample}.npy')

def RegressionFunc(Data, *p):
    return np.array([ CalcYields(x[0], x[1], p)[0 if x[2] == 'Q' else 1] for x in Data.data() ])
    #return CalcYields(Data[0], Data[1], p)[0 if Data[2] == 'Q' else 1]

class DataWrapper(object):
    def __init__(self, X):
        self.X = X

    def astype(self, t):
        return self.X

    def data(self):
        return self.X
    
def main():
    # Load DM data from the csv into a Pandas dataframe. We
    # require that a G4 source be added in order to actually
    # do anything with the data.
    DMDir = '/home/mueller/Projects/ArNEST/NobleG4/'
    DMData = pd.read_csv(f'{DMDir}DMData.csv')
    NaNMask = [ str(x) != 'nan' for x in DMData['G4Sample'] ]
    DMData = DMData[NaNMask]
    ExcludeMask = [ x == 1 for x in DMData['Include'] ]
    DMData = DMData[ExcludeMask]
    
    SourceDir = '/home/mueller/Projects/SimpleNobleG4Detector/Macros/G4Output/'
    for s in np.unique(DMData['G4Sample']): SaveData(SourceDir, s)

    G4Data = dict()
    for s in np.unique(DMData['G4Sample']):
        G4Data[s] = LoadData(SourceDir, s)

    Fields = DMData['Field'].to_numpy() / 1000
    Sources = DMData['G4Sample'].to_numpy()
    Types = DMData['Type'].to_numpy()
    RegData = [ (G4Data[Sources[i]], Fields[i], Types[i]) for i in range(len(Fields)) ]

    Yields = DMData['Yield'].to_numpy()
    Sigma = (DMData['YLow'].to_numpy() + DMData['YHigh'].to_numpy()) / 2.0
    Sigma[ Sigma == 0 ] = 0.05*Yields[ Sigma == 0 ]

    global RFunc
    RFunc = Model3
    
    #P0 = np.array([0.07, -0.85])
    #P0 = np.array([0.8, 0.05])
    #P0 = np.array([0.8, 0.05, -1.0])
    P0 = np.array([0.898, 30.53, -0.0051, -0.82])#, -0.87])
    Bounds = ([0,0,-0.01,-2],[1.25,100,-0.001,0])
    xscale = [0.01,0.1,0.0001,0.01]
    pOpt, pCov = curve_fit(RegressionFunc, DataWrapper(RegData), Yields, p0=P0, sigma=Sigma, check_finite=False, bounds=Bounds, verbose=1, xtol=1e-10, x_scale=xscale)


    np.set_printoptions(precision=3)
    
    print(f'Optimum parameters: {pOpt}')
    print(f'Covariance matrix:\n{pCov}')

    Predictions = RegressionFunc(DataWrapper(RegData), *pOpt)#pOpt[0], pOpt[1], pOpt[2])

    DMData['Prediction'] = Predictions
    
    Residuals = (((Predictions - Yields) / (Sigma))**2) / (len(Predictions) - 4)
    DMData['Residuals'] = Residuals
    print(f'Chi^2/NDF: {np.round(np.sum(Residuals),4)}')

    #print(DMData.head(250))
    DMData.to_csv('FitResults2.csv', index=False)
    
    #p0, p1 = np.meshgrid(np.linspace(0, 1, 100),
    #                     np.linspace(-1.5, -0.5, 100))

    #p0 = p0.flatten()
    #p1 = p1.flatten()
    #Res = list()
    #for i in range(len(p0)):
    #    if (i % 100) == 0: print(f'Gridspace searched: {i} / 10000 ({int(i/100)}%)')
    #    Predictions = RegressionFunc(RegData, p0[i], p1[i])
    #    Residuals = (((Predictions - Yields) / (Sigma))**2) / (len(Predictions) - 2)
    #    Res.append(np.sum(Residuals)) 
    #np.save('ParScan.npy', np.array(Res))
    #Res = np.load('ParScan.npy')

    #Figure = plt.figure()
    #Ax = Figure.add_subplot(1,1,1)
    #Ax.imshow(Res.reshape(p0.shape),
    #          interpolation='bilinear',
    #          origin='lower',
    #          extent=[0, 1, -1.5, -0.5])
              
    #Figure.savefig('ParScan.png')
     
if __name__ == '__main__':
    main()
