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
from jax import device_put
import jax.numpy as jnp
import time
from time import time
  
# basic logger setup
logging.basicConfig(
    level=logging.INFO,
    format="[%(levelname)s] %(message)s",
    handlers=[
        logging.FileHandler("birks.log"),
        logging.StreamHandler(sys.stdout)
    ]
)

def timing(func):
    # This function shows the execution time of 
    # the function object passed
    def wrap_func(*args, **kwargs):
        t1 = time()
        result = func(*args, **kwargs)
        t2 = time()
        logging.info(f'Function {func.__name__!r} executed in {(t2-t1):.4f}s')
        return result
    return wrap_func

# if you wish you can turn on some timing info with the decorator.
# somewhat deprecated in favor of the Birks function below
#@timing
def Birks_deprecated(
    dE:     float,
    dx:     float,
    F:      float,
    A:      float=0.93,
    B:      float=0.062,
    C:      float=1.0,
):
    """
    Birks function which takes in three parameters, A, B, C
    and computes the Birks approximation dQ.
    The law for charge yield is a function of dE/dx and
    the electric field F:    
        
        dQ/dx(dE/dx,F | A,B,C) 
            for some parameters A, B and C

    Birk's law for charge yield is given by:

        dQ/dx = (A * dE/dx) / (1 + k * B * F^C * dE/dx)
    """
    numerator   = A * (dE / dx)
    denominator = 1 + (B * jnp.power(F,C)) * (dE / dx)
    return (numerator / denominator)

# if you wish, you can turn on some timing info with the decorator.
#@timing
def Birks(
    dEdx:   float,
    F:      float,
    A:      float=0.93,
    B:      float=0.062,
    C:      float=1.0,
):
    """
    Birks function which takes in three parameters, A, B, C
    and computes the Birks recombination approximation dQ/dE.
    The law for charge yield is a function of dE/dx and
    the electric field F:    
        
        dQ/dE(dE/dx,F | A,B,C) 
            for some parameters A, B and C

    Birk's law for charge yield is given by:

        dQ/dE = A / (1 + k * B * F^C * dE/dx)
    """
    numerator   = A 
    denominator = 1 + (B * jnp.power(F,C)) * (dEdx)
    return (numerator / denominator)

"""
Below is a JIT compiled version of the above function
"""
Birks_jit = jit(Birks)

@jit
def Birks_vmap(
    dEdx:   float,
    F:      float,
    A:      float=0.93,
    B:      float=0.062,
    C:      float=1.0,
):
    """
    Below is a JIT compiled and vectorized version of the above function.
    This is meant to take a collection of 1d arrays (A,B,C) which first
    creates a meshgrid of the A,B,C values and then sends them through
    a vectorized version of the recombination function.
    """    
    # generate the cartesian product of the vectors
    AA, BB, CC = jnp.meshgrid(A,B,C)
    return vmap(
        Birks, 
        in_axes=(None, None, 0, 0, 0), 
        out_axes=0
    )(dEdx, F, AA.flatten(), BB.flatten(), CC.flatten())

if __name__ == "__main__":

    size = 300
    dEdx = np.random.normal(5,10,size).astype(np.float32)
    F    = np.random.normal(500,10,size).astype(np.float32)

    num_samples = 100
    A = np.random.normal(0.93, 0.05, num_samples)
    B = np.random.normal(0.062, 0.05, num_samples)
    C = np.random.normal(1., 0.05, num_samples)

    result = Birks_vmap(dEdx, F, A, B, C)
    print(result.shape)
    print(result)


