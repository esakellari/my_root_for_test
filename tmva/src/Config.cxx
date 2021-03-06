// @(#)root/tmva $Id$   
// Author: Andreas Hoecker, Joerg Stelzer, Fredrik Tegenfeldt, Helge Voss

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : Config                                                                *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      Implementation                                                            *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Andreas Hoecker    <Andreas.Hocker@cern.ch>     - CERN, Switzerland       *
 *      Joerg Stelzer      <Joerg.Stelzer@cern.ch>      - CERN, Switzerland       *
 *      Fredrik Tegenfeldt <Fredrik.Tegenfeldt@cern.ch> - Iowa State U., USA      *
 *      Helge Voss         <Helge.Voss@cern.ch>         - MPI-K Heidelberg, GER   *
 *                                                                                *
 * Copyright (c) 2006:                                                            *
 *      CERN, Switzerland                                                         *
 *      Iowa State U., USA                                                        *
 *      MPI-K Heidelberg, Germany                                                 *
 *                                                                                *
 * Redistribution and use in source and binary forms, with or without             *
 * modification, are permitted according to the terms listed in LICENSE           *
 * (http://mva.sourceforge.net/license.txt)                                       *
 **********************************************************************************/

#include "TMVA/Config.h"
#include "TMVA/MsgLogger.h"

ClassImp(TMVA::Config)

#if __cplusplus > 199711L
std::atomic<TMVA::Config*> TMVA::Config::fgConfigPtr{ 0 };
#else
TMVA::Config* TMVA::Config::fgConfigPtr = 0;
#endif

TMVA::Config& TMVA::gConfig() { return TMVA::Config::Instance(); }

//_______________________________________________________________________
TMVA::Config::Config() :
   fUseColoredConsole    ( kTRUE  ),
   fSilent               ( kFALSE ),
   fWriteOptionsReference( kFALSE ),
   fDrawProgressBar      ( kTRUE ),
   fLogger               ( new MsgLogger("Config") )
{
   // constructor - set defaults
   
   // plotting
   fVariablePlotting.fTimesRMS = 8.0;
   fVariablePlotting.fNbins1D  = 40;
   fVariablePlotting.fNbins2D  = 300;
   fVariablePlotting.fMaxNumOfAllowedVariablesForScatterPlots = 20;
   
   fVariablePlotting.fNbinsMVAoutput   = 40;
   fVariablePlotting.fNbinsXOfROCCurve = 100;

   // IO names
   fIONames.fWeightFileDir           = "weights";
   fIONames.fWeightFileExtension     = "weights";
   fIONames.fOptionsReferenceFileDir = "optionInfo";
}

//_______________________________________________________________________
TMVA::Config::~Config()
{
   // destructor
   delete fLogger;
}

//_______________________________________________________________________
void TMVA::Config::DestroyInstance()
{
   // static function: destroy TMVA instance
#if __cplusplus > 199711L
  delete fgConfigPtr.exchange(0);
#else
   if (fgConfigPtr != 0) { delete fgConfigPtr; fgConfigPtr = 0;}
#endif
}

//_______________________________________________________________________
TMVA::Config& TMVA::Config::Instance()
{
   // static function: returns  TMVA instance
#if __cplusplus > 199711L
  if(!fgConfigPtr) {
    TMVA::Config* tmp = new Config();
    TMVA::Config* expected = 0;
    if(! fgConfigPtr.compare_exchange_strong(expected,tmp) ) {
      //another thread beat us to the switch
      delete tmp;
    }
  }
  return *fgConfigPtr;
#else
   return fgConfigPtr ? *fgConfigPtr :*(fgConfigPtr = new Config());
#endif
}

