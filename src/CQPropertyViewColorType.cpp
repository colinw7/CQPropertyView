#include <CQPropertyViewColorType.h>
#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>
#include <CQPropertyViewTree.h>
#include <CQColorEdit.h>
#include <cassert>

CQPropertyViewColorType::
CQPropertyViewColorType()
{
}

CQPropertyViewEditorFactory *
CQPropertyViewColorType::
getEditor() const
{
  return new CQPropertyViewColorEditor;
}

bool
CQPropertyViewColorType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQPropertyViewColorType::
draw(CQPropertyViewItem *, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &index,
     const QVariant &value, const ItemState &itemState)
{
  delegate->drawColor(painter, option, value.value<QColor>(), index, itemState);
}

//------

CQPropertyViewColorEditor::
CQPropertyViewColorEditor()
{
}

QWidget *
CQPropertyViewColorEditor::
createEdit(QWidget *parent)
{
  auto *tree = (parent ? qobject_cast<CQPropertyViewTree *>(parent->parentWidget()) : nullptr);

  auto *edit = new CQColorEdit(parent);

  edit->setAutoFillBackground(true);

  if (tree)
    QObject::connect(edit, SIGNAL(menuHidden()), tree, SLOT(closeEditorSlot()));

  return edit;
}

void
CQPropertyViewColorEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQColorEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(colorChanged(const QColor&)), obj, method);
}

QVariant
CQPropertyViewColorEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQColorEdit *>(w);
  assert(edit);

  return edit->colorName();
}

void
CQPropertyViewColorEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQColorEdit *>(w);
  assert(edit);

  auto str = var.toString();

  edit->setColorName(str);
}
