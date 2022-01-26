// C++ includes.
#include <iostream>
#include <vector>

// CUDA includes.
#include <cuda.h>
#include <cuda_runtime.h>

// Custom includes.
#include "EvalCUDA.hh"

#define NBINS 100
#define BINL 0.5
#define NEVENTS 5000

namespace ReCUDA
{
	__global__ void GetR( double *dQBins, double *dEBins, double *R )
	{
		int Index = NBINS * threadIdx.x + blockIdx.x;
		R[ Index ] = dQBins[ Index ] / dEBins[ Index ];
	}
	
	__global__ void Birks( 
		double 	*dE, 
		double 	*dx, 
		int 	*B, 
		double 	F, 
		double* p, 
		size_t 	n, 
		double 	*dQBins, 
		double 	*dEBins 
	)
	{
		int ThreadID = blockIdx.x * blockDim.x + threadIdx.x;
		int ParameterID = 2 * threadIdx.y;
		double StepdQ, StepdE;
		
		if( ThreadID < n )
		{
			StepdQ = ( p[ParameterID + 0] * ( dE[ThreadID] / dx[ThreadID] ) /
				( 1 + ( p[ParameterID + 1] / F ) *
				( dE[ThreadID] / dx[ThreadID] ) ) ) * dx[ThreadID];
			StepdE = dE[ThreadID];
		
			if( !std::isnan(StepdQ) ) {
				atomicAdd(dQBins + NBINS * threadIdx.y + B[ThreadID], StepdQ / (BINL * NEVENTS));
			}
			if( !std::isnan(StepdE) ) {
				atomicAdd(dEBins + NBINS * threadIdx.y + B[ThreadID], StepdE / (BINL * NEVENTS));
			}
		}
	}

	__global__ void DMBirks( double *dE, double *dx, double F, size_t n, double* p, double* QY )
	{
		int ThreadID = blockIdx.x * blockDim.x + threadIdx.x;
		int ParameterID = 2 * threadIdx.y;
		double StepdQ;
		
		if( ThreadID < n )
		{
			StepdQ = ( p[ParameterID + 0] * ( dE[ThreadID] / dx[ThreadID] ) /
				( 1 + ( p[ParameterID + 1] / F ) *
				( dE[ThreadID] / dx[ThreadID] ) ) ) * dx[ThreadID];
			if( !std::isnan(StepdQ) ) {
				atomicAdd( QY + threadIdx.y, StepdQ / ( 1000 ) );
			}
		}
	}

	void DMBirksWrapper(
		const std::vector<double> &dE, 
		const std::vector<double> &dx,
		const std::vector<double> &F, 
		const std::vector<double> &p,
		std::vector< std::vector<double> > &QY 
	)
	{
		// Set the thread-dimensions of each block.
		const int NThreadEntries(32);
		const int NThreadParameters(32);
		const dim3 ThreadDimensions( NThreadEntries, NThreadParameters );
		
		// We will need a few constants/dimensions for the calculation.
		const size_t N(dE.size());
		const size_t NFields(F.size());

		// Create the pointers for the arrays in device memory.
		double *DeviceG4dE;
		double *DeviceG4dx;
		double *DeviceP;
		double *DeviceQY;

		// Now we need to allocate memory on the device for these arrays. The raw input only needs
		// to be copied once, but the parameters will only be processed in batches of 32 (allocate
		// memory in loop).

		cudaMalloc( (void**)&DeviceG4dE, sizeof(double) * N );
		cudaMalloc( (void**)&DeviceG4dx, sizeof(double) * N );
		cudaMalloc( (void**)&DeviceP, sizeof(double) * 64 );
		cudaMalloc( (void**)&DeviceQY, sizeof(double) * 32 );

		// Now copy the raw G4 input to the device.
		cudaMemcpy( DeviceG4dE, &dE[0], sizeof(double) * N, cudaMemcpyHostToDevice );
		cudaMemcpy( DeviceG4dx, &dx[0], sizeof(double) * N, cudaMemcpyHostToDevice );

		// So the major time cost with outsourcing the recombination calculation to a GPU is in
		// copying to and from memory, and other overhead. We can minimize this by copying the
		// G4 input to the device memory once and performing all calculations on it at once. We will
		// need to copy the dQ calculations back to the host and the batch of parameters to the
		// device, but this is relatively little compared to the large size of the raw input.

		size_t ParHead;
		for( size_t fi(0); fi < NFields; ++fi )
		{
		
			ParHead = 0;
			while( ParHead + 64 <= p.size() )
			{
				// Reset the arrays allocated for the QY calculation.
				cudaMemset( DeviceQY, 0, sizeof(double) * 32 );
				// Copy the current batch of parameters to the device.
				cudaMemcpy( DeviceP, &p[ParHead], sizeof(double) * 64, cudaMemcpyHostToDevice );
				// Make a kernel call for this batch of parameters and field.
				DMBirks <<< ( N / NThreadEntries + 1 ), ThreadDimensions >>>( DeviceG4dE,
												DeviceG4dx,
												F[fi],
												N,
												DeviceP,
												DeviceQY );
				cudaDeviceSynchronize();
				// Retrieve any error states (if any).
				cudaError_t Error = cudaGetLastError();
				if( Error != cudaSuccess ) {
					std::cerr << "CUDA error: " << cudaGetErrorString(Error) << std::endl;
				}	
				
				// Copy the resulting calculated values back to the host.
				cudaMemcpy( &QY[fi][ 32 * (ParHead / 64) ], DeviceQY, sizeof(double) * 32, cudaMemcpyDeviceToHost );
				ParHead += 64;
			}
		}
		
		// Once we are finished we need to release the memory that was allocated on the device.
		cudaFree( DeviceG4dE );
		cudaFree( DeviceG4dx );
		cudaFree( DeviceP );
		cudaFree( DeviceQY );
		
	}
	
	void BirksWrapper(
		const std::vector<double> &dE, 
		const std::vector<double> &dx,
		const std::vector<int> &B, 
		const std::vector<double> &F,
		const std::vector<double> &p, 
		std::vector<std::vector<double>>  &RBins,
		std::vector<std::vector<double>>  &dEBins 
	)
	{
		// Set the thread-dimensions of each block.
		const int NThreadEntries(32);
		const int NThreadParameters(32);
		const dim3 ThreadDimensions( NThreadEntries, NThreadParameters );
		
		// We will need a few constants/dimensions for the calculation.
		const size_t N(dE.size());
		const size_t NFields(F.size());
		
		// Create the pointers for the arrays in device memory.
		double *DeviceG4dE;
		double *DeviceG4dx;
		double *DevicedQBins;
		double *DevicedEBins;
		double *DeviceRBins;
		double *DeviceP;
		int *DeviceB;

		// Now we need to allocate memory on the device for these arrays. The raw input only needs
		// to be copied once, but the parameters will only be processed in batches of 32 (allocate
		// memory in loop).
		cudaMalloc( (void**)&DeviceG4dE, sizeof(double) * N );
		cudaMalloc( (void**)&DeviceG4dx, sizeof(double) * N );
		cudaMalloc( (void**)&DevicedQBins, sizeof(double) * 32 * NBINS );
		cudaMalloc( (void**)&DevicedEBins, sizeof(double) * 32 * NBINS );
		cudaMalloc( (void**)&DeviceRBins, sizeof(double) * 32 * NBINS );
		cudaMalloc( (void**)&DeviceP, sizeof(double) *  64 );
		cudaMalloc( (void**)&DeviceB, sizeof(int) * N );

		// Now copy the raw G4 input to the device.
		cudaMemcpy( DeviceG4dE, &dE[0], sizeof(double) * N, cudaMemcpyHostToDevice );
		cudaMemcpy( DeviceG4dx, &dx[0], sizeof(double) * N, cudaMemcpyHostToDevice );
		cudaMemcpy( DeviceB, &B[0], sizeof(int) * N, cudaMemcpyHostToDevice );
		
		// So the major time cost with outsourcing the recombination calculation to a GPU is in
		// copying to and from memory, and other overhead. We can minimize this by copying the
		// G4 input to the device memory once and performing all calculations on it at once. We will
		// need to copy the dQ calculations back to the host and the batch of parameters to the
		// device, but this is relatively little compared to the large size of the raw input.
		
		size_t ParHead;
		for( size_t fi(0); fi < NFields; ++fi )
		{
			ParHead = 0;
			while( ParHead + 64 <= p.size() )
			{
				// Reset the arrays allocated for the segment dQ and dE calculation.
				cudaMemset( DevicedQBins, 0, sizeof(double) * 32 * NBINS );
				cudaMemset( DevicedEBins, 0, sizeof(double) * 32 * NBINS );
				// Copy the current batch of parameters to the device.
				cudaMemcpy( DeviceP, &p[ParHead], sizeof(double) * 64, cudaMemcpyHostToDevice );
				// Make a kernel call for this batch of parameters and field.
				Birks <<< ( N / NThreadEntries + 1 ), ThreadDimensions >>>( DeviceG4dE,
											DeviceG4dx,
											DeviceB,
											F[fi],
											DeviceP,
											N,
											DevicedQBins,
											DevicedEBins );
				cudaDeviceSynchronize();
				// Retrieve any error states (if any).
				cudaError_t Error = cudaGetLastError();
				if( Error != cudaSuccess ) {
					std::cerr << "CUDA error: " << cudaGetErrorString(Error) << std::endl;
				}
				// Copy the resulting calculated values back to the host.
				GetR<<< NBINS, 32 >>>( DevicedQBins, DevicedEBins, DeviceRBins );
				cudaMemcpy( &RBins[fi][ 32 * NBINS * (ParHead / 64) ], DeviceRBins, sizeof(double) * 32 * NBINS, cudaMemcpyDeviceToHost );
				cudaMemcpy( &dEBins[fi][ 32 * NBINS * (ParHead / 64) ], DevicedEBins, sizeof(double) * 32 * NBINS, cudaMemcpyDeviceToHost );
				ParHead += 64;
			}
		}
		// Once we are finished we need to release the memory that was allocated on the device.
		cudaFree( DeviceG4dE );
		cudaFree( DeviceG4dx );
		cudaFree( DevicedQBins );
		cudaFree( DevicedEBins );
		cudaFree( DeviceRBins );
		cudaFree( DeviceP );
		cudaFree( DeviceB );
	}
}
