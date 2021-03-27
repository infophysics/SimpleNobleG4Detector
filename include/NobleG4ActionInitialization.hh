#ifndef LArActionInitialization_h
#define LArActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

/// Action initialization class.

class LArActionInitialization : public G4VUserActionInitialization
{
  public:
    LArActionInitialization();
    virtual ~LArActionInitialization();

    virtual void BuildForMaster() const;
    virtual void Build() const;
};

#endif

    
