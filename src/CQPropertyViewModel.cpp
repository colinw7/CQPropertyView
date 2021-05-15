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
  auto *parentItem = this->item(parent);

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
  auto *item = this->item(index);

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
  else if (role == Qt::BackgroundRole) {
    if (item->isDirty())
      return QColor(240, 100, 100); // red
  }

  return QVariant();
}

bool
CQPropertyViewModel::
setData(const QModelIndex &index, const QVariant &value, int role)
{
  auto *item = this->item(index);

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
  auto *parentItem = this->item(parent);

  if (! parentItem)
    return QModelIndex();

  if (row < 0 || row >= numItemChildren(parentItem))
    return QModelIndex();

  if (column < 0 || column >= 2)
    return QModelIndex();

  auto *childItem = itemChild(parentItem, row);

  auto ind = createIndex(row, column, childItem);

  assert(this->item(ind) == childItem);

  return ind;
}

QModelIndex
CQPropertyViewModel::
parent(const QModelIndex &index) const
{
  auto *item = this->item(index);

  if (! item)
    return QModelIndex();

  auto *parent = item->parent();

  if (! parent || parent == root())
    return QModelIndex();

  auto *parentParent = parent->parent();

  if (! parentParent)
    parentParent = root();

  //---

  int i = 0;

  for (const auto &child : itemChildren(parentParent)) {
    if (child == parent) {
      auto ind = createIndex(i, 0, parent);

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
  auto *item = this->item(index);

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

  auto pathParts = path.split('/', QString::SkipEmptyParts);

  auto *parentItem = hierItem(pathParts, /*create*/true, /*alias*/false, /*hidden*/true);

  auto *item = new CQPropertyViewItem(this, parentItem, object, name);

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
  auto *item = propertyItem(object, path, '.', /*create*/false, /*alias*/true, /*hidden*/true);

  if (! item)
    return false;

  if (! item->setData(value))
    return false;

  auto ind1 = indexFromItem(item, 0);
  auto ind2 = indexFromItem(item, 1);

  emit dataChanged(ind1, ind2);

  return true;
}

bool
CQPropertyViewModel::
getProperty(const QObject *object, const QString &path, QVariant &value) const
{
  const auto *item =
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
  const auto *item =
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

  auto pathParts = path.split('/', QString::SkipEmptyParts);

  auto *item = hierItem(pathParts, /*create*/false, /*alias*/false, /*hidden*/true);

  if (item && item->parent())
    item->parent()->removeChild(item);

  endResetModel();
}

void
CQPropertyViewModel::
hideProperty(const QString &path, const QObject *object)
{
  auto *item = propertyItem(const_cast<QObject *>(object), path, '/',
                            /*create*/false, /*alias*/false, /*hidden*/true);
  if (! item) return;

  item->setHidden(true);
}

bool
CQPropertyViewModel::
nameToPath(const QObject *object, const QString &name, QString &path) const
{
  assert(object);

  auto *item = objectItem(object);
  if (! item) return false;

  auto *item1 = getNamedItem(item, name);
  if (! item1) return false;

  path = item1->path(".", /*alias*/true, item);

  return true;
}

CQPropertyViewItem *
CQPropertyViewModel::
getNamedItem(CQPropertyViewItem *item, const QString &name) const
{
  for (const auto &child : itemChildren(item)) {
    if (child->object() && child->name() == name)
      return child;

    auto *item1 = getNamedItem(child, name);

    if (item1)
      return item1;
  }

  return nullptr;
}

void
CQPropertyViewModel::
setObjectRoot(const QString &path, QObject *obj)
{
  auto pathParts = path.split('/', QString::SkipEmptyParts);

  auto *item = hierItem(pathParts, /*create*/true, /*alias*/false, /*hidden*/true);

  assert(item->root() == nullptr || item->root() == obj);

  item->setRoot(obj);
}

void
CQPropertyViewModel::
objectNames(const QObject *object, QStringList &names, bool hidden) const
{
  assert(object);

  auto *item = objectItem(object);
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
      auto *item1 = itemChild(item, i, hidden);

      itemNames(rootItem, object, item1, names, hidden);
    }
  }
  else {
    if (! hidden && item->isHidden())
      return;

    auto name = item->path(".", /*alias*/true, rootItem);

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

  auto *item = this->item(index, ok);

  assert(ok);

  return item;
}

CQPropertyViewItem *
CQPropertyViewModel::
item(const QModelIndex &index, bool &ok) const
{
  ok = true;

  auto *item = static_cast<CQPropertyViewItem *>(index.internalPointer());

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
  const auto *item = objectItem(object);

  if (! item)
    return nullptr;

  auto strs = path.split(splitChar);

  return const_cast<CQPropertyViewModel *>(this)->
           hierItem(const_cast<CQPropertyViewItem *>(item), strs, create, alias, hidden);
}

CQPropertyViewItem *
CQPropertyViewModel::
propertyItem(QObject *object, const QString &path, QChar splitChar,
             bool create, bool alias, bool hidden)
{
  auto *item = objectItem(object);

  if (! item)
    return nullptr;

  auto strs = path.split(splitChar);

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

  const auto &path = pathParts[0];

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

  auto *item = new CQPropertyViewItem(this, parentItem, nullptr, path);

  parentItem->addChild(item);

  return hierItem(item, pathParts.mid(1), create, alias, hidden);
}

CQPropertyViewItem *
CQPropertyViewModel::
root() const
{
  if (! root_) {
    auto *th = const_cast<CQPropertyViewModel *>(this);

    th->root_ = new CQPropertyViewItem(th, nullptr, nullptr, "");
  }

  return root_;
}

CQPropertyViewItem *
CQPropertyViewModel::
objectItem(const QObject *obj) const
{
  auto *root = this->root();

  return objectItem(root, obj);
}

CQPropertyViewItem *
CQPropertyViewModel::
objectItem(CQPropertyViewItem *parent, const QObject *obj) const
{
  // check children
  int num = numItemChildren(parent);

  for (int i = 0; i < num; ++i) {
    auto *item = itemChild(parent, i);

    // explicit root for object
    if      (item->root() == obj)
      return item;

    // matching item's root is highest null object parent
    if (item->object() == obj) {
      auto *item1 = parent;

      while (item1 && item1->parent() && ! item1->parent()->object())
        item1 = item1->parent();

      return item1;
    }
  }

  // check hier children
  for (int i = 0; i < num; ++i) {
    auto *item = itemChild(parent, i);

    auto *item1 = objectItem(item, obj);

    if (item1)
      return item1;
  }

  return nullptr;
}

QModelIndex
CQPropertyViewModel::
indexFromItem(CQPropertyViewItem *item, int column) const
{
  auto *root = this->root();

  if (item == root)
    return QModelIndex();

  if (item->isHidden() && ! isShowHidden())
    return QModelIndex();

  auto *parentItem = item->parent();

  int num = numItemChildren(parentItem);

  for (int i = 0; num; ++i) {
    if (itemChild(parentItem, i) == item) {
      auto parentInd = indexFromItem(parentItem, 0);

      auto ind = index(i, column, parentInd);

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

//------

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
  auto *rootItem = this->root();

  if (obj) {
    auto *item = objectItem(rootObj);

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
    auto *item = itemChild(parent, i);

    int num1 = numItemChildren(item);

    if (num1 > 0) {
      getChangedItemNameValues(rootItem, obj, item, nameValues, tcl);
    }
    else {
      if (! item->isEditable())
        continue;

      if (obj && item->object() != obj)
        continue;

      auto initStr = item->initStr();
      auto dataStr = item->dataStr();

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
  auto path = item->path(".", /*alias*/true, rootItem);

  if (tcl)
    nameValues[path] = item->tclData();
  else
    nameValues[path] = item->dataStr();
}

//-----

void
CQPropertyViewModel::
updateDirty()
{
  Items items;

  auto *rootItem = this->root();

  getDirtyItems(rootItem, items);

  for (auto &item : items)
    item->applyDirty();
}

void
CQPropertyViewModel::
getDirtyItems(CQPropertyViewItem *parent, Items &items) const
{
  int num = numItemChildren(parent);

  for (int i = 0; i < num; ++i) {
    auto *item = itemChild(parent, i);

    int num1 = numItemChildren(item);

    if (num1 > 0) {
      getDirtyItems(item, items);
    }
    else {
      if (item->isDirty())
        items.push_back(item);
    }
  }
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
