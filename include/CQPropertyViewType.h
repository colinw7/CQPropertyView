#ifndef CQPropertyViewType_H
#define CQPropertyViewType_H

#include <QString>

class CQPropertyViewEditorFactory;
class CQPropertyViewDelegate;
class CQPropertyViewItem;

class QStyleOptionViewItem;
class QPainter;
class QModelIndex;
class QVariant;

/*!
 * \brief type base class
 *
 * support:
 *  . create editor
 *  . delegate draw
 */
class CQPropertyViewType {
 public:
  CQPropertyViewType() { }

  virtual ~CQPropertyViewType() { }

  virtual CQPropertyViewEditorFactory *getEditor() const = 0;

  virtual bool setEditorData(CQPropertyViewItem *item, const QVariant &value);

  virtual void draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate,
                    QPainter *painter, const QStyleOptionViewItem &option,
                    const QModelIndex &index, const QVariant &value, bool inside);

  virtual QString tip(const QVariant &value) const;

  virtual QString userName() const { return ""; }

  //---

  //! get set registered name
  const QString &name() const { return name_; }
  void setName(const QString &name) { name_ = name; }

 protected:
  QString name_; //!< registered name
};

#endif
