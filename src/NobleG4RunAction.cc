#include <sstream>
#include "NobleG4RunAction.hh"
#include "NobleG4PrimaryGeneratorAction.hh"
#include "NobleG4DetectorConstruction.hh"
#include "NobleG4Analysis.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4GenericMessenger.hh"
#include "G4AccumulableManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

NobleG4RunAction::NobleG4RunAction()
: G4UserRunAction(),
  fEnergy(0.0),
  fElectrons(0.0),
  fPhotons(0.0),
  fEventLevelTuple(true)
{
  // Create the Generic Messenger. Used for interacting
  // with custom macro commands.
  fMessenger = new G4GenericMessenger(this,
				      "/NobleG4/tuple/",
				      "Control of n-tuple quantities");

  // Now create the command that will interact with the
  // Run Action.
  //G4String RawCommand("");
  //G4GenericMessenger::Command& TupleCommand = fMessenger->DeclareProperty("event",
  //RawCommand,
  //"Set to use event-level metrics in n-tuple");
  fMessenger->DeclareMethod("event", &NobleG4RunAction::SetTupleState);
  
  //TupleCommand.SetParameterName("event", true);
  //TupleCommand.SetDefaultValue("true");
  //G4cout << "Raw Command: " << RawCommand << G4endl;
 
  // Register accumulables to the accumulable manager.
  G4AccumulableManager* AccumulableManager = G4AccumulableManager::Instance();
  AccumulableManager->RegisterAccumulable(fEnergy);
  AccumulableManager->RegisterAccumulable(fElectrons);
  AccumulableManager->RegisterAccumulable(fPhotons);
}

NobleG4RunAction::~NobleG4RunAction()
{
  // Cleanup the Generic Messenger instance.
  delete fMessenger;
  
  // Cleanup the Analysis Manager instance.
  delete G4AnalysisManager::Instance();
}

void NobleG4RunAction::BeginOfRunAction(const G4Run* Run)
{ 
  // Inform the RunManager to save random number seed.
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);

  // Reset accumulables to their initial values.
  G4AccumulableManager* AccumulableManager = G4AccumulableManager::Instance();
  AccumulableManager->Reset();

  // Open the analysis output file.
  auto AnalysisManager = G4AnalysisManager::Instance();
  //std::stringstream FileName;
  //FileName << "1MeV_electron_Run" << Run->GetRunID();
  AnalysisManager->OpenFile();//FileName.str());
}

void NobleG4RunAction::EndOfRunAction(const G4Run* Run)
{
  // Check to see that events were generated.
  G4int NEvents = Run->GetNumberOfEvent();
  if (NEvents == 0) return;

  // Merge accumulables. 
  G4AccumulableManager* AccumulableManager = G4AccumulableManager::Instance();
  AccumulableManager->Merge();

  // Retrieve total energy deposited in the volume.
  G4double Energy  = fEnergy.GetValue();
  G4double Electrons = fElectrons.GetValue();
  G4double Photons = fPhotons.GetValue();

  // Retrieve the Generator Action
  const NobleG4PrimaryGeneratorAction* GeneratorAction
   = static_cast<const NobleG4PrimaryGeneratorAction*>
     (G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());

  // It is useful to print out information about the primary generator
  // and the general run conditions. Retrieve the relevant information
  // here.
  G4String RunCondition;
  if (GeneratorAction)
  {
    // Retrieve primary particle type/energy, then store in RunCondition.
    const G4ParticleGun* ParticleGun = GeneratorAction->GetParticleGun();
    G4double ParticleEnergy = ParticleGun->GetParticleEnergy();
    RunCondition += ParticleGun->GetParticleDefinition()->GetParticleName();
    RunCondition += " of ";
    RunCondition += G4BestUnit(ParticleEnergy,"Energy");
  }
        
  // Some of this next bit comes from a Geant4 example, and I'm not entirely
  // certain what it means. Possibly some handling of multi-threaded processing
  // of runs?
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

  // Print out a summary of the run (number of events, primary particle,
  // primary particle energy, general run conditions, and total energy
  // deposited in the active volume).
  G4cout
     << G4endl
     << " The run consists of " << NEvents << " "<< RunCondition
     << G4endl
     << " Cumulated deposited energy per run, in active volume : " 
     << G4BestUnit(Energy,"Energy")
     << G4endl
     << " Average electrons/event : " << Electrons/NEvents << G4endl
     << " Average photons/event : " << Photons/NEvents << G4endl
     << "------------------------------------------------------------"
     << G4endl
     << G4endl;

  // Retrieve the Analysis Manager in order to write and close the output
  // analysis file.
  auto AnalysisManager = G4AnalysisManager::Instance();
  AnalysisManager->Write();
  AnalysisManager->CloseFile();
}

void NobleG4RunAction::AddEnergy(G4double Energy)
{
  fEnergy += Energy;
}

void NobleG4RunAction::AddElectrons(G4double Electrons)
{
  fElectrons += Electrons;
}

void NobleG4RunAction::AddPhotons(G4double Photons)
{
  fPhotons += Photons;
}

void NobleG4RunAction::SetTupleState(G4String Val)
{
  G4cout << "SET TUPLE STATE: " << Val << G4endl;
  fEventLevelTuple = (Val == "true");
  
  // Use the helper function construct the analysis n-tuple.
  if(fEventLevelTuple)
    {
      G4cout << "Constructing event-level n-tuple." << G4endl;
      ConstructEventTuple();
    }
  else
    {
      G4cout << "Constructing step-level n-tuple." << G4endl;
      ConstructStepTuple();
    }
}
