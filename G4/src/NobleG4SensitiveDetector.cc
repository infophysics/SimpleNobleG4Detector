#include "NobleG4SensitiveDetector.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"

NobleG4SensitiveDetector::NobleG4SensitiveDetector(G4String SDName)
  : G4VSensitiveDetector(SDName)
{
  G4cout << "Creating Sensitive Detector with name: " << SDName << G4endl;

  // Declare the name of the collection that we will be using.
  collectionName.insert("NobleG4HitCollection");
}

NobleG4SensitiveDetector::~NobleG4SensitiveDetector() {}

void NobleG4SensitiveDetector::Initialize(G4HCofThisEvent* HCE)
{
  // Create the Hit Collection.
  HitCollection = new NobleG4HitCollection(GetName(), collectionName[0]);

  // GetCollectionID() is a heavy operation, so only do it when necessary.
  static G4int HCID = -1;
  if (HCID < 0) HCID = GetCollectionID(0);

  // Register the Hits Collection object in the Hits Collections of This Event.
  HCE->AddHitsCollection(HCID, HitCollection);
}

G4bool NobleG4SensitiveDetector::ProcessHits(G4Step *Step, G4TouchableHistory*)
{
  // Retrieve the initial and final coordinates of the step.
  G4ThreeVector x0 = Step->GetPreStepPoint()->GetPosition();
  G4ThreeVector x1 = Step->GetPostStepPoint()->GetPosition();

  NobleG4Hit* Hit = new NobleG4Hit(Step->GetTrack()->GetTrackID());
  Hit->SetP0(x0);
  Hit->SetP1(x1);
  Hit->SetdE(Step->GetTotalEnergyDeposit() - Step->GetNonIonizingEnergyDeposit());
  Hit->Setdx(Step->GetStepLength());
  Hit->SetParentID(Step->GetTrack()->GetParentID());

  HitCollection->insert(Hit);
  
  return true;
}

void NobleG4SensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
  auto N  = HitCollection->entries();

  // A few parameters for the tracking.
  float BinSize(5.0); // Millimeters.
  const int NBins(100);
  float SecondaryThreshold(0.2); // Centimeters.

  // Loop over collected hits and add up the cumulative length of the primary track.
  std::vector<double> CumulativeLength;
  double TotalLength(0);
  // Will need these for the last primary step later.
  G4ThreeVector StartPoint, EndPoint;
  unsigned int NPrimaries(0);

  for ( std::size_t i(0); i < N; ++i)
  {
    auto Hit = (*HitCollection)[i];
    if( Hit->IsPrimary() )
    {
      ++NPrimaries;
      TotalLength += Hit->Getdx();
      CumulativeLength.push_back(TotalLength);
      StartPoint = Hit->P1();
      EndPoint = Hit->P0();
    }
  }

  // Now that we have the length information, we can make claims about the residual range
  // of each step. This requires another loop, again over only the primary track. At each
  // step we calculate which bin the step belongs in and assign it to that bin if it is
  // less than the configured NBins. Additionally, we want to store the start and end points
  // of the "edge" tracks of the bin in order to find the perpendicular distance of secondaries
  // to the bin in the next step. Note that "start" and "end" are relative to working
  // backwards from the end of the track.
  std::vector< std::pair<G4ThreeVector, G4ThreeVector> > BinPoints;
  int CurrentBin(0), b(-1);
  for ( std::size_t i(N-1); i+1 > 0; --i)
  {
    auto Hit = (*HitCollection)[i];
    if( Hit->IsPrimary() )
    {
      --NPrimaries;
      b = int( (TotalLength - CumulativeLength[NPrimaries]) / BinSize );
      // Check to see if we are in a different bin. If so, we need to keep track of the start
      // and end points of the bin.
      if( (b < NBins+1) && (b != CurrentBin) )
      {
	BinPoints.push_back(std::make_pair(StartPoint, EndPoint));
	StartPoint = EndPoint;
	CurrentBin = b;
      }
      // Additionally, we want to make the bin assignment and upkeep the end point so that it
      // remains at the end of the current step.
      if( b < NBins )
      {
	EndPoint = Hit->P0();
	Hit->SetRRBin(b);
      }
      // Most steps belong to a bin that is outside the allowed number of bins.
      else Hit->SetRRBin(-1);
    }
  }

  // The secondaries are up next. They can only be binned up once the bins have been defined
  // geometrically (start/end points). For each secondary we will need to compute the
  // perpendicular distance to each bin segment and assign it to the bin that is closest to
  // (assuming any of them are within the configured secondary threshold). This is essentially
  // a cylinder cut on secondaries with the central axis taken to be the track direction
  // within each bin.
  float ShortestDistance(1000), Distance, Gamma;
  G4ThreeVector ShortestVector;
  for ( std::size_t i(0); i < N; ++i)
  {
    auto Hit = (*HitCollection)[i];
    if( !Hit->IsPrimary() )
    {
      for( std::size_t n(0); n < BinPoints.size(); ++n)
      {
	// The approach is this: define the component of the vector formed by the secondary
	// end point along the bin (the "z" cylindrical coordinate) and the component
	// perpendicular to the bin (the "rho" cylindrical coordinate). Gamma essentially
	// parameterizes the former, then we write down this vector ("ShortestVector"). The
	// perpendicular component is therefore the magnitude of the difference between this
	// component and the end point of the secondary itself. 
	Gamma = ( Hit->P1() - BinPoints.at(n).first ).dot( BinPoints.at(n).second - BinPoints.at(n).first) / ( BinPoints.at(n).second - BinPoints.at(n).first ).mag2();
	ShortestVector = BinPoints.at(n).first + Gamma * ( BinPoints.at(n).second - BinPoints.at(n).first );
	Distance = ( ShortestVector - Hit->P1() ).mag();
	
	// It is possible for the end point to lie somewhere along the line formed by the line
	// segment (the bin start/end points), but not actually close to the bin itself. A
	// requirement on the "Gamma" coordinate of [0,1] restricts the secondaries to a
	// cylinder immediately around the bin rather than to some cylinder extended infinitely
	// in either direction. Here we enforce this requirement and keep track of the
	// shortest distance and corresponding bin.
	if( Gamma >= 0 && Gamma <= 1.0 && Distance < ShortestDistance )
	{
	  b = n;
	  ShortestDistance = Distance;
	}
      }
      
      // Now we have shortest distance to a binned track along with its corresponding bin. We
      // can now make a decision about whether it is sufficiently close to the bin to pass
      // the cylinder cut.
      if( ShortestDistance >= SecondaryThreshold ) b = -1;
      Hit->SetRRBin(b);
      ShortestDistance = 1000; // Reset.
    }
  }
}
