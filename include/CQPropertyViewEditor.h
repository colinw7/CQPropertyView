#ifndef CQPropertyViewEditor_H
#define CQPropertyViewEditor_H

#include <QString>
#include <QVariant>
#include <map>

class QWidget;
class QObject;

//------

/*!
 * \brief Base class for editor factories
 */
class CQPropertyViewEditorFactory {
 public:
  CQPropertyViewEditorFactory() { }

  virtual ~CQPropertyViewEditorFactory() { }

  virtual QWidget *createEdit(QWidget *parent=0) = 0;

  virtual void connect(QWidget *w, QObject *obj, const char *method) = 0;

  virtual QVariant getValue(QWidget *w) = 0;

  virtual void setValue(QWidget *w, const QVariant &var) = 0;
};

//------

/*!
 * \brief Manager class for registered editors
 */
class CQPropertyViewEditorMgr {
 public:
  CQPropertyViewEditorMgr();

 ~CQPropertyViewEditorMgr();

  void setEditor(const QString &typeName, CQPropertyViewEditorFactory *editor);

  CQPropertyViewEditorFactory *getEditor(const QString &typeName) const;

 private:
  using Editors = std::map<QString, CQPropertyViewEditorFactory *>;

  Editors editors_;
};

#endif
