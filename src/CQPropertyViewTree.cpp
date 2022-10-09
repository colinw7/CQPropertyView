#include <CQPropertyViewTree.h>
#include <CQPropertyViewFilter.h>
#include <CQPropertyViewModel.h>
#include <CQPropertyViewDelegate.h>
#include <CQPropertyViewItem.h>
#include <CQHeaderView.h>

#include <QApplication>
#include <QHeaderView>
#include <QMouseEvent>
#include <QClipboard>
#include <QMenu>
#include <set>
#include <iostream>

CQPropertyViewTree::
CQPropertyViewTree(QWidget *parent) :
 QTreeView(parent), model_(new CQPropertyViewModel)
{
  init();

  modelAllocated_ = true;
}

CQPropertyViewTree::
CQPropertyViewTree(QWidget *parent, CQPropertyViewModel *model) :
 QTreeView(parent), model_(model)
{
  init();
}

CQPropertyViewTree::
~CQPropertyViewTree()
{
  if (modelAllocated_)
    delete model_;

  delete filter_;
}

void
CQPropertyViewTree::
init()
{
  setObjectName("propertyView");

  //--

  filter_ = new CQPropertyViewFilter(this);

  if (model_) {
    connect(model_, SIGNAL(valueChanged(QObject *, const QString &)),
            this, SIGNAL(valueChanged(QObject *, const QString &)));
    connect(model_, SIGNAL(valueChanged(QObject *, const QString &)),
            this, SLOT(redraw()));

    model_->setTree(this);

    filter_->setSourceModel(model_);

    setModel(filter_);
  }

  //--

  setHeader(new CQHeaderView(this));

  header()->setStretchLastSection(true);
//header()->setSectionResizeMode(QHeaderView::Interactive);
//header()->setSectionResizeMode(QHeaderView::ResizeToContents);

  //--

  setSelectionMode(ExtendedSelection);

  setUniformRowHeights(true);

  setAlternatingRowColors(true);

  setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);

  //--

  // Set Item Delegate
  delegate_ = new CQPropertyViewDelegate(this);

  setItemDelegate(delegate_);

  connect(delegate_, SIGNAL(closeEditor(QWidget*, QAbstractItemDelegate::EndEditHint)),
          this, SLOT(closeEditorSlot(QWidget*, QAbstractItemDelegate::EndEditHint)));

  //--

  // handle click (for bool check box)
  connect(this, SIGNAL(clicked(const QModelIndex &)),
          this, SLOT(itemClickedSlot(const QModelIndex &)));

  //---

  // handle selection
  auto *sm = this->selectionModel();

  if (sm)
    connect(sm, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(itemSelectionSlot()));

  //---

  // add menu
  setContextMenuPolicy(Qt::CustomContextMenu);

  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
          this, SLOT(customContextMenuSlot(const QPoint&)));
}

void
CQPropertyViewTree::
setPropertyModel(CQPropertyViewModel *model)
{
  // disconnect current model
  if (model_) {
    disconnect(model_, SIGNAL(valueChanged(QObject *, const QString &)),
               this, SIGNAL(valueChanged(QObject *, const QString &)));
    disconnect(model_, SIGNAL(valueChanged(QObject *, const QString &)),
               this, SLOT(redraw()));

    model_->setTree(nullptr);
  }

  //---

  model_ = model;

  //---

  // connect new model
  if (model_) {
    connect(model_, SIGNAL(valueChanged(QObject *, const QString &)),
            this, SIGNAL(valueChanged(QObject *, const QString &)));
    connect(model_, SIGNAL(valueChanged(QObject *, const QString &)),
            this, SLOT(redraw()));

    model_->setTree(this);

    filter_->setSourceModel(model_);

    setModel(filter_);
  }
}

void
CQPropertyViewTree::
setMouseHighlight(bool b)
{
  mouseHighlight_ = b;

  setMouseTracking(mouseHighlight_);
}

void
CQPropertyViewTree::
setFilter(const QString &filter)
{
  filter_->setFilter(filter);
}

void
CQPropertyViewTree::
modelResetSlot()
{
  //std::cerr << "model reset" << std::endl;
}

void
CQPropertyViewTree::
redraw()
{
  viewport()->update();
}

void
CQPropertyViewTree::
clear()
{
  model_->clear();
}

void
CQPropertyViewTree::
addProperty(const QString &path, QObject *obj, const QString &name, const QString &alias)
{
  model_->addProperty(path, obj, name, alias);
}

bool
CQPropertyViewTree::
setProperty(QObject *object, const QString &path, const QVariant &value)
{
  bool rc = model_->setProperty(object, path, value);

  redraw();

  return rc;
}

bool
CQPropertyViewTree::
getProperty(const QObject *object, const QString &path, QVariant &value) const
{
  return model_->getProperty(object, path, value);
}

void
CQPropertyViewTree::
selectObject(const QObject *obj)
{
  auto *root = model_->root();

  for (int i = 0; i < model_->numItemChildren(root); ++i) {
    auto *item = model_->itemChild(root, i);

    if (selectObject(item, obj))
      return;
  }
}

bool
CQPropertyViewTree::
selectObject(CQPropertyViewItem *item, const QObject *obj)
{
  auto *obj1 = item->object();

  if (obj1 == obj) {
    if (item->parent()) {
      selectItem(item->parent(), true);
      return true;
    }
  }

  for (int i = 0; i < model_->numItemChildren(item); ++i) {
    auto *item1 = model_->itemChild(item, i);

    if (selectObject(item1, obj))
      return true;
  }

  return false;
}

void
CQPropertyViewTree::
deselectAllObjects()
{
  auto *sm = this->selectionModel();

  sm->clear();
}

bool
CQPropertyViewTree::
setCurrentProperty(QObject *object, const QString &path)
{
  auto *item = model_->propertyItem(object, path);

  if (! item)
    return false;

  auto ind = indexFromItem(item, 0, /*map*/true);

  if (! ind.isValid())
    return false;

  setCurrentIndex(ind);

  return true;
}

void
CQPropertyViewTree::
resizeColumns()
{
  resizeColumnToContents(0);
  resizeColumnToContents(1);

  header()->setStretchLastSection(false);
  header()->setStretchLastSection(true);
}

void
CQPropertyViewTree::
expandAll()
{
  auto *root = model_->root();

  expandAll(root);
}

void
CQPropertyViewTree::
expandAll(CQPropertyViewItem *item)
{
  expandItemTree(item);

  for (int i = 0; i < model_->numItemChildren(item); ++i) {
    auto *item1 = model_->itemChild(item, i);

    expandAll(item1);
  }
}

void
CQPropertyViewTree::
collapseAll()
{
  auto *root = model_->root();

  collapseAll(root);
}

void
CQPropertyViewTree::
collapseAll(CQPropertyViewItem *item)
{
  collapseItemTree(item);

  for (int i = 0; i < model_->numItemChildren(item); ++i) {
    auto *item1 = model_->itemChild(item, i);

    collapseAll(item1);
  }
}

void
CQPropertyViewTree::
expandSelected()
{
  auto indices = this->selectedIndexes();

  for (int i = 0; i < indices.length(); ++i) {
    auto *item = getModelItem(indices[i]);

    expandItemTree(item);
  }

  resizeColumns();

  for (int i = 0; i < indices.length(); ++i) {
    auto *item = getModelItem(indices[i]);

    scrollToItem(item);
  }
}

void
CQPropertyViewTree::
getSelectedObjects(Objs &objs)
{
  auto indices = this->selectedIndexes();

  for (int i = 0; i < indices.length(); ++i) {
    auto *item = getModelItem(indices[i]);

    QObject *obj;
    QString  path;

    getItemData(item, obj, path);

    objs.push_back(obj);
  }
}

//---

bool
CQPropertyViewTree::
isShowHidden() const
{
  return (model_ && model_->isShowHidden());
}

void
CQPropertyViewTree::
setShowHidden(bool b)
{
  if (model_ && b != model_->isShowHidden()) {
    saveState();

    model_->setShowHidden(b);

    model_->reset();

    restoreState();
  }
}

//---

void
CQPropertyViewTree::
saveState()
{
  assert(model_);

  stateData_.expandPaths.clear();

  auto ind = indexAt(QPoint(0, 0));

  stateData_.topItem.clear();

  itemPath(ind, stateData_.topItem);

//std::cerr << "Top : "; printPath(stateData_.topItem);

  stateData_.topIndex = QModelIndex();

  saveState1(QModelIndex(), stateData_, 0);
}

void
CQPropertyViewTree::
saveState1(const QModelIndex &parent, StateData &stateData, int depth)
{
  auto *filterModel = this->filterModel();

  int nr = filterModel->rowCount(parent);

  for (int r = 0; r < nr; ++r) {
    auto ind = filterModel->index(r, 0, parent);

    if (! filterModel->hasChildren(ind))
      continue;

    if (isExpanded(ind)) {
      ItemPath path;

      itemPath(ind, path);

      stateData.expandPaths[depth].push_back(path);

    //std::cerr << "Save : "; printPath(path);
    }

    saveState1(ind, stateData, depth + 1);
  }
}

void
CQPropertyViewTree::
restoreState()
{
  assert(model_);

  restoreState1(QModelIndex(), stateData_, 0);

  redraw();

  if (stateData_.topIndex.isValid()) {
#if 0
    ItemPath topItem;
    auto ind1 = indexAt(QPoint(0, 0));
    itemPath(ind1, topItem);
    std::cerr << "Top : "; printPath(topItem);

    auto rect = visualRect(stateData_.topIndex);
    int dy = rect.top();
    std::cerr << "DY : " << dy << "\n";
#endif

    scrollTo(stateData_.topIndex, QAbstractItemView::EnsureVisible);

#if 0
    rect = visualRect(stateData_.topIndex);
    dy = rect.top();
    std::cerr << "DY : " << dy << "\n";

    topItem.clear();
    ind1 = indexAt(QPoint(0, 0));
    itemPath(ind1, topItem);
    std::cerr << "Top : "; printPath(topItem);
#endif

    scrollTo(stateData_.topIndex, QAbstractItemView::PositionAtTop);

#if 0
    rect = visualRect(stateData_.topIndex);
    dy = rect.top();
    std::cerr << "DY : " << dy << "\n";

    topItem.clear();
    ind1 = indexAt(QPoint(0, 0));
    itemPath(ind1, topItem);
    std::cerr << "Top : "; printPath(topItem);
#endif
  }
}

void
CQPropertyViewTree::
restoreState1(const QModelIndex &parent, StateData &stateData, int depth)
{
  auto pd = stateData.expandPaths.find(depth);

  //---

  auto itemMatch = [&](const ItemPath &path1, const ItemPath &path2) {
    int np = path1.length();

    if (path2.length() != np)
      return false;

    for (int i = 0; i < np; ++i) {
      if (path1[i] != path2[i])
        return false;
    }

    return true;
  };

  auto hasPath = [&](const ItemPath &path) {
    auto &itemPaths = (*pd).second;

    for (const auto &path1 : itemPaths) {
      if (itemMatch(path, path1))
        return true;
    }

    return false;
  };

  //---

  auto *filterModel = this->filterModel();

  int nr = filterModel->rowCount(parent);

  for (int r = 0; r < nr; ++r) {
    auto ind = filterModel->index(r, 0, parent);

    ItemPath path;

    itemPath(ind, path);

    if (itemMatch(stateData_.topItem, path))
      stateData.topIndex = ind;

    if (! filterModel->hasChildren(ind))
      continue;

    if (pd != stateData.expandPaths.end()) {
      if (hasPath(path)) {
        setExpanded(ind, true);

      //std::cerr << "Restore : "; printPath(path);
      }
    }

    restoreState1(ind, stateData, depth + 1);
  }
}

void
CQPropertyViewTree::
itemPath(const QModelIndex &ind, ItemPath &path) const
{
  auto *filterModel = this->filterModel();

  if (ind.parent().isValid())
    itemPath(ind.parent(), path);

  auto str = filterModel->data(ind, Qt::DisplayRole).toString();

  path.push_back(str);
}

#if 0
void
CQPropertyViewTree::
printPath(const ItemPath &path) const
{
  std::cerr << path.join("|").toStdString() << "\n";
}
#endif

//---

void
CQPropertyViewTree::
autoUpdateSlot(bool b)
{
  if (! model_)
    return;

  if (b)
    updateDirtySlot();

  model_->setAutoUpdate(b);
}

void
CQPropertyViewTree::
updateDirtySlot()
{
  if (! model_)
    return;

  emit startUpdate();

  model_->updateDirty();

  emit endUpdate();
}

//---

void
CQPropertyViewTree::
search(const QString &text)
{
  auto searchStr = text;

  if (searchStr.length() && searchStr[searchStr.length() - 1] != '*')
    searchStr += "*";

  if (searchStr.length() && searchStr[0] != '*')
    searchStr = "*" + searchStr;

  QRegExp regexp(searchStr, Qt::CaseSensitive, QRegExp::Wildcard);

  auto *root = model_->root();

  // get matching items
  Items items;

  for (int i = 0; i < model_->numItemChildren(root); ++i) {
    auto *item = model_->itemChild(root, i);

    searchItemTree(item, regexp, items);
  }

  // select matching items
  auto *sm = this->selectionModel();

  sm->clear();

  for (uint i = 0; i < items.size(); ++i) {
    auto *item = items[i];

    selectItem(item, true);
  }

  //---

  // ensure selection expanded
  for (uint i = 0; i < items.size(); ++i) {
    auto *item = items[i];

    expandItemTree(item);
  }

  //---

  // make item visible
  resizeColumns();

  for (uint i = 0; i < items.size(); ++i) {
    auto *item = items[i];

    scrollToItem(item);
  }
}

void
CQPropertyViewTree::
searchItemTree(CQPropertyViewItem *item, const QRegExp &regexp, Items &items)
{
  auto itemText = item->aliasName();

  if (regexp.exactMatch(itemText))
    items.push_back(item);

  int n = model_->numItemChildren(item);

  for (int i = 0; i < n; ++i) {
    auto *item1 = model_->itemChild(item, i);

    searchItemTree(item1, regexp, items);
  }
}

void
CQPropertyViewTree::
expandItemTree(CQPropertyViewItem *item)
{
  while (item) {
    expandItem(item);

    item = item->parent();
  }
}

void
CQPropertyViewTree::
collapseItemTree(CQPropertyViewItem *item)
{
  while (item) {
    collapseItem(item);

    item = item->parent();
  }
}

void
CQPropertyViewTree::
itemClickedSlot(const QModelIndex &index)
{
  auto *item = getModelItem(index);

  if (item && index.column() == 1) {
    if (item->click()) {
      update(index);
    }
  }

  //---

  QObject *obj;
  QString  path;

  getItemData(item, obj, path);

  emit itemClicked(obj, path);
}

void
CQPropertyViewTree::
itemSelectionSlot()
{
  // filter model indices
  auto indices = this->selectedIndexes();
  if (indices.empty()) return;

  auto ind = indices[0];

  assert(ind.model() == filter_);

  auto *item = getModelItem(ind);

  QObject *obj;
  QString  path;

  getItemData(item, obj, path);

  emit itemSelected(obj, path);
}

CQPropertyViewItem *
CQPropertyViewTree::
getModelItem(const QModelIndex &ind, bool map) const
{
  if (map) {
    bool ok;

    auto *item = model_->item(ind, ok);

    if (! item)
      return nullptr;

    assert(! ok);

    auto ind1 = filter_->mapToSource(ind);

    auto *item1 = model_->item(ind1);

    return item1;
  }
  else {
    auto *item = model_->item(ind);

    return item;
  }
}

void
CQPropertyViewTree::
getItemData(CQPropertyViewItem *item, QObject* &obj, QString &path)
{
  path = item->path("/");

  //---

  // use object from first branch child
  auto *item1 = item;

  int n = model_->numItemChildren(item1);

  while (n > 0) {
    item1 = model_->itemChild(item1, 0);

    n = model_->numItemChildren(item1);
  }

  obj = item1->object();
}

void
CQPropertyViewTree::
customContextMenuSlot(const QPoint &pos)
{
  // Map point to global from the viewport to account for the header.
  menuPos_ = viewport()->mapToGlobal(pos);

  menuItem_ = getModelItem(indexAt(pos));

  if (isItemMenu()) {
    if (menuItem_) {
      QObject *obj;
      QString  path;

      getItemData(menuItem_, obj, path);

      if (obj) {
        showContextMenu(obj, menuPos_);

        return;
      }
    }
  }

  //---

  auto *menu = new QMenu;

  //---

  addMenuItems(menu);

  //---

  menu->exec(menuPos_);

  delete menu;
}

void
CQPropertyViewTree::
addMenuItems(QMenu *menu)
{
  addStandardMenuItems(menu);
}

void
CQPropertyViewTree::
addStandardMenuItems(QMenu *menu)
{
  auto addAction = [&](const QString &text, const char *slotName) {
    auto *action = new QAction(text, menu);

    connect(action, SIGNAL(triggered()), this, slotName);

    menu->addAction(action);

    return action;
  };

  auto addCheckAction = [&](const QString &text, bool checked, const char *slotName) {
    auto *action = new QAction(text, menu);

    action->setCheckable(true);
    action->setChecked(checked);

    connect(action, SIGNAL(triggered(bool)), this, slotName);

    menu->addAction(action);

    return action;
  };

  //---

  (void) addAction("Expand All"  , SLOT(expandAll()));
  (void) addAction("Collapse All", SLOT(collapseAll()));

  //---

  menu->addSeparator();

  (void) addCheckAction("Show Hidden", isShowHidden(), SLOT(setShowHidden(bool)));

  //---

  auto *copyAction = addAction("Copy", SLOT(copySlot()));

  copyAction->setShortcut(QKeySequence::Copy);

  //---

  if (model()) {
    menu->addSeparator();

    (void) addCheckAction("Auto Update", model_->isAutoUpdate(), SLOT(autoUpdateSlot(bool)));

    if (! model_->isAutoUpdate())
      (void) addAction("Update Dirty", SLOT(updateDirtySlot()));
  }

  //---

  menu->addSeparator();

  (void) addAction("Print"        , SLOT(printSlot()));
  (void) addAction("Print Changed", SLOT(printChangedSlot()));
}

void
CQPropertyViewTree::
showContextMenu(QObject *obj, const QPoint &globalPos)
{
  emit menuExec(obj, globalPos);
}

void
CQPropertyViewTree::
mouseMoveEvent(QMouseEvent *me)
{
  if (! isMouseHighlight())
    return;

  auto ind = indexAt(me->pos());

  if (ind.isValid()) {
    auto *item = getModelItem(ind);

    if (item) {
      if (! isMouseInd(ind)) {
        setMouseInd(ind);

        redraw();
      }

      return;
    }
  }

  if (! isMouseInd(QModelIndex())) {
    unsetMouseInd();

    redraw();
  }
}

void
CQPropertyViewTree::
leaveEvent(QEvent *)
{
  if (! isMouseHighlight()) return;

  unsetMouseInd();

  redraw();
}

void
CQPropertyViewTree::
keyPressEvent(QKeyEvent *ke)
{
  if (ke->matches(QKeySequence::Copy)) {
    auto p = QCursor::pos();

    copyAt(p, /*html*/false);
  }
  else if (ke->key() == Qt::Key_Escape) {
    closeCurrentEditor();
  }
  else
    QTreeView::keyPressEvent(ke);
}

void
CQPropertyViewTree::
showEvent(QShowEvent *)
{
  if (! shown_) {
    if (isResizeOnShow())
      resizeColumns();

    shown_ = true;
  }
}

void
CQPropertyViewTree::
resizeEvent(QResizeEvent *e)
{
  QTreeView::resizeEvent(e);
}

void
CQPropertyViewTree::
scrollToItem(CQPropertyViewItem *item)
{
  auto ind = indexFromItem(item, 0, /*map*/true);

  if (ind.isValid())
    scrollTo(ind);
}

void
CQPropertyViewTree::
selectItem(CQPropertyViewItem *item, bool selected)
{
  auto *sm = this->selectionModel();

  auto ind = indexFromItem(item, 0, /*map*/true);

  if (ind.isValid()) {
    if (selected) {
      sm->select(ind, QItemSelectionModel::Select);
    }
    else {
    //sm->select(ind, QItemSelectionModel::Deselect);
    }
  }
}

void
CQPropertyViewTree::
expandItem(CQPropertyViewItem *item)
{
  auto ind = indexFromItem(item, 0, /*map*/true);

  if (ind.isValid())
    setExpanded(ind, true);
}

void
CQPropertyViewTree::
collapseItem(CQPropertyViewItem *item)
{
  auto ind = indexFromItem(item, 0, /*map*/true);

  if (ind.isValid())
    setExpanded(ind, false);
}

void
CQPropertyViewTree::
editItem(CQPropertyViewItem *item)
{
  auto ind = indexFromItem(item, 1, /*map*/true);

  if (ind.isValid())
    edit(ind);
}

void
CQPropertyViewTree::
copySlot() const
{
  copyAt(menuPos_, /*html*/false);
}

void
CQPropertyViewTree::
copyAt(const QPoint &p, bool html) const
{
  auto ind = indexAt(viewport()->mapFromGlobal(p));

  if (ind.isValid()) {
    auto *item = getModelItem(ind);
    if (! item) return;

    QString value;

    if      (ind.column() == 0)
      value = item->nameTip(html);
    else if (ind.column() == 1)
      value = item->valueTip(html);
    else
      return;

    auto *clipboard = QApplication::clipboard();

    clipboard->setText(value, QClipboard::Clipboard);
    clipboard->setText(value, QClipboard::Selection);
  }
}

void
CQPropertyViewTree::
printSlot() const
{
  auto indices = this->selectionModel()->selectedRows();

  for (int i = 0; i < indices.length(); ++i) {
    auto *item = getModelItem(indices[i]);

    auto path = item->path(".", /*alias*/true);

    std::cerr << path.toStdString() << "=" << item->dataStr().toStdString() << "\n";
  }
}

void
CQPropertyViewTree::
printChangedSlot() const
{
  CQPropertyViewModel::NameValues nameValues;

  model_->getChangedNameValues(nameValues, /*tcl*/false);

  for (const auto &nv : nameValues)
    std::cerr << nv.first.toStdString() << "=" << nv.second.toString().toStdString() << "\n";
}

void
CQPropertyViewTree::
closeEditorSlot()
{
  closeCurrentEditor();
}

void
CQPropertyViewTree::
closeCurrentEditor()
{
  auto *editor = delegate_->getEditor();
  if (! editor) return;

  if (! delegate_->isEditing())
    return;

  delegate_->setModelData(editor, model(), delegate_->getEditorIndex());

  // turn off edit triggers so we don't start a new editor
  auto triggers = editTriggers();

  setEditTriggers(QAbstractItemView::NoEditTriggers);

  // close editor
  QAbstractItemView::closeEditor(editor, QAbstractItemDelegate::NoHint);

  // restore edit triggers
  setEditTriggers(triggers);

  //setSelectedIndex(delegate_->getEditorIndex().row());

  delegate_->setEditing(false);
}

void
CQPropertyViewTree::
closeEditorSlot(QWidget *, QAbstractItemDelegate::EndEditHint)
{
  delegate_->setEditing(false);
}

QModelIndex
CQPropertyViewTree::
indexFromItem(CQPropertyViewItem *item, int column, bool map) const
{
  auto ind = model_->indexFromItem(item, column);

  if (! ind.isValid())
    return QModelIndex();

  if (map) {
    auto *filterModel = this->filterModel();

    return filterModel->mapFromSource(ind);
  }

  return ind;
}

void
CQPropertyViewTree::
setMouseInd(const QModelIndex &i)
{
  assert(i.isValid());

  hasMouseInd_ = true;
  mouseInd_    = i;
}

void
CQPropertyViewTree::
unsetMouseInd()
{
  hasMouseInd_ = false;
  mouseInd_    = QModelIndex();
}

bool
CQPropertyViewTree::
isMouseInd(const QModelIndex &i)
{
  if (! isMouseHighlight())
    return false;

  if (i.isValid()) {
    if (! hasMouseInd_)
      return false;

    assert(i.model() == mouseInd_.model());

    if (mouseInd_.parent() != i.parent())
      return false;

    return (mouseInd_.row() == i.row());
  }
  else {
    return ! hasMouseInd_;
  }
}
