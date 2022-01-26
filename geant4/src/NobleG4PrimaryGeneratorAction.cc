#include "NobleG4PrimaryGeneratorAction.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

NobleG4PrimaryGeneratorAction::NobleG4PrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(),
  fParticleGun(0), 
  fNobleG4Box(0)
{
  G4int NParticle = 1;
  fParticleGun  = new G4ParticleGun(NParticle);

  // Default particle kinematic
  G4ParticleTable* ParticleTable = G4ParticleTable::GetParticleTable();
  G4String ParticleName;
  G4ParticleDefinition* Particle
    = ParticleTable->FindParticle(ParticleName="e-");
  fParticleGun->SetParticleDefinition(Particle);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
  fParticleGun->SetParticleEnergy(1.*MeV);
}

NobleG4PrimaryGeneratorAction::~NobleG4PrimaryGeneratorAction()
{
  delete fParticleGun;
}

void NobleG4PrimaryGeneratorAction::GeneratePrimaries(G4Event* Event)
{
  // This function is called at the begining of each event.
  
  G4double CubeX = 0;
  G4double CubeY = 0;
  G4double CubeZ = 0;

  // Check for existence of the detector volume (G4Box).
  if (!fNobleG4Box)
  {
    // If no detector volume is store, retrieve it.
    G4LogicalVolume* NobleG4LV
      = G4LogicalVolumeStore::GetInstance()->GetVolume("World");
    if ( NobleG4LV ) fNobleG4Box = dynamic_cast<G4Box*>(NobleG4LV->GetSolid());
  }

  // Retrieve the dimensions of the detector volume.
  if ( fNobleG4Box ) {
    CubeX = fNobleG4Box->GetXHalfLength()*2.;
    CubeY = fNobleG4Box->GetYHalfLength()*2.;
    CubeZ = fNobleG4Box->GetZHalfLength()*2.;
  }  
  else  {
    // No volume found, so throw an exception.
    G4ExceptionDescription msg;
    msg << "World volume of box shape not found.\n"; 
    msg << "Perhaps you have changed geometry.\n";
    msg << "The gun will be place at the center.";
    G4Exception("NobleG4PrimaryGeneratorAction::GeneratePrimaries()",
     "MyCode0002",JustWarning,msg);
  }

  // Generate primary vertex. The primary vertex is uniformly
  // generated across 80% of the XY face of the volume, and
  // at a depth of 10%.
  //G4double Size = 0.8; 
  //G4double X0 = Size * CubeX * (G4UniformRand()-0.5);
  //G4double Y0 = Size * CubeY * (G4UniformRand()-0.5);
  G4double X0 = 0 * CubeX;
  G4double Y0 = 0 * CubeY;
  G4double Z0 = -0.4 * CubeZ;
  
  fParticleGun->SetParticlePosition(G4ThreeVector(X0,Y0,Z0));
  fParticleGun->GeneratePrimaryVertex(Event);
}
