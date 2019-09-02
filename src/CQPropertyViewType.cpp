#include <CQPropertyViewType.h>
#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>
#include <CQPropertyViewUtil.h>

bool
CQPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  item->setData(value);

  return true;
}

void
CQPropertyViewType::
draw(CQPropertyViewItem *, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &index,
     const QVariant &value, bool inside)
{
  QString str = CQPropertyViewUtil::variantToString(value);

  delegate->drawString(painter, option, str, index, inside);
}

QString
CQPropertyViewType::
tip(const QVariant &value) const
{
  return value.toString();
}
