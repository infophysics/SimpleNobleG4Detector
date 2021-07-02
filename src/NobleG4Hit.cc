#include "NobleG4Hit.hh"

NobleG4Hit::NobleG4Hit(const G4int id)
{
  ID = id;
}

NobleG4Hit::~NobleG4Hit() {}

void NobleG4Hit::Print()
{
  G4cout << "Hit with ID: " << ID << " (PID: "
	 << PID << ") at "
	 << p0 << " and "
	 << p1 << " with dE: "
	 << dE << " and dx: "
	 << dx << " ("
	 << dE/dx << " MeV/cm)." << G4endl;
}

