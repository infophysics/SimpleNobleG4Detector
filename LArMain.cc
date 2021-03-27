#include "LArDetectorConstruction.hh"
#include "LArActionInitialization.hh"

#include "G4RunManagerFactory.hh"

#include "G4UImanager.hh"
#include "FTFP_BERT.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "Randomize.hh"


int main(int argc,char** argv)
{
  // Detect interactive mode (if no arguments) and define UI session.
  G4UIExecutive* UI = 0;
  if ( argc == 1 ) {
    UI = new G4UIExecutive(argc, argv);
  }

  // Construct the default run manager.
  auto* RunManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);

  // Set mandatory initialization classes.

  // Detector construction.
  RunManager->SetUserInitialization(new LArDetectorConstruction());

  // Set the physics list.
  G4VModularPhysicsList* PhysicsList = new FTFP_BERT;
  PhysicsList->SetVerboseLevel(1);
  RunManager->SetUserInitialization(PhysicsList);
    
  // User action initialization.
  RunManager->SetUserInitialization(new LArActionInitialization());
  
  // Initialize the visualization.
  G4VisManager* VisManager = new G4VisExecutive;
  VisManager->Initialize();

  // Get the pointer to the User Interface manager
  G4UImanager* UIManager = G4UImanager::GetUIpointer();

  // Process macro or start UI session
  if ( ! UI ) { 
    // batch mode
    G4String Command = "/control/execute ";
    G4String FileName = argv[1];
    UIManager->ApplyCommand(Command+FileName);
  }
  else { 
    // interactive mode
    UIManager->ApplyCommand("/control/execute init_vis.mac");
    UI->SessionStart();
    delete UI;
  }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted 
  // in the main() program !
  
  delete VisManager;
  delete RunManager;
}
