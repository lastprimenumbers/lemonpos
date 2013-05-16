#ifndef LIMIT_EDITOR_H
#define LIMIT_EDITOR_H

#include <QDialog>
#include "../../dataAccess/azahar.h"
#include "../../src/structs.h"
#include "ui_limiteditor.h"



namespace Ui {
class limit_editor;
}

class limiteditor : public QDialog
{
    Q_OBJECT
    
public:
    explicit limiteditor(QWidget *parent = 0);
    ~limiteditor();
    QSqlDatabase db;
    void setDb(QSqlDatabase pippo);


private slots:
    void addLimit();
    
private:
    Ui::limit_editor *ui;
};

#endif // LIMIT_EDITOR_H
