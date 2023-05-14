#ifndef CQPropertyViewType_H
#define CQPropertyViewType_H

#include <CQPropertyViewDelegate.h>
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
  using EditorFactory = CQPropertyViewEditorFactory;
  using ViewDelegate  = CQPropertyViewDelegate;
  using ViewItem      = CQPropertyViewItem;
  using ItemState     = ViewDelegate::ItemState;

 public:
  CQPropertyViewType() { }

  virtual ~CQPropertyViewType() { }

  //---

  int margin() const { return margin_; }
  void setMargin(int m) { margin_ = m; }

  //---

  virtual EditorFactory *getEditor() const = 0;

  virtual bool setEditorData(ViewItem *item, const QVariant &value);

  virtual void draw(ViewItem *item, const ViewDelegate *delegate,
                    QPainter *painter, const QStyleOptionViewItem &option,
                    const QModelIndex &index, const QVariant &value,
                    const ItemState &itemState);

  virtual QString tip(const QVariant &value) const;

  virtual QString userName() const { return ""; }

  //---

  //! get set registered name
  const QString &name() const { return name_; }
  void setName(const QString &name) { name_ = name; }

 protected:
  QString name_;         //!< registered name
  int     margin_ { 4 }; //! left/right margin
};

#endif
