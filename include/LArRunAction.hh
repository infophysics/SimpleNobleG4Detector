#ifndef LArRunAction_h
#define LArRunAction_h 1

#include "G4UserRunAction.hh"
#include "G4Accumulable.hh"
#include "globals.hh"

class G4Run;

/// Run action class
///
/// In EndOfRunAction(), it calculates the dose in the selected volume 
/// from the energy deposit accumulated via stepping and event actions.
/// The computed dose is then printed on the screen.

class LArRunAction : public G4UserRunAction
{
  public:
    LArRunAction();
    virtual ~LArRunAction();

    // virtual G4Run* GenerateRun();
    virtual void BeginOfRunAction(const G4Run*);
    virtual void EndOfRunAction(const G4Run*);

    void AddEdep (G4double edep);
    void FilldEdx(G4double dE, G4double dx);

  private:
    G4Accumulable<G4double> fEdep;
    G4Accumulable<G4double> fEdep2;
};

#endif

