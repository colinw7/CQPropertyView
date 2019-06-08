#ifndef CQPropertyViewModel_H
#define CQPropertyViewModel_H

#include <QAbstractItemModel>
#include <vector>

class CQPropertyViewItem;

/*!
 * \brief Property view model
 */
class CQPropertyViewModel : public QAbstractItemModel {
  Q_OBJECT

  Q_PROPERTY(bool showHidden READ isShowHidden WRITE setShowHidden)

 public:
  using NameValues = std::map<QString,QVariant>;

 public:
  CQPropertyViewModel();
 ~CQPropertyViewModel();

  CQPropertyViewItem *root() const;

  //---

  bool isShowHidden() const { return showHidden_; }
  void setShowHidden(bool b) { showHidden_ = b; }

  //---

  int columnCount(const QModelIndex &parent=QModelIndex()) const override;

  int rowCount(const QModelIndex &parent=QModelIndex()) const override;

  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

  QVariant data(const QModelIndex &index, int role) const override;

  bool setData(const QModelIndex &index, const QVariant &value, int role) override;

  QModelIndex index(int row, int column, const QModelIndex &parent) const override;

  QModelIndex parent(const QModelIndex &index) const override;

  Qt::ItemFlags flags(const QModelIndex &index) const override;

  //---

  void clear();

  CQPropertyViewItem *addProperty(const QString &path, QObject *object, const QString &name,
                                  const QString &alias="");

  bool setProperty(QObject *object, const QString &path, const QVariant &value);
  bool getProperty(const QObject *object, const QString &path, QVariant &value) const;

  bool getTclProperty(const QObject *object, const QString &path, QVariant &value) const;

  void removeProperties(const QString &path, QObject *object=nullptr);

  void hideProperty(const QString &path, const QObject *object);

  void setObjectRoot(const QString &path, QObject *obj);

  const CQPropertyViewItem *propertyItem(const QObject *object, const QString &path,
                                         bool hidden=false) const;
  CQPropertyViewItem *propertyItem(QObject *object, const QString &path, bool hidden=false);

  CQPropertyViewItem *item(const QModelIndex &index, bool &ok) const;
  CQPropertyViewItem *item(const QModelIndex &index) const;

  QModelIndex indexFromItem(CQPropertyViewItem *item, int column) const;

  void refresh();
  void reset();

  void objectNames(const QObject *object, QStringList &names, bool hidden=false) const;

  void getChangedNameValues(NameValues &nameValues, bool tcl=false) const;

  void getChangedNameValues(const QObject *object, NameValues &nameValues, bool tcl=false) const;

  void getChangedNameValues(const QObject *root, const QObject *object,
                            NameValues &nameValues, bool tcl=false) const;

 public:
  typedef std::vector<CQPropertyViewItem *> Children;

  int numItemChildren(CQPropertyViewItem *item, bool hidden=false) const;

  const Children &itemChildren(CQPropertyViewItem *item, bool hidden=false) const;

  CQPropertyViewItem *itemChild(CQPropertyViewItem *item, int i, bool hidden=false) const;

 private:
  const CQPropertyViewItem *propertyItem(const QObject *object, const QString &path,
                                         QChar splitChar, bool create, bool alias,
                                         bool hidden) const;
  CQPropertyViewItem *propertyItem(QObject *object, const QString &path,
                                   QChar splitChar, bool create, bool alias, bool hidden);

  const CQPropertyViewItem *hierItem(const QStringList &pathPaths, bool create=false,
                                     bool alias=false, bool hidden=false) const;
  CQPropertyViewItem *hierItem(const QStringList &pathPaths, bool create=false,
                               bool alias=false, bool hidden=false);

  CQPropertyViewItem *hierItem(CQPropertyViewItem *parentRow, const QStringList &pathPaths,
                               bool create=false, bool alias=false, bool hidden=false);

  CQPropertyViewItem *objectItem(const QObject *obj) const;

  CQPropertyViewItem *objectItem(CQPropertyViewItem *parent, const QObject *obj) const;

  void itemNames(CQPropertyViewItem *rootItem, const QObject *object,
                 CQPropertyViewItem *item, QStringList &names, bool hidden=false) const;

  void getChangedItemNameValues(CQPropertyViewItem *rootItem, const QObject *object,
                                CQPropertyViewItem *item, NameValues &nameValues,
                                bool tcl=false) const;

  void addNameValue(CQPropertyViewItem *rootItem, CQPropertyViewItem *item,
                    NameValues &nameValues, bool tcl=false) const;

 signals:
  void valueChanged(QObject *, const QString &);

 private:
  bool                showHidden_ { false };   //!< show hidden properties
  CQPropertyViewItem *root_       { nullptr }; //!< root item
};

#endif
