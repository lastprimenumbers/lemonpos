#ifndef UPGRADE_H
#define UPGRADE_H

#include <QWizard>

namespace Ui {
class Upgrade;
}

class Upgrade : public QWizard
{
    Q_OBJECT
    
public:
    explicit Upgrade(QWidget *parent = 0);
    ~Upgrade();
    // accetta azahar e versione corrente
    // si ricollega come root e fa il backup
    // si ricollega come utente e lancia tutti gli aggiornamenti
    // si ricollega come root e applica

private:
    Ui::Upgrade *ui;
};

#endif // UPGRADE_H
