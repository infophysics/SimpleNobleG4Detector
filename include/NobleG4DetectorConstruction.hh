#ifndef LArDetectorConstruction_h
#define LArDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

// Detector construction class to define materials and geometry.

class LArDetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    LArDetectorConstruction();
    virtual ~LArDetectorConstruction();

    virtual G4VPhysicalVolume* Construct();

    G4LogicalVolume* GetSensitiveVolume() const { return fSensitiveVolume; }

  protected:
    G4LogicalVolume*  fSensitiveVolume;
};

#endif
