#ifndef CQPropertyViewTree_H
#define CQPropertyViewTree_H

#include <QTreeView>
#include <QAbstractItemDelegate>

class CQPropertyViewModel;
class CQPropertyViewItem;
class CQPropertyViewFilter;
class CQPropertyViewDelegate;

/*!
 * \brief Property view tree view
 */
class CQPropertyViewTree : public QTreeView {
  Q_OBJECT

  Q_PROPERTY(bool itemMenu       READ isItemMenu       WRITE setItemMenu      )
  Q_PROPERTY(bool mouseHighlight READ isMouseHighlight WRITE setMouseHighlight)
  Q_PROPERTY(bool resizeOnShow   READ isResizeOnShow   WRITE setResizeOnShow  )

 public:
  typedef std::vector<CQPropertyViewItem *> Items;
  typedef std::vector<QObject *>            Objs;

 public:
  CQPropertyViewTree(QWidget *parent, CQPropertyViewModel *model);

  virtual ~CQPropertyViewTree();

  CQPropertyViewModel *propertyModel() const { return model_; }

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

  bool isMouseHighlight() const { return mouseHighlight_; }
  void setMouseHighlight(bool b);

  bool isResizeOnShow() const { return resizeOnShow_; }
  void setResizeOnShow(bool b) { resizeOnShow_ = b; }

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

 signals:
  void valueChanged(QObject *, const QString &);

  void itemClicked( QObject *obj, const QString &path);
  void itemSelected(QObject *obj, const QString &path);

  void menuExec(QObject *obj, const QPoint &gpos);

 public slots:
  void redraw();

  void expandAll();
  void collapseAll();

  void expandSelected();

  void showHidden(bool);

  void setFilter(const QString &filter);
  void search(const QString &text);

  void printSlot() const;
  void printChangedSlot() const;

  void closeEditorSlot();

 protected slots:
  void modelResetSlot();

  void itemClickedSlot(const QModelIndex &index);

  void itemSelectionSlot();

  void customContextMenuSlot(const QPoint &pos);

  void closeEditorSlot(QWidget *, QAbstractItemDelegate::EndEditHint);

 protected:
  bool selectObject(CQPropertyViewItem *item, const QObject *obj);

  void getItemData(CQPropertyViewItem *item, QObject* &obj, QString &path);

  void searchItemTree(CQPropertyViewItem *item, const QRegExp &regexp, Items &items);

  void expandItemTree(CQPropertyViewItem *item);
  void collapseItemTree(CQPropertyViewItem *item);

  void showContextMenu(QObject *obj, const QPoint &globalPos);

  void mouseMoveEvent(QMouseEvent *) override;

  void leaveEvent(QEvent *) override;

  void keyPressEvent(QKeyEvent *ke) override;

  void showEvent(QShowEvent *e) override;

  void resizeEvent(QResizeEvent *e) override;

  QModelIndex indexFromItem(CQPropertyViewItem *item, int column, bool map=false) const;

 private:
  CQPropertyViewModel*    model_          { nullptr };
  CQPropertyViewFilter*   filter_         { nullptr };
  CQPropertyViewDelegate* delegate_       { nullptr };
  bool                    itemMenu_       { false };
  bool                    mouseHighlight_ { false };
  bool                    resizeOnShow_   { true };
  bool                    hasMouseInd_    { false };
  bool                    shown_          { false };
  QModelIndex             mouseInd_;
  CQPropertyViewItem*     menuItem_       { nullptr };
};

#endif
