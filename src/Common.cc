
#include <algorithm>

#include "Common.h"
#include "AuxFunc.h"

//----------------------------------------------------------------
// 
//----------------------------------------------------------------
std::string ToLower( std::string str ) {

    std::string lowerStr( str );
    std::transform( lowerStr.begin(), lowerStr.end(),
                    lowerStr.begin(), ::tolower );
    
    return lowerStr;
}

//----------------------------------------------------------------
// true : str has only digits   false : str has non-digits
//----------------------------------------------------------------
bool OnlyDigits( std::string str ) {

    if ( not str.size() ) {
        throw std::runtime_error( "OnlyDigits(): String is empty.\n" );
    }

    bool onlyDigits = true; 
    std::string str_( str );
    
    // remove whitespace
    str_.erase( std::remove_if( str_.begin(), str_.end(), ::isspace ),
                str_.end() );

    const char *cstr = str_.c_str(); // isdigit() is a C function...
    
    for ( size_t i = 0; i < str_.size(); i++ ) {
        if ( not isdigit( cstr[i] ) ) { onlyDigits = false; break; }
    }

    return onlyDigits;
}

//----------------------------------------------------------------
// SplitString
//
// Purpose: like Python string.split()
//
// Arguments: inString    : string to be split
//            delimeters  : string of delimeters
//
// Note:  A typical delimeter string: delimeters = " \t,\n:;()"
//           
// Return: vector of tokens
//----------------------------------------------------------------
std::vector<std::string> SplitString( std::string inString, 
                                      std::string delimeters ) {

  size_t pos       = 0;
  size_t eos       = 0;
  size_t wordStart = 0;
  size_t wordEnd   = 0;

  bool foundStart = false;
  bool foundEnd   = false;

  std::vector<std::string> splitString;
  
  std::string word;

  eos = inString.length();

  while ( pos <= eos ) {
    if ( not foundStart ) {
      if ( delimeters.find( inString[pos] ) == delimeters.npos ) {
	// this char (inString[pos]) is not a delimeter
	wordStart  = pos;
	foundStart = true;
	pos++;
	continue;
      }
    }
    if ( foundStart and not foundEnd ) {
      if ( delimeters.find( inString[pos] ) != delimeters.npos 
	   or pos == eos ) {
	// this char (inString[pos]) is a delimeter or
	// at the end of the string
	wordEnd  = pos;
	foundEnd = true;
      }
    }
    if ( foundStart and foundEnd ) {
      foundStart = false;
      foundEnd   = false;
      
      word = inString.substr( wordStart, wordEnd - wordStart );
      
      // remove whitespace
      word.erase( std::remove_if( word.begin(), word.end(), ::isspace ),
                  word.end() );
      
      splitString.push_back( word );
    }
    if ( pos == eos ) {
      break;
    }
    pos++;
  }

  return splitString;
}

//----------------------------------------------------------------
// 
//----------------------------------------------------------------
VectorError ComputeError( std::valarray< double > obsIn,
                          std::valarray< double > predIn ) {

#ifdef DEBUG_ALL
    std::cout << "ComputeError(): obs(" << obsIn.size() << "): ";
    for ( auto o : obsIn ) { std::cout << o << " "; }
    std::cout << std::endl;
    std::cout << "ComputeError(): pred(" << predIn.size() << "): ";
    for ( auto p : predIn ) { std::cout << p << " "; }
    std::cout << std::endl;
#endif

    // Check for nan in vectors
    size_t nanObs  = 0;
    size_t nanPred = 0;
    for ( auto o : obsIn )  { if ( std::isnan( o ) ) { nanObs++;  } }
    for ( auto p : predIn ) { if ( std::isnan( p ) ) { nanPred++; } }

    if ( nanObs != nanPred ) {
        std::stringstream errMsg;
        errMsg << "ComputeError(): obs has " << nanObs << " nan, pred has "
               << nanPred << " nan.  ComputeError result invalid.\n";
        std::cout << errMsg.str() << std::flush;
    }

    // JP: Assume that nan are at the beginning of predictions and
    //     at the end of observations... probably not a robust assumption
    std::valarray<double> pred( predIn.size() - 2 * nanPred );
    std::valarray<double> obs ( predIn.size() - 2 * nanPred );
    if ( nanPred > 0 ) {
        // ignore nanPred initial pred, and nanObs end obs
        pred = predIn[ std::slice( nanPred, pred.size() - nanPred - 1, 1 ) ];
        obs  = obsIn [ std::slice( nanPred, pred.size() - nanPred - 1, 1 ) ];
    }
    else {
        pred = std::valarray<double>( predIn );
        obs  = std::valarray<double>( obsIn );
    }

#ifdef DEBUG_ALL
    std::cout << "ComputeError(): obs(" << obs.size() << "): ";
    for ( auto o : obs ) { std::cout << o << " "; }
    std::cout << std::endl;
    std::cout << "ComputeError(): pred(" << pred.size() << "): ";
    for ( auto p : pred ) { std::cout << p << " "; }
    std::cout << std::endl;
#endif

    size_t N = pred.size();
        
    std::valarray< double > two( 2, N );

    double sumPred    = pred.sum();
    double sumObs     = obs.sum();
    double meanPred   = sumPred / N;
    double meanObs    = sumObs  / N;
    double sumSqrPred = pow( pred, two ).sum();
    double sumSqrObs  = pow( obs,  two ).sum();
    double sumErr     = abs( obs - pred ).sum();
    double sumSqrErr  = pow( obs - pred, two ).sum();
    double sumProd    = ( obs * pred ).sum();
    double rho;

    if ( sumSqrPred * N == sumSqrPred ) {
        rho = 0;
    }
    else {
        rho = ( sumProd - N * meanObs * meanPred ) /
            ( std::sqrt( ( sumSqrObs  - N * pow( meanObs,  2 ) ) ) *
              std::sqrt( ( sumSqrPred - N * pow( meanPred, 2 ) ) ) );
    }

    VectorError vectorError = VectorError();
    
    vectorError.RMSE = sqrt( sumSqrErr / N );

    vectorError.MAE = sumErr / N;

    vectorError.rho = rho;
    
    return vectorError;
}
