#include <CQPropertyViewTest.h>
#include <CQPropertyViewTree.h>
#include <CQPropertyViewFilter.h>
#include <CQPropertyViewModel.h>
#include <CQApp.h>
#include <CQUtil.h>
#include <CQIconCombo.h>

//#include <modeltest.h>

#include <QApplication>
#include <QSplitter>
#include <QStackedWidget>
#include <QLineEdit>
#include <QHBoxLayout>

#include <svg/filter_svg.h>
#include <svg/search_svg.h>

int
main(int argc, char **argv)
{
  CQApp app(argc, argv);

  CQUtil::initProperties();

  Dialog *dialog = new Dialog;

  dialog->show();

  return app.exec();
}

Dialog::
Dialog()
{
  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);

  QSplitter *splitter = new QSplitter;

  splitter->setObjectName("splitter");

  QStackedWidget *stack = new QStackedWidget;

  stack->setObjectName("stack");

  Widget *widget1 = new Widget; widget1->setObjectName("widget1");
  Widget *widget2 = new Widget; widget2->setObjectName("widget2");
  Widget *widget3 = new Widget; widget3->setObjectName("widget3");

  stack->addWidget(widget1);
  stack->addWidget(widget2);
  stack->addWidget(widget3);

  splitter->addWidget(stack);

  QFrame *viewFrame = new QFrame;

  viewFrame->setObjectName("viewFrame");

  QVBoxLayout *viewLayout = new QVBoxLayout(viewFrame);
  viewLayout->setMargin(0); viewLayout->setSpacing(2);

  //---

  QFrame *filterFrame = new QFrame;

  filterFrame->setObjectName("filterFrame");

  viewLayout->addWidget(filterFrame);

  QHBoxLayout *filterLayout = new QHBoxLayout(filterFrame);

  filterEdit_ = new QLineEdit;

  filterEdit_->setObjectName("filterEdit");

  connect(filterEdit_, SIGNAL(returnPressed()), this, SLOT(filterSlot()));

  filterLayout->addWidget(filterEdit_);

  filterCombo_ = new CQIconCombo;

  filterCombo_->setObjectName("filterCombo");

  filterCombo_->addItem(CQPixmapCacheInst->getIcon("FILTER"), "Filter");
  filterCombo_->addItem(CQPixmapCacheInst->getIcon("SEARCH"), "Search");

  filterLayout->addWidget(filterCombo_);

  //---

  model_ = new CQPropertyViewModel;

  view_ = new CQPropertyViewTree(this, model_);

  //new ModelTest(view_->propertyModel());
  //new ModelTest(view_->filterModel());

  view_->addProperty("", view_, "mouseHighlight");

  widget3->addPropeties("widget/widget3", view_);
  widget1->addPropeties("widget1", view_);
  widget2->addPropeties("widget2", view_);

  viewLayout->addWidget(view_);

  splitter->addWidget(viewFrame);

  layout->addWidget(splitter);
}

Dialog::
~Dialog()
{
  delete model_;
}

void
Dialog::
filterSlot()
{
  if (filterCombo_->currentIndex() == 0)
    view_->setFilter(filterEdit_->text());
  else
    view_->search(filterEdit_->text());
}

//------

Widget::
Widget()
{
}

void
Widget::
addPropeties(const QString &path, CQPropertyViewTree *view)
{
  view->addProperty(path, this, "integer"  );
  view->addProperty(path, this, "real"     );
  view->addProperty(path, this, "string"   );
  view->addProperty(path, this, "enum"     );
  view->addProperty(path, this, "color"    );
  view->addProperty(path, this, "font"     );
  view->addProperty(path, this, "pointf"   );
  view->addProperty(path, this, "rectf"    );
  view->addProperty(path, this, "sizef"    );
  view->addProperty(path, this, "palette"  );
  view->addProperty(path, this, "alignment");
  view->addProperty(path, this, "lineDash" );
  view->addProperty(path, this, "angle"    );
}

QSize
Widget::
sizeHint() const
{
  return QSize(600, 600);
}
