#include <CQPropertyViewIntegerType.h>
#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>
#include <CQPropertyView.h>
#include <QSpinBox>
#include <cassert>

CQPropertyViewIntegerType::
CQPropertyViewIntegerType()
{
}

CQPropertyViewEditorFactory *
CQPropertyViewIntegerType::
getEditor() const
{
  return new CQPropertyViewIntegerEditor;
}

bool
CQPropertyViewIntegerType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQPropertyViewIntegerType::
draw(CQPropertyViewItem *, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &index,
     const QVariant &value, const ItemState &itemState)
{
  delegate->drawString(painter, option, value.toString(), index, itemState);
}

//------

CQPropertyViewIntegerEditor::
CQPropertyViewIntegerEditor(int min, int max, int step) :
 min_(min), max_(max), step_(step)
{
}

QWidget *
CQPropertyViewIntegerEditor::
createEdit(QWidget *parent)
{
  auto *spin = new QSpinBox(parent);

  auto *item = CQPropertyViewMgrInst->editItem();

  if (item) {
    QVariant vmin = item->minValue();
    QVariant vmax = item->maxValue();

    if (vmin.isValid()) { bool ok; int i = vmin.toInt(&ok); if (ok) min_ = i; }
    if (vmax.isValid()) { bool ok; int i = vmax.toInt(&ok); if (ok) max_ = i; }
  }

  spin->setRange(min_, max_);
  spin->setSingleStep(step_);

  return spin;
}

void
CQPropertyViewIntegerEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *spin = qobject_cast<QSpinBox *>(w);
  assert(spin);

  QObject::connect(spin, SIGNAL(valueChanged(int)), obj, method);
}

QVariant
CQPropertyViewIntegerEditor::
getValue(QWidget *w)
{
  auto *spin = qobject_cast<QSpinBox *>(w);
  assert(spin);

  return QVariant(spin->value());
}

void
CQPropertyViewIntegerEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *spin = qobject_cast<QSpinBox *>(w);
  assert(spin);

  int i = var.toInt();

  spin->setValue(i);
}
