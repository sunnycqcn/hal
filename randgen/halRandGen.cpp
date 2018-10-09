/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 *
 * Released under the MIT license, see LICENSE.txt
 */
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <ctime>

#include <halCLParser.h>
#include "hal.h"
#include "halRandomData.h"


using namespace std;
using namespace hal;
using namespace H5;

struct Options {
   double _meanDegree;
   double _maxBranchLength;
   hal_size_t _maxGenomes;
   hal_size_t _minSegmentLength;
   hal_size_t _maxSegmentLength;
   hal_size_t _minSegments;
   hal_size_t _maxSegments;
   int _seed;
   string _halFile;
};

static const Options defaultSm = {0.75, 0.1, 5, 10, 1000, 5, 10, 2000000, ""};
static const Options defaultMed = {1.25,  0.7, 20, 2, 50, 1000, 50000, 2000000, ""};
static const Options defaultBig = {2,  0.7, 50, 2, 500, 100, 5000, 2000000, ""};
static const Options defaultLrg = {2, 1, 100, 2, 10, 10000, 500000, 2000000, ""};

static CLParserPtr initParser() {
    CLParserPtr parser = halCLParserInstance(true);
    parser->setDescription("Generate a random HAL alignment file");
    parser->addOption("-preset", "one of small, medium, big, large [medium]", "medium");
    parser->addOption("meanDegree", "[" + std::to_string(defaultMed._meanDegree), defaultMed._meanDegree);
    parser->addOption("maxBranchLength", "[" + std::to_string(defaultMed._maxBranchLength), defaultMed._maxBranchLength);
    parser->addOption("maxGenomes", "[" + std::to_string(defaultMed._maxGenomes), defaultMed._maxGenomes);
    parser->addOption("minSegmentLength", "[" + std::to_string(defaultMed._minSegmentLength), defaultMed._minSegmentLength);
    parser->addOption("maxSegmentLength", "[" + std::to_string(defaultMed._maxSegmentLength), defaultMed._maxSegmentLength);
    parser->addOption("maxSegments", "[" + std::to_string(defaultMed._maxSegments), defaultMed._maxSegments);
    parser->addOption("minSegments", "[" + std::to_string(defaultMed._minSegments), defaultMed._minSegments);
    parser->addOption("seed", "[system time]\n", 0);
    parser->addArgument("halFile", "path to toutput HAL alignment file");
    return parser;
}

template <typename T>
void updateOption(CLParserPtr parser,
                  const string& name,
                  T& val) {
    if (parser->specifiedOption(name)) {
        val = parser->getOption<T>(name);
    }
}

static Options getPresetDefault(CLParserPtr parser) {
    const string preset = parser->getOption<string>("preset");
    if (preset == "small") {
        return defaultSm;
    } else if (preset == "medium") {
        return defaultMed;
    } else if (preset == "big") {
        return defaultBig;
    } else if (preset == "large") {
        return defaultLrg;
    } else {
        throw hal_exception(" invalid --preset value: " + preset);
    }
}

static Options parseOptions(CLParserPtr parser)
{
    Options options = getPresetDefault(parser);
    updateOption(parser, "--meanDegree", options._meanDegree);
    updateOption(parser, "--maxBranchLength", options._maxBranchLength);
    updateOption(parser, "--maxGenoems", options._maxGenomes);
    updateOption(parser, "--minSegmentLength", options._minSegmentLength);
    updateOption(parser, "--maxSegmentLength", options._maxSegmentLength);
    updateOption(parser, "--minSegments", options._minSegments);
    updateOption(parser, "--maxSegments", options._maxSegments);
    updateOption(parser, "--seed", options._seed);
    options._halFile = parser->getArgument<string>("halFile");
    return options;
}
 
int main(int argc, char** argv)
{
    CLParserPtr parser = initParser();
    parser->parseOptions(argc, argv);
    Options options = parseOptions(parser);
        
  try
  {
    AlignmentPtr alignment = hdf5AlignmentInstance();
    alignment->setOptionsFromParser(parser);

    alignment->createNew(options._halFile);

    // call the crappy unit-test simulator 
    createRandomAlignment(alignment,
                          options._meanDegree,
                          options._maxBranchLength,
                          options._maxGenomes,
                          options._minSegmentLength,
                          options._maxSegmentLength,
                          options._minSegments,
                          options._maxSegments,
                          options._seed);
    
    alignment->close();
  }
  catch(hal_exception& e)
  {
    cerr << "hal exception caught: " << e.what() << endl;
    return 1;
  }
  catch(exception& e)
  {
    cerr << "Exception caught: " << e.what() << endl;
    return 1;
  }
  
  return 0;
}
