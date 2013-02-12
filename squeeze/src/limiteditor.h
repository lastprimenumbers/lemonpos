#ifndef LIMIT_EDITOR_H
#define LIMIT_EDITOR_H

#include <QDialog>
#include "../../dataAccess/azahar.h"

namespace Ui {
class limiteditor;
}

class limiteditor : public QDialog
{
    Q_OBJECT
    
public:
    explicit limiteditor(QWidget *parent = 0);
    ~limiteditor();
    
private:
    Ui::limiteditor *ui;
};

#endif // LIMIT_EDITOR_H
