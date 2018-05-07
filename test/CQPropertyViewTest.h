#ifndef CQPropertyViewTest_H
#define CQPropertyViewTest_H

#include <QDialog>
#include <QFrame>

#include <CQUtil.h>
#include <CLineDash.h>
#include <CAngle.h>

class CQIconCombo;
class CQPropertyViewModel;
class CQPropertyViewTree;

//---

class Dialog : public QDialog {
  Q_OBJECT

 public:
  Dialog();
 ~Dialog();

 private slots:
  void filterSlot();

 private:
  QLineEdit*           filterEdit_  { nullptr };
  CQIconCombo*         filterCombo_ { nullptr };
  CQPropertyViewModel* model_       { nullptr };
  CQPropertyViewTree*  view_        { nullptr };
};

//---

class Widget : public QFrame {
  Q_OBJECT

  Q_PROPERTY(int           integer   READ getInt      WRITE setInt      )
  Q_PROPERTY(double        real      READ getReal     WRITE setReal     )
  Q_PROPERTY(QString       string    READ getString   WRITE setString   )
  Q_PROPERTY(Enum          enum      READ getEnum     WRITE setEnum     )
  Q_PROPERTY(QColor        color     READ color       WRITE setColor    )
  Q_PROPERTY(QFont         font      READ font        WRITE setFont     )
  Q_PROPERTY(QPointF       pointf    READ pointf      WRITE setPointf   )
  Q_PROPERTY(QRectF        rectf     READ rectf       WRITE setRectf    )
  Q_PROPERTY(QSizeF        sizef     READ sizef       WRITE setSizef    )
  Q_PROPERTY(QPalette      palette   READ palette     WRITE setPalette  )
  Q_PROPERTY(Qt::Alignment alignment READ alignment   WRITE setAlignment)
  Q_PROPERTY(CLineDash     lineDash  READ getLineDash WRITE setLineDash )
  Q_PROPERTY(CAngle        angle     READ getAngle    WRITE setAngle    )

  Q_ENUMS(Enum)

 public:
  enum class Enum {
    ONE,
    TWO,
    THREE
  };

 public:
  Widget();

  void addPropeties(const QString &path, CQPropertyViewTree *tree);

  int getInt() const { return integer_; }
  void setInt(int i) { integer_ = i; }

  double getReal() const { return real_; }
  void setReal(double r) { real_ = r; }

  const QString &getString() const { return string_; }
  void setString(const QString &s) { string_ = s; }

  const Enum &getEnum() const { return enumVal_; }
  void setEnum(const Enum &e) { enumVal_ = e; }

  const QColor &color() const { return color_; }
  void setColor(const QColor &v) { color_ = v; }

  const QFont &font() const { return font_; }
  void setFont(const QFont &v) { font_ = v; }

  const QPointF &pointf() const { return pointf_; }
  void setPointf(const QPointF &v) { pointf_ = v; }

  const QRectF &rectf() const { return rectf_; }
  void setRectf(const QRectF &v) { rectf_ = v; }

  const QSizeF &sizef() const { return sizef_; }
  void setSizef(const QSizeF &v) { sizef_ = v; }

  const QPalette &palette() const { return palette_; }
  void setPalette(const QPalette &v) { palette_ = v; }

  const CLineDash &getLineDash() const { return lineDash_; }
  void setLineDash(const CLineDash &v) { lineDash_ = v; }

  const CAngle &getAngle() const { return angle_; }
  void setAngle(const CAngle &v) { angle_ = v; }

  const Qt::Alignment &alignment() const { return alignment_; }
  void setAlignment(const Qt::Alignment &v) { alignment_ = v; }

  QSize sizeHint() const;

 private:
  int           integer_   { 1 };
  double        real_      { 2.3 };
  QString       string_    { "test" };
  Enum          enumVal_   { Widget::Enum::ONE };
  QColor        color_     { Qt::red };
  QFont         font_;
  QPointF       pointf_    { 2, 4 };
  QRectF        rectf_     { 1, 1, 4, 4 };
  QSizeF        sizef_     { 10, 20 };
  QPalette      palette_;
  CLineDash     lineDash_  { 2, 2 };
  CAngle        angle_     { 45 };
  Qt::Alignment alignment_ { Qt::AlignCenter };
};

#endif
