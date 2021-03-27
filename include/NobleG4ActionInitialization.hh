#ifndef NobleG4ActionInitialization_h
#define NobleG4ActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

/// Action initialization class.

class NobleG4ActionInitialization : public G4VUserActionInitialization
{
  public:
    NobleG4ActionInitialization();
    virtual ~NobleG4ActionInitialization();

    virtual void BuildForMaster() const;
    virtual void Build() const;
};

#endif

    
