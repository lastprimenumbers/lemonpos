//#ifndef CLIENTTAG_H
//#define CLIENTTAG_H
#include <QWidget>
#include <QtSql>

namespace Ui {
class clienttag;
}

class clienttag : public QWidget
{
    Q_OBJECT
    
public:
    explicit clienttag(QWidget *parent = 0);
    ~clienttag();
    QSqlDatabase db;
    void setDb(QSqlDatabase db);
    void setTags(QStringList tags);
    QStringList getTags();
    
private:
    Ui::clienttag *ui;

private slots:
  void addTag();
  void removeTag();
  void createTag();
};

//#endif // CLIENTTAG_H
