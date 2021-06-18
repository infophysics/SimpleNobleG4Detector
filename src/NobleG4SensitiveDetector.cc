#include "NobleG4SensitiveDetector.hh"

/*class NobleG4SensitiveDetector : public G4VSensitiveDetector
{
public:
  // Constructor/destructor for the sensitive volume.
  NobleG4SensitiveDetector(G4String SDName);
  ~NobleG4SensitiveDetector();

  // Member functions for the sensitive volumen.
  G4bool ProcessHits(G4Step *Step, G4TouchableHistory *THist);
  void Initialize(G4HCofThisEvent* HCE);
  void EndOfEvent(G4HCofThisEvent* HCE);
  };*/

NobleG4SensitiveDetector::NobleG4SensitiveDetector(G4String SDName)
  : G4VSensitiveDetector(SDName)
{
  G4cout << "Creating Sensitive Detector with name: " << SDName << G4endl;

  // Declare the name of the collection that we will be using.
  collectionName.insert("NobleG4VoxelHitCollection");
}

void NobleG4SensitiveDetector::Initialize(G4HCofThisEvent* HCE)
{
  // Create the Hit Collection.
  hitCollection = new NobleG4VoxelHitCollection(GetName(), collectionName[0]);

  // GetCollectionID() is a heavy operation, so only do it when necessary.
  static G4int HCID = -1;
  if (HCID < 0) HCID = GetCollectionID(0);

  // Register the Hits Collection object in the Hits Collections of This Event.
  HCE->AddHitsCollection(HCID, hitCollection);
}

G4bool NobleG4SensitiveDetector::ProcessHits(G4Step *Step, G4TouchableHistor *THist)
{
  // Retrieve the initial and final coordinates of the step.
  G4ThreeVector x0 = Step->GetPreStepPoint()->GetTouchableHandle()->GetPosition();
  G4ThreeVector x1 = Step->GetPostStepPoint()->GetTouchableHandle()->GetPosition();

  
  
  return true;
}
