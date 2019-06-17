#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <cassert>

CQPropertyViewModel::
CQPropertyViewModel() :
 QAbstractItemModel()
{
  setObjectName("viewModel");
}

CQPropertyViewModel::
~CQPropertyViewModel()
{
  clear();
}

int
CQPropertyViewModel::
columnCount(const QModelIndex &) const
{
  return 2;
}

int
CQPropertyViewModel::
rowCount(const QModelIndex &parent) const
{
  CQPropertyViewItem *parentItem = this->item(parent);

  if (! parentItem)
    return 0;

  return numItemChildren(parentItem);
}

QVariant
CQPropertyViewModel::
headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation != Qt::Horizontal)
    return QVariant();

  if (role == Qt::DisplayRole) {
    if      (section == 0)
      return "Name";
    else if (section == 1)
      return "Value";
    else
      return QVariant();
  }

  return QVariant();
}

QVariant
CQPropertyViewModel::
data(const QModelIndex &index, int role) const
{
  CQPropertyViewItem *item = this->item(index);

  if (! item)
    return QVariant();

  if      (role == Qt::DisplayRole || role == Qt::EditRole) {
    if      (index.column() == 0)
      return item->name();
    else if (index.column() == 1)
      return item->data();
    else
      return QVariant();
  }
  else if (role == Qt::ToolTipRole) {
    if      (index.column() == 0)
      return item->nameTip();
    else if (index.column() == 1)
      return item->valueTip();
  }

  return QVariant();
}

bool
CQPropertyViewModel::
setData(const QModelIndex &index, const QVariant &value, int role)
{
  CQPropertyViewItem *item = this->item(index);

  if (! item)
    return false;

  if (role == Qt::DisplayRole || role == Qt::EditRole) {
    if      (index.column() == 0)
      return false;
    else if (index.column() == 1) {
      item->setData(value);

      emit dataChanged(index, index);

      return true;
    }
    else
      return false;
  }

  return false;
}

QModelIndex
CQPropertyViewModel::
index(int row, int column, const QModelIndex &parent) const
{
  CQPropertyViewItem *parentItem = this->item(parent);

  if (! parentItem)
    return QModelIndex();

  if (row < 0 || row >= numItemChildren(parentItem))
    return QModelIndex();

  if (column < 0 || column >= 2)
    return QModelIndex();

  CQPropertyViewItem *childItem = itemChild(parentItem, row);

  QModelIndex ind = createIndex(row, column, childItem);

  assert(this->item(ind) == childItem);

  return ind;
}

QModelIndex
CQPropertyViewModel::
parent(const QModelIndex &index) const
{
  CQPropertyViewItem *item = this->item(index);

  if (! item)
    return QModelIndex();

  CQPropertyViewItem *parent = item->parent();

  if (! parent || parent == root())
    return QModelIndex();

  CQPropertyViewItem *parentParent = parent->parent();

  if (! parentParent)
    parentParent = root();

  //---

  int i = 0;

  for (const auto &child : itemChildren(parentParent)) {
    if (child == parent) {
      QModelIndex ind = createIndex(i, 0, parent);

      assert(this->item(ind) == parent);

      return ind;
    }

    ++i;
  }

  return QModelIndex();
}

Qt::ItemFlags
CQPropertyViewModel::
flags(const QModelIndex &index) const
{
  CQPropertyViewItem *item = this->item(index);

  Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

  if (item->isEditable())
    flags |= Qt::ItemIsEditable;

  return flags;
}

void
CQPropertyViewModel::
clear()
{
  delete root_;

  root_ = nullptr;
}

CQPropertyViewItem *
CQPropertyViewModel::
addProperty(const QString &path, QObject *object, const QString &name, const QString &alias)
{
  beginResetModel();

  QStringList pathParts = path.split('/', QString::SkipEmptyParts);

  CQPropertyViewItem *parentItem =
    hierItem(pathParts, /*create*/true, /*alias*/false, /*hidden*/true);

  CQPropertyViewItem *item = new CQPropertyViewItem(parentItem, object, name);

  connect(item, SIGNAL(valueChanged(QObject *, const QString &)),
          this, SIGNAL(valueChanged(QObject *, const QString &)));

  parentItem->addChild(item);

  if (alias != "")
    item->setAlias(alias);

  endResetModel();

  return item;
}

bool
CQPropertyViewModel::
setProperty(QObject *object, const QString &path, const QVariant &value)
{
  CQPropertyViewItem *item =
    propertyItem(object, path, '.', /*create*/false, /*alias*/true, /*hidden*/true);

  if (! item)
    return false;

  if (! item->setData(value))
    return false;

  QModelIndex ind1 = indexFromItem(item, 0);
  QModelIndex ind2 = indexFromItem(item, 1);

  emit dataChanged(ind1, ind2);

  return true;
}

bool
CQPropertyViewModel::
getProperty(const QObject *object, const QString &path, QVariant &value) const
{
  const CQPropertyViewItem *item =
    propertyItem(object, path, '.', /*create*/false, /*alias*/true, /*hidden*/true);

  if (! item)
    return false;

  value = item->data();

  return true;
}

bool
CQPropertyViewModel::
getTclProperty(const QObject *object, const QString &path, QVariant &value) const
{
  const CQPropertyViewItem *item =
    propertyItem(object, path, '.', /*create*/false, /*alias*/true, /*hidden*/true);

  if (! item)
    return false;

  value = item->tclData();

  return true;
}

void
CQPropertyViewModel::
removeProperties(const QString &path, QObject *)
{
  beginResetModel();

  QStringList pathParts = path.split('/', QString::SkipEmptyParts);

  CQPropertyViewItem *item =
    hierItem(pathParts, /*create*/false, /*alias*/false, /*hidden*/true);

  if (item && item->parent())
    item->parent()->removeChild(item);

  endResetModel();
}

void
CQPropertyViewModel::
hideProperty(const QString &path, const QObject *object)
{
  CQPropertyViewItem *item =
    propertyItem(const_cast<QObject *>(object), path, '/',
                 /*create*/false, /*alias*/false, /*hidden*/true);
  if (! item) return;

  item->setHidden(true);
}

void
CQPropertyViewModel::
setObjectRoot(const QString &path, QObject *obj)
{
  QStringList pathParts = path.split('/', QString::SkipEmptyParts);

  CQPropertyViewItem *item =
    hierItem(pathParts, /*create*/true, /*alias*/false, /*hidden*/true);

  assert(item->root() == nullptr || item->root() == obj);

  item->setRoot(obj);
}

void
CQPropertyViewModel::
objectNames(const QObject *object, QStringList &names, bool hidden) const
{
  assert(object);

  CQPropertyViewItem *item = objectItem(object);
  if (! item) return;

  itemNames(item, object, item, names, hidden);
}

void
CQPropertyViewModel::
itemNames(CQPropertyViewItem *rootItem, const QObject *object,
          CQPropertyViewItem *item, QStringList &names, bool hidden) const
{
  if (item->object() && item->object() != object)
    return;

  int num = numItemChildren(item, hidden);

  if (num > 0) {
    for (int i = 0; i < num; ++i) {
      CQPropertyViewItem *item1 = itemChild(item, i, hidden);

      itemNames(rootItem, object, item1, names, hidden);
    }
  }
  else {
    if (! hidden && item->isHidden())
      return;

    QString name = item->path(".", /*alias*/true, rootItem);

    names.push_back(name);
  }
}

CQPropertyViewItem *
CQPropertyViewModel::
item(const QModelIndex &index) const
{
  if (! index.isValid())
    return root();

  bool ok;

  CQPropertyViewItem *item = this->item(index, ok);

  assert(ok);

  return item;
}

CQPropertyViewItem *
CQPropertyViewModel::
item(const QModelIndex &index, bool &ok) const
{
  ok = true;

  CQPropertyViewItem *item = static_cast<CQPropertyViewItem *>(index.internalPointer());

  if (item)
    ok = item->isValid();

  return item;
}

const CQPropertyViewItem *
CQPropertyViewModel::
propertyItem(const QObject *object, const QString &path, bool hidden) const
{
  return propertyItem(object, path, '.', /*create*/false, /*alias*/true, hidden);
}

CQPropertyViewItem *
CQPropertyViewModel::
propertyItem(QObject *object, const QString &path, bool hidden)
{
  return propertyItem(object, path, '.', /*create*/false, /*alias*/true, hidden);
}

const CQPropertyViewItem *
CQPropertyViewModel::
propertyItem(const QObject *object, const QString &path, QChar splitChar,
             bool create, bool alias, bool hidden) const
{
  const CQPropertyViewItem *item = objectItem(object);

  if (! item)
    return nullptr;

  QStringList strs = path.split(splitChar);

  return const_cast<CQPropertyViewModel *>(this)->
           hierItem(const_cast<CQPropertyViewItem *>(item), strs, create, alias, hidden);
}

CQPropertyViewItem *
CQPropertyViewModel::
propertyItem(QObject *object, const QString &path, QChar splitChar,
             bool create, bool alias, bool hidden)
{
  CQPropertyViewItem *item = objectItem(object);

  if (! item)
    return nullptr;

  QStringList strs = path.split(splitChar);

  return hierItem(item, strs, create, alias, hidden);
}

const CQPropertyViewItem *
CQPropertyViewModel::
hierItem(const QStringList &pathParts, bool create, bool alias, bool hidden) const
{
  return const_cast<CQPropertyViewModel *>(this)->
           hierItem(const_cast<CQPropertyViewItem *>(root()), pathParts, create, alias, hidden);
}

CQPropertyViewItem *
CQPropertyViewModel::
hierItem(const QStringList &pathParts, bool create, bool alias, bool hidden)
{
  return hierItem(root(), pathParts, create, alias, hidden);
}

CQPropertyViewItem *
CQPropertyViewModel::
hierItem(CQPropertyViewItem *parentItem, const QStringList &pathParts,
         bool create, bool alias, bool hidden)
{
  if (pathParts.empty())
    return parentItem;

  const QString &path = pathParts[0];

  if (path.length() == 0)
    return nullptr;

  for (const auto &child : itemChildren(parentItem, hidden)) {
    if (! alias) {
      if (! child->object() && child->name() == path) {
        if (pathParts.size() == 1)
          return child;

        return hierItem(child, pathParts.mid(1), create, alias, hidden);
      }
    }
    else {
      if (child->name() == path || child->alias() == path) {
        if (pathParts.size() == 1)
          return child;

        return hierItem(child, pathParts.mid(1), create, alias, hidden);
      }
    }
  }

  if (! create)
    return nullptr;

  CQPropertyViewItem *item = new CQPropertyViewItem(parentItem, nullptr, path);

  parentItem->addChild(item);

  return hierItem(item, pathParts.mid(1), create, alias, hidden);
}

CQPropertyViewItem *
CQPropertyViewModel::
root() const
{
  if (! root_) {
    CQPropertyViewModel *th = const_cast<CQPropertyViewModel *>(this);

    th->root_ = new CQPropertyViewItem(nullptr, nullptr, "");
  }

  return root_;
}

CQPropertyViewItem *
CQPropertyViewModel::
objectItem(const QObject *obj) const
{
  CQPropertyViewItem *root = this->root();

  return objectItem(root, obj);
}

CQPropertyViewItem *
CQPropertyViewModel::
objectItem(CQPropertyViewItem *parent, const QObject *obj) const
{
  // check children
  int num = numItemChildren(parent);

  for (int i = 0; i < num; ++i) {
    CQPropertyViewItem *item = itemChild(parent, i);

    // explicit root for object
    if      (item->root() == obj)
      return item;

    // matching item's root is highest null object parent
    if (item->object() == obj) {
      CQPropertyViewItem *item1 = parent;

      while (item1 && item1->parent() && ! item1->parent()->object())
        item1 = item1->parent();

      return item1;
    }
  }

  // check hier children
  for (int i = 0; i < num; ++i) {
    CQPropertyViewItem *item = itemChild(parent, i);

    CQPropertyViewItem *item1 = objectItem(item, obj);

    if (item1)
      return item1;
  }

  return nullptr;
}

QModelIndex
CQPropertyViewModel::
indexFromItem(CQPropertyViewItem *item, int column) const
{
  CQPropertyViewItem *root = this->root();

  if (item == root)
    return QModelIndex();

  if (item->isHidden() && ! isShowHidden())
    return QModelIndex();

  CQPropertyViewItem *parentItem = item->parent();

  int num = numItemChildren(parentItem);

  for (int i = 0; num; ++i) {
    if (itemChild(parentItem, i) == item) {
      QModelIndex parentInd = indexFromItem(parentItem, 0);

      QModelIndex ind = index(i, column, parentInd);

      return ind;
    }
  }

  return QModelIndex();
}

void
CQPropertyViewModel::
refresh()
{
  int nr = rowCount(QModelIndex());

  beginInsertRows(QModelIndex(), 0, nr);
  endInsertRows  ();
}

void
CQPropertyViewModel::
reset()
{
  beginResetModel();
  endResetModel();
}

void
CQPropertyViewModel::
getChangedNameValues(NameValues &nameValues, bool tcl) const
{
  return getChangedNameValues(nullptr, nameValues, tcl);
}

void
CQPropertyViewModel::
getChangedNameValues(const QObject *obj, NameValues &nameValues, bool tcl) const
{
  getChangedNameValues(obj, obj, nameValues, tcl);
}

void
CQPropertyViewModel::
getChangedNameValues(const QObject *rootObj, const QObject *obj,
                     NameValues &nameValues, bool tcl) const
{
  CQPropertyViewItem *rootItem = this->root();

  if (obj) {
    CQPropertyViewItem *item = objectItem(rootObj);

    if (item)
      rootItem = item;
  }

  return getChangedItemNameValues(rootItem, obj, rootItem, nameValues, tcl);
}

void
CQPropertyViewModel::
getChangedItemNameValues(CQPropertyViewItem *rootItem, const QObject *obj,
                         CQPropertyViewItem *parent, NameValues &nameValues, bool tcl) const
{
  int num = numItemChildren(parent);

  for (int i = 0; i < num; ++i) {
    CQPropertyViewItem *item = itemChild(parent, i);

    int num1 = numItemChildren(item);

    if (num1 > 0) {
      getChangedItemNameValues(rootItem, obj, item, nameValues, tcl);
    }
    else {
      if (! item->isEditable())
        continue;

      if (obj && item->object() != obj)
        continue;

      QString initStr = item->initStr();
      QString dataStr = item->dataStr();

      if (initStr != dataStr)
        addNameValue(rootItem, item, nameValues, tcl);
    }
  }
}

void
CQPropertyViewModel::
addNameValue(CQPropertyViewItem *rootItem, CQPropertyViewItem *item,
             NameValues &nameValues, bool tcl) const
{
  QString path = item->path(".", /*alias*/true, rootItem);

  if (tcl)
    nameValues[path] = item->tclData();
  else
    nameValues[path] = item->dataStr();
}

//------

int
CQPropertyViewModel::
numItemChildren(CQPropertyViewItem *item, bool hidden) const
{
  if (hidden || isShowHidden())
    return item->numChildren();
  else
    return item->numVisibleChildren();
}

const CQPropertyViewModel::Children &
CQPropertyViewModel::
itemChildren(CQPropertyViewItem *item, bool hidden) const
{
  if (hidden || isShowHidden())
    return item->children();
  else
    return item->visibleChildren();
}

CQPropertyViewItem *
CQPropertyViewModel::
itemChild(CQPropertyViewItem *item, int i, bool hidden) const
{
  if (hidden || isShowHidden())
    return item->child(i);
  else
    return item->visibleChild(i);
}
