#ifndef CQPropertyViewModel_H
#define CQPropertyViewModel_H

#include <QAbstractItemModel>
#include <vector>

class CQPropertyViewItem;

class CQPropertyViewModel : public QAbstractItemModel {
  Q_OBJECT

 public:
  using NameValues = std::map<QString,QVariant>;

 public:
  CQPropertyViewModel();
 ~CQPropertyViewModel();

  CQPropertyViewItem *root() const;

  //---

  int columnCount(const QModelIndex &) const override;

  int rowCount(const QModelIndex &parent) const override;

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
  bool getProperty(QObject *object, const QString &path, QVariant &value);

  void removeProperties(const QString &path, QObject *object=nullptr);

  CQPropertyViewItem *propertyItem(QObject *object, const QString &path);

  CQPropertyViewItem *item(const QModelIndex &index, bool &ok) const;
  CQPropertyViewItem *item(const QModelIndex &index) const;

  QModelIndex indexFromItem(CQPropertyViewItem *item, int column) const;

  void refresh();

  void objectNames(QObject *object, QStringList &strs) const;

  void getChangedNameValues(NameValues &nameValues) const;
  void getChangedNameValues(const QObject *object, NameValues &nameValues) const;

 private:
  CQPropertyViewItem *propertyItem(QObject *object, const QString &path,
                                   QChar splitChar, bool create, bool alias);

  CQPropertyViewItem *hierItem(const QStringList &pathPaths, bool create=false, bool alias=false);

  CQPropertyViewItem *hierItem(CQPropertyViewItem *parentRow, const QStringList &pathPaths,
                               bool create=false, bool alias=false);

  CQPropertyViewItem *objectItem(const QObject *obj) const;

  CQPropertyViewItem *objectItem(CQPropertyViewItem *parent, const QObject *obj) const;

  void itemNames(CQPropertyViewItem *rootItem, QObject *object,
                 CQPropertyViewItem *item, QStringList &strs) const;

  void getChangedItemNameValues(const QObject *object, CQPropertyViewItem *item,
                                NameValues &nameValues) const;

  void addNameValue(CQPropertyViewItem *item, NameValues &nameValues) const;

 signals:
  void valueChanged(QObject *, const QString &);

 private:
  CQPropertyViewItem *root_ { nullptr };
};

#endif
