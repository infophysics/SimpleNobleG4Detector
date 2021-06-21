#include "NobleG4SensitiveDetector.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"

NobleG4SensitiveDetector::NobleG4SensitiveDetector(G4String SDName)
  : G4VSensitiveDetector(SDName)
{
  G4cout << "Creating Sensitive Detector with name: " << SDName << G4endl;

  // Declare the name of the collection that we will be using.
  collectionName.insert("NobleG4HitCollection");
}

NobleG4SensitiveDetector::~NobleG4SensitiveDetector() {}

void NobleG4SensitiveDetector::Initialize(G4HCofThisEvent* HCE)
{
  // Create the Hit Collection.
  HitCollection = new NobleG4HitCollection(GetName(), collectionName[0]);

  // GetCollectionID() is a heavy operation, so only do it when necessary.
  static G4int HCID = -1;
  if (HCID < 0) HCID = GetCollectionID(0);

  // Register the Hits Collection object in the Hits Collections of This Event.
  HCE->AddHitsCollection(HCID, HitCollection);
}

G4bool NobleG4SensitiveDetector::ProcessHits(G4Step *Step, G4TouchableHistory*)
{
  // Retrieve the initial and final coordinates of the step.
  G4ThreeVector x0 = Step->GetPreStepPoint()->GetPosition();
  G4ThreeVector x1 = Step->GetPostStepPoint()->GetPosition();

  NobleG4Hit* Hit = new NobleG4Hit(Step->GetTrack()->GetCurrentStepNumber());
  Hit->SetP0(x0);
  Hit->SetP1(x1);
  Hit->SetdE(Step->GetTotalEnergyDeposit() - Step->GetNonIonizingEnergyDeposit());
  Hit->Setdx(Step->GetStepLength());
  Hit->SetParentID(Step->GetTrack()->GetParentID());

  HitCollection->insert(Hit);
  
  return true;
}

void NobleG4SensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
  G4cout << "********END OF EVENT FOR SENSITIVE VOLUME*********" << G4endl;
  auto N  = HitCollection->entries();
  //for ( std::size_t i(0); i < N; ++i)
  //{
  //  (*HitCollection)[i]->Print();
  //}
}
