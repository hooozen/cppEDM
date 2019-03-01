//file to test the smap algo against pyedm

#include "TestCommon.h"

//smap params for reference
/*
   DataFrame< double > data,
   std::string pathOut,
   std::string predictFile,
   std::string lib,
   std::string pred,
   int         E,
   int         Tp,
   int         knn,
   int         tau,
   double      theta,
   std::string columns,
   std::string target,
   std::string smapFile,
   std::string jacobians,
   bool        embedded,
   bool        verbose
   */

int main () {

    //---------------------------------------------------------
    // test smap with simple circle test
    //  - already embedded 
    //---------------------------------------------------------
    // Load pyEdm output from:
    //   ./Predict.py -i circle.csv -r x -c x y -e -m smap -t 4
    //   -l 1 100 -p 101 198 -T 1 -P -o Smap_circle_pyEDM.csv
    //---------------------------------------------------------
    DataFrame < double > pyOutput( "./data/", "Smap_circle_pyEDM.csv" );
    
    //generate cpp output
    DataFrame < double > circleData( "../data/", "circle.csv" );
    
    SMapValues smapVals = SMap ( circleData,
                                 "./data/", "Smap_circle_cppEDM.csv",
                                 " 1 100 ", "101 198", 2, 1, 0, 1, 4,
                                 "x y", "x", "", "", true, true );
    
    DataFrame < double > cppOutput = smapVals.predictions;
    
    //run comparison
    MakeTest ( "circle.csv test", pyOutput, cppOutput );


    //---------------------------------------------------------
    // Load pyEdm output from:
    //   ./Predict.py -i block_3sp.csv -r x_t -c x_t y_t z_t -e -m smap -t 2
    //   -l 1 99 -p 100 198 -T 1 -P -o Smap_embd_block_3sp_pyEDM.csv
    //---------------------------------------------------------
    pyOutput = DataFrame <double> ("./data/", "Smap_embd_block_3sp_pyEDM.csv");

    smapVals = SMap ( "../data/", "block_3sp.csv",
                      "./data/", "Smap_embd_block_3sp_cppEDM.csv",
                      " 1 99 ", "100 198", 3, 1, 0, 1, 2,
                      "x_t y_t z_t", "x_t", "", "", true, true );

    cppOutput = smapVals.predictions;
    
    //run comparison
    MakeTest ( "block_3sp test", pyOutput, cppOutput );    
}
