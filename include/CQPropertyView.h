#ifndef CQPropertyView_H
#define CQPropertyView_H

#include <QString>
#include <map>

class CQPropertyViewType;
class CQPropertyViewEditorMgr;
class CQPropertyViewEditorFactory;
class CQPropertyViewItem;

#define CQPropertyViewMgrInst CQPropertyViewMgr::instance()

class CQPropertyViewMgr {
 public:
  static CQPropertyViewMgr *instance();

 ~CQPropertyViewMgr();

  void addType(const QString &name, CQPropertyViewType *type);

  CQPropertyViewType *getType(const QString &name) const;

  CQPropertyViewEditorFactory *getEditor(const QString &name) const;

  CQPropertyViewItem *editItem() const { return editItem_; }
  void setEditItem(CQPropertyViewItem *editItem) { editItem_ = editItem; }

 private:
  CQPropertyViewMgr();

 private:
  typedef std::map<QString,CQPropertyViewType *> Types;

  Types                       types_;
  CQPropertyViewEditorMgr*    editorMgr_ { nullptr };
  mutable CQPropertyViewItem* editItem_  { nullptr };
};

#endif
