#include <CQPropertyViewItem.h>
#include <CQPropertyViewModel.h>
#include <CQPropertyViewDelegate.h>
#include <CQPropertyViewEditor.h>
#include <CQPropertyView.h>
#include <CQPropertyViewType.h>
#include <CQPropertyViewUtil.h>
#include <CQUtil.h>

#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>

class CQPropertyViewItemTableTip {
 public:
  void addRow(const QString &name, const QString &value) {
    if (! inTable_)
      str_ += "<table>\n";

    str_ += QString("<tr><th>%1</th><td>&nbsp;</td><td>%2</td></tr>\n").
               arg(escapeText(name)).arg(escapeText(value));

    inTable_ = true;
  }

  QString str() {
    if (inTable_)
      str_ += "</table>\n";

    return str_;
  }

 private:
  QString escapeText(const QString &str) const {
    QString str1;

    int i   = 0;
    int len = str.length();

    while (i < len) {
      if      (str[i] == '<') {
        str1 += "&lt;"; ++i;
      }
      else if (str[i] == '>') {
        str1 += "&gt;"; ++i;
      }
      else if (str[i] == '"') {
        str1 += "&quot;"; ++i;
      }
      else if (str[i] == '&') {
        str1 += "&amp;"; ++i;
      }
      else
        str1 += str[i++];
    }

    return str1;
  }

 private:
  QString str_;
  bool    inTable_ { false };
};

//---

CQPropertyViewItem::
CQPropertyViewItem(CQPropertyViewModel *model, CQPropertyViewItem *parent,
                   QObject *object, const QString &name) :
 model_(model), parent_(parent), object_(object), name_(name)
{
  setObjectName(name);

  CQUtil::PropInfo propInfo;

  if (object_ && CQUtil::getPropInfo(object_, name_, &propInfo) && propInfo.isWritable())
    editable_ = true;
  else
    editable_ = false;

  initValue_ = data();
}

CQPropertyViewItem::
~CQPropertyViewItem()
{
  for (auto &child : children())
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

  for (auto &child : this->children()) {
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
    auto *th = const_cast<CQPropertyViewItem *>(this);

    assert(th->visibleChildren_.empty());

    th->visibleChildrenSet_ = false;

    if (! isHierHidden()) {
      for (auto &child : children_) {
        if (child->isHierHidden()) {
          th->visibleChildrenSet_ = true;
          break;
        }
      }

      if (th->visibleChildrenSet_) {
        for (auto &child : children_) {
          if (! child->isHierHidden())
            th->visibleChildren_.push_back(child);
        }
      }

      th->visibleChildrenValid_ = true;
    }
    else
      th->visibleChildrenSet_ = true;
  }

  if (visibleChildrenSet_)
    return visibleChildren_;
  else
    return children_;
}

void
CQPropertyViewItem::
invalidateVisible()
{
  visibleChildrenValid_ = false;
  visibleChildrenSet_   = false;

  visibleChildren_.clear();

  if (parent())
    parent()->invalidateVisible();
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

  if (object())
    return false;

  return true;
}

//---

QObject *
CQPropertyViewItem::
hierObject() const
{
  auto *obj = this->object();

  if (obj)
    return obj;

  for (auto &child : children_) {
    auto *obj = child->object();

    if (obj)
      return obj;
  }

  for (auto &child : children_) {
    auto *obj = child->hierObject();

    if (obj)
      return obj;
  }

  return nullptr;
}

CQPropertyViewItem &
CQPropertyViewItem::
setHidden(bool b)
{
  hidden_ = b;

  invalidateVisible();

  return *this;
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

  const auto *item = this;

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

  if (object() && CQUtil::getPropInfo(object(), name(), &propInfo))
    typeName = propInfo.typeName();

  QVariant var = this->data();

  if (propInfo.isEnumType())
    return false;

  if (typeName == "bool" && propInfo.isWritable()) {
    if (! setData(! var.toBool())) {
      //std::cerr << "Failed to set property " << name().toStdString() << std::endl;
    }

    emit valueChanged(object(), name());

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

  if (object() && CQUtil::getPropInfo(object(), name(), &propInfo))
    typeName = propInfo.typeName();

  QVariant var = this->data();

  if (propInfo.isEnumType()) {
    int value = var.toInt();

    QString str;

    if (CQUtil::getPropInfoEnumValueName(propInfo, value, str))
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

  if (object() && CQUtil::getPropInfo(object(), name(), &propInfo))
    typeName = propInfo.typeName();

  QVariant var = this->data();

  if (propInfo.isEnumType()) {
    int value = var.toInt();

    QString str;

    if (CQUtil::getPropInfoEnumValueName(propInfo, value, str))
      var = str;
  }

  auto *editor = editor_;

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

    auto *combo = new QComboBox(parent);

    combo->setObjectName("combo");

    combo->addItems(names);
    combo->setCurrentIndex(combo->findText(valueStr));

    connect(combo, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateValue()));

    widget_ = combo;
  }
  // bool - create toggle
  // TODO: use button press (no need to edit) see CQCheckTree.cpp
  else if (typeName == "bool") {
    auto *check = new QCheckBox(parent);

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
    auto *combo = new QComboBox(parent);

    combo->setObjectName("combo");

    combo->addItems(values());
    combo->setCurrentIndex(combo->findText(valueStr));

    connect(combo, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateValue()));

    return combo;
  }
  else {
    auto *edit = new QLineEdit(parent);

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

  if (object() && CQUtil::getPropInfo(object(), name(), &propInfo) && propInfo.isWritable()) {
    QString typeName = propInfo.typeName();

    auto *type = CQPropertyViewMgrInst->getType(typeName);

    if      (type) {
      if (! type->setEditorData(this, value)) {
        //std::cerr << "Failed to set property " << name().toStdString() << std::endl;
      }
    }
    else if (propInfo.isEnumType()) {
      QString name = CQUtil::variantToString(value);

      int ind;

      if (CQUtil::getPropInfoEnumNameValue(propInfo, name, ind)) {
        QVariant v(ind);

        if (! this->setData(v)) {
          //std::cerr << "Failed to set property " << name().toStdString() << std::endl;
        }
      }
      else {
        //std::cerr << "Failed to set property " << name().toStdString() << std::endl;
      }
    }
    else {
      if (! this->setData(value)) {
        //std::cerr << "Failed to set property " << name().toStdString() << std::endl;
      }
    }

    emit valueChanged(object(), name());
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

  if (object() && CQUtil::getPropInfo(object(), name(), &propInfo))
    typeName = propInfo.typeName();

  auto *editor = editor_;

  if (! editor)
    editor = CQPropertyViewMgrInst->getEditor(typeName);

  if      (editor) {
    QVariant var = editor->getValue(widget_);

    setEditorData(var);
  }
  else if (propInfo.isEnumType()) {
    auto *combo = qobject_cast<QComboBox *>(widget_);
    assert(combo);

    QString text = combo->currentText();

    setEditorData(text);
  }
  else if (typeName == "bool") {
    auto *check = qobject_cast<QCheckBox *>(widget_);
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
  auto *edit = qobject_cast<QLineEdit *>(widget_);

  if (edit)
    return edit->text();

  auto *combo = qobject_cast<QComboBox *>(widget_);

  if (combo)
    return combo->currentText();

  assert(false);

  return "";
}

QVariant
CQPropertyViewItem::
data() const
{
  if (isDirty())
    return dirtyValue();

  //---

  QVariant var;

  if (! object() || ! CQUtil::getProperty(object(), name(), var))
    var = QVariant();

  return var;
}

bool
CQPropertyViewItem::
setData(const QVariant &value)
{
  if (! isEditable())
    return false;

  if (model_->isAutoUpdate()) {
    setDirty(false);

    if (! object() || ! CQUtil::setProperty(object(), name(), value))
      return false;
  }
  else {
    setDirty(true);
    setDirtyValue(value);
  }

  return true;
}

bool
CQPropertyViewItem::
applyDirty()
{
  setDirty(false);

  if (! object() || ! CQUtil::setProperty(object(), name(), dirtyValue()))
    return false;

  return true;
}

QVariant
CQPropertyViewItem::
tclData() const
{
  if (isDirty())
    return dirtyValue();

  //---

  QVariant var;

  if (! object() || ! CQUtil::getTclProperty(object(), name(), var))
    var = QVariant();

  return var;
}

QString
CQPropertyViewItem::
typeName() const
{
  CQUtil::PropInfo propInfo;
  QString          typeName;

  if (object() && CQUtil::getPropInfo(object(), name(), &propInfo))
    typeName = propInfo.typeName();

  return typeName;
}

bool
CQPropertyViewItem::
isEnum() const
{
  CQUtil::PropInfo propInfo;

  if (object() && CQUtil::getPropInfo(object(), name(), &propInfo))
    return propInfo.isEnumType();

  return false;
}

QString
CQPropertyViewItem::
nameTip(bool html) const
{
  QString tip = path(".", /*alias*/true);

  if (! object())
    return tip;

  if (html) {
    CQPropertyViewItemTableTip tableTip;

    tableTip.addRow("Property", tip);

    addCommonType(tableTip);

    return tableTip.str();
  }

  return tip;
}

QString
CQPropertyViewItem::
valueTip(bool html) const
{
  if (! object())
    return "";

  QString tip = calcTip();

  if (html) {
    CQPropertyViewItemTableTip tableTip;

    tableTip.addRow("Value", tip);

    addCommonType(tableTip);

    return tableTip.str();
  }

  return tip;
}

void
CQPropertyViewItem::
addCommonType(CQPropertyViewItemTableTip &tableTip) const
{
  if (desc() != "")
    tableTip.addRow("Description", desc());

  QString userTypeName = this->userTypeName();

  if (userTypeName != "")
    tableTip.addRow("Type", userTypeName);

  if (minValue().isValid() && minValue().toString().length())
    tableTip.addRow("Min", minValue().toString());

  if (maxValue().isValid() && maxValue().toString().length())
    tableTip.addRow("Max", maxValue().toString());
}

QString
CQPropertyViewItem::
calcTip() const
{
  CQUtil::PropInfo propInfo;
  QString          typeName;

  if (object() && CQUtil::getPropInfo(object(), name(), &propInfo))
    typeName = propInfo.typeName();

  QVariant var = this->data();

  auto *type = CQPropertyViewMgrInst->getType(typeName);

  if (type)
    return type->tip(var);

  if (typeName == "bool")
    return (var.toBool() ? "true" : "false");

  if      (propInfo.isEnumType()) {
    int value = var.toInt();

    QString str;

    if (CQUtil::getPropInfoEnumValueName(propInfo, value, str))
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
  CQUtil::PropInfo propInfo;
  QString          typeName;

  if (object() && CQUtil::getPropInfo(object(), name(), &propInfo))
    typeName = propInfo.typeName();

  QVariant var = this->data();

  auto *type = CQPropertyViewMgrInst->getType(typeName);

  CQPropertyViewDelegate::ItemState itemState;

  itemState.inside = isInside();
  itemState.dirty  = isDirty();

  if      (type) {
    type->draw(this, delegate, painter, option, index, var, itemState);
  }
  else if (typeName == "bool") {
    delegate->drawCheckInside(painter, option, var.toBool(), index, itemState);
  }
  else if (propInfo.isEnumType()) {
    int value = var.toInt();

    QString str;

    if (CQUtil::getPropInfoEnumValueName(propInfo, value, str))
      delegate->drawString(painter, option, str, index, itemState);
  }
  else if (var.type() == QVariant::UserType) {
    QString str;

    if (! CQUtil::userVariantToString(var, str))
      return false;

    delegate->drawString(painter, option, str, index, itemState);
  }
  else {
    QString str;

    if (! CQUtil::variantToString(var, str))
      return false;

    delegate->drawString(painter, option, str, index, itemState);
  }

  return true;
}

QString
CQPropertyViewItem::
initStr() const
{
  return CQPropertyViewUtil::variantToString(initValue());
}

QString
CQPropertyViewItem::
dataStr() const
{
  return CQPropertyViewUtil::variantToString(data());
}

#if 0
bool
CQPropertyViewItem::
enumValueToString(const CQUtil::PropInfo &propInfo, int value, QString &str) const
{
  return CQUtil::getPropInfoEnumValueName(propInfo, value, str);
#if 0
  const QStringList &names = propInfo.enumNames();

  if (ind < 0 || ind >= names.count())
    return false;

  str = names[ind];

  return true;
#endif
}
#endif

#if 0
bool
CQPropertyViewItem::
enumStringToValue(const CQUtil::PropInfo &propInfo, const QString &str, int &value) const
{
  return CQUtil::getPropInfoEnumNameValue(propInfo, str, value);
#if 0
  const QStringList &names = propInfo.enumNames();

  for (int i = 0; i < names.size(); ++i) {
    if (str == names[i]) {
      ind = i;
      return true;
    }
  }

  return false;
#endif
}
#endif

QString
CQPropertyViewItem::
userTypeName() const
{
  QString typeName = this->typeName();

  QString userTypeName = CQPropertyViewMgrInst->userName(typeName);

  if (userTypeName != "")
    return userTypeName;

  if      (typeName == "QString")
    return "string";
  else if (typeName == "QRectF" || typeName == "QRect")
    return "rectangle";
  else if (typeName == "QSizeF" || typeName == "QSize")
    return "size";
  else if (typeName == "QPointF" || typeName == "QPoint")
    return "point";
  else if (typeName == "Qt::Alignment")
    return "alignment";

  if (isEnum())
    return "enum";

  return typeName;
}
