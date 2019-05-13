#include <CQPropertyView.h>
#include <CQPropertyViewEditor.h>
#include <CQPropertyViewIntegerType.h>
#include <CQPropertyViewRealType.h>
#include <CQPropertyViewColorType.h>
#include <CQPropertyViewFontType.h>
#include <CQPropertyViewPointFType.h>
#include <CQPropertyViewRectFType.h>
#include <CQPropertyViewSizeFType.h>
#ifdef CQPROPERTY_VIEW_PALETTE
#include <CQPropertyViewPaletteType.h>
#endif
#include <CQPropertyViewAlignType.h>
#ifdef CQPROPERTY_VIEW_LINE_DASH
#include <CQPropertyViewLineDashType.h>
#endif
#ifdef CQPROPERTY_VIEW_ANGLE
#include <CQPropertyViewAngleType.h>
#endif
#include <cassert>

CQPropertyViewMgr *
CQPropertyViewMgr::
instance()
{
  static CQPropertyViewMgr *instance;

  if (! instance)
    instance = new CQPropertyViewMgr;

  return instance;
}

CQPropertyViewMgr::
CQPropertyViewMgr()
{
  editorMgr_ = new CQPropertyViewEditorMgr;

  addType("int"          , new CQPropertyViewIntegerType );
  addType("double"       , new CQPropertyViewRealType    );
  addType("QColor"       , new CQPropertyViewColorType   );
  addType("QFont"        , new CQPropertyViewFontType    );
  addType("QPointF"      , new CQPropertyViewPointFType  );
  addType("QRectF"       , new CQPropertyViewRectFType   );
  addType("QSizeF"       , new CQPropertyViewSizeFType   );
#ifdef CQPROPERTY_VIEW_PALETTE
  addType("QPalette"     , new CQPropertyViewPaletteType );
#endif
  addType("Qt::Alignment", new CQPropertyViewAlignType   );
#ifdef CQPROPERTY_VIEW_LINE_DASH
  addType("CLineDash"    , new CQPropertyViewLineDashType);
#endif
#ifdef CQPROPERTY_VIEW_ANGLE
  addType("CAngle"       , new CQPropertyViewAngleType   );
#endif
}

CQPropertyViewMgr::
~CQPropertyViewMgr()
{
  delete editorMgr_;

  for (auto &p : types_) {
    CQPropertyViewType *type = p.second;

    delete type;
  }
}

void
CQPropertyViewMgr::
addType(const QString &name, CQPropertyViewType *type)
{
  assert(! getType(name));

  types_[name] = type;

  type->setName(name);

  editorMgr_->setEditor(name, type->getEditor());
}

CQPropertyViewType *
CQPropertyViewMgr::
getType(const QString &name) const
{
  auto p = types_.find(name);

  if (p == types_.end())
    return nullptr;

  return (*p).second;
}

CQPropertyViewEditorFactory *
CQPropertyViewMgr::
getEditor(const QString &name) const
{
  return editorMgr_->getEditor(name);
}

void
CQPropertyViewMgr::
setUserName(const QString &typeName, const QString &userName)
{
  userNameMap_[typeName] = userName;
}

QString
CQPropertyViewMgr::
userName(const QString &name) const
{
  CQPropertyViewType *type = getType(name);

  if (type)
    return type->userName();

  auto p = userNameMap_.find(name);

  if (p != userNameMap_.end())
    return (*p).second;

  return "";
}
