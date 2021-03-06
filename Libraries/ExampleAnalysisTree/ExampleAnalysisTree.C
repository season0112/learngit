#include "AnalysisEvent.hh"
#include "ExampleAnalysisTree.hh"
#include "AMSGeometry.h"
#include "TrackerTrack.h"
#include "MvaImplementation.hh"
#include "MvaInterface.hh"
#include "Tracker.h"
#include "TrackerTrackFit.h"
#include "Event.h"
#include <TrdLikelihoodCalculation.hh>
#include <cmath>
#include <AnalysisEvent.hh>
#define INFO_OUT_TAG "ExampleAnalysisTree"
#include "debugging.hh"
#include "Utilities.hh"
#include "AnalysisParticle.hh"
#include "EcalEnergyReconstruction.hh"
#include "EcalLongitudinalShowerFit.hh"
#include "Environment.hh"
#include "ParticleId.hh"
#include "ResolutionModels.hh"
#include "SlowControlLookup.hh"
#include <TrackerTrackCoordinates.h>
#include "ECALShower.h"
#include "EcalEnergyReconstruction.hh"
#include "EcalLongitudinalShowerFit.hh"

ExampleAnalysisTree::ExampleAnalysisTree()
  : IO::TreeInterface("ExampleAnalysisTree", "Example analysis tree") {

  RegisterBranches();

 // Load proton charge-confusion MVA
   const auto* mvaInterfaceCC = Mva::MvaInterface::CreateMvaInterfaceByName("Mva::ProtonChargeConfusionMvaInterface");
      fProtonChargeConfusionMva = mvaInterfaceCC->CreateMvaImplementation();
}


template <typename T>
double CalculateMeanChargeForLayers(unsigned int first, unsigned int last, const std::vector<T>& charges) {

  double charge = 0;
  double numberOfLayers = 0;
  assert(first > 0);
  assert(last <= charges.size());
  for (unsigned int i = first; i <= last; ++i) {
    const double layerCharge = charges[i - 1];
    if (layerCharge > 0) {
      charge += layerCharge;
      ++numberOfLayers;
    }
  }
  return charge / numberOfLayers;
}


void ExampleAnalysisTree::Fill(const Analysis::Event& event) {
//    ZTrackerLayer=ZTrackerLayer();
        ZTrackerLayer1=AC::AMSGeometry::ZTrackerLayer1;
        ZTrackerLayer2=AC::AMSGeometry::ZTrackerLayer2;
        ZTrackerLayer3=AC::AMSGeometry::ZTrackerLayer3;
        ZTrackerLayer4=AC::AMSGeometry::ZTrackerLayer4;
        ZTrackerLayer5=AC::AMSGeometry::ZTrackerLayer5;
        ZTrackerLayer6=AC::AMSGeometry::ZTrackerLayer6;
        ZTrackerLayer7=AC::AMSGeometry::ZTrackerLayer7;
        ZTrackerLayer8=AC::AMSGeometry::ZTrackerLayer8;
        ZTrackerLayer9=AC::AMSGeometry::ZTrackerLayer9;
        assert(UnassociatedHitZLayer().empty());
        assert(UnassociatedHitXCoordinate().empty());
        assert(UnassociatedHitYCoordinate().empty());
        assert(UnassociatedChargeX().empty());
        assert(UnassociatedChargeY().empty());
  for(auto & unassociatedhit: event.RawEvent()->Tracker().UnassociatedReconstructedHits()){
    UnassociatedHitZLayer().emplace_back(unassociatedhit.Layer());
    UnassociatedHitXCoordinate().emplace_back(unassociatedhit.X());
    UnassociatedHitYCoordinate().emplace_back(unassociatedhit.Y());
    UnassociatedChargeX().emplace_back(unassociatedhit.QX());
    UnassociatedChargeY().emplace_back(unassociatedhit.QY());
    }

  McMomentum= event.McMomentum(); 

  if (const Analysis::Particle* primaryParticle = event.PrimaryParticle()) {
/*
      if (const AC::ECALShower* shower = primaryParticle->EcalShower()){
      EcalBDT_EnergyD = shower->EcalBDTv7_EnergyD();
      EcalBDT_EnergyD_Smoothed = shower->EcalBDTv7_EnergyD_Smoothed();
      }
*/  
    EcalBDT_EnergyD = primaryParticle->EcalEstimator(AC::ECALShower::BDTv7_EnergyD);
    EcalBDT_EnergyD_Smoothed = primaryParticle->EcalEstimator(AC::ECALShower::BDTv7_EnergyD_Smoothed);

    InnerTrackerCharge = primaryParticle->TrackerCharge();
    // Example on how to fill a vector.
    assert(TrackerCharges().empty());
    TrackerCharges().emplace_back(primaryParticle->TrackerChargeFor(Analysis::Particle::TrkLay1));
    TrackerCharges().emplace_back(primaryParticle->TrackerChargeFor(Analysis::Particle::TrkLay2));
    TrackerCharges().emplace_back(primaryParticle->TrackerChargeFor(Analysis::Particle::TrkLay3));
    TrackerCharges().emplace_back(primaryParticle->TrackerChargeFor(Analysis::Particle::TrkLay4));
    TrackerCharges().emplace_back(primaryParticle->TrackerChargeFor(Analysis::Particle::TrkLay5));
    TrackerCharges().emplace_back(primaryParticle->TrackerChargeFor(Analysis::Particle::TrkLay6));
    TrackerCharges().emplace_back(primaryParticle->TrackerChargeFor(Analysis::Particle::TrkLay7));
    TrackerCharges().emplace_back(primaryParticle->TrackerChargeFor(Analysis::Particle::TrkLay8));
    TrackerCharges().emplace_back(primaryParticle->TrackerChargeFor(Analysis::Particle::TrkLay9));
      if (const AC::TrackerTrack* trackertrack=primaryParticle->TrackerTrack()){
        assert(HitXCoordinate().empty());
        assert(HitYCoordinate().empty());
        assert(HitZLayer().empty());
        assert(ChargeX().empty());
        assert(ChargeY().empty());
        assert(RigidityWithoutThisHit().empty());
        assert(ClusterWidthX().empty());
        assert(ClusterWidthY().empty());
        assert(ResidualYInMicrons().empty());
          for(auto& hit: trackertrack->ReconstructedHits()){
    ///////////////////////fill XYZ and ChargeXY Coordinate.
            HitZLayer().emplace_back(hit.Layer());
            HitXCoordinate().emplace_back(hit.X());
            HitYCoordinate().emplace_back(hit.Y());
            ChargeX().emplace_back(hit.QX());
            ChargeY().emplace_back(hit.QY());
            ClusterWidthX().emplace_back(hit.ClusterWidthX());
            ClusterWidthY().emplace_back(hit.ClusterWidthY());
            RigidityWithoutThisHit().emplace_back(hit.RigidityWithoutThisHit());
            ResidualYInMicrons().emplace_back(hit.ResidualYInMicrons());
          }
  ////////////////TrackerLayerPatternClassification
          CCBDT=trackertrack->CCBDT();
          CCBDTLapp=trackertrack->CCBDTLapp();
          CCLikelihood=trackertrack->CCLikelihood();
          Pattern=trackertrack->TrackerLayerPatternClassification();
        }
     if (const AC::TrackerTrackCoordinates* trackertrackcoordinates=primaryParticle->TrackerTrackCoordinates()){
          XLayer1=trackertrackcoordinates->XLayer1();
          YLayer1=trackertrackcoordinates->YLayer1();
          XLayer2=trackertrackcoordinates->XLayer2();
          YLayer2=trackertrackcoordinates->YLayer2();
          XLayer3=trackertrackcoordinates->XLayer3();
          YLayer3=trackertrackcoordinates->YLayer3();
          XLayer4=trackertrackcoordinates->XLayer4();
          YLayer4=trackertrackcoordinates->YLayer4();
          XLayer5=trackertrackcoordinates->XLayer5();
          YLayer5=trackertrackcoordinates->YLayer5();
          XLayer6=trackertrackcoordinates->XLayer6();
          YLayer6=trackertrackcoordinates->YLayer6();
          XLayer7=trackertrackcoordinates->XLayer7();
          YLayer7=trackertrackcoordinates->YLayer7();
          XLayer8=trackertrackcoordinates->XLayer8();
          YLayer8=trackertrackcoordinates->YLayer8();
          XLayer9=trackertrackcoordinates->XLayer9();
          YLayer9=trackertrackcoordinates->YLayer9();
        }
  }

 // 16 MVA varabiles
 
  const AC::Event* rawEvent = event.RawEvent();
  assert(rawEvent);
 
  const Analysis::Particle* particle = event.PrimaryParticle();
  assert(particle);

  const AC::TrackerTrack* trackerTrack = particle->TrackerTrack();
  assert(trackerTrack);  

  const int refitPattern = AC::PGMA + AC::RebuildFromTDV;
  const AC::ParticleHypothesis particleHypothesis = AC::DefaultMass;

  int iFit = trackerTrack->GetFitFuzzy(AC::Choutko, AC::All, refitPattern, particleHypothesis);
  if (iFit >= 0) {
    const AC::TrackerTrackFit& trackFit = trackerTrack->TrackFits().at(iFit);
    Rigidity = trackFit.Rigidity();
    InverseRigidityError = trackFit.InverseRigidityError();
    Chi2TrackerX = trackFit.ChiSquareNormalizedX();
    Chi2TrackerY = trackFit.ChiSquareNormalizedY();
    YResidualJLayer1 = trackFit.YResidualJLayer1();
    YResidualJLayer9 = trackFit.YResidualJLayer9();
  }

  iFit = trackerTrack->GetFitFuzzy(AC::Choutko, AC::Inner, refitPattern, particleHypothesis);
  if (iFit >= 0) {
    const AC::TrackerTrackFit& trackFit = trackerTrack->TrackFits().at(iFit);
    RigidityInner = trackFit.Rigidity();
    Chi2TrackerXInner = trackFit.ChiSquareNormalizedX();
    Chi2TrackerYInner = trackFit.ChiSquareNormalizedY();
    InverseRigidityErrorInner = trackFit.InverseRigidityError();
  }

  iFit = trackerTrack->GetFitFuzzy(AC::Alcaraz, AC::All, refitPattern, particleHypothesis);
  if (iFit >= 0) {
    const AC::TrackerTrackFit& trackFit = trackerTrack->TrackFits().at(iFit);
    RigidityAlcaraz = trackFit.Rigidity();
    InverseRigidityErrorAlcaraz = trackFit.InverseRigidityError();
  }

  iFit = trackerTrack->GetFitFuzzy(AC::ChikanianF, AC::All, refitPattern, particleHypothesis);
  if (iFit >= 0) {
    const AC::TrackerTrackFit& trackFit = trackerTrack->TrackFits().at(iFit);
    RigidityChikanian = trackFit.Rigidity();
    Chi2TrackerYChikanian = trackFit.ChiSquareNormalizedY();
    InverseRigidityErrorChikanian = trackFit.InverseRigidityError();
  }

  iFit = trackerTrack->GetFitFuzzy(AC::Choutko, AC::InnerPlusL1, refitPattern, particleHypothesis);
  if (iFit >= 0) {
    const AC::TrackerTrackFit& trackFit = trackerTrack->TrackFits().at(iFit);
    RigidityInnerL1 = trackFit.Rigidity();
    Chi2TrackerXInnerL1 = trackFit.ChiSquareNormalizedX();
    Chi2TrackerYInnerL1 = trackFit.ChiSquareNormalizedY();
  }

  iFit = trackerTrack->GetFitFuzzy(AC::Choutko, AC::InnerPlusL9, refitPattern, particleHypothesis);
  if (iFit >= 0) {
    const AC::TrackerTrackFit& trackFit = trackerTrack->TrackFits().at(iFit);
    RigidityInnerL9 = trackFit.Rigidity();
    Chi2TrackerXInnerL9 = trackFit.ChiSquareNormalizedX();
    Chi2TrackerYInnerL9 = trackFit.ChiSquareNormalizedY();
  }


    iFit = trackerTrack->GetFitFuzzy(AC::Choutko, AC::UpperHalf, refitPattern, particleHypothesis);
  if (iFit >= 0) {
    const AC::TrackerTrackFit& trackFit = trackerTrack->TrackFits().at(iFit);
    RigidityInnerUpperHalf = trackFit.Rigidity();
    Chi2TrackerXInnerUpperHalf = trackFit.ChiSquareNormalizedX();
    Chi2TrackerYInnerUpperHalf = trackFit.ChiSquareNormalizedY();
  }

  iFit = trackerTrack->GetFitFuzzy(AC::Choutko, AC::LowerHalf, refitPattern, particleHypothesis);
  if (iFit >= 0) {
    const AC::TrackerTrackFit& trackFit = trackerTrack->TrackFits().at(iFit);
    RigidityInnerLowerHalf = trackFit.Rigidity();
    Chi2TrackerXInnerLowerHalf = trackFit.ChiSquareNormalizedX();
    Chi2TrackerYInnerLowerHalf = trackFit.ChiSquareNormalizedY();

  }


  if (RigidityInnerL1() + RigidityInnerL9() != 0)
    RigidityAsymmetry = (RigidityInnerL1() - RigidityInnerL9()) / (RigidityInnerL1() + RigidityInnerL9());

  if (RigidityInnerL9() + RigidityInner() != 0)
    RigidityAsymmetryL9 = (RigidityInnerL9() - RigidityInner()) / (RigidityInnerL9() + RigidityInner());
 

  if (Chi2TrackerYInner() + Chi2TrackerY() != 0)
    Chi2TrackerYAsymmetry = (Chi2TrackerYInner() - Chi2TrackerY()) / (Chi2TrackerYInner() + Chi2TrackerY());

  if (RigidityInner() != 0 && Rigidity() != 0)
    InnerMaxSpanRigidityMatching = 100.0 * ((1.0 / RigidityInner()) - (1.0 / Rigidity()));

  if (RigidityInnerL1() != 0 && RigidityInnerL9() != 0)
    L1L9RigidityMatching = 100.0 * ((1.0 / RigidityInnerL1()) - (1.0 / RigidityInnerL9()));

  if (RigidityInnerUpperHalf() != 0 && RigidityInnerLowerHalf() != 0)
    L24L58RigidityMatching = 100.0 * ((1.0 / RigidityInnerUpperHalf()) - (1.0 / RigidityInnerLowerHalf()));

  if (Chi2TrackerXInner() > 0)
    Log10Chi2TrackerXInner = std::log10(Chi2TrackerXInner());

  if (Chi2TrackerYInner() > 0)
    Log10Chi2TrackerYInner = std::log10(Chi2TrackerYInner());

  if (Chi2TrackerX() > 0)
    Log10Chi2TrackerX = std::log10(Chi2TrackerX());

  if (Chi2TrackerY() > 0)
    Log10Chi2TrackerY = std::log10(Chi2TrackerY());


  {
    const double charge58 = CalculateMeanChargeForLayers(5, 8, TrackerCharges());
    const double charge24 = CalculateMeanChargeForLayers(2, 4, TrackerCharges());
    if (charge58 > 0 && charge24 > 0 && InnerTrackerCharge() > 0)
      TrackerL58L24ChargeAsymmetry = (charge58 - charge24) / InnerTrackerCharge();
  }


  if (TrackerCharges().at(8) > 0)
    TrackerL9Charge = TrackerCharges().at(8);

  {
    const double charge = CalculateMeanChargeForLayers(7, 8, TrackerCharges());
    if (charge > 0) {
      TrackerL78Charge = charge;
    }
  }


  UpperTofCharge = particle->UpperTofCharge();
  LowerTofCharge = particle->LowerTofCharge();
  RichBeta = particle->RichBeta();

  bool pXeOk = false;
  const double pXe = Utilities::SlowControlLookup::Self()->QueryXenonPressure(particle->AnaEvent()->TimeStamp(), pXeOk);
  Analysis::TrdLikelihoodCalculation likelihoodCalculation(particle->TrdHitsFromTrackerTrack(), pXe);

  TrdLogLikelihoodRatioElectronProtonTracker = likelihoodCalculation.ComputeLogLikelihoodRatio(ParticleId::Electron, ParticleId::Proton, particle->Rigidity());
  TrdLogLikelihoodRatioProtonHeliumTracker = likelihoodCalculation.ComputeLogLikelihoodRatio(ParticleId::Proton, ParticleId::Helium, particle->Rigidity());


 // Proton CC MVA
  fProtonChargeConfusionMva->ProcessEvent(event);
       if (fProtonChargeConfusionMva->IsApplicable())
                 ProtonCCMVABDT = fProtonChargeConfusionMva->EvaluateClassifier(fProtonChargeConfusionMva->CategoryForEvent(std::abs(Rigidity())));

}

void ExampleAnalysisTree::UpdateInMemoryBranches() {

}
