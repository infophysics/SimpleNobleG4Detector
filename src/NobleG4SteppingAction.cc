#include "NobleG4SteppingAction.hh"
#include "NobleG4EventAction.hh"
#include "NobleG4DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"

#include "g4root.hh"

NobleG4SteppingAction::NobleG4SteppingAction(NobleG4EventAction* EventAction)
: G4UserSteppingAction(),
  fEventAction(EventAction),
  fSensitiveVolume(0)
{}

NobleG4SteppingAction::~NobleG4SteppingAction()
{}


void NobleG4SteppingAction::UserSteppingAction(const G4Step* Step)
{
  if (!fSensitiveVolume) { 
    const NobleG4DetectorConstruction* DetectorConstruction
      = static_cast<const NobleG4DetectorConstruction*>
        (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    fSensitiveVolume = DetectorConstruction->GetSensitiveVolume();   
  }

  // Get the volume of the current step.
  G4LogicalVolume* LogicalVolume 
    = Step->GetPreStepPoint()->GetTouchableHandle()
      ->GetVolume()->GetLogicalVolume();
      
  // Check if we are in scoring volume.
  if (LogicalVolume != fSensitiveVolume) return;

  // Collect energy deposited in this step.
  auto AnalysisManager = G4AnalysisManager::Instance();

  G4double EdepStep = Step->GetTotalEnergyDeposit();
  G4double dx = Step->GetStepLength();
  fEventAction->AddEdep(EdepStep);
  //fEventAction->FilldEdx(EdepStep, dx);
  AnalysisManager->FillH1(0, EdepStep);
  AnalysisManager->FillH1(1, dx);
  AnalysisManager->FillH1(2, EdepStep/dx);
}
