/*                                                                              
 * PDFedit - free program for PDF document manipulation.                        
 * Copyright (C) 2006, 2007  PDFedit team:      Michal Hocko, 
 *                                              Miroslav Jahoda,       
 *                                              Jozef Misutka, 
 *                                              Martin Petricek                                             
 *
 * Project is hosted on http://sourceforge.net/projects/pdfedit                                                                      
 */ 
// vim:tabstop=4:shiftwidth=4:noexpandtab:textwidth=80

#include "static.h"
#include "carray.h"
#include "cpdf.h"
#include "factories.h"

//=====================================================================================
namespace pdfobjects {
//=====================================================================================

using namespace std;
using namespace boost;
	

//=====================================================================================
// CArray
//=====================================================================================

//
// Constructors
//

//
// Protected constructor
//
CArray::CArray (CPdf& p, Object& o, const IndiRef& rf) : IProperty (&p,rf) 
{
	// Build the tree from xpdf object
	utils::complexValueFromXpdfObj<pArray,Value&> (*this, o, value);
}

//
// Protected constructor
//
CArray::CArray (Object& o)
{
	// Build the tree from xpdf object
	utils::complexValueFromXpdfObj<pArray,Value&> (*this, o, value);
}


//
// Get methods
//

//
//
//
void 
CArray::getStringRepresentation (string& str) const 
{
	utils::complexValueToString<CArray> (value,str);
}


//
//
//
shared_ptr<IProperty>
CArray::getProperty (PropertyId id) const
{
	//kernelPrintDbg (debug::DBG_DBG,"getProperty() " << id);

	if (id >= value.size())
		throw OutOfRange ();
	
	shared_ptr<IProperty> ip = value[id];
	// Set mode only if pdf is valid
	_setMode (ip,id);

	return ip;
}


//
// Set methods
//

//
//
//
void 
CArray::setPdf (CPdf* pdf)
{
	// Set pdf to this object
	IProperty::setPdf (pdf);

	// Set new pdf to all its children
	Value::iterator it = value.begin();
	for (; it != value.end(); ++it)
		(*it)->setPdf (pdf);
}

//
//
//
void 
CArray::setIndiRef (const IndiRef& rf)
{
	// Set pdf to this object
	IProperty::setIndiRef (rf);

	// Set new pdf to all its children
	Value::iterator it = value.begin();
	for (; it != value.end(); ++it)
		(*it)->setIndiRef (rf);
}


//
// 
//
void
CArray::delProperty (PropertyId id)
{
	//kernelPrintDbg (debug::DBG_DBG,"delProperty(" << id << ")");

	// Check if we are out of bounds
	if (id >= value.size())
		throw OutOfRange ();
	
	// Check whether we can make the change
	this->canChange();

	shared_ptr<IProperty> oldip = value[id];

	// Delete that item
	value.erase (remove (value.begin(), value.end(), oldip));

	if (hasValidPdf (this))
	{
		assert (hasValidRef (this));
		
		// Create contest
		shared_ptr<ObserverContext> context (_createContext (oldip,id));	
	
		try {
			// notify observers and dispatch the change
			_objectChanged (shared_ptr<IProperty> (new CNull), context);
			
		}catch (PdfException&)
		{
			assert (!"Should not happen.. Condition must be included in CPdf::canChange()...");
			throw;
		}

		
		// Be sure
		oldip->setPdf (NULL);
		oldip->setIndiRef (IndiRef());

	}else
		{ assert (!hasValidRef (this)); }
}


//
// Correctly to add an object (without name) can be done only to Array object
//
shared_ptr<IProperty>
CArray::addProperty (const IProperty& newIp)
{
	//kernelPrintDbg (debug::DBG_DBG,"addProperty(...)");
	return addProperty (value.size(), newIp);
}

//
// Correctly to add an object (without name) can be done only to Array object
//
// REMARK: because of the compiler, we can't put PropertyId here
//
shared_ptr<IProperty>
CArray::addProperty (PropertyId position, const IProperty& newIp)
{
	//kernelPrintDbg (debug::DBG_DBG,"addProperty(" << position << ")");

	//
	// Check if we add to a valid position
	//
	if (position > value.size())
		throw OutOfRange ();
	
	// Check whether we can make the change
	this->canChange();

	// Clone the added property
	shared_ptr<IProperty> newIpClone = newIp.clone ();
	assert (newIpClone);
	
	// Inherit id, gen number and pdf
	newIpClone->setPdf (this->getPdf());
	newIpClone->setIndiRef (this->getIndiRef());
	
	// Find the correct position
	Value::iterator it;
	if (value.size() == position)
		it = value.end ();
	else
		for (it = value.begin(); 0 != position; ++it, --position)
			;
	// Insert it
	value.insert (it,newIpClone);
	
	if (hasValidPdf (this))
	{
		assert (hasValidRef (this));
		
		// Create contest
		shared_ptr<ObserverContext> context (_createContext(shared_ptr<IProperty>(new CNull ()), position));

		try {
			// notify observers and dispatch the change
			_objectChanged (newIpClone, context);
			
		}catch (PdfException&)
		{
			assert (!"Should not happen.. Condition must be included in CPdf::canChange()...");
			throw;
		}

	}else
		{ assert (!hasValidRef (this)); }

	
	// Set mode only if pdf is valid
	_setMode (newIpClone, position);
	
	return newIpClone;
}


//
//
//
shared_ptr<IProperty>
CArray::setProperty (PropertyId id, IProperty& newIp)
{
	//kernelPrintDbg (debug::DBG_DBG, "setProperty(" << id << ")");

	// Check the bounds, if fails add it
	if (id >= value.size())
		return addProperty (id, newIp);

	// Check whether we can make the change
	this->canChange();

	// Save the old one
	shared_ptr<IProperty> oldip = value[id];
	// Clone the added property
	shared_ptr<IProperty> newIpClone = newIp.clone ();
	assert (newIpClone);
	
	// Inherit id, gen number and pdf
	newIpClone->setIndiRef (this->getIndiRef());
	newIpClone->setPdf (this->getPdf());

	// Construct item, and replace it with this one
	replace (value.begin(), value.end(), oldip, newIpClone);

	//
	// Dispatch change if we are in valid pdf
	// 
	if (hasValidPdf (this))
	{	
		assert (hasValidRef (this));
		
		// Create contest
		shared_ptr<ObserverContext> context (_createContext (oldip,id));	

		try {
			// notify observers and dispatch the change
			_objectChanged (newIpClone, context);

		}catch (PdfException&)
		{
			assert (!"Should not happen.. Condition must be included in CPdf::canChange()...");
			throw;
		}

		// Be sure
		oldip->setPdf (NULL);
		oldip->setIndiRef (IndiRef());

	}else
		{ assert (!hasValidRef (this)); }

	// Set mode only if pdf is valid
	_setMode (newIpClone, id);
	
	return newIpClone;
}


//
// Helper methods
//

//
//
//
::Object*
CArray::_makeXpdfObject () const
{
	//kernelPrintDbg (debug::DBG_DBG,"_makeXpdfObject");
	::Object * arrayObj = XPdfObjectFactory::getInstance();
	arrayObj->initArray(this->getPdf()->getCXref());
	::Array * array = arrayObj->getArray();

	Value::const_iterator it = value.begin();
	for (; it != value.end(); ++it)
		array->add((*it)->_makeXpdfObject());
	assert(array->getLength() == getPropertyCount());
	return arrayObj;
}


//
//
//
void 
CArray::_objectChanged (boost::shared_ptr<IProperty> newValue, boost::shared_ptr<const ObserverContext> context)
{
	// Do not notify anything if we are not in a valid pdf
	if (!hasValidPdf (this))
		return;
	assert (hasValidRef (this));

	// Dispatch the change
	this->dispatchChange ();
	
	if (context)
	{
		// Notify everybody about this change
		this->notifyObservers (newValue, context);

	}else
	{
		assert (!"Invalid context");
		throw CObjInvalidOperation ();
	}
}

//
// Set mode
//
void
CArray::_setMode (boost::shared_ptr<IProperty> ip, PropertyId id) const
{
	/* FIXME this is not good implementation because we don't have
	 * information about array name here. So if it is direct child
	 * of some dictionary, we will set mode for the array correctly
	 * (according mode rules) but all children properties have
	 * different mode which match the most general setting (
	 * .: mdAdvanced in our default configuration).
	 * We need to get the name of direct property from the parent 
	 * dictionary. Until this is implemented we will simply set 
	 * child's mode to the array mode.
	 */
	/*
	configuration::ModeController* modecontroller = NULL;
	if (hasValidPdf (this) && (NULL != (modecontroller=this->getPdf()->getModeController())))
	{
		assert (modecontroller);		
		
		ostringstream oss;
		oss << id;

		PropertyMode mode = modecontroller->getMode ("",oss.str());
		if (mdUnknown == mode)
			ip->setMode (this->getMode());
		else
			ip->setMode (mode);
	}
	*/
	ip->setMode(this->getMode());
}



//
// Clone method
//
IProperty*
CArray::doClone () const
{
	//kernelPrintDbg (debug::DBG_DBG,"");
	
	// Make new complex object
	// NOTE: We do not want to inherit any IProperty variable
	CArray* clone_ = _newInstance ();
	
	// Loop through all items and clone them as well and finally add them to the new object
	Value::const_iterator it = value.begin ();
	for (; it != value.end (); ++it)
		clone_->value.push_back ((*it)->clone());

	return clone_;
}

//
//
//
IProperty::ObserverContext* 
CArray::_createContext (shared_ptr<IProperty> changedIp, PropertyId id)
{
	//kernelPrintDbg (debug::DBG_DBG, "");

	// Create the context
	return new CArrayComplexObserverContext (changedIp, id);
}



//=====================================================================================
} // namespace pdfobjects
//=====================================================================================

