#ifndef CQPropertyViewTree_H
#define CQPropertyViewTree_H

#include <QTreeView>
#include <QAbstractItemDelegate>

#include <map>
#include <vector>

class CQPropertyViewModel;
class CQPropertyViewItem;
class CQPropertyViewFilter;
class CQPropertyViewDelegate;
class CQHeaderView;

/*!
 * \brief Property view tree view
 */
class CQPropertyViewTree : public QTreeView {
  Q_OBJECT

  Q_PROPERTY(bool itemMenu       READ isItemMenu       WRITE setItemMenu      )
  Q_PROPERTY(bool showHidden     READ isShowHidden     WRITE setShowHidden    )
  Q_PROPERTY(bool mouseHighlight READ isMouseHighlight WRITE setMouseHighlight)
  Q_PROPERTY(bool resizeOnShow   READ isResizeOnShow   WRITE setResizeOnShow  )
  Q_PROPERTY(bool resizeOnExpand READ isResizeOnExpand WRITE setResizeOnExpand)

 public:
  using Items = std::vector<CQPropertyViewItem *>;
  using Objs  = std::vector<QObject *>;

 public:
  CQPropertyViewTree(QWidget *parent=nullptr);
  CQPropertyViewTree(QWidget *parent, CQPropertyViewModel *model);

  virtual ~CQPropertyViewTree();

  CQPropertyViewModel *propertyModel() const { return model_; }
  void setPropertyModel(CQPropertyViewModel *model);

  CQPropertyViewFilter *filterModel() const { return filter_; }

  void clear();

  void addProperty(const QString &path, QObject *obj,
                   const QString &name, const QString &alias="");

  bool setProperty(QObject *object, const QString &path, const QVariant &value);
  bool getProperty(const QObject *object, const QString &path, QVariant &value) const;

  void selectObject(const QObject *obj);

  void deselectAllObjects();

  bool setCurrentProperty(QObject *object, const QString &path);

  void getSelectedObjects(Objs &objs);

  bool isItemMenu() const { return itemMenu_; }
  void setItemMenu(bool b) { itemMenu_ = b; }

  bool isShowHidden() const;

  bool isMouseHighlight() const { return mouseHighlight_; }
  void setMouseHighlight(bool b);

  bool isResizeOnShow() const { return resizeOnShow_; }
  void setResizeOnShow(bool b) { resizeOnShow_ = b; }

  bool isResizeOnExpand() const { return resizeOnExpand_; }
  void setResizeOnExpand(bool b) { resizeOnExpand_ = b; }

  void setMouseInd(const QModelIndex &i);
  void unsetMouseInd();
  bool isMouseInd(const QModelIndex &i);

  CQPropertyViewItem *getModelItem(const QModelIndex &index, bool map=true) const;

  void resizeColumns();

  void expandAll(CQPropertyViewItem *item);
  void collapseAll(CQPropertyViewItem *item);

  void scrollToItem(CQPropertyViewItem *item);

  void expandItem(CQPropertyViewItem *item);
  void collapseItem(CQPropertyViewItem *item);

  void selectItem(CQPropertyViewItem *item, bool selected);

  void editItem(CQPropertyViewItem *item);

  void closeCurrentEditor();

  virtual void addMenuItems(QMenu *menu);

  void addStandardMenuItems(QMenu *menu);

  CQPropertyViewItem *menuItem() const { return menuItem_; }

  virtual void initEditor(QWidget *) { }

 Q_SIGNALS:
  void valueChanged(QObject *, const QString &);

  void itemClicked( QObject *obj, const QString &path);
  void itemSelected(QObject *obj, const QString &path);

  void menuExec(QObject *obj, const QPoint &gpos);

  void startUpdate();
  void endUpdate();

 public Q_SLOTS:
  void redraw();

  void expandAll();
  void collapseAll();

  void expandSelected();

  void setShowHidden(bool);

  void setFilter(const QString &filter);
  void search(const QString &text);

  void copySlot() const;

  void printSlot() const;
  void printChangedSlot() const;

  void closeEditorSlot();

 protected Q_SLOTS:
  void modelResetSlot();

  void itemClickedSlot(const QModelIndex &index);

  void expandSlot(const QModelIndex &);

  void itemSelectionSlot();

  void customContextMenuSlot(const QPoint &pos);

  void closeEditorSlot(QWidget *, QAbstractItemDelegate::EndEditHint);

  void autoUpdateSlot(bool);

  void updateDirtySlot();

 protected:
  void init();

  void copyAt(const QPoint &p, bool html) const;

  bool selectObject(CQPropertyViewItem *item, const QObject *obj);

  void getItemData(CQPropertyViewItem *item, QObject* &obj, QString &path);

  void searchItemTree(CQPropertyViewItem *item, const QRegExp &regexp, Items &items);

  void expandItemTree  (CQPropertyViewItem *item);
  void collapseItemTree(CQPropertyViewItem *item);

  //---

  void saveState();

  void restoreState();

  //---

  void showContextMenu(QObject *obj, const QPoint &globalPos);

  void mouseMoveEvent(QMouseEvent *) override;

  void leaveEvent(QEvent *) override;

  void keyPressEvent(QKeyEvent *ke) override;

  void showEvent(QShowEvent *e) override;

  void resizeEvent(QResizeEvent *e) override;

  QModelIndex indexFromItem(CQPropertyViewItem *item, int column, bool map=false) const;

 private:
  using ItemPath       = QStringList;
  using ItemPaths      = std::vector<ItemPath>;
  using DepthItemPaths = std::map<int, ItemPaths>;

  struct StateData {
    ItemPath       topItem;
    DepthItemPaths expandPaths;
    QModelIndex    topIndex;
  };

  void saveState1   (const QModelIndex &parent, StateData &stateData, int depth);
  void restoreState1(const QModelIndex &parent, StateData &stateData, int depth);

  void itemPath(const QModelIndex &ind, ItemPath &path) const;

//void printPath(const ItemPath &path) const;

 private:
  CQHeaderView*           header_         { nullptr };
  CQPropertyViewModel*    model_          { nullptr };
  bool                    modelAllocated_ { false };
  CQPropertyViewFilter*   filter_         { nullptr };
  CQPropertyViewDelegate* delegate_       { nullptr };
  bool                    itemMenu_       { false };
  bool                    mouseHighlight_ { false };
  bool                    resizeOnShow_   { true };
  bool                    resizeOnExpand_ { true };
  bool                    hasMouseInd_    { false };
  bool                    shown_          { false };
  QModelIndex             mouseInd_;
  CQPropertyViewItem*     menuItem_       { nullptr };
  QPoint                  menuPos_;
  StateData               stateData_;
};

#endif
