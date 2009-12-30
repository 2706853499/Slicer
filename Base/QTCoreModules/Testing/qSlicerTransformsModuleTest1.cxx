/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer

=========================================================================auto=*/

#include "qSlicerCoreApplication.h"
#include "qSlicerTransformsModule.h"

#include <stdlib.h>

#include "TestingMacros.h"

int qSlicerTransformsModuleTest1(int argc, char * argv [] )
{
  if( argc < 2 )
    {
    std::cerr << "Missing arguments" << std::endl;
    return EXIT_FAILURE;
    }

  int argc2 = argc - 1;
  char ** argv2 = argv + 1;

  qSlicerCoreApplication app( argc2, argv2 );

  qSlicerCoreApplication * aptr = app.application();

  if( aptr != (&app) )
    {
    std::cerr << "Problem with the application() singleton" << std::endl;
    return EXIT_FAILURE;
    }
  
  aptr->initialize();

  qSlicerTransformsModule * transformsModule = new qSlicerTransformsModule;
  transformsModule->initialize(aptr->appLogic());

  if (transformsModule->logic() != transformsModule->logic())
    {
    std::cerr << "The logic must be always the same." << std::endl;
    return EXIT_FAILURE;
    }

  if (transformsModule->widgetRepresentation() != transformsModule->widgetRepresentation())
    {
    std::cerr << "The logic must be always the same." << std::endl;
    return EXIT_FAILURE;
    }
  delete transformsModule;

  return EXIT_SUCCESS;
}

