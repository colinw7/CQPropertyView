#ifndef CQPropertyViewDelegate_H
#define CQPropertyViewDelegate_H

#include <QItemDelegate>
//#include <CAngle.h>
#include <QPointer>

class CQPropertyViewTree;
class CQPropertyViewItem;

/*!
 * \brief Item Delegate class to handle custom editing of view items
 */
class CQPropertyViewDelegate : public QItemDelegate {
  Q_OBJECT

 public:
  struct ItemState {
    bool inside = false;
    bool dirty  = false;
  };

 public:
  CQPropertyViewDelegate(CQPropertyViewTree *view);

  // Override to create editor
  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const override;

  // Override to get content from editor
  void setEditorData(QWidget *editor, const QModelIndex &index) const override;

  void setModelData(QWidget *editor, QAbstractItemModel *model,
                    const QModelIndex &index) const override;

  void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                            const QModelIndex &index) const override;

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const override;

  CQPropertyViewItem *getModelItem(const QModelIndex &index) const;

  QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index ) const override;

  QWidget *createEdit(QWidget *parent, const QString &text) const;

  void drawBackground(QPainter *painter, const QStyleOptionViewItem &option,
                      const QModelIndex &index, const ItemState &itemState) const;

  void drawCheckInside(QPainter *painter, const QStyleOptionViewItem &option,
                       bool checked, const QModelIndex &index, const ItemState &itemState) const;

  void drawColor(QPainter *painter, const QStyleOptionViewItem &option,
                 const QColor &c, const QModelIndex &index, const ItemState &itemState) const;
  void drawFont (QPainter *painter, const QStyleOptionViewItem &option,
                 const QFont &f, const QModelIndex &index, const ItemState &itemState) const;
  void drawPoint(QPainter *painter, const QStyleOptionViewItem &option,
                 const QPointF &p, const QModelIndex &index, const ItemState &itemState) const;
  void drawSize (QPainter *painter, const QStyleOptionViewItem &option,
                 const QSizeF &s, const QModelIndex &index, const ItemState &itemState) const;
  void drawRect (QPainter *painter, const QStyleOptionViewItem &option,
                 const QRectF &r, const QModelIndex &index, const ItemState &itemState) const;
#if 0
  void drawAngle(QPainter *painter, const QStyleOptionViewItem &option,
                 const CAngle &a, const QModelIndex &index, const ItemState &itemState) const;
#endif
  void drawString(QPainter *painter, const QStyleOptionViewItem &option,
                  const QString &str, const QModelIndex &index, const ItemState &itemState) const;

  //---

  // get/set is editing
  bool isEditing() const { return editing_ && editor_; }
  void setEditing(bool b) { editing_ = b; }

  // get current editor
  QWidget *getEditor() const { return editor_; }

  // get editor index
  QModelIndex getEditorIndex() const { return editorIndex_; }

  bool eventFilter(QObject *obj, QEvent *event) override;

 private:
  using WidgetP = QPointer<QWidget>;

  CQPropertyViewTree* view_        { nullptr }; //!< parent view
  WidgetP             editor_;                  //!< current editor
  bool                editing_     { false };   //!< is editing
  QModelIndex         editorIndex_;             //!< editor model index
};

#endif
