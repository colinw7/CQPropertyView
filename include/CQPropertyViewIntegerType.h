#ifndef CQPropertyViewIntegerType_H
#define CQPropertyViewIntegerType_H

#include <CQPropertyViewType.h>

/*!
 * \brief type for int
 */
class CQPropertyViewIntegerType : public CQPropertyViewType {
 public:
  CQPropertyViewIntegerType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate,
            QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, const ItemState &itemState) override;
};

//------

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for int
 */
class CQPropertyViewIntegerEditor : public CQPropertyViewEditorFactory {
 public:
  CQPropertyViewIntegerEditor(int min=INT_MIN, int max=INT_MAX, int step=1);

  QWidget *createEdit(QWidget *parent) override;

  void connect(QWidget *w, QObject *obj, const char *method) override;

  QVariant getValue(QWidget *w) override;

  void setValue(QWidget *w, const QVariant &var) override;

 private:
  int min_  { INT_MIN };
  int max_  { INT_MAX };
  int step_ { 1 };
};

#endif
