#ifndef NobleG4DetectorConstruction_h
#define NobleG4DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

// Detector construction class to define materials and geometry.

class NobleG4DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    NobleG4DetectorConstruction();
    virtual ~NobleG4DetectorConstruction();

    virtual G4VPhysicalVolume* Construct();

    G4LogicalVolume* GetActiveVolume() const { return fActiveVolume; }

  protected:
    G4LogicalVolume*  fActiveVolume;
};

#endif
