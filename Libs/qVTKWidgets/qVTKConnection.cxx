#include "qVTKConnection.h"

// qMRML includes
#include "qVTKObjectEventsObserver.h"

// VTK includes
#include <vtkObject.h>
#include <vtkSmartPointer.h>
#include <vtkCallbackCommand.h>

// QT includes
#include <QRegExp>
#include <QString>
#include <QTextStream>
#include <QDebug>

//-----------------------------------------------------------------------------
class qVTKConnectionPrivate: public qCTKPrivate<qVTKConnection>
{
public:
  enum
    {
    ARG_UNKOWN = 0,
    ARG_VTKOBJECT_AND_VTKOBJECT,
    ARG_VTKOBJECT_VOID_ULONG_VOID
    };

  qVTKConnectionPrivate()
    {
    this->Callback = vtkSmartPointer<vtkCallbackCommand>::New();
    this->Callback->SetCallback(qVTKConnection::DoCallback);
    this->VTKObject  = 0;
    this->QtObject   = 0;
    this->VTKEvent = vtkCommand::NoEvent;
    this->Priority = 0.0;
    this->SlotType = ARG_UNKOWN;
    this->Parent      = 0;
    this->Established = false;
    this->Blocked     = false;
    }

  ~qVTKConnectionPrivate()
    {
    if(this->VTKObject && this->Established)
      {
      this->VTKObject->RemoveObserver(this->Callback);
      //Qt takes care of disconnecting slots
      }
    }

  vtkSmartPointer<vtkCallbackCommand> Callback;
  vtkObject*                          VTKObject;
  qVTKObjectEventsObserver*           Parent;
  const QObject*                      QtObject;
  unsigned long                       VTKEvent;
  QString                             QtSlot;
  float                               Priority;
  int                                 SlotType;
  bool                                Established;
  bool                                Blocked;
};

//-----------------------------------------------------------------------------
qVTKConnection::qVTKConnection(qVTKObjectEventsObserver* parent):
  Superclass(parent)
{
  QCTK_INIT_PRIVATE(qVTKConnection);
  QCTK_D(qVTKConnection);
  
  d->Callback->SetClientData(this);
  Q_ASSERT(parent);
  d->Parent = parent;
}

//-----------------------------------------------------------------------------
void qVTKConnection::printAdditionalInfo()
{
  this->Superclass::dumpObjectInfo();

  QCTK_D(qVTKConnection);
  
  qDebug() << "qVTKConnection:" << this << endl
           << " VTKObject:" << d->VTKObject->GetClassName()
             << "(" << d->VTKObject << ")" << endl
           << " Parent:" << d->Parent << endl
           << " QtObject:" << d->QtObject << endl
           << " VTKEvent:" << d->VTKEvent << endl
           << " QtSlot:" << d->QtSlot << endl
           << " SlotType:" << d->SlotType << endl
           << " Priority:" << d->Priority << endl
           << " Established:" << d->Established << endl
           << " Blocked:" << d->Blocked;
}

//-----------------------------------------------------------------------------
QString qVTKConnection::getShortDescription()
{
  QCTK_D(qVTKConnection);
  
  return Self::getShortDescription(d->VTKObject, d->VTKEvent, d->QtObject, d->QtSlot);
}

//-----------------------------------------------------------------------------
QString qVTKConnection::getShortDescription(vtkObject* vtk_obj, unsigned long vtk_event,
    const QObject* qt_obj, QString qt_slot)
{
  QString ret;
  QTextStream ts( &ret );
  ts << (vtk_obj ? vtk_obj->GetClassName() : "NULL") << "/"
     << vtk_event << "/" << qt_obj << "/" << qt_slot;
  return ret;
}

//-----------------------------------------------------------------------------
bool qVTKConnection::ValidateParameters(vtkObject* vtk_obj, unsigned long /*vtk_event*/,
  const QObject* qt_obj, QString qt_slot)
{
  if (!vtk_obj)
    {
    qCritical() << "vtkObject is NULL";
    return false;
    }
  if (!qt_obj)
    {
    qCritical() << "QObject is NULL";
    return false;
    }
  if (qt_slot.isEmpty())
    {
    qCritical() << "QtSlot is NULL";
    return false;
    }
  return true;
}

//-----------------------------------------------------------------------------
void qVTKConnection::SetParameters(vtkObject* vtk_obj, unsigned long vtk_event,
  const QObject* qt_obj, QString qt_slot, float priority)
{
  QCTK_D(qVTKConnection);
  
  if (!Self::ValidateParameters(vtk_obj, vtk_event, qt_obj, qt_slot)) { return; }

  d->VTKObject = vtk_obj;
  d->QtObject = qt_obj;
  d->VTKEvent = vtk_event;
  d->QtSlot = qt_slot;
  d->Priority = priority;

  int strSize = qt_slot.count();
  if (qt_slot.contains(QRegExp(QString("\\( ?vtkObject ?\\* ?, ?vtkObject ?\\* ?\\)"))))
    {
    d->SlotType = qVTKConnectionPrivate::ARG_VTKOBJECT_AND_VTKOBJECT;
    }
  else
    {
    d->SlotType = qVTKConnectionPrivate::ARG_VTKOBJECT_VOID_ULONG_VOID;
    }
}

//-----------------------------------------------------------------------------
void qVTKConnection::SetEstablished(bool enable)
{
  QCTK_D(qVTKConnection);
  
  if (d->Established == enable)
    {
    qCritical() << "Failed to " << (enable ? "establish" : "break")
                << " an already " << (enable ? "established" : "broken") << " connection";
    return;
    }

  if (enable)
    {
    this->EstablishConnection();
    }
  else
    {
    this->BreakConnection();
    }
}

//-----------------------------------------------------------------------------
void qVTKConnection::SetBlocked(bool block)
{
  QCTK_D(qVTKConnection);
  
  if (d->Blocked == block)
    {
    qCritical() << "Failed to " << (block ? "block" : "unblock")
                << " an already " << (block ? "blocked" : "unblocked") << " connection";
    return;
    }
  d->Blocked = block;
}
/*
#define QVTK_EMPTY_ARG
#define QVTK_OBJECT_INTERNAL_CONNECT1(ARG1)     \
QObject::connect(                               \
  this, SIGNAL(emitExecute(ARG1)),              \
  d->QtObject,                                  \
  d->QtSlot.toLatin1().data());

#define QVTK_OBJECT_INTERNAL_CONNECT2(ARG1,ARG2)   \
QObject::connect(                                  \
  this, SIGNAL(emitExecute(ARG1, ARG2)),           \
  d->QtObject,                                     \
  d->QtSlot.toLatin1().data());

#define QVTK_OBJECT_INTERNAL_CONNECT3(ARG1,ARG2,ARG3)  \
QObject::connect(                                      \
  this, SIGNAL(emitExecute(ARG1, ARG2, ARG3)),         \
  d->QtObject,                                         \
  d->QtSlot.toLatin1().data());

#define QVTK_OBJECT_INTERNAL_CONNECT4(ARG1,ARG2,ARG3,ARG4)   \
QObject::connect(                                            \
  this, SIGNAL(emitExecute(ARG1, ARG2, ARG3, ARG4)),         \
  d->QtObject,                                               \
  d->QtSlot.toLatin1().data());
*/
//-----------------------------------------------------------------------------
void qVTKConnection::EstablishConnection()
{
  QCTK_D(qVTKConnection);
  
  if (d->Established) { return; }

  switch (d->SlotType)
    {
    case qVTKConnectionPrivate::ARG_VTKOBJECT_AND_VTKOBJECT:
      //QVTK_OBJECT_INTERNAL_CONNECT2(vtkObject*,vtkObject*);
      QObject::connect(this, SIGNAL(emitExecute(vtkObject*, vtkObject*)),
        d->QtObject,d->QtSlot.toLatin1().data());
      break;
    case qVTKConnectionPrivate::ARG_VTKOBJECT_VOID_ULONG_VOID:
      //QVTK_OBJECT_INTERNAL_CONNECT4(vtkObject*,void*,unsigned long,void*);
      QObject::connect(this, SIGNAL(emitExecute(vtkObject*, void*, unsigned long, void*)),
        d->QtObject, d->QtSlot.toLatin1().data());
      break;
    default:
      Q_ASSERT(false);
      qCritical() << "Failed to establish qVTKConnection - "
            << "The slot (" << d->QtSlot <<  ") owned by "
            << "QObject(" << d->QtObject->objectName() << ")"
            << " seems to have a wrong signature.";
      break;
    }

  // Make a connection between this and the vtk object
  d->VTKObject->AddObserver(d->VTKEvent, d->Callback, d->Priority);

  // If necessary, observe vtk DeleteEvent
  if(d->VTKEvent != vtkCommand::DeleteEvent)
    {
    d->VTKObject->AddObserver(vtkCommand::DeleteEvent, d->Callback);
    }

  // Remove itself from its parent when vtkObject is deleted
  QObject::connect(d->QtObject, SIGNAL(destroyed(QObject*)), this, SLOT(deleteConnection()));

  d->Established = true;
}
/*
#undef QVTK_EMPTY_ARG
#undef QVTK_OBJECT_INTERNAL_CONNECT1
#undef QVTK_OBJECT_INTERNAL_CONNECT2
#undef QVTK_OBJECT_INTERNAL_CONNECT3
#undef QVTK_OBJECT_INTERNAL_CONNECT4
*/
//-----------------------------------------------------------------------------
void qVTKConnection::BreakConnection()
{
  QCTK_D(qVTKConnection);
  
  Q_ASSERT(d->Established);
  if (!d->Established) { return; }

  d->VTKObject->RemoveObserver(d->Callback);

  switch (d->SlotType)
    {
    case qVTKConnectionPrivate::ARG_VTKOBJECT_AND_VTKOBJECT:
      //QVTK_OBJECT_INTERNAL_CONNECT2(vtkObject*,vtkObject*);
      QObject::disconnect(this, SIGNAL(emitExecute(vtkObject*, vtkObject*)),
        d->QtObject,d->QtSlot.toLatin1().data());
      break;
    case qVTKConnectionPrivate::ARG_VTKOBJECT_VOID_ULONG_VOID:
      //QVTK_OBJECT_INTERNAL_CONNECT4(vtkObject*,void*,unsigned long,void*);
      QObject::disconnect(this, SIGNAL(emitExecute(vtkObject*, void*, unsigned long, void*)),
        d->QtObject, d->QtSlot.toLatin1().data());
      break;
    default:
      Q_ASSERT(false);
      qCritical() << "Failed to establish qVTKConnection - "
            << "The slot (" << d->QtSlot <<  ") owned by "
            << "QObject(" << d->QtObject->objectName() << ")"
            << " seems to have a wrong signature.";
      break;
    }

  QObject::disconnect(
    d->QtObject, SIGNAL(destroyed(QObject*)),
    this, SLOT(deleteConnection()));

  d->Established = false;
}

//-----------------------------------------------------------------------------
bool qVTKConnection::IsEqual(vtkObject* vtk_obj, unsigned long vtk_event,
    const QObject* qt_obj, QString qt_slot)
{
  QCTK_D(qVTKConnection);
  
  if (d->VTKObject != vtk_obj)
    {
    return false;
    }
  if (vtk_event != vtkCommand::NoEvent && d->VTKEvent != vtk_event)
    {
    return false;
    }
  if (qt_obj && d->QtObject != qt_obj)
    {
    return false;
    }
  if (!qt_slot.isEmpty() && 
      (QString(d->QtSlot).remove(' ').compare(
        QString(qt_slot).remove(' ')) != 0))
    {
    return false;
    }
  return true;
}

//-----------------------------------------------------------------------------
QCTK_GET_CXX(qVTKConnection, int, GetSlotType, SlotType);

//-----------------------------------------------------------------------------
void qVTKConnection::DoCallback(vtkObject* vtk_obj, unsigned long event,
                                 void* client_data, void* call_data)
{
  qVTKConnection* conn = static_cast<qVTKConnection*>(client_data);
  Q_ASSERT(conn);
  conn->Execute(vtk_obj, event, client_data, call_data);
}

//-----------------------------------------------------------------------------
// callback from VTK to emit signal
void qVTKConnection::Execute(vtkObject* vtk_obj, unsigned long vtk_event,
  void* client_data, void* call_data)
{
  QCTK_D(qVTKConnection);
  
  if (d->Blocked) { return; }
  if(vtk_event != vtkCommand::DeleteEvent ||
     (vtk_event == vtkCommand::DeleteEvent && d->VTKEvent == vtkCommand::DeleteEvent))
    {
    vtkObject* callDataAsVtkObject = 0;
    switch (d->SlotType)
      {
      case qVTKConnectionPrivate::ARG_VTKOBJECT_AND_VTKOBJECT:
        if (d->VTKEvent == vtk_event)
          {
          callDataAsVtkObject = reinterpret_cast<vtkObject*>( call_data );
          if (!callDataAsVtkObject)
            {
            qCritical() << "The VTKEvent(" << d->VTKEvent<< ") triggered by vtkObject("
              << d->VTKObject->GetClassName() << ") "
              << "doesn't return data of type vtkObject." << endl
              << "The slot (" << d->QtSlot <<  ") owned by "
              << "QObject(" << d->QtObject->objectName() << ")"
              << " may be incorrect.";
            }
          emit this->emitExecute( vtk_obj, callDataAsVtkObject );
          }
        break;
      case qVTKConnectionPrivate::ARG_VTKOBJECT_VOID_ULONG_VOID:
        emit this->emitExecute(vtk_obj, call_data, vtk_event, client_data);
        break;
      default:
        // Should never reach
        qCritical() << "Unknown SlotType:" << d->SlotType;
        return;
        break;
      }
    }

  if(vtk_event == vtkCommand::DeleteEvent)
    {
    //qDebug("--------------------------> qVTKConnection::Execute - removeConnection <--------------------------");
    d->Parent->removeConnection(d->VTKObject, d->VTKEvent,
                                d->QtObject, d->QtSlot.toLatin1().data());
    }
}

//-----------------------------------------------------------------------------
void qVTKConnection::deleteConnection()
{
  qctk_d()->Parent->removeConnection( this );
}
