#ifndef NobleG4Analysis_h
#define NobleG4Analysis_h 1
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "g4root.hh"

G4double Recombination(const G4double dE, const G4double dx, const G4double F);

// Helper function for constructing the (step-level) analysis n-tuple.
void ConstructStepTuple();

// Helper function for constructing the (event-level) analysis n-tuple.
void ConstructEventTuple();

// Helper function for constructing the (tracking) analysis n-tuple.
void ConstructTrackingTuple();

// Helper function for populating the (step-level) analysis n-tuple.
void PopulateStepTuple(const G4int N, const G4double dE, const G4double dx);

// Helper function for populating the (event-level) analysis n-tuple.
void PopulateEventTuple(const G4int N, const G4double E, const G4double e, const G4double p);

// Helper function for populating the (tracking) analysis n-tuple.
void PopulateTrackingTuple(const G4int N, const G4double dE, const G4double dx, const G4int B);

#endif
