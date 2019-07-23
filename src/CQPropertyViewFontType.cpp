#include <CQPropertyViewFontType.h>
#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>
#include <CQPropertyViewTree.h>
#include <CQFontEdit.h>
#include <cassert>

CQPropertyViewFontType::
CQPropertyViewFontType()
{
}

CQPropertyViewEditorFactory *
CQPropertyViewFontType::
getEditor() const
{
  return new CQPropertyViewFontEditor;
}

bool
CQPropertyViewFontType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQPropertyViewFontType::
draw(CQPropertyViewItem *, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &index,
     const QVariant &value, bool inside)
{
  delegate->drawFont(painter, option, value.value<QFont>(), index, inside);
}

//------

CQPropertyViewFontEditor::
CQPropertyViewFontEditor()
{
}

QWidget *
CQPropertyViewFontEditor::
createEdit(QWidget *parent)
{
  CQPropertyViewTree *tree =
   (parent ? qobject_cast<CQPropertyViewTree *>(parent->parentWidget()) : nullptr);

  CQFontEdit *edit = new CQFontEdit(parent);

  edit->setAutoFillBackground(true);

  if (tree)
    QObject::connect(edit, SIGNAL(menuHidden()), tree, SLOT(closeEditorSlot()));

  return edit;
}

void
CQPropertyViewFontEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQFontEdit *edit = qobject_cast<CQFontEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(fontChanged(const QString&)), obj, method);
}

QVariant
CQPropertyViewFontEditor::
getValue(QWidget *w)
{
  CQFontEdit *edit = qobject_cast<CQFontEdit *>(w);
  assert(edit);

  return edit->fontName();
}

void
CQPropertyViewFontEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQFontEdit *edit = qobject_cast<CQFontEdit *>(w);
  assert(edit);

  QString str = var.toString();

  edit->setFontName(str);
}
