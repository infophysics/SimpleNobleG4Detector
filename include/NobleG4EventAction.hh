#ifndef LArEventAction_h
#define LArEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

class LArRunAction;

/// Event action class.

class LArEventAction : public G4UserEventAction
{
  public:
    LArEventAction(LArRunAction* runAction);
    virtual ~LArEventAction();

    virtual void BeginOfEventAction(const G4Event* event);
    virtual void EndOfEventAction(const G4Event* event);

    void AddEdep(G4double edep) { fEdep += edep; }

  private:
    LArRunAction* fRunAction;
    G4double     fEdep;
};

#endif

    
