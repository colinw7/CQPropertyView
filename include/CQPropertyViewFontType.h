#ifndef CQPropertyViewFontType_H
#define CQPropertyViewFontType_H

#include <CQPropertyViewType.h>

/*!
 * \brief type for QFont
 */
class CQPropertyViewFontType : public CQPropertyViewType {
 public:
  CQPropertyViewFontType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate,
            QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, const ItemState &itemState) override;
};

//------

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for QFont
 */
class CQPropertyViewFontEditor : public CQPropertyViewEditorFactory {
 public:
  CQPropertyViewFontEditor();

  QWidget *createEdit(QWidget *parent) override;

  void connect(QWidget *w, QObject *obj, const char *method) override;

  QVariant getValue(QWidget *w) override;

  void setValue(QWidget *w, const QVariant &var) override;
};

#endif
