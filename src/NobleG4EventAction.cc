#include "NobleG4EventAction.hh"
#include "NobleG4RunAction.hh"
#include "NobleG4Analysis.hh"
#include "NobleG4Hit.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"

NobleG4EventAction::NobleG4EventAction(NobleG4RunAction* RunAction)
: G4UserEventAction(),
  fRunAction(RunAction),
  fHCID(-1),
  fEnergy(0.0),
  fElectrons(0.0),
  fPhotons(0.0)
{} 

NobleG4EventAction::~NobleG4EventAction()
{}

NobleG4HitCollection* NobleG4EventAction::GetHitsCollection(G4int HCID, const G4Event* Event) const
{
  // Retrieve the hit collection from the event by ID.
  auto HitsCollection 
    = static_cast<NobleG4HitCollection*>(
        Event->GetHCofThisEvent()->GetHC(HCID));
  
  return HitsCollection;
}


void NobleG4EventAction::BeginOfEventAction(const G4Event*)
{    
  fEnergy = 0.0;
  fElectrons = 0.0;
  fPhotons = 0.0;
}

void NobleG4EventAction::EndOfEventAction(const G4Event* Event)
{   
  // Accumulate statistics in the Run Action.
  fRunAction->AddEnergy(fEnergy);
  fRunAction->AddElectrons(fElectrons);
  fRunAction->AddPhotons(fPhotons);

  // Populate (event-level) analysis n-tuple.
  if( this->GetEventOutput() ) PopulateEventTuple(Event->GetEventID(), fEnergy, fElectrons, fPhotons);

  // Populate (tracking) analysis n-tuple.
  if( this->GetTrackingOutput() )
  {
    // If the hit collection ID has not yet been defined, then retrieve it.
    if( fHCID == -1 ) fHCID = G4SDManager::GetSDMpointer()->GetCollectionID("NobleG4HitCollection");

    // Retrieve the hit collection and the number of hit entries.
    auto HitCollection = GetHitsCollection(fHCID, Event);
    auto N = HitCollection->entries();

    // Loop over each hit in the collection and save the hits that have been binned.
    for ( std::size_t i(0); i < N; ++i)
    {
      auto Hit = (*HitCollection)[i];
      if( Hit->IsBinned() )
      {
	PopulateTrackingTuple( Event->GetEventID(),
			       Hit->GetdE(),
			       Hit->Getdx(),
			       Hit->GetRRBin() );
      }
    }
  }    
}

G4bool NobleG4EventAction::GetTrackingOutput()
{
  return fRunAction->GetTrackingOutput();
}

G4bool NobleG4EventAction::GetEventOutput()
{
  return fRunAction->GetEventOutput();
}

G4double NobleG4EventAction::GetField()
{
  return fRunAction->GetField();
}
