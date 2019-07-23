#ifndef CQPropertyViewRectFType_H
#define CQPropertyViewRectFType_H

#include <CQPropertyViewType.h>

/*!
 * \brief type for QRectF
 */
class CQPropertyViewRectFType : public CQPropertyViewType {
 public:
  CQPropertyViewRectFType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate,
            QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const override;
};

//------

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for QRectF
 */
class CQPropertyViewRectFEditor : public CQPropertyViewEditorFactory {
 public:
  CQPropertyViewRectFEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
