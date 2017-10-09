// LHEF3.cc is a part of the PYTHIA event generator.
// Copyright (C) 2014 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL version 2, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// This file is written by Stefan Prestel.
// It contains the main class for LHEF 3.0 functionalities.
// Function definitions.

#include "Pythia8/LHEF3.h"

namespace Pythia8 {

//==========================================================================

// The XMLTag struct is used to represent all information within an XML tag. 
// It contains the attributes as a map, any sub-tags as a vector of pointers 
// to other XMLTag objects, and any other information as a single string.

//==========================================================================

// The LHAweights struct.

//--------------------------------------------------------------------------

// Construct from XML tag.

LHAweights::LHAweights(const XMLTag & tag) {
  for ( map<string,string>::const_iterator it = tag.attr.begin();
    it != tag.attr.end(); ++it ) {
    string v = it->second.c_str();
    attributes[it->first] = v;
  }

  contents = tag.contents;

  istringstream iss(tag.contents);
  double w;
  while ( iss >> w ) weights.push_back(w);
}

//--------------------------------------------------------------------------

// Print out.

void LHAweights::print(ostream & file) const {
  file << "<weights";
  for ( map<string,string>::const_iterator it = attributes.begin();
        it != attributes.end(); ++it )
    file << " " << it->first << "=\"" << it->second << "\"";
  file << " >";
  for ( int j = 0, M = weights.size(); j < M; ++j ) file << " " << weights[j];
  file << "</weights>" << endl;
}

//==========================================================================

// The LHAscales struct: Collect different scales relevant for an event.

//--------------------------------------------------------------------------

// Construct from an XML-tag.

LHAscales::LHAscales(const XMLTag & tag, double defscale)
  : muf(defscale), mur(defscale), mups(defscale), SCALUP(defscale) {
  for ( map<string,string>::const_iterator it = tag.attr.begin();
        it != tag.attr.end(); ++it ) {
    double v = atof(it->second.c_str());
    if ( it->first == "muf" ) muf = v;
    else if ( it->first == "mur" ) mur = v;
    else if ( it->first == "mups" ) mups = v;
    else attributes[it->first] = v;
  }
  contents = tag.contents;
}

//--------------------------------------------------------------------------

// Print out the corresponding XML-tag.

void LHAscales::print(ostream & file) const {
  file << "<scales";
  file << " muf=\"" << muf << "\"";
  file << " mur=\"" << mur << "\"";
  file << " mups=\"" << mups << "\"";
  for ( map<string,double>::const_iterator it = attributes.begin();
        it != attributes.end(); ++it )
    file << " " << it->first << "=\"" << it->second << "\"";
  file << " />" << endl;
}

//==========================================================================

// The LHAgenerator struct: Collect generator information for an event file.

//--------------------------------------------------------------------------

// Construct from an XML-tag

LHAgenerator::LHAgenerator(const XMLTag & tag, string defname)
  : name(defname), version(defname), contents(defname) {
  for ( map<string,string>::const_iterator it = tag.attr.begin();
        it != tag.attr.end(); ++it ) {
    string v = it->second.c_str();
    if ( it->first == "name" ) name = v;
    else if ( it->first == "version" ) version = v;
    else attributes[it->first] = v;
  }
  contents = tag.contents;
}

//--------------------------------------------------------------------------

// Print out the corresponding XML-tag.

void LHAgenerator::print(ostream & file) const {
  file << "<generator";
  if ( name    != "" ) file << " name=\""    << name    << "\"";
  if ( version != "" ) file << " version=\"" << version << "\"";
  for ( map<string,string>::const_iterator it = attributes.begin();
        it != attributes.end(); ++it )
    file << " " << it->first << "=\"" << it->second << "\"";
  file << " >";
  file << contents;
  file << "<generator" << endl;
}

//==========================================================================

// The LHAwgt struct: Collect the wgt information.

//--------------------------------------------------------------------------

// Construct from an XML-tag

LHAwgt::LHAwgt(const XMLTag & tag, double defwgt)
  : id(""), contents(defwgt) {
  for ( map<string,string>::const_iterator it = tag.attr.begin();
        it != tag.attr.end(); ++it ) {
    string v = it->second;
    if ( it->first == "id" ) id = v;
    else attributes[it->first] = v;
  }
  contents = atof(tag.contents.c_str());
}

//--------------------------------------------------------------------------

// Print out the corresponding XML-tag.

void LHAwgt::print(ostream & file) const {
  file << "<wgt";
  if ( id    != "" ) file << " id=\""    << id << "\"";
  for ( map<string,string>::const_iterator it = attributes.begin();
        it != attributes.end(); ++it )
    file << " " << it->first << "=\"" << it->second << "\"";
  file << " >";
  file << contents;
  file << "</wgt>" << endl;
}

//==========================================================================

// The LHAweight struct: Collect the weight information.

//--------------------------------------------------------------------------

// Construct from an XML-tag.

LHAweight::LHAweight(const XMLTag & tag, string defname)
  : id(defname), contents(defname) {
  for ( map<string,string>::const_iterator it = tag.attr.begin();
        it != tag.attr.end(); ++it ) {
    string v = it->second;
    if ( it->first == "id" ) id = v;
    else attributes[it->first] = v;
  }
  contents = tag.contents;
}

//--------------------------------------------------------------------------

// Print out the corresponding XML-tag.

void LHAweight::print(ostream & file) const {
  file << "<weight";
  if ( id  != "" ) file << " id=\""    << id << "\"";
  for ( map<string,string>::const_iterator it = attributes.begin();
        it != attributes.end(); ++it )
    file << " " << it->first << "=\"" << it->second << "\"";
  file << " >";
  file << contents;
  file << "</weight>" << endl;
}

//==========================================================================

// The LHAweightgroup struct: The LHAweightgroup assigns a group-name to a set
// of LHAweight objects.

//--------------------------------------------------------------------------

// Construct a group of LHAweight objects from an XML tag and
// insert them in the given vector.

LHAweightgroup::LHAweightgroup(const XMLTag & tag) {

  for ( map<string,string>::const_iterator it = tag.attr.begin();
        it != tag.attr.end(); ++it ) {
    string v = it->second.c_str();
    if ( it->first == "name" ) name = v;
    else attributes[it->first] = v;
  }

  contents = tag.contents;
    
  // Now add the weight's step by step.
  string s;
  vector<XMLTag*> tags = XMLTag::findXMLTags(tag.contents, &s);
  for ( int i = 0, N = tags.size(); i < N; ++i ) {
    const XMLTag & tagnow = *tags[i];
    LHAweight wt(tagnow);
    weights.insert(make_pair(wt.id, wt));
  }
  for ( int i = 0, N = tag.tags.size(); i < N; ++i ) {
    const XMLTag & tagnow = *tag.tags[i];
    const LHAweight & wt(tagnow);
    weights.insert(make_pair(wt.id, wt));
  }

}

//--------------------------------------------------------------------------

// Print out the corresponding XML-tag.

void LHAweightgroup::printweights(ostream & file) const {
  for ( map<string,LHAweight>::const_iterator it = weights.begin();
        it != weights.end(); ++it ) it->second.print(file);
}

//==========================================================================

// The LHArwgt struct: Assigns a group-name to a set of LHAwgt objects.

//--------------------------------------------------------------------------

// Construct a group of LHAwgt objects from an XML tag and
// insert them in the given vector.

LHArwgt::LHArwgt(const XMLTag & tag) {

  for ( map<string,string>::const_iterator it = tag.attr.begin();
        it != tag.attr.end(); ++it ) {
    string v = it->second.c_str();
    attributes[it->first] = v;
  }
  contents = tag.contents;

  // Now add the wgt's step by step.
  string s;
  vector<XMLTag*> tags = XMLTag::findXMLTags(tag.contents, &s);
  for ( int i = 0, N = tags.size(); i < N; ++i ) {
    const XMLTag & tagnow = *tags[i];
    LHAwgt wt(tagnow);
    wgts.insert(make_pair(wt.id, wt));
  }
  for ( int i = 0, N = tag.tags.size(); i < N; ++i ) {
    const XMLTag & tagnow = *tag.tags[i];
    LHAwgt wt(tagnow);
    wgts.insert(make_pair(wt.id, wt));
  }
}

//==========================================================================

// The LHAinitrwgt assigns a group-name to a set of LHAweightgroup objects.

//--------------------------------------------------------------------------

// Construct a group of LHAweightgroup objects from an XML tag and
// insert them in the given vector.

LHAinitrwgt::LHAinitrwgt(const XMLTag & tag) {
  for ( map<string,string>::const_iterator it = tag.attr.begin();
        it != tag.attr.end(); ++it ) {
    string v = it->second.c_str();
    attributes[it->first] = v;
  }
  contents = tag.contents;

  // Now add the wgt's step by step.
  string s;
  vector<XMLTag*> tags = XMLTag::findXMLTags(tag.contents, &s);
  for ( int i = 0, N = tags.size(); i < N; ++i ) {
    const XMLTag & tagnow = *tags[i];
    if ( tagnow.name == "weightgroup" ) {
      LHAweightgroup wgroup(tagnow);
      string wgname = wgroup.name;
      weightgroups.insert(make_pair(wgname, wgroup));
      string ss;
      vector<XMLTag*> tags2 = XMLTag::findXMLTags(tagnow.contents, &ss);
      for ( int k = 0, M = tags2.size(); k < M; ++k ) {
        const XMLTag & tagnow2 = *tags2[k];
        if ( tagnow2.name == "weight" ) {
          LHAweight wt(tagnow2);
          string wtname = wt.id;
          weights.insert(make_pair(wtname, wt));
        }
      }
    } else if ( tagnow.name == "weight" ) {
      LHAweight wt(tagnow);
      string wtname = wt.id;
      weights.insert(make_pair(wtname, wt));
    }
  }

  // Now add the wgt's step by step.
  for ( int i = 0, N = tag.tags.size(); i < N; ++i ) {
    const XMLTag & tagnow = *tag.tags[i];
    if ( tagnow.name == "weightgroup" ) {
      LHAweightgroup wgroup(tagnow);
      string wgname = wgroup.name;
      weightgroups.insert(make_pair(wgname, wgroup));
      string ss;
      vector<XMLTag*> tags2 = XMLTag::findXMLTags(tagnow.contents, &ss);
      for ( int k = 0, M = tags2.size(); k < M; ++k ) {
        const XMLTag & tagnow2 = *tags2[k];
        if ( tagnow2.name == "weight" ) {
          LHAweight wt(tagnow2);
          string wtname = wt.id;
          weights.insert(make_pair(wtname, wt));
        }
      }
      for ( int k = 0, M = tagnow.tags.size(); k < M; ++k ) {
        const XMLTag & tagnow2 = *tagnow.tags[k];
        if ( tagnow2.name == "weight" ) {
          LHAweight wt(tagnow2);
          string wtname = wt.id;
          weights.insert(make_pair(wtname, wt));
        }
      }
    } else if ( tagnow.name == "weight" ) {
      LHAweight wt(tagnow);
      string wtname = wt.id;
      weights.insert(make_pair(wtname, wt));
    }
  }
}

//==========================================================================

// The HEPEUP class is a simple container corresponding to the Les Houches
// accord (<A HREF="http://arxiv.org/abs/hep-ph/0109068">hep-ph/0109068</A>)
// common block with the same name. The members are named in the same
// way as in the common block. However, fortran arrays are represented
// by vectors, except for the arrays of length two which are
// represented by pair objects.

//--------------------------------------------------------------------------

// Copy information from the given HEPEUP.

HEPEUP & HEPEUP::setEvent(const HEPEUP & x) {

  NUP = x.NUP;
  IDPRUP = x.IDPRUP;
  XWGTUP = x.XWGTUP;
  XPDWUP = x.XPDWUP;
  SCALUP = x.SCALUP;
  AQEDUP = x.AQEDUP;
  AQCDUP = x.AQCDUP;
  IDUP = x.IDUP;
  ISTUP = x.ISTUP;
  MOTHUP = x.MOTHUP;
  ICOLUP = x.ICOLUP;
  PUP = x.PUP;
  VTIMUP = x.VTIMUP;
  SPINUP = x.SPINUP;
  heprup = x.heprup;
  scales = x.scales;
  weights = x.weights;
  weights_detailed = x.weights_detailed;
  weights_compressed = x.weights_compressed;
  rwgt = x.rwgt;
  attributes = x.attributes;
  return *this;

}

//--------------------------------------------------------------------------

// Reset the HEPEUP object.

void HEPEUP::reset() {
  NUP = 0;
  weights_detailed.clear();
  weights_compressed.clear();
  weights.clear();
  rwgt.clear();
  scales.clear();
  attributes.clear();
}

//--------------------------------------------------------------------------

// Assuming the NUP variable, corresponding to the number of
// particles in the current event, is correctly set, resize the
// relevant vectors accordingly.

void HEPEUP::resize() {
  IDUP.resize(NUP);
  ISTUP.resize(NUP);
  MOTHUP.resize(NUP);
  ICOLUP.resize(NUP);
  PUP.resize(NUP, vector<double>(5));
  VTIMUP.resize(NUP);
  SPINUP.resize(NUP);
}

//==========================================================================

// The Reader class is initialized with a stream from which to read a
// version 1/2 Les Houches Accord event file. In the constructor of
// the Reader object the optional header information is read and then
// the mandatory init is read. After this the whole header block
// including the enclosing lines with tags are available in the public
// headerBlock member variable. Also the information from the init
// block is available in the heprup member variable and any additional
// comment lines are available in initComments. After each successful
// call to the readEvent() function the standard Les Houches Accord
// information about the event is available in the hepeup member
// variable and any additional comments in the eventComments
// variable. A typical reading sequence would look as follows:

//--------------------------------------------------------------------------

// Used internally in the constructors to read header and init blocks.
bool Reader::init() {

  bool readingHeader = false;
  bool readingInit = false;

  // Make sure we are reading a LHEF file:
  getLine();

  if ( currentLine.find("<LesHouchesEvents" ) == string::npos )
    return false;
  version = 0;
  if ( currentLine.find("version=\"1" ) != string::npos )
    version = 1;
  else if ( currentLine.find("version=\"2" ) != string::npos )
    version = 2;
  else if ( currentLine.find("version=\"3" ) != string::npos )
    version = 3;
  else
    return false;

  // Loop over all lines until we hit the </init> tag.
  while ( getLine() && currentLine.find("</init>") == string::npos ) {
    if ( currentLine.find("<header") != string::npos ) {
      // We have hit the header block, so we should dump this and
      // all following lines to headerBlock until we hit the end of
      // it.
      readingHeader = true;
      headerBlock = currentLine + "\n";
    }
    else if ( currentLine.find("<init>") != string::npos ) {
      // We have hit the init block, so we should expect to find the
      // standard information in the following.
      readingInit = true;

      // The first line tells us how many lines to read next.
      getLine();
      istringstream iss(currentLine);
      if ( !( iss >> heprup.IDBMUP.first >> heprup.IDBMUP.second
                  >> heprup.EBMUP.first >> heprup.EBMUP.second
                  >> heprup.PDFGUP.first >> heprup.PDFGUP.second
                  >> heprup.PDFSUP.first >> heprup.PDFSUP.second
                  >> heprup.IDWTUP >> heprup.NPRUP ) ) {
        heprup.NPRUP = -42;
        return false;
      }
      heprup.resize();

      for ( int i = 0; i < heprup.NPRUP; ++i ) {
        getLine();
        istringstream isss(currentLine);
        if ( !( isss >> heprup.XSECUP[i] >> heprup.XERRUP[i]
                    >> heprup.XMAXUP[i] >> heprup.LPRUP[i] ) ) {
          heprup.NPRUP = -42;
          return false;
        }
      }
    }
    else if ( currentLine.find("</header>") != string::npos ) {
      // The end of the header block. Dump this line as well to the
      // headerBlock and we're done.
      readingHeader = false;
      headerBlock += currentLine + "\n";
    }
    else if ( readingHeader ) {
      // We are in the process of reading the header block. Dump the
      // line to haderBlock.
      headerBlock += currentLine + "\n";
      headerComments += currentLine + "\n";
    }
    else if ( readingInit ) {
      // Here we found a comment line. Dump it to initComments.
      initComments += currentLine + "\n";
    }
    else {
      // We found some other stuff outside the standard tags.
      outsideBlock += currentLine + "\n";
    }
  }

  if ( !file ) heprup.NPRUP = -42;

  // Scan the header block for XML tags
  string leftovers;
  vector<XMLTag*> tags1 = XMLTag::findXMLTags(headerComments, &leftovers);
  if ( leftovers.find_first_not_of(" \t\n") == string::npos )
    leftovers="";

  for ( int i = 0, N = tags1.size(); i < N; ++i ) {
    const XMLTag & tag = *tags1[i];

    if ( tag.name == "initrwgt" ) {
      LHAinitrwgt irwgt(tag);
      heprup.initrwgt = irwgt;
      for ( int j = 0, M = tag.tags.size(); j < M; ++j ) {
        XMLTag & ctag = *tag.tags[j];
        if ( ctag.name == "weightgroup" ) {
          LHAweightgroup wgroup(ctag);
          string wgname = wgroup.name;
          heprup.weightgroups.insert(make_pair(wgname, wgroup));

          string ss;
          vector<XMLTag*> tags2 = XMLTag::findXMLTags(ctag.contents, &ss);
          for ( int k = 0, O = tags2.size(); k < O; ++k ) {
            const XMLTag & tagnow2 = *tags2[k];
            if ( tagnow2.name == "weight" ) {
              LHAweight wt(tagnow2);
              string wtname = wt.id;
              heprup.weights.insert(make_pair(wtname, wt));
            }
          }
          for ( int k = 0, O = ctag.tags.size(); k < O; ++k ) {
            const XMLTag & tagnow2 = *ctag.tags[k];
            if ( tagnow2.name == "weight" ) {
              LHAweight wt(tagnow2);
              string wtname = wt.id;
              heprup.weights.insert(make_pair(wtname, wt));
            }
          }
        } else if ( ctag.name == "weight" ) {
          string tname = ctag.attr["id"];
          heprup.weights.insert(make_pair(tname, LHAweight(ctag)));
        }
      }
    }
    delete &tag;
  }

  heprup.generators.clear();
  // Scan the init block for XML tags
  leftovers="";
  vector<XMLTag*> tags2 = XMLTag::findXMLTags(initComments, &leftovers);
  if ( leftovers.find_first_not_of(" \t\n") == string::npos )
    leftovers="";

  for ( int i = 0, N = tags2.size(); i < N; ++i ) {
    const XMLTag & tag = *tags2[i];
    if ( tag.name == "generator" ) {
      heprup.generators.push_back(LHAgenerator(tag));
    }
    delete &tag;
  }

  // Done
  return true;

}

//--------------------------------------------------------------------------

// Read an event from the file and store it in the hepeup
// object. Optional comment lines are stored in the eventComments
// member variable. return true if the read was successful.

bool Reader::readEvent(HEPEUP * peup) {

  HEPEUP & eup = (peup? *peup: hepeup);
  eup.clear();
  eup.heprup = &heprup;

  // Check if the initialization was successful. Otherwise we will
  // not read any events.
  if ( heprup.NPRUP < 0 ) return false;
  eventComments = "";
  outsideBlock = "";
  eup.NUP = 0;

  // Keep reading lines until we hit the next event or the end of
  // the event block. Save any inbetween lines. Exit if we didn't
  // find an event.
  while ( getLine() && currentLine.find("<event") == string::npos )
    outsideBlock += currentLine + "\n";

  // Get event attributes.
  if (currentLine != "") {
    string eventLine(currentLine);
    eventLine += "</event>";
    vector<XMLTag*> evtags = XMLTag::findXMLTags(eventLine);
    XMLTag & evtag = *evtags[0];
    for ( map<string,string>::const_iterator it = evtag.attr.begin();
          it != evtag.attr.end(); ++it ) {
      string v = it->second.c_str();
      eup.attributes[it->first] = v;
    }
  }

  if ( !getLine()  ) return false;

  // We found an event. The first line determines how many
  // subsequent particle lines we have.
  istringstream iss(currentLine);
  if ( !( iss >> eup.NUP >> eup.IDPRUP >> eup.XWGTUP
              >> eup.SCALUP >> eup.AQEDUP >> eup.AQCDUP ) )
    return false;
  eup.resize();

  // Read all particle lines.
  for ( int i = 0; i < eup.NUP; ++i ) {
    if ( !getLine() ) return false;
    istringstream isss(currentLine);
    if ( !( isss >> eup.IDUP[i] >> eup.ISTUP[i]
                >> eup.MOTHUP[i].first >> eup.MOTHUP[i].second
                >> eup.ICOLUP[i].first >> eup.ICOLUP[i].second
                >> eup.PUP[i][0] >> eup.PUP[i][1] >> eup.PUP[i][2]
                >> eup.PUP[i][3] >> eup.PUP[i][4]
                >> eup.VTIMUP[i] >> eup.SPINUP[i] ) )
      return false;
  }

  // Now read any additional comments.
  while ( getLine() && currentLine.find("</event>") == string::npos )
    eventComments += currentLine + "\n";

  if ( !file ) return false;

  eup.scales = LHAscales(eup.SCALUP);

  // Scan the init block for XML tags
  string leftovers;
  vector<XMLTag*> tags = XMLTag::findXMLTags(eventComments, &leftovers);
  if ( leftovers.find_first_not_of(" \t\n") == string::npos )
    leftovers="";
  eventComments = leftovers;

  for ( int i = 0, N = tags.size(); i < N; ++i ) {
    XMLTag & tag = *tags[i];

    if ( tag.name == "weights" ) {
      LHAweights wts(tag);
      eup.weights = wts;

      for ( int k = 0, M = int(wts.weights.size()); k < M; ++k ) {
        eup.weights_compressed.push_back(wts.weights[k]);
      }

    }
    else if ( tag.name == "scales" ) {
      eup.scales = LHAscales(tag, eup.SCALUP);
    }
    else if ( tag.name == "rwgt" ) {
      LHArwgt rwgt(tag);
      eup.rwgt = rwgt;
      string s;
      vector<XMLTag*> tags2 = XMLTag::findXMLTags(rwgt.contents, &s);
      for ( int k = 0, M = tags2.size(); k < M; ++k ) {
        const XMLTag & tagnow = *tags2[k];
        if ( tagnow.name == "wgt" ) {
          LHAwgt wt(tagnow);
          eup.weights_detailed.insert(make_pair(wt.id, wt.contents));
        }
      }
      for ( int k = 0, M = tag.tags.size(); k < M; ++k ) {
        const XMLTag & tagnow = *tag.tags[k];
        if ( tagnow.name == "wgt" ) {
          LHAwgt wt(tagnow);
          eup.weights_detailed.insert(make_pair(wt.id, wt.contents));
        }
      }
    }
    delete &tag;
  }

  return true;
    
}

//==========================================================================

}