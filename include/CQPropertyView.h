#ifndef CQPropertyView_H
#define CQPropertyView_H

#include <QString>
#include <map>

class CQPropertyViewType;
class CQPropertyViewEditorMgr;
class CQPropertyViewEditorFactory;
class CQPropertyViewItem;

#define CQPropertyViewMgrInst CQPropertyViewMgr::instance()

/*!
 * \brief Property view manager for registered types
 */
class CQPropertyViewMgr {
 public:
  static CQPropertyViewMgr *instance();

 ~CQPropertyViewMgr();

  void addType(const QString &name, CQPropertyViewType *type);

  CQPropertyViewType *getType(const QString &name) const;

  CQPropertyViewEditorFactory *getEditor(const QString &name) const;

  CQPropertyViewItem *editItem() const { return editItem_; }
  void setEditItem(CQPropertyViewItem *editItem) { editItem_ = editItem; }

  CQPropertyViewItem *drawItem() const { return drawItem_; }
  void setDrawItem(CQPropertyViewItem *drawItem) { drawItem_ = drawItem; }

  QString userName(const QString &typeName) const;

  void setUserName(const QString &typeName, const QString &userName);

 private:
  CQPropertyViewMgr();

 private:
  using Types       = std::map<QString,CQPropertyViewType *>;
  using UserNameMap = std::map<QString,QString>;

  Types                       types_;
  UserNameMap                 userNameMap_;
  CQPropertyViewEditorMgr*    editorMgr_ { nullptr };
  mutable CQPropertyViewItem* editItem_  { nullptr };
  mutable CQPropertyViewItem* drawItem_  { nullptr };
};

#endif
