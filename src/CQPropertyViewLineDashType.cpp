#include <CQPropertyViewLineDashType.h>
#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>
#include <CQLineDash.h>
#include <QPainter>

CQPropertyViewLineDashType::
CQPropertyViewLineDashType()
{
}

CQPropertyViewEditorFactory *
CQPropertyViewLineDashType::
getEditor() const
{
  return new CQPropertyViewLineDashEditor;
}

bool
CQPropertyViewLineDashType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQPropertyViewLineDashType::
draw(CQPropertyViewItem *, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &index,
     const QVariant &value, const ItemState &itemState)
{
  delegate->drawBackground(painter, option, index, itemState);

  CLineDash dash = value.value<CLineDash>();

  auto icon = CQLineDash::dashIcon(dash);

  auto str = QString(dash.toString().c_str());

  QFontMetrics fm(option.font);

  int w = fm.width(str);

  //---

  auto irect = option.rect;

  irect.setLeft(irect.left() + w + 2*margin());

  painter->drawPixmap(irect, icon.pixmap(option.rect.size()));

  //--

  auto option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 2*margin());

  delegate->drawString(painter, option1, str, index, itemState);
}

QString
CQPropertyViewLineDashType::
tip(const QVariant &value) const
{
  auto str = QString(value.value<CLineDash>().toString().c_str());

  return str;
}

//------

CQPropertyViewLineDashEditor::
CQPropertyViewLineDashEditor()
{
}

QWidget *
CQPropertyViewLineDashEditor::
createEdit(QWidget *parent)
{
  auto *edit = new CQLineDash(parent);

  return edit;
}

void
CQPropertyViewLineDashEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQLineDash *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(valueChanged(const CLineDash &)), obj, method);
}

QVariant
CQPropertyViewLineDashEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQLineDash *>(w);
  assert(edit);

  return QVariant::fromValue(edit->getLineDash());
}

void
CQPropertyViewLineDashEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQLineDash *>(w);
  assert(edit);

  CLineDash dash = var.value<CLineDash>();

  edit->setLineDash(dash);
}
