#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>
#include <CQPropertyViewEditor.h>
#include <CQPropertyView.h>
#include <CQPropertyViewType.h>
#include <CQUtil.h>

#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>

namespace {

QString variantToString(const QVariant &var) {
  QString str;

  if (var.type() == QVariant::UserType) {
    if (! CQUtil::userVariantToString(var, str))
      return "";
  }
  else {
    if (! CQUtil::variantToString(var, str))
      return "";
  }

  return str;
}

}

//---

CQPropertyViewItem::
CQPropertyViewItem(CQPropertyViewItem *parent, QObject *object, const QString &name) :
 parent_(parent), object_(object), name_(name)
{
  CQUtil::PropInfo propInfo;

  if (CQUtil::getPropInfo(object_, name_, &propInfo) && propInfo.isWritable())
    editable_ = true;

  initValue_ = data();
}

CQPropertyViewItem::
~CQPropertyViewItem()
{
  for (auto &child : children_)
    delete child;
}

//---

void
CQPropertyViewItem::
addChild(CQPropertyViewItem *row)
{
  children_.push_back(row);

  invalidateVisible();
}

void
CQPropertyViewItem::
removeChild(CQPropertyViewItem *row)
{
  bool found = false;

  Children children;

  for (auto &child : children_) {
    if (child != row)
      children.push_back(child);
    else
      found = true;
  }

  if (found) {
    std::swap(children_, children);

    delete row;

    invalidateVisible();
  }
}

//---

const CQPropertyViewItem::Children &
CQPropertyViewItem::
visibleChildren() const
{
  if (! visibleChildrenValid_) {
    CQPropertyViewItem *th = const_cast<CQPropertyViewItem *>(this);

    th->anyChildrenHidden_ = false;

    for (auto &child : children_) {
      if (child->isHierHidden()) {
        th->anyChildrenHidden_ = true;
        break;
      }
    }

    if (anyChildrenHidden_) {
      for (auto &child : children_) {
        if (! child->isHierHidden())
          th->visibleChildren_.push_back(child);
      }
    }

    th->visibleChildrenValid_ = true;
  }

  if (anyChildrenHidden_)
    return visibleChildren_;
  else
    return children_;
}

void
CQPropertyViewItem::
invalidateVisible()
{
  visibleChildrenValid_ = false;
  anyChildrenHidden_    = false;

  visibleChildren_.clear();
}

bool
CQPropertyViewItem::
isHierHidden() const
{
  if (isHidden())
    return true;

  for (auto &child : children_) {
    if (! child->isHierHidden())
      return false;
  }

  return false;
}

//---

QObject *
CQPropertyViewItem::
hierObject() const
{
  QObject *obj = this->object();

  if (obj)
    return obj;

  for (auto &child : children_) {
    QObject *obj = child->object();

    if (obj)
      return obj;
  }

  for (auto &child : children_) {
    QObject *obj = child->hierObject();

    if (obj)
      return obj;
  }

  return nullptr;
}

QString
CQPropertyViewItem::
aliasName() const
{
  QString name = this->alias();

  if (name == "")
    name = this->name();

  return name;
}

QString
CQPropertyViewItem::
path(const QString &sep, bool alias, CQPropertyViewItem *root) const
{
  QString path;

  const CQPropertyViewItem *item = this;

  while (item) {
    QString name = (alias ? item->aliasName() : item->name());

    if (name.length()) {
      if (path.length())
        path = name + sep + path;
      else
        path = name;
    }

    item = item->parent();

    if (root && item == root)
      break;
  }

  return path;
}

// handle click
bool
CQPropertyViewItem::
click()
{
  CQUtil::PropInfo propInfo;
  QString          typeName;

  if (CQUtil::getPropInfo(object_, name_, &propInfo))
    typeName = propInfo.typeName();

  QVariant var = this->data();

  if (propInfo.isEnumType())
    return false;

  if (typeName == "bool" && propInfo.isWritable()) {
    if (! setData(! var.toBool())) {
      //std::cerr << "Failed to set property " << name_.toStdString() << std::endl;
    }

    emit valueChanged(object_, name_);

    return true;
  }

  return false;
}

/*! get property value
*/
QVariant
CQPropertyViewItem::
getEditorData() const
{
  CQUtil::PropInfo propInfo;
  QString          typeName;

  if (CQUtil::getPropInfo(object_, name_, &propInfo))
    typeName = propInfo.typeName();

  QVariant var = this->data();

  if (propInfo.isEnumType()) {
    int ind = var.toInt();

    QString str;

    if (enumIndToString(propInfo, ind, str))
      var = str;
  }

  return var;
}

/*! create editor widget for property
*/
QWidget *
CQPropertyViewItem::
createEditor(QWidget *parent)
{
  CQUtil::PropInfo propInfo;
  QString          typeName;

  if (CQUtil::getPropInfo(object_, name_, &propInfo))
    typeName = propInfo.typeName();

  QVariant var = this->data();

  if (propInfo.isEnumType()) {
    int ind = var.toInt();

    QString str;

    if (enumIndToString(propInfo, ind, str))
      var = str;
  }

  CQPropertyViewEditorFactory *editor = editor_;

  if (! editor)
    editor = CQPropertyViewMgrInst->getEditor(typeName);

  if      (editor) {
    CQPropertyViewMgrInst->setEditItem(this);

    widget_ = editor->createEdit(parent);

    editor->setValue(widget_, var);

    editor->connect(widget_, this, SLOT(updateValue()));
  }
  else if (propInfo.isEnumType()) {
    QString valueStr = var.toString();

    const QStringList &names = propInfo.enumNames();

    QComboBox *combo = new QComboBox(parent);

    combo->setObjectName("combo");

    combo->addItems(names);
    combo->setCurrentIndex(combo->findText(valueStr));

    connect(combo, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateValue()));

    widget_ = combo;
  }
  // bool - create toggle
  // TODO: use button press (no need to edit) see CQCheckTree.cpp
  else if (typeName == "bool") {
    QCheckBox *check = new QCheckBox(parent);

    check->setObjectName("check");

    check->setChecked(var.toBool());

    check->setText(check->isChecked() ? "true" : "false");

    check->setAutoFillBackground(true);
    //check->setLayoutDirection(Qt::RightToLeft);

    connect(check, SIGNAL(stateChanged(int)), this, SLOT(updateValue()));

    widget_ = check;
  }
  else if (var.type() == QVariant::UserType) {
    QString valueStr;

    if (! CQUtil::userVariantToString(var, valueStr)) {
      //std::cerr << "Failed to convert to string" << std::endl;
    }

    widget_ = createDefaultEdit(parent, valueStr);
  }
  else {
    QString valueStr;

    if (! CQUtil::variantToString(var, valueStr)) {
      //std::cerr << "Failed to convert to string" << std::endl;
    }

    widget_ = createDefaultEdit(parent, valueStr);
  }

  return widget_;
}

QWidget *
CQPropertyViewItem::
createDefaultEdit(QWidget *parent, const QString &valueStr)
{
  if (values().length()) {
    QComboBox *combo = new QComboBox(parent);

    combo->setObjectName("combo");

    combo->addItems(values());
    combo->setCurrentIndex(combo->findText(valueStr));

    connect(combo, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateValue()));

    return combo;
  }
  else {
    QLineEdit *edit = new QLineEdit(parent);

    edit->setObjectName("edit");

    edit->setText(valueStr);

    connect(edit, SIGNAL(editingFinished()), this, SLOT(updateValue()));

    return edit;
  }
}

/*! set property from variant
*/
void
CQPropertyViewItem::
setEditorData(const QVariant &value)
{
  CQUtil::PropInfo propInfo;

  if (CQUtil::getPropInfo(object_, name_, &propInfo) && propInfo.isWritable()) {
    QString typeName = propInfo.typeName();

    CQPropertyViewType *type = CQPropertyViewMgrInst->getType(typeName);

    if      (type) {
      if (! type->setEditorData(this, value)) {
        //std::cerr << "Failed to set property " << name_.toStdString() << std::endl;
      }
    }
    else if (propInfo.isEnumType()) {
      QString name = CQUtil::variantToString(value);

      int ind = -1;

      if (enumStringToInd(propInfo, name, ind)) {
        QVariant v(ind);

        if (! this->setData(v)) {
          //std::cerr << "Failed to set property " << name_.toStdString() << std::endl;
        }
      }
    }
    else {
      if (! this->setData(value)) {
        //std::cerr << "Failed to set property " << name_.toStdString() << std::endl;
      }
    }

    emit valueChanged(object_, name_);
  }
}

/*! update property on widget change
*/
void
CQPropertyViewItem::
updateValue()
{
  CQUtil::PropInfo propInfo;
  QString          typeName;

  if (CQUtil::getPropInfo(object_, name_, &propInfo))
    typeName = propInfo.typeName();

  CQPropertyViewEditorFactory *editor = editor_;

  if (! editor)
    editor = CQPropertyViewMgrInst->getEditor(typeName);

  if      (editor) {
    QVariant var = editor->getValue(widget_);

    setEditorData(var);
  }
  else if (propInfo.isEnumType()) {
    QComboBox *combo = qobject_cast<QComboBox *>(widget_);
    assert(combo);

    QString text = combo->currentText();

    setEditorData(text);
  }
  else if (typeName == "bool") {
    QCheckBox *check = qobject_cast<QCheckBox *>(widget_);
    assert(check);

    check->setText(check->isChecked() ? "true" : "false");

    QString text = (check->isChecked() ? "1" : "0");

    setEditorData(text);
  }
  else if (propInfo.type() == QVariant::UserType) {
    QString text = getDefaultValue();

    QVariant var = this->data();

    if (CQUtil::userVariantFromString(var, text))
      setEditorData(var);
  }
  else {
    QString text = getDefaultValue();

    setEditorData(text);
  }
}

QString
CQPropertyViewItem::
getDefaultValue() const
{
  QLineEdit *edit = qobject_cast<QLineEdit *>(widget_);

  if (edit)
    return edit->text();

  QComboBox *combo = qobject_cast<QComboBox *>(widget_);

  if (combo)
    return combo->currentText();

  assert(false);

  return "";
}

bool
CQPropertyViewItem::
isWritable() const
{
  return true;
}

QVariant
CQPropertyViewItem::
data() const
{
  QVariant var;

  if (! CQUtil::getProperty(object_, name_, var))
    var = QVariant();

  return var;
}

bool
CQPropertyViewItem::
setData(const QVariant &value)
{
  if (! CQUtil::setProperty(object_, name_, value))
    return false;

  return true;
}

QString
CQPropertyViewItem::
nameTip() const
{
  QString tip = path(".", /*alias*/true);

  if (desc() != "")
    return QString("%1 (%2)").arg(desc()).arg(tip);
  else
    return tip;
}

QString
CQPropertyViewItem::
valueTip() const
{
  QString tip = calcTip();

  if (desc() != "")
    return QString("%1 (%2)").arg(desc()).arg(tip);
  else
    return tip;
}

QString
CQPropertyViewItem::
calcTip() const
{
  CQUtil::PropInfo propInfo;
  QString          typeName;

  if (CQUtil::getPropInfo(object_, name_, &propInfo))
    typeName = propInfo.typeName();

  QVariant var = this->data();

  CQPropertyViewType *type = CQPropertyViewMgrInst->getType(typeName);

  if (type)
    return type->tip(var);

  if (typeName == "bool")
    return (var.toBool() ? "true" : "false");

  if      (propInfo.isEnumType()) {
    int ind = var.toInt();

    QString str;

    if (enumIndToString(propInfo, ind, str))
      return str;
  }
  else if (var.type() == QVariant::UserType) {
    QString str;

    if (! CQUtil::userVariantToString(var, str))
      return "";

    return str;
  }

  return var.toString();
}

bool
CQPropertyViewItem::
paint(const CQPropertyViewDelegate *delegate, QPainter *painter,
      const QStyleOptionViewItem &option, const QModelIndex &index)
{
  bool inside = isInside();

  CQUtil::PropInfo propInfo;
  QString          typeName;

  if (CQUtil::getPropInfo(object_, name_, &propInfo))
    typeName = propInfo.typeName();

  QVariant var = this->data();

  CQPropertyViewType *type = CQPropertyViewMgrInst->getType(typeName);

  if      (type) {
    type->draw(delegate, painter, option, index, var, inside);
  }
  else if (typeName == "bool") {
    delegate->drawCheckInside(painter, option, var.toBool(), index, inside);
  }
  else if (propInfo.isEnumType()) {
    int ind = var.toInt();

    QString str;

    if (enumIndToString(propInfo, ind, str))
      delegate->drawString(painter, option, str, index, inside);
  }
  else if (var.type() == QVariant::UserType) {
    QString str;

    if (! CQUtil::userVariantToString(var, str))
      return false;

    delegate->drawString(painter, option, str, index, inside);
  }
  else {
    QString str;

    if (! CQUtil::variantToString(var, str))
      return false;

    delegate->drawString(painter, option, str, index, inside);
  }

  return true;
}

QString
CQPropertyViewItem::
initStr() const
{
  return variantToString(initValue());
}

QString
CQPropertyViewItem::
dataStr() const
{
  return variantToString(data());
}

bool
CQPropertyViewItem::
enumIndToString(const CQUtil::PropInfo &propInfo, int ind, QString &str) const
{
  const QStringList &names = propInfo.enumNames();

  if (ind < 0 || ind >= names.count())
    return false;

  str = names[ind];

  return true;
}

bool
CQPropertyViewItem::
enumStringToInd(const CQUtil::PropInfo &propInfo, const QString &str, int &ind) const
{
  const QStringList &names = propInfo.enumNames();

  for (int i = 0; i < names.size(); ++i) {
    if (str == names[i]) {
      ind = i;
      return true;
    }
  }

  return false;
}
