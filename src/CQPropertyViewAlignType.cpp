#include <CQPropertyViewAlignType.h>
#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>
#include <CQAlignEdit.h>

CQPropertyViewAlignType::
CQPropertyViewAlignType()
{
}

CQPropertyViewEditorFactory *
CQPropertyViewAlignType::
getEditor() const
{
  return new CQPropertyViewAlignEditor;
}

bool
CQPropertyViewAlignType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQPropertyViewAlignType::
draw(CQPropertyViewItem *, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &index,
     const QVariant &value, const ItemState &itemState)
{
  auto str = CQAlignEdit::toString((Qt::Alignment) value.toInt());

  delegate->drawString(painter, option, str, index, itemState);
}

QString
CQPropertyViewAlignType::
tip(const QVariant &value) const
{
  return CQAlignEdit::toString((Qt::Alignment) value.toInt());
}

//------

CQPropertyViewAlignEditor::
CQPropertyViewAlignEditor()
{
}

QWidget *
CQPropertyViewAlignEditor::
createEdit(QWidget *parent)
{
  auto *edit = new CQAlignEdit(parent);

  return edit;
}

void
CQPropertyViewAlignEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQAlignEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(valueChanged(Qt::Alignment)), obj, method);
}

QVariant
CQPropertyViewAlignEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQAlignEdit *>(w);
  assert(edit);

  return QVariant(edit->align());
}

void
CQPropertyViewAlignEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQAlignEdit *>(w);
  assert(edit);

  int i = var.toInt();

  edit->setAlign((Qt::Alignment) i);
}
