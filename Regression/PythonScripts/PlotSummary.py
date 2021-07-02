import numpy as np
import uproot
import pandas as pd
import matplotlib.pyplot as plt
import yaml

def main():
    cfg="""
    Datasets:
      - Name: 'ICARUS_Muons'
        FigTitle: 'ICARUS Muons'
        Data: '../data/ICARUS_3Ton_muons.csv'
        Summary: '../build/ICARUS_Muons_Summary.root'
        Fields: [0.5, 0.35, 0.2]
        Range: [0, 20]
      - Name: 'ICARUS_Protons'
        FigTitle: 'ICARUS Protons'
        Data: '../data/ICARUS_3Ton_protons.csv'
        Summary: '../build/ICARUS_Protons_Summary.root'
        Fields: [0.5, 0.35, 0.2]
        Range: [0, 40]
    """
    CFG = yaml.load(cfg, Loader=yaml.FullLoader)

    for Dataset in CFG['Datasets']:
        Summaries = [ uproot.open(Dataset['Summary'])[f'Field{i}'] for i in range(len(Dataset['Fields'])) ]
        DFs = [ LoadData(Dataset['Data'], Dataset['Fields'][i]) for i in range(len(Dataset['Fields'])) ]
        PlotField(DFs, Summaries, Dataset)#, Dataset['Fields'], Dataset['Name'], Dataset['FigTitle'])
    
    #ICARUS_Muons_Summary = uproot.open('../build/ICARUS_Muons_Summary.root')
    #F0 = ICARUS_Muons_Summary['Field0']

def PlotField(Data, Summary, CFG):# Fields, Title, FigTitle):
    Fields = CFG['Fields']
    Title = CFG['Name']
    FigTitle = CFG['FigTitle']
    Range = CFG['Range']
    Figure = plt.figure(figsize=(8,6), constrained_layout=True)
    Ax = Figure.add_subplot()
    for fi, F in enumerate(Fields):
        Ax.errorbar(Data[fi][0].to_numpy(), Data[fi][4].to_numpy(), yerr=Data[fi][4].to_numpy() * Data[fi][5].to_numpy(), fmt='o', label=f'{F} kV/cm', markersize=4, fillstyle='none')
        Ax.scatter(Summary[fi].array('dEdx'), Summary[fi].array('R'), s=1)
        

    Ax.set_xlim(Range[0], Range[1])
    Ax.set_ylim(0, 1)
    Ax.set_xlabel('dE/dx [MeV/cm]')
    Ax.set_ylabel('Recombination')
    Ax.set_title(FigTitle)
    Ax.legend()
    Figure.savefig(Title, dpi=300)
    
def LoadData(File, Field):
    DF = pd.read_csv(File, header=None)
    DF = DF[ DF[2] == Field ]
    return DF
    

if __name__ == '__main__':
    main()
