#ifndef NobleG4Hit_h
#define NobleG4Hit_h 1

#include "G4VHit.hh"
#include "G4Allocator.hh"
#include "G4THitsCollection.hh"
#include "G4ThreeVector.hh"

class NobleG4Hit : public G4VHit
{
public:
  NobleG4Hit(const G4int id);
  ~NobleG4Hit();

  void Print();
  void SetP0(const G4ThreeVector PreStep){ p0 = PreStep; }
  void SetP1(const G4ThreeVector PostStep){ p1 = PostStep; }
  void SetdE(const G4double e){ dE = e; }
  void Setdx(const G4double x){ dx = x; }
  void SetParentID(const G4int pid){ PID = pid; }

  G4ThreeVector P0() const { return p0; }
  G4ThreeVector P1() const { return p1; }
  G4double GetdE() const { return dE; }
  G4double Getdx() const { return dx; }

private:
  G4int ID;
  G4int PID;
  G4ThreeVector p0;
  G4ThreeVector p1;
  G4double dE;
  G4double dx;
};

typedef G4THitsCollection<NobleG4Hit> NobleG4HitCollection;

#endif
