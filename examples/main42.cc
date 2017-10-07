// main42.cc is a part of the PYTHIA event generator.
// Copyright (C) 2011 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL version 2, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// Studies of hadron-level and parton-level minimum-bias quantities,
// comparing internal default PDF with one from LHAPDF.
// Major differences indicate need for major retuning, e.g. pT0Ref. 

// Access time information.
#include <ctime>

#include "Pythia.h"

using namespace Pythia8; 

int main() {

  // Machine: 1 = Tevatron, 2 = LHC. Statistics.
  int machine = 1;
  int nEvent  = 10000;

  // Select new PDF set; LHAPDF file name conventions.
  //string pdfSet = "cteq5l.LHgrid";
  //string pdfSet = "cteq61.LHpdf";
  //string pdfSet = "cteq61.LHgrid";
  //string pdfSet = "MRST2004nlo.LHgrid";
  string pdfSet = "MRST2001lo.LHgrid";

  // Histograms for hadron-level quantities.
  double nMax = (machine == 1) ? 199.5 : 399.5;
  Hist nChargedOld("n_charged old PDF", 100, -0.5, nMax);
  Hist nChargedNew("n_charged new PDF", 100, -0.5, nMax);
  Hist nChargedRat("n_charged new/old PDF", 100, -0.5, nMax);
  Hist ySpecOld("y charged distribution old PDF", 100, -10., 10.); 
  Hist ySpecNew("y charged distribution new PDF", 100, -10., 10.); 
  Hist ySpecRat("y charged distribution new/old PDF", 100, -10., 10.); 
  Hist pTSpecOld("pT charged distribution old PDF", 100, 0., 20.); 
  Hist pTSpecNew("pT charged distribution new PDF", 100, 0., 20.); 
  Hist pTSpecRat("pT charged distribution new/old PDF", 100, 0., 20.); 
  Hist avgPTnChOld("<pT>(n_charged) old PDF", 100, -0.5, nMax);  
  Hist avgPTnChNew("<pT>(n_charged) new PDF", 100, -0.5, nMax);  
  Hist avgPTnChRat("<pT>(n_charged) new/old PDF", 100, -0.5, nMax);  

  // Histograms for parton-level quantities.
  Hist xDistOld("MI log(x) distribution old PDF", 80, -8., 0.); 
  Hist xDistNew("MI log(x) distribution new PDF", 80, -8., 0.); 
  Hist xDistRat("MI log(x) distribution new/old PDF", 80, -8., 0.); 
  Hist pTDistOld("MI pT (=Q) distribution old PDF", 100, 0., 20.); 
  Hist pTDistNew("MI pT (=Q) distribution new PDF", 100, 0., 20.); 
  Hist pTDistRat("MI pT (=Q) distribution new/old PDF", 100, 0., 20.); 

  // Loop over one default run and one with new PDF.
  for (int iRun = 0; iRun < 2; ++iRun) {

    // Get starting time in seconds..
    time_t tBegin = time(0);

    // Generator.
    Pythia pythia;
    Event& event = pythia.event;

    // Generate minimum-bias events, with or without double diffraction.
    pythia.readString("SoftQCD:minBias = on");  
    //pythia.readString("SoftQCD:doubleDiffractive = on"); 

    // Generate QCD jet events, above some threshold.
    //pythia.readString("HardQCD:all = on"); 
    //pythia.readString("PhaseSpace:pTHatMin = 50."); 

    // In second run pick new PDF set.
    if (iRun == 1) {
      pythia.readString("PDF:useLHAPDF = on");
      pythia.readString("PDF:LHAPDFset = " + pdfSet);

     // Allow extrapolation of PDF's beyond x and Q2 boundaries, at own risk.
     // Default behaviour is to freeze PDF's at boundaries.
     pythia.readString("PDF:extrapolateLHAPDF = on");

      // Need to change pT0Ref depending on choice of PDF.
      // One possibility: retune to same <n_charged>.
      //pythia.readString("MultipleInteractions:pT0Ref = 2.17");
    }

    // Tevatron/LHC initialization. 
    double eCM = (machine == 1) ? 1960. : 14000.;
    if (machine == 1) pythia.init( 2212, -2212, eCM);
    else              pythia.init( 2212,  2212, eCM);
    pythia.settings.listChanged();
   
    // Begin event loop.
    for (int iEvent = 0; iEvent < nEvent; ++iEvent) {

      // Generate events.  Skip if error.
      if (!pythia.next()) continue;

      // Statistics on multiplicity and pT.
      int    nCh   = 0;
      double pTsum = 0.;
      for (int i = 0; i < event.size(); ++i) 
      if (event[i].isFinal() && event[i].isCharged()) {
        ++nCh;
        pTsum += event[i].pT();

        // Fill histograms for charged y and pT spectra..
        if (iRun == 0) {
          ySpecOld.fill( event[i].y() );
          pTSpecOld.fill( event[i].pT() );
        } else {
          ySpecNew.fill( event[i].y() );
          pTSpecNew.fill( event[i].pT()  );
        }
      }
      
      // Fill histograms for summed quantities.
      if (iRun == 0) {
        nChargedOld.fill( nCh );
        avgPTnChOld.fill( nCh, pTsum / max(1, nCh) );
      } else {
        nChargedNew.fill( nCh );
        avgPTnChNew.fill( nCh, pTsum / max(1, nCh) );
      }

      // Loop through event record and fill x of all incoming partons.
      for (int i = 1; i < event.size(); ++i) 
      if (event[i].status() == -21 || event[i].status() == -31) {
        double x = 2. * event[i].e() / eCM;
        if (iRun == 0) xDistOld.fill( log10(x) );
        else           xDistNew.fill( log10(x) );
      }

      // Loop through multiple interactions list and fill pT of all MI's.
      for (int i = 0; i < pythia.info.nMI(); ++i) {
        double pT = pythia.info.pTMI(i);
        if (iRun == 0) pTDistOld.fill( pT );
        else           pTDistNew.fill( pT );
      }

    // End of event loop.
    }

    // Statistics. 
    pythia.statistics( true );

    // Get finishing time in seconds. Print used time.
    time_t tEnd = time(0);
    cout << "\n This subrun took " << tEnd - tBegin << " seconds \n" << endl;

  // End of loop over two runs.
  }

  // Form <pT>(n_charged) ratios.
  avgPTnChOld /= nChargedOld;
  avgPTnChNew /= nChargedNew;

  // Take ratios of new to old distributions.
  nChargedRat  = nChargedNew / nChargedOld;
  ySpecRat     = ySpecNew    / ySpecOld;
  pTSpecRat    = pTSpecNew    / pTSpecOld;
  avgPTnChRat  = avgPTnChNew / avgPTnChOld;
  xDistRat     = xDistNew    / xDistOld;
  pTDistRat    = pTDistNew   / pTDistOld;

  // Print histograms.
  cout << nChargedOld << nChargedNew << nChargedRat
       << ySpecOld    << ySpecNew    << ySpecRat
       << pTSpecOld   << pTSpecNew   << pTSpecRat
       << avgPTnChOld << avgPTnChNew << avgPTnChRat 
       << xDistOld    << xDistNew    << xDistRat 
       << pTDistOld   << pTDistNew   << pTDistRat;

  // Second part of study, as simple extra check:
  // Begin fill shape of effective PDF at typical MI Q2 = 10 scale:
  // F_effective(x) = (9/4) x*g(x) + Sum_i (x*q_i(x) + x*qbar_i(x)).
  double Q2 = 10.;
  PDF* oldPDF = new CTEQ5L(2212);
  PDF* newPDF = new LHAPDF(2212, pdfSet, 0);

  // Histograms.  
  Hist effFlinOld("F_effective( x, Q2 = 10) old", 100 , 0., 1.);
  Hist effFlinNew("F_effective( x, Q2 = 10) new", 100 , 0., 1.);
  Hist effFlinRat("F_effective( x, Q2 = 10) new/old", 100 , 0., 1.);
  Hist effFlogOld("F_effective( x, Q2 = 10) old", 80 , -8., 0.);
  Hist effFlogNew("F_effective( x, Q2 = 10) new", 80 , -8., 0.);
  Hist effFlogRat("F_effective( x, Q2 = 10) new/old", 80 , -8., 0.);
    
  // Loop over x values, in a linear scale.
  for (int iX = 0; iX < 99; ++iX) {
    double x = 0.005 + 0.01 * iX;

    // Evaluate old summed PDF.
    double oldSum = 2.25 * oldPDF->xf( 21, x, Q2);   
    for (int i = 1; i < 6; ++i) 
      oldSum += oldPDF->xf( i, x, Q2) + oldPDF->xf( -i, x, Q2);  
    effFlinOld.fill ( x, oldSum ); 

    // Evaluate new summed PDF.
    double newSum = 2.25 * newPDF->xf( 21, x, Q2);   
    for (int i = 1; i < 6; ++i) 
      newSum += newPDF->xf( i, x, Q2) + newPDF->xf( -i, x, Q2);  
    effFlinNew.fill ( x, newSum ); 

  //End loop over x values, in a linear scale.
  }
    
  // Loop over x values, in a logarithmic scale
  for (int iX = 0; iX < 80; ++iX) {
    double xLog = -(0.1 * iX + 0.05);
    double x = pow( 10., xLog); 

    // Evaluate old summed PDF.
    double oldSum = 2.25 * oldPDF->xf( 21, x, Q2);   
    for (int i = 1; i < 6; ++i) 
      oldSum += oldPDF->xf( i, x, Q2) + oldPDF->xf( -i, x, Q2);  
    effFlogOld.fill ( xLog, oldSum ); 

    // Evaluate new summed PDF.
    double newSum = 2.25 * newPDF->xf( 21, x, Q2);   
    for (int i = 1; i < 6; ++i) 
      newSum += newPDF->xf( i, x, Q2) + newPDF->xf( -i, x, Q2);  
    effFlogNew.fill ( xLog, newSum ); 

  //End loop over x values, in a logarithmic scale.
  }

  // Take ratios of new to old distributions.
  effFlinRat   = effFlinNew  / effFlinOld;
  effFlogRat   = effFlogNew  / effFlogOld;

  // Print histograms.
  cout << effFlinOld  << effFlinNew  << effFlinRat 
       << effFlogOld  << effFlogNew  << effFlogRat;

  // Done.
  return 0;
}
