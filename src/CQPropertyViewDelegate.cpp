#include <CQPropertyViewDelegate.h>
#include <CQPropertyViewTree.h>
#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQPropertyView.h>

#include <QApplication>
#include <QTreeWidget>
#include <QLineEdit>
#include <QPainter>
#include <QLayout>

#include <cassert>

/*! create tree view delegate
*/
CQPropertyViewDelegate::
CQPropertyViewDelegate(CQPropertyViewTree *view) :
 QItemDelegate(view), view_(view)
{
}

/*! create editor for view item
*/
QWidget *
CQPropertyViewDelegate::
createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const
{
  if (index.column() != 1)
    return nullptr;

  auto *item = view_->getModelItem(index);
  assert(item);

  auto *th = const_cast<CQPropertyViewDelegate *>(this);

  auto *w = item->createEditor(parent);

  assert(w);

  w->updateGeometry();

  if (w->layout())
    w->layout()->invalidate();

  //w->setFixedSize(sizeHint(option, index));

  w->installEventFilter(th);

  th->editor_      = w;
  th->editing_     = true;
  th->editorIndex_ = index;

  return w;
}

/*! get data to display in view item
*/
void
CQPropertyViewDelegate::
setEditorData(QWidget *, const QModelIndex &index) const
{
  if (index.column() != 1)
    return;

  auto *item = view_->getModelItem(index);
  assert(item);

  auto var = item->data();

  if (var.isNull()) {
    //std::cerr << "Failed to get model data" << std::endl;
  }

  item->setEditorData(var);
}

/*! store displayed view item data in model
*/
void
CQPropertyViewDelegate::
setModelData(QWidget *, QAbstractItemModel *model, const QModelIndex &index) const
{
  if (index.column() != 1)
    return;

  auto *item = view_->getModelItem(index);
  assert(item);

  auto var = item->getEditorData();

  model->setData(index, var);
}

/*! update geometry
*/
void
CQPropertyViewDelegate::
updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                     const QModelIndex &index) const
{
  if (index.column() != 1)
    return;

  editor->setGeometry(option.rect);
}

/*! draw item
*/
void
CQPropertyViewDelegate::
paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  auto *item = view_->getModelItem(index);
  assert(item);

  CQPropertyViewMgrInst->setDrawItem(item);

  //---

  auto option1 = option;

  //---

  // read only is drawn italic
  if (! item->isEditable()) {
    option1.font.setItalic(true);
  }

  //---

  ItemState itemState;

  itemState.inside = view_->isMouseInd(index);

  //---

  // property name
  if      (index.column() == 0) {
    auto label = item->aliasName();

    drawString(painter, option, label, index, itemState);

    //QItemDelegate::paint(painter, option1, index);
  }
  // property value
  else if (index.column() == 1) {
    if (itemState.inside)
      item->setInside(true);

    if (! item->paint(this, painter, option1, index))
      QItemDelegate::paint(painter, option1, index);

    if (itemState.inside)
      item->setInside(false);
  }
}

/*! size hint
*/
QSize
CQPropertyViewDelegate::
sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  // TODO: add item->sizeHint() virtual
  auto size = QItemDelegate::sizeHint(option, index);

  size.setHeight(size.height() + 2);

  return size;
}

/*! create edit widget
*/
QWidget *
CQPropertyViewDelegate::
createEdit(QWidget *parent, const QString &text) const
{
  auto *edit = new QLineEdit(parent);

  edit->setObjectName("edit");

  edit->setText(text);

  edit->setMouseTracking(false);
  edit->setFocusPolicy(Qt::NoFocus);
  edit->setAcceptDrops(false);
  edit->setFrame(false);
  edit->setReadOnly(true);

  // set background of preview text to window background
  auto plt = QApplication::palette();
  auto bgColor = plt.color(QPalette::Window);
  QString styleStr;
  styleStr.sprintf("background: #%2x%2x%2x", bgColor.red(), bgColor.green(), bgColor.blue());
  edit->setStyleSheet(styleStr);

  return edit;
}

void
CQPropertyViewDelegate::
drawBackground(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index, const ItemState & /*itemState*/) const
{
  QItemDelegate::drawBackground(painter, option, index);
}

void
CQPropertyViewDelegate::
drawCheckInside(QPainter *painter, const QStyleOptionViewItem &option,
                bool checked, const QModelIndex &index, const ItemState &itemState) const
{
  drawBackground(painter, option, index, itemState);

  Qt::CheckState checkState = (checked ? Qt::Checked : Qt::Unchecked);

  auto rect = option.rect;

  rect.setWidth(option.rect.height());

  rect.adjust(0, 1, -3, -2);

  QItemDelegate::drawCheck(painter, option, rect, checkState);

  QFontMetrics fm(painter->font());

  int x = rect.right() + 4;
//int y = rect.top() + fm.ascent();

  QRect rect1;

  rect1.setCoords(x, option.rect.top(), option.rect.right(), option.rect.bottom());

  //painter->drawText(x, y, (checked ? "true" : "false"));
  drawDisplay(painter, option, rect1, checked ? "true" : "false");
}

void
CQPropertyViewDelegate::
drawColor(QPainter *painter, const QStyleOptionViewItem &option,
          const QColor &c, const QModelIndex &index, const ItemState &itemState) const
{
  drawBackground(painter, option, index, itemState);

  auto rect = option.rect;

  rect.setWidth(option.rect.height());

  rect.adjust(0, 1, -3, -2);

  painter->setBrush(QBrush(c));
  painter->setPen(QColor(Qt::black)); // TODO: contrast border

//painter->fillRect(rect, QBrush(c));
  painter->drawRect(rect);

  int x = rect.right() + 2;
//int y = rect.top() + fm.ascent();

  QRect rect1;

  rect1.setCoords(x, option.rect.top(), option.rect.right(), option.rect.bottom());

  drawDisplay(painter, option, rect1, c.name());
}

void
CQPropertyViewDelegate::
drawFont(QPainter *painter, const QStyleOptionViewItem &option,
         const QFont &f, const QModelIndex &index, const ItemState &itemState) const
{
  static QString previewStr("Abc");

  drawBackground(painter, option, index, itemState);

  auto rect = option.rect;

  rect.setWidth(option.rect.height());

  rect.adjust(0, 1, -3, -2);

  auto f1 = f;
  auto f2 = painter->font();

  QFontMetrics fm1(f1);
  QFontMetrics fm2(f2);

  int fw = fm1.width(previewStr);
  int fh = fm1.height();

  if (fh > rect.height()) {
    f1.setPixelSize(rect.height());

    fm1 = QFontMetrics(f1);

    fw = fm1.width(previewStr);
  }

  int x1 = rect.left();
  int y1 = rect.top() + fm1.ascent();

  painter->save();

  painter->setFont(f1);

  auto fg = view_->palette().color(QPalette::Text);

  painter->setPen(fg);

  painter->drawText(x1, y1, previewStr);

  painter->restore();

  int x2 = x1 + fw + 4;
//int y2 = rect.top() + fm2.ascent();

  QRect rect1;

  rect1.setCoords(x2, option.rect.top(), option.rect.right(), option.rect.bottom());

//painter->drawText(x2, y2, f.toString());
  drawDisplay(painter, option, rect1, f.toString());
}

void
CQPropertyViewDelegate::
drawPoint(QPainter *painter, const QStyleOptionViewItem &option,
         const QPointF &p, const QModelIndex &index, const ItemState &itemState) const
{
  drawBackground(painter, option, index, itemState);

  auto rect = option.rect;

  //rect.setWidth(option.rect.height());

  //QFontMetrics fm(painter->font());

  auto str = QString("%1 %2").arg(p.x()).arg(p.y());

  drawDisplay(painter, option, rect, str);
}

void
CQPropertyViewDelegate::
drawSize(QPainter *painter, const QStyleOptionViewItem &option,
         const QSizeF &s, const QModelIndex &index, const ItemState &itemState) const
{
  drawBackground(painter, option, index, itemState);

  auto rect = option.rect;

  //rect.setWidth(option.rect.height());

  //QFontMetrics fm(painter->font());

  auto str = QString("%1 %2").arg(s.width()).arg(s.height());

  drawDisplay(painter, option, rect, str);
}

void
CQPropertyViewDelegate::
drawRect(QPainter *painter, const QStyleOptionViewItem &option,
         const QRectF &r, const QModelIndex &index, const ItemState &itemState) const
{
  drawBackground(painter, option, index, itemState);

  auto rect = option.rect;

  //rect.setWidth(option.rect.height());

  //QFontMetrics fm(painter->font());

  auto str = QString("{%1 %2} {%3 %4}").arg(r.left ()).arg(r.top   ()).
                                        arg(r.right()).arg(r.bottom());

  drawDisplay(painter, option, rect, str);
}

#if 0
void
CQPropertyViewDelegate::
drawAngle(QPainter *painter, const QStyleOptionViewItem &option,
         const CAngle &a, const QModelIndex &index, const ItemState &itemState) const
{
  drawBackground(painter, option, index, itemState);

  auto rect = option.rect;

  //rect.setWidth(option.rect.height());

  //QFontMetrics fm(painter->font());

  auto str = QString("%1").arg(a.degrees());

  drawDisplay(painter, option, rect, str);
}
#endif

void
CQPropertyViewDelegate::
drawString(QPainter *painter, const QStyleOptionViewItem &option, const QString &str,
           const QModelIndex &index, const ItemState &itemState) const
{
  drawBackground(painter, option, index, itemState);

  auto rect = option.rect;

  if (itemState.inside) {
    auto option1 = option;

    auto c = QColor(100, 100, 200);

    option1.palette.setColor(QPalette::WindowText, c);
    option1.palette.setColor(QPalette::Text      , c);

    drawDisplay(painter, option1, rect, str);
  }
  else
    drawDisplay(painter, option, rect, str);

  //painter->fillRect(option.rect, Qt::red);
}

void
CQPropertyViewDelegate::
drawDisplay(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect,
            const QString &str) const
{
  QItemDelegate::drawDisplay(painter, option, rect, str);
}

bool
CQPropertyViewDelegate::
eventFilter(QObject *obj, QEvent *event)
{
  return QObject::eventFilter(obj, event);
}
