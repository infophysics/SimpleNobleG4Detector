"""
Python script for running the regression algorithms
"""
from dataclasses import dataclass, field
from re import L
import numpy as np
import scipy as sp
import torch
import uproot
import csv
import sys
import os
import logging
from matplotlib import pyplot as plt
from jax import grad, jit, vmap

# basic logger setup
logging.basicConfig(
    level=logging.INFO,
    format="[%(levelname)s] %(message)s",
    handlers=[
        logging.FileHandler("regression.log"),
        logging.StreamHandler(sys.stdout)
    ]
)

@dataclass
class Dataset:
    """
    Class for holding experimental fit info
    """
    input_file: str
    name:   str
    dedx:       list = field(default_factory=list)
    dedx_sig:   list = field(default_factory=list)
    efield:     list = field(default_factory=list)
    efield_sig: list = field(default_factory=list)
    recob:      list = field(default_factory=list)
    recob_sig:  list = field(default_factory=list)

    def __post_init__(self):
        logging.info(f"Attempting to add input file: {self.input_file}.")
        if not os.path.isfile(self.input_file):
            logging.error(f"Specified input file '{self.input_file}' not in path.")
            raise ValueError(f"Specified input file '{self.input_file}' not in path.")
        logging.info(f"Adding input file: {self.input_file}.")
        try:
            with open(self.input_file,"r") as file:
                reader = csv.reader(file, delimiter=",")
                for row in reader:
                    self.dedx.append(float(row[0]))
                    self.dedx_sig.append(float(row[1]))
                    self.efield.append(float(row[2]))
                    self.efield_sig.append(float(row[3]))
                    self.recob.append(float(row[4]))
                    self.recob_sig.append(float(row[5]))
        except Exception as e:
            logging.error(f"Error reading csv file: {e}.")
            raise ValueError(f"Error reading csv file: {e}.")
        logging.info(f"Added {len(self.dedx)} data points from file {self.input_file}.")
        logging.info(f"Adding unique field value(s)...")
        self.unique_efield = np.unique(self.efield)
        logging.info(f"Added {len(self.unique_efield)} unique field value(s).")
        # set bins
        self.set_num_bins(100)
        self.set_bin_length(1.0)

    def __len__(self):
        return len(self.x)

    def set_num_bins(self,
        num_bins,
    ):
        """Set the number of bins for the fit"""
        self.num_bins = num_bins
        logging.info(f"Set number of bins for {self.name}: {self.num_bins}.")
    
    def set_bin_length(self,
        bin_length,
    ):
        """Set the bin length for the fit"""
        self.bin_length = bin_length
        logging.info(f"Set bin_length for {self.name}: {self.bin_length}.")

@dataclass
class NuDataset(Dataset):
    """
    Class for holding neutrino dataset info
    """

    def eval(self,
        parameters: list,
        num_params: int=3,
    ):
        logging.info(f"Starting evaluation of nu dataset {self.name}...")
        p_size = int(len(parameters)/num_params)
        num_fields = len(self.unique_efield)
        logging.info(f"Number of parameter steps: {p_size}.")
        logging.info(f"Number of unique field values: {num_fields}.")

        

@dataclass
class DMDataset(Dataset):
    """
    Class for holding DM dataset info
    """

    def eval(self,
        parameters: list,
        chis:       list,
    ):
        pass


if __name__ == "__main__":

    dataset = NuDataset(
        "../../Data/ArgoNeuT_protons.csv", 
        "argoneut_protons"
    )

