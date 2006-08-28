/** @file
 TreeItemAnnotationContainer - class holding vector with annotations
 @author Martin Petricek
*/

#include <cobject.h>
#include "treeitemannotationcontainer.h"
#include "treeitemannotation.h"
#include "multitreewindow.h"
#include "treedata.h"
#include "util.h"

namespace gui {

class TreeData;

using namespace std;

/**
 constructor of TreeItemAnnotationContainer - create root item from given object
 @param _data TreeData containing necessary information about tree in which this item will be inserted
 @param parent QListView in which to put this item
 @param _op Vector with annotations
 @param _page Page with annotations
 @param nameId Internal name of this item
 @param name Caption of this item - will be shown in treeview
 @param after Item after which this one will be inserted
 */
TreeItemAnnotationContainer::TreeItemAnnotationContainer(TreeData *_data,QListView *parent,const AnnotationVector &_op,boost::shared_ptr<CPage> _page,const QString name/*=QString::null*/,QListViewItem *after/*=NULL*/,const QString &nameId/*=NULL*/):TreeItemAbstract(nameId,_data,parent,after) {
 op=_op;
 page=_page;
 init(name);
 reload();
}

/**
 constructor of TreeItemAnnotationContainer - create child item from given object
 @param _data TreeData containing necessary information about tree in which this item will be inserted
 @param parent QListViewItem under which to put this item
 @param _op Vector with annotations
 @param _page Page with annotations
 @param nameId Internal name of this item
 @param name Caption of this item - will be shown in treeview
 @param after Item after which this one will be inserted
 */
TreeItemAnnotationContainer::TreeItemAnnotationContainer(TreeData *_data,QListViewItem *parent,const AnnotationVector &_op,boost::shared_ptr<CPage> _page,const QString name/*=QString::null*/,QListViewItem *after/*=NULL*/,const QString &nameId/*=NULL*/):TreeItemAbstract(nameId,_data,parent,after) {
 op=_op;
 page=_page;
 init(name);
 reload();
}

/**
 initialize captions of this item from name
 @param name Caption of item
 */
void TreeItemAnnotationContainer::init(const QString &name) {
 if (name.isNull()) {
  setText(0,QObject::tr("<no name>"));
 } else {
  setText(0,name);
 }
 // object type
 setText(1,QObject::tr("Annotations"));
 setText(2,"");
}

/** default destructor */
TreeItemAnnotationContainer::~TreeItemAnnotationContainer() {
}

//See TreeItemAbstract for description of this virtual method
TreeItemAbstract* TreeItemAnnotationContainer::createChild(const QString &name,__attribute__((unused)) ChildType typ,QListViewItem *after/*=NULL*/) {
 size_t position=name.toUInt();
 return new TreeItemAnnotation(data,this,op[position],page,name,after);
}

//See TreeItemAbstract for description of this virtual method
bool TreeItemAnnotationContainer::validChild(const QString &name,QListViewItem *oldChild) {
 TreeItemAnnotation* oper=dynamic_cast<TreeItemAnnotation*>(oldChild);
 if (!oper) return false;
 size_t position=name.toUInt();
 return (oper->getObject()==op[position]);
}

//See TreeItemAbstract for description of this virtual method
ChildType TreeItemAnnotationContainer::getChildType(__attribute__((unused)) const QString &name) {
 return 1;//Just one type : Annotation
}

//See TreeItemAbstract for description of this virtual method
QStringList TreeItemAnnotationContainer::getChildNames() {
 return util::countList(op.size());
}

//See TreeItemAbstract for description of this virtual method
QSCObject* TreeItemAnnotationContainer::getQSObject() {
 return NULL;//TODO: return something reasonable
}

//See TreeItemAbstract for description of this virtual method
void TreeItemAnnotationContainer::remove() {
 // Do nothing, it doesn't make sense to delete this container
 return;
}

//See TreeItemAbstract for description of this virtual method
void TreeItemAnnotationContainer::reloadSelf() {
 //We have annotations already inside, so do nothing
 return;
}

//See TreeItemAbstract for description of this virtual method
bool TreeItemAnnotationContainer::haveChild() {
 return op.size()>0;
}


} // namespace gui