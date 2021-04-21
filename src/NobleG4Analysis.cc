#include <cmath>
#include "NobleG4Analysis.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"
#include "g4root.hh"

void ConstructStepTuple()
{
  // Construct the n-tuple. To be called during the RunAction.
  auto AnalysisManager = G4AnalysisManager::Instance();
  G4cout << "Using " << AnalysisManager->GetType() << G4endl;

  // Set verbosity and n-tuple merging parameters.
  AnalysisManager->SetVerboseLevel(1);
  AnalysisManager->SetNtupleMerging(true);

  // Create the n-tuple and initialize each branch.
  AnalysisManager->CreateNtuple("Kinematics", "Kinematics");
  AnalysisManager->CreateNtupleDColumn("dE");      // Energy deposited in a single step.
  //AnalysisManager->CreateNtupleDColumn("dx");      // Length of a single step.
  AnalysisManager->CreateNtupleDColumn("dEdx");    // Average dE/dx of a single step.
  //AnalysisManager->CreateNtupleDColumn("dI");      // Ionizing energy deposited in a single step.
  //AnalysisManager->CreateNtupleDColumn("dIdx");    // Average dI/dx of a single step.
  //AnalysisManager->CreateNtupleDColumn("dSx");     // Length of a single step in X.
  //AnalysisManager->CreateNtupleDColumn("dSy");     // Length of a single step in Y.
  //AnalysisManager->CreateNtupleDColumn("dSz");     // Length of a single step in Z.
  //AnalysisManager->CreateNtupleIColumn("id");      // ID of the step's particle.
  //AnalysisManager->CreateNtupleIColumn("pdg");     // PDG code of the step's particle.
  //AnalysisManager->CreateNtupleDColumn("x0");      // X-coordinate of step starting point.
  //AnalysisManager->CreateNtupleDColumn("y0");      // Y-coordinate of step starting point.
  //AnalysisManager->CreateNtupleDColumn("z0");      // Z-coordinate of step starting point.
  //AnalysisManager->CreateNtupleDColumn("x1");      // X-coordinate of step ending point.
  //AnalysisManager->CreateNtupleDColumn("y1");      // Y-coordinate of step ending point.
  //AnalysisManager->CreateNtupleDColumn("z1");      // Z-coordinate of step ending point.
  AnalysisManager->CreateNtupleIColumn("N");       // Event number of the step.
  //AnalysisManager->CreateNtupleDColumn("E");       // Average kinetic energy of the particle in a single step.
  //AnalysisManager->CreateNtupleDColumn("R");       // Escape probability of the step.
  AnalysisManager->FinishNtuple();
}

void ConstructStepH2()
{
  // Retrieve an instance of the Analysis Manager.
  auto AnalysisManager = G4AnalysisManager::Instance();
  G4cout << "Using " << AnalysisManager->GetType() << G4endl;

  // Create the H2 and initialize.
  AnalysisManager->CreateH2("dEdx", "dE/dx vs. dE",
			   100, 0, 25,
			   100, 0, 0.1);
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

G4double CalcR(const G4double dEdx, const G4double Field)
{
  G4double Eps = 0.07*pow(Field, -0.85);
  return (1 - ( Eps * dEdx ) / ( 1 + Eps * dEdx ));
}

G4double CalcRForStep(const G4Step* Step, const G4double Field)
{
  G4double dE = Step->GetTotalEnergyDeposit();
  G4double dx = Step->GetStepLength() / 10;
  return CalcR(dE/dx, Field);
}

G4double ArCalcQY(const G4double dE, const G4double R)
{
  return (R*dE) / (23.6 / 1000000);
}

G4double ArCalcLY(const G4double dE, const G4double R)
{
  return (dE / (19.5 / 1000000)) - ArCalcQY(dE, R);
}

void PopulateStepTuple(const G4Step* Step)
{
  // Get an instance of the Analysis Manager.
  auto AnalysisManager = G4AnalysisManager::Instance();

  // Retrieve/calculate various step-level quantities.
  G4double dE = Step->GetTotalEnergyDeposit();
  //G4double dI = dE - Step->GetNonIonizingEnergyDeposit();
  G4double dx = Step->GetStepLength() / 10;
  G4ThreeVector dS = Step->GetDeltaPosition();
  //G4double dSx = dS.x();
  //G4double dSy = dS.y();
  //G4double dSz = dS.z();

  // Retrieve the current track and corresponding track ID/PDG code.
  //G4Track* Track = Step->GetTrack();
  //G4int TrackID = Track->GetTrackID();
  //G4int PDG = Track->GetParticleDefinition()->GetPDGEncoding();

  // Retrieve the pre- and post- step points.
  //G4StepPoint* p0 = Step->GetPreStepPoint();
  //G4StepPoint* p1 = Step->GetPostStepPoint();
  //G4ThreeVector x0 = p0->GetPosition();
  //G4ThreeVector x1 = p1->GetPosition();

  // Calculate the average particle kinetic energy for the step.
  //G4double KE = (p0->GetKineticEnergy() + p1->GetKineticEnergy())/2.0;

  // Retrieve the current event number.
  G4int NEvent = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();

  // Calculate the escape probability.
  //G4double R = CalcR(dE/dx);

  // Fill the n-tuple.
  AnalysisManager->FillNtupleDColumn(0, dE);
  //AnalysisManager->FillNtupleDColumn(1, dx);
  AnalysisManager->FillNtupleDColumn(1, dE/dx);
  //AnalysisManager->FillNtupleDColumn(3, dI);
  //AnalysisManager->FillNtupleDColumn(4, dI/dx);
  //AnalysisManager->FillNtupleDColumn(5, dSx);
  //AnalysisManager->FillNtupleDColumn(6, dSy);
  //AnalysisManager->FillNtupleDColumn(7, dSz);
  //AnalysisManager->FillNtupleIColumn(8, TrackID);
  //AnalysisManager->FillNtupleIColumn(9, PDG);
  //AnalysisManager->FillNtupleDColumn(10, x0.x());
  //AnalysisManager->FillNtupleDColumn(11, x0.y());
  //AnalysisManager->FillNtupleDColumn(12, x0.z());
  //AnalysisManager->FillNtupleDColumn(13, x1.x());
  //AnalysisManager->FillNtupleDColumn(14, x1.y());
  //AnalysisManager->FillNtupleDColumn(15, x1.z());
  AnalysisManager->FillNtupleIColumn(2, NEvent);
  //AnalysisManager->FillNtupleDColumn(17, KE);
  //AnalysisManager->FillNtupleDColumn(18, R);
  AnalysisManager->AddNtupleRow();
}

void PopulateStepH2(const G4Step* Step)
{
  // Get an instance of the Analysis Manager.
  auto AnalysisManager = G4AnalysisManager::Instance();

  // Retrieve/calculate various step-level quantities.
  G4double dE = Step->GetTotalEnergyDeposit();
  G4double dx = Step->GetStepLength() / 10;
  G4double dEdx = dE/dx;

  AnalysisManager->FillH2(0, dEdx, dE);
}

void PopulateEventTuple(const G4double N, const G4double E, const G4double e, const G4double p)
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
