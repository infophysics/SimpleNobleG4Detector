#include <cmath>
#include "NobleG4Analysis.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"
#include "g4root.hh"

G4double Recombination(const G4double dE, const G4double dx, const G4double F)
{
  p[3] = { 1.0, 0.05, 1.0 };
  return ( p[0] * dE ) / ( 1 + ( p[1] / pow(F, p[2]) ) * ( dE / dx ) );
}

void ConstructStepTuple()
{
  // Construct the n-tuple. To be called during the RunAction.
  auto AnalysisManager = G4AnalysisManager::Instance();
  G4cout << "Using " << AnalysisManager->GetType() << G4endl;

  // Set verbosity and n-tuple merging parameters.
  AnalysisManager->SetVerboseLevel(1);
  AnalysisManager->SetNtupleMerging(true);

  // Create the n-tuple and initialize each branch.
  AnalysisManager->CreateNtuple("G4Tree", "G4Tree");
  AnalysisManager->CreateNtupleDColumn("dE");      // Energy deposited in a single step.
  AnalysisManager->CreateNtupleDColumn("dx");      // Length of a single step.
  AnalysisManager->CreateNtupleIColumn("N");       // Event number of the step.
  AnalysisManager->CreateNtupleIColumn("B");       // PDG code.
  AnalysisManager->FinishNtuple();
}

void ConstructEventTuple()
{
  // Construct the n-tuple. To be called during the RunAction.
  auto AnalysisManager = G4AnalysisManager::Instance();
  G4cout << "Using " << AnalysisManager->GetType() << G4endl;

  // Set verbosity and n-tuple merging parameters.
  AnalysisManager->SetVerboseLevel(1);
  AnalysisManager->SetNtupleMerging(true);

  // Create the n-tuple and initialize each branch.
  AnalysisManager->CreateNtuple("EventKinematics", "EventKinematics");
  AnalysisManager->CreateNtupleIColumn("N");      // Event number.
  AnalysisManager->CreateNtupleDColumn("E");      // Total energy.
  AnalysisManager->CreateNtupleDColumn("e");      // Total electrons.
  AnalysisManager->CreateNtupleDColumn("p");      // Total photons.
  AnalysisManager->FinishNtuple();
}

void ConstructTrackingTuple()
{
  // Construct the n-tuple. To be called during the RunAction.
  auto AnalysisManager = G4AnalysisManager::Instance();
  G4cout << "Using " << AnalysisManager->GetType() << G4endl;

  // Set verbosity and n-tuple merging parameters.
  AnalysisManager->SetVerboseLevel(1);
  AnalysisManager->SetNtupleMerging(true);

  // Create the n-tuple and initialize each branch.
  AnalysisManager->CreateNtuple("G4Tree", "G4Tree");
  AnalysisManager->CreateNtupleIColumn("N");      // Event number.
  AnalysisManager->CreateNtupleDColumn("dE");     // Ionizing energy of the step.
  AnalysisManager->CreateNtupleDColumn("dx");     // Step length.
  AnalysisManager->CreateNtupleIColumn("B");      // Assigned RR bin number.
  AnalysisManager->FinishNtuple();
}

void PopulateStepTuple(const G4int N, const G4double dE, const G4double dx)
{
  // Get an instance of the Analysis Manager.
  auto AnalysisManager = G4AnalysisManager::Instance();

  // Fill the n-tuple.
  AnalysisManager->FillNtupleDColumn(0, dE);
  AnalysisManager->FillNtupleDColumn(1, dx);
  AnalysisManager->FillNtupleIColumn(2, N);
  AnalysisManager->FillNtupleIColumn(3, 0);
  AnalysisManager->AddNtupleRow();
}

void PopulateEventTuple(const G4int N, const G4double E, const G4double e, const G4double p)
{
  // Get an instance of the Analysis Manager.
  auto AnalysisManager = G4AnalysisManager::Instance();

  // Fill the n-tuple.
  AnalysisManager->FillNtupleIColumn(0, N);
  AnalysisManager->FillNtupleDColumn(1, E);
  AnalysisManager->FillNtupleDColumn(2, e);
  AnalysisManager->FillNtupleDColumn(3, p);
  AnalysisManager->AddNtupleRow();
}

void PopulateTrackingTuple(const G4int N, const G4double dE, const G4double dx, const G4int B)
{
  // Get an instance of the Analysis Manager.
  auto AnalysisManager = G4AnalysisManager::Instance();

  // Fill the n-tuple.
  AnalysisManager->FillNtupleIColumn(0, N);
  AnalysisManager->FillNtupleDColumn(1, dE);
  AnalysisManager->FillNtupleDColumn(2, dx/10);
  AnalysisManager->FillNtupleIColumn(3, B);
  AnalysisManager->AddNtupleRow();
}
