#ifndef CQPropertyViewPointFType_H
#define CQPropertyViewPointFType_H

#include <CQPropertyViewType.h>

/*!
 * \brief type for QPointF
 */
class CQPropertyViewPointFType : public CQPropertyViewType {
 public:
  CQPropertyViewPointFType();

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
 * \brief editor factory for QPointF
 */
class CQPropertyViewPointFEditor : public CQPropertyViewEditorFactory {
 public:
  CQPropertyViewPointFEditor(double min=-1E50, double max=1E50, double step=1.0);

  double min() const { return min_; }
  double max() const { return max_; }

  double step() const { return step_; }

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);

 private:
  double min_  { 0.0 };
  double max_  { 1.0 };
  double step_ { 1.0 };
};

#endif
