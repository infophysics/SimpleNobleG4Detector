#include "LArPrimaryGeneratorAction.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

LArPrimaryGeneratorAction::LArPrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(),
  fParticleGun(0), 
  fLArBox(0)
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
  fParticleGun->SetParticleEnergy(100.*MeV);
}

LArPrimaryGeneratorAction::~LArPrimaryGeneratorAction()
{
  delete fParticleGun;
}

void LArPrimaryGeneratorAction::GeneratePrimaries(G4Event* GenEvent)
{
  // This function is called at the begining of each event.
  
  G4double CubeX = 0;
  G4double CubeY = 0;
  G4double CubeZ = 0;

  if (!fLArBox)
  {
    G4LogicalVolume* LArLV
      = G4LogicalVolumeStore::GetInstance()->GetVolume("World");
    if ( LArLV ) fLArBox = dynamic_cast<G4Box*>(LArLV->GetSolid());
  }

  if ( fLArBox ) {
    CubeX = fLArBox->GetXHalfLength()*2.;
    CubeY = fLArBox->GetYHalfLength()*2.;
    CubeZ = fLArBox->GetZHalfLength()*2.;
  }  
  else  {
    G4ExceptionDescription msg;
    msg << "World volume of box shape not found.\n"; 
    msg << "Perhaps you have changed geometry.\n";
    msg << "The gun will be place at the center.";
    G4Exception("LArPrimaryGeneratorAction::GeneratePrimaries()",
     "MyCode0002",JustWarning,msg);
  }

  G4double Size = 0.8; 
  G4double X0 = Size * CubeX * (G4UniformRand()-0.5);
  G4double Y0 = Size * CubeY * (G4UniformRand()-0.5);
  G4double Z0 = -0.5 * CubeZ;
  
  fParticleGun->SetParticlePosition(G4ThreeVector(X0,Y0,Z0));
  fParticleGun->GeneratePrimaryVertex(GenEvent);
}
