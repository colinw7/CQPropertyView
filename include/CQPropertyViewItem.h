#ifndef CQPropertyViewItem_H
#define CQPropertyViewItem_H

#include <CQUtil.h>
#include <QObject>
#include <QPointer>
#include <QStyleOptionViewItem>
#include <QString>
#include <QVariant>
#include <vector>
#include <cassert>

class CQPropertyViewDelegate;
class CQPropertyViewEditorFactory;
class QPainter;
class QWidget;

class CQPropertyViewItem : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString name     READ name       WRITE setName    )
  Q_PROPERTY(QString alias    READ alias      WRITE setAlias   )
  Q_PROPERTY(bool    editable READ isEditable WRITE setEditable)
  Q_PROPERTY(bool    hidden   READ isHidden   WRITE setHidden  )
  Q_PROPERTY(bool    inside   READ isInside   WRITE setInside  )

 public:
  typedef std::vector<CQPropertyViewItem *> Children;

 public:
  CQPropertyViewItem(CQPropertyViewItem *parent, QObject *object, const QString &name);

 ~CQPropertyViewItem();

  //---

  //! check if valid
  bool isValid() const { return id_ == 0xFEEDBEEF; }

  //! get parent item
  CQPropertyViewItem *parent() const { return parent_; }

  //! get associated object
  QObject *object() const { return object_; }

  //! get associated object from this item or child items
  QObject *hierObject() const;

  //---

  // get number of children
  int numChildren() const { return children_.size(); }

  //! get children
  const Children &children() const { return children_; }

  // get child
  CQPropertyViewItem *child(int i) const {
    assert(i >= 0 && i < numChildren());

    return children()[i];
  }

  //---

  //! add child
  void addChild(CQPropertyViewItem *row);

  //! remove child
  void removeChild(CQPropertyViewItem *row);

  //---

  //! get number of visible children
  int numVisibleChildren() const { return visibleChildren().size(); }

  //! get visible children
  const Children &visibleChildren() const;

  //! get visible child
  CQPropertyViewItem *visibleChild(int i) const {
   assert(i >= 0 && i < numVisibleChildren());

    return visibleChildren()[i];
  }

  //! invalidate cached visible children
  void invalidateVisible();

  //---

  //! get/set name
  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  //! get/set alias
  const QString &alias() const { return alias_; }
  CQPropertyViewItem &setAlias(const QString &s) { alias_ = s; return *this; }

  //! get/set editable
  bool isEditable() const { return editable_; }
  CQPropertyViewItem &setEditable(bool b) { editable_ = b; return *this; }

  //! get/set hidden
  bool isHidden() const { return hidden_; }
  CQPropertyViewItem &setHidden(bool b) {
    hidden_ = b; if (parent_) parent_->invalidateVisible(); return *this;
  }

  //! get/set inside
  bool isInside() const { return inside_; }
  void setInside(bool b) { inside_ = b; }

  //! get/set enum values
  const QStringList &values() const { return values_; }
  void setValues(const QStringList &v) { values_ = v; }

  //! get alias name
  QString aliasName() const;

  //! get path
  QString path(const QString &sep="/", bool alias=false, CQPropertyViewItem *root=nullptr) const;

  //! get initial value as string
  QString initStr() const;

  //! get data value as string
  QString dataStr() const;

  //! set editor
  void setEditorFactory(CQPropertyViewEditorFactory *editor) { editor_ = editor; }

  //! handle click
  bool click();

  //! create editor widget
  QWidget *createEditor(QWidget *parent);

  //! set widget property from variant
  void setEditorData(const QVariant &value);

  //! get editor widget text
  QVariant getEditorData() const;

  //! is writable
  bool isWritable() const;

  //! initial value
  QVariant initValue() const { return initValue_; }

  //! get/set data
  QVariant data() const;
  bool setData(const QVariant &value);

  //! get tip
  QString tip() const;

  //! paint item
  bool paint(const CQPropertyViewDelegate *delegate, QPainter *painter,
             const QStyleOptionViewItem &option, const QModelIndex &index);

 private:
  QWidget *createDefaultEdit(QWidget *parent, const QString &valueStr);

  QString getDefaultValue() const;

 signals:
  //! emitted when value changed
  void valueChanged(QObject *obj, const QString &name);

 private slots:
  //! called when the editor widget is changed
  void updateValue();

  bool enumIndToString(const CQUtil::PropInfo &propInfo, int ind, QString &str) const;
  bool enumStringToInd(const CQUtil::PropInfo &propInfo, const QString &str, int &ind) const;

 private:
  uint                         id_       { 0xFEEDBEEF }; //! unique id
  CQPropertyViewItem*          parent_   { nullptr };    //! parent item
  QPointer<QObject>            object_;                  //! associated objects
  QString                      name_;                    //! name
  QString                      alias_;                   //! alias
  Children                     children_;                //! child items
  QVariant                     initValue_;               //! init value
  bool                         editable_ { false };      //! is editable
  bool                         hidden_   { false };      //! is hidden
  bool                         inside_   { false };      //! is mouse inside
  QWidget*                     widget_   { nullptr };    //! edit widget
  CQPropertyViewEditorFactory *editor_   { nullptr };    //! editor interface
  QStringList                  values_;                  //! enum values

  Children visibleChildren_;                //! visible child items
  bool     visibleChildrenValid_ { false }; //! visible child items valid
  bool     anyChildrenHidden_    { false }; //! any children hidden
};

#endif
