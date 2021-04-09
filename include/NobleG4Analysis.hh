#ifndef NobleG4Analysis_h
#define NobleG4Analysis_h 1
#include "G4Step.hh"
#include "g4root.hh"

// Helper function for constructing the (step-level) analysis n-tuple.
void ConstructStepTuple();

// Helper function for constructing the (step-level) analysis H2.
void ConstructStepH2();

// Helper function for constructing the (event-level) analysis n-tuple.
void ConstructEventTuple();

// Helper function for calculating the escape probability.
G4double CalcR(const G4double dEdx);

// Helper function for calculating the escape probability.
G4double CalcRForStep(const G4Step* Step);

// Helper function for calculating the number of electrons (Argon).
G4double ArCalcQY(const G4double dE, const G4double R);

// Helper function for calculating the number of photons (Argon).
G4double ArCalcLY(const G4double dE, const G4double R);

// Helper function for populating the (step-level) analysis n-tuple.
void PopulateStepTuple(const G4Step* Step);

// Helper function for populating the (step-level) analysis H2.
void PopulateStepH2(const G4Step* Step);

// Helper function for populating the (event-level) analysis n-tuple.
void PopulateEventTuple(const G4double N, const G4double E, const G4double e, const G4double p);

#endif
