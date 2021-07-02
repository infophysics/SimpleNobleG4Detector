#include "NobleG4SteppingAction.hh"
#include "NobleG4EventAction.hh"
#include "NobleG4DetectorConstruction.hh"
#include "NobleG4Analysis.hh"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"

NobleG4SteppingAction::NobleG4SteppingAction(NobleG4EventAction* EventAction)
: G4UserSteppingAction(),
  fEventAction(EventAction),
  fActiveVolume(0)
{}

NobleG4SteppingAction::~NobleG4SteppingAction()
{}


void NobleG4SteppingAction::UserSteppingAction(const G4Step* Step)
{
  // Check to see if the Active Volume has already been set.
  if (!fActiveVolume) { 
    const NobleG4DetectorConstruction* DetectorConstruction
      = static_cast<const NobleG4DetectorConstruction*>
        (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    fActiveVolume = DetectorConstruction->GetActiveVolume();   
  }

  // Get the volume of the current step.
  G4LogicalVolume* LogicalVolume 
    = Step->GetPreStepPoint()->GetTouchableHandle()
      ->GetVolume()->GetLogicalVolume();
      
  // Check if we are in Active Volume.
  if (LogicalVolume != fActiveVolume) return;

  // Collect energy deposited in this step.
  G4double dE = Step->GetTotalEnergyDeposit();

  // Calculate escape factor.
  G4double R = CalcRForStep(Step, fEventAction->GetField());

  // Calculate and collect electrons/photons.
  G4double e = ArCalcQY(dE, R);
  G4double p = ArCalcLY(dE, R);
  
  fEventAction->AddEnergy(dE);
  fEventAction->AddElectrons(e);
  fEventAction->AddPhotons(p);

  // Use a helper function to populate the analysis n-tuple.
  //if(!fEventAction->GetTupleState()) PopulateStepTuple(Step);
  //if(!fEventAction->GetTupleState()) PopulateStepTuple(Step);
}
