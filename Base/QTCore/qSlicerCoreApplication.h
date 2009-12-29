/*=auto=========================================================================

 Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) 
 All Rights Reserved.

 See Doc/copyright/copyright.txt
 or http://www.slicer.org/copyright/copyright.txt for details.

 Program:   3D Slicer

=========================================================================auto=*/

#ifndef __qSlicerCoreApplication_h
#define __qSlicerCoreApplication_h

// qCTK includes
#include <qCTKPimpl.h>

// QT includes
#include <QApplication>

#include "qSlicerBaseQTCoreExport.h"

class vtkSlicerApplicationLogic;
class vtkMRMLScene;
class qSlicerModuleManager;
class qSlicerCoreIOManager; 
class qSlicerCoreApplicationPrivate;

class Q_SLICER_BASE_QTCORE_EXPORT qSlicerCoreApplication : public QApplication
{
  Q_OBJECT

public:

  typedef QApplication Superclass;
  qSlicerCoreApplication(int &argc, char **argv);
  virtual ~qSlicerCoreApplication();

  // Description:
  // Return a reference to the application singleton
  static qSlicerCoreApplication* application();

  // Description:
  // Instantiate MRMLScene and Application logic.
  //  - Configure scene
  //  - AppLogic is set as a scene observer.
  //  - Create processing thread
  void initialize();
  
  // Description:
  // initialize paths for module discovery
  // DEPRECATED: This method was used by the KWWidgets GUI only and it will be
  // removed once the QT GUI is functional.
  void initializePaths(const QString& programPath);
  
  // Description:
  // Return true if the application has been initialized
  // Note: initialize() should be called only one time.
  bool initialized() const;

  // Description:
  // Convenient method allowing to set the initialized flag
  // DEPRECATED: This method was used by the KWWidgests GUI only and it will be
  // removed once the QT GUI is functional. Only vtkSlicerApplication should use
  // that method. Improper use of that function may lead to unstable state
  void setInitialized(bool initialized); 

  // Description:
  // Set/Get MRML Scene
  // DEPRECATED: This method was used by the KWWidgests GUI only and it will be
  // removed once the QT GUI is functional.
  void setMRMLScene(vtkMRMLScene * scene);
  vtkMRMLScene* mrmlScene() const;

  // Description:
  // Set/Get application logic
  // DEPRECATED: This method was used by the KWWidgets GUI only and it will be
  // removed once the QT GUI is functional.
  void setAppLogic(vtkSlicerApplicationLogic* appLogic);
  vtkSlicerApplicationLogic* appLogic() const;
  
  // Description:
  // Get slicer home directory
  QString slicerHome() const;

  // Description:
  // Convenient method to set slicer home directory
  // Note: SlicerHome should be auto-discovered. See implementation details.
  void setSlicerHome(const QString& slicerHome);

  // Description:
  // Return the temporary directory associated with the application
  QString tempDirectory() const; 

  // Description:
  // Initialize paths
  void initializeLoadableModulesPaths();
  void initializeCmdLineModulesPaths();

  // Description:
  // Set/Get the module manager
  qSlicerModuleManager* moduleManager()const;
  void setModuleManager(qSlicerModuleManager* moduleManager);

  // Description:
  // Set/Get the IO manager
  qSlicerCoreIOManager* coreIOManager()const;
  void setCoreIOManager(qSlicerCoreIOManager* ioManager); 

signals:
  void currentMRMLSceneChanged(vtkMRMLScene* mrmlScene);

private:
  QCTK_DECLARE_PRIVATE(qSlicerCoreApplication);
};

#endif
