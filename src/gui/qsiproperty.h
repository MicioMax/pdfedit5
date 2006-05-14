#ifndef __QSIPROPERTY_H__
#define __QSIPROPERTY_H__

#include <qobject.h>
#include <iproperty.h>
#include "qscobject.h"

class QString;

namespace gui {

using namespace pdfobjects;

class Base;

/*= This object represent one property in dictionary or array */
class QSIProperty : public QSCObject {
 Q_OBJECT
public:
 QSIProperty(boost::shared_ptr<IProperty> _ip,Base *_base);
 QSIProperty(QSIProperty &source);
 virtual ~QSIProperty();
 boost::shared_ptr<IProperty> get() const;
public slots:
 /*- Return text representation of this property */
 QString getText();
 /*- Get integer representation of this property, return 0 if it cannot be represented as integer */
 int getInt();
 /*-
  Get type identifier of this Property.
  Can be one of: Null, Bool, Int, Real, String, Name, Ref, Array, Dict, Stream
 */
 QString getType();
 /*- Get human readable and localized name of type of this Property */
 QString getTypeName();
 /*-
  Set value of this property.
  Work only on Bool, Int, Real, String or Name types,
  automatically converts value if type of property is different than type of parameter
  Will do nothing if called on different types (Dict, Array, etc ...)
 */
 void set(const QString &value);
 /*- Overloaded variant of set method */
 void set(int value);
 /*- Overloaded variant of set method */
 void set(double value);
 /*- Overloaded variant of set method */
 void set(bool value);
protected:
 QSIProperty(boost::shared_ptr<IProperty> _ip, QString _className,Base *_base);
protected:
 /** Object held in class*/
 boost::shared_ptr<IProperty> obj;
};

} // namespace gui

#endif
