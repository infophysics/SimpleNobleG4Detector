#include "LArRunAction.hh"
#include "LArPrimaryGeneratorAction.hh"
#include "LArDetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include "g4root.hh"

LArRunAction::LArRunAction()
: G4UserRunAction(),
  fEdep(0.),
  fEdep2(0.)
{ 
  // Register accumulable to the accumulable manager.
  G4AccumulableManager* AccumulableManager = G4AccumulableManager::Instance();
  AccumulableManager->RegisterAccumulable(fEdep);
  AccumulableManager->RegisterAccumulable(fEdep2);

  auto AnalysisManager = G4AnalysisManager::Instance();
  G4cout << "Using " << AnalysisManager->GetType() << G4endl;

  AnalysisManager->SetVerboseLevel(1);
  AnalysisManager->SetNtupleMerging(true);
  AnalysisManager->CreateH1("dE","dE in LAr", 50, 0., 10*MeV);
  AnalysisManager->CreateH1("dx","dx in LAr", 50, 0., 10*cm);
  AnalysisManager->CreateH1("dEdx","dE/dx in LAr", 50, 0., 50*MeV/cm);
}

LArRunAction::~LArRunAction()
{
  delete G4AnalysisManager::Instance();
}

void LArRunAction::BeginOfRunAction(const G4Run*)
{ 
  // Inform the RunManager to save random number seed.
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);

  // Reset accumulables to their initial values.
  G4AccumulableManager* AccumulableManager = G4AccumulableManager::Instance();
  AccumulableManager->Reset();

  auto AnalysisManager = G4AnalysisManager::Instance();
  G4String FileName = "test";
  AnalysisManager->OpenFile(FileName);
}

void LArRunAction::EndOfRunAction(const G4Run* Run)
{
  G4int NEvents = Run->GetNumberOfEvent();
  if (NEvents == 0) return;

  // Merge accumulables. 
  G4AccumulableManager* AccumulableManager = G4AccumulableManager::Instance();
  AccumulableManager->Merge();

  // Compute dose = total energy deposit in a run and its variance
  G4double edep  = fEdep.GetValue();
  G4double edep2 = fEdep2.GetValue();
  
  G4double RMS = edep2 - edep*edep/NEvents;
  if (RMS > 0.) RMS = std::sqrt(RMS); else RMS = 0.;  

  // Run conditions
  //  note: There is no primary generator action object for "master"
  //        run manager for multi-threaded mode.
  const LArPrimaryGeneratorAction* GeneratorAction
   = static_cast<const LArPrimaryGeneratorAction*>
     (G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
  G4String RunCondition;
  if (GeneratorAction)
  {
    const G4ParticleGun* ParticleGun = GeneratorAction->GetParticleGun();
    RunCondition += ParticleGun->GetParticleDefinition()->GetParticleName();
    RunCondition += " of ";
    G4double ParticleEnergy = ParticleGun->GetParticleEnergy();
    RunCondition += G4BestUnit(ParticleEnergy,"Energy");
  }
        
  // Print
  if (IsMaster()) {
    G4cout
     << G4endl
     << "--------------------End of Global Run-----------------------";
  }
  else {
    G4cout
     << G4endl
     << "--------------------End of Local Run------------------------";
  }
  
  G4cout
     << G4endl
     << " The run consists of " << NEvents << " "<< RunCondition
     << G4endl
     << " Cumulated Edep per run, in scoring volume : " 
     << G4BestUnit(edep,"Energy") << " RMS = " << G4BestUnit(RMS,"Energy")
     << G4endl
     << "------------------------------------------------------------"
     << G4endl
     << G4endl;

  auto AnalysisManager = G4AnalysisManager::Instance();
  AnalysisManager->Write();
  AnalysisManager->CloseFile();
}

void LArRunAction::AddEdep(G4double edep)
{
  fEdep  += edep;
  fEdep2 += edep*edep;
}

void LArRunAction::FilldEdx(G4double dE, G4double dx)
{
  auto AnalysisManager = G4AnalysisManager::Instance();
  AnalysisManager->FillH1(0, dE);
  AnalysisManager->FillH1(1, dx);
  AnalysisManager->FillH1(2, dE/dx);
}
