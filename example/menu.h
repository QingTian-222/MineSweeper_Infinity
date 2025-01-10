#ifndef MENU_H
#define MENU_H

#include <QGraphicsScene>
#include <QLabel>
#include <QWheelEvent>
#include <QEvent>
#include <QWidget>
#include <qgraphicsview.h>

QT_BEGIN_NAMESPACE
namespace Ui { class menu; }
QT_END_NAMESPACE

class menu : public QWidget
{
    Q_OBJECT

public:
    menu(QWidget *parent = nullptr);
    ~menu();

    void extracted(QLabel *&label);
    void moveAnimation(QLabel *label, QPoint end, QPoint endsize);
    void shakeWidget(int duration, int range, int shakeCount);
    bool eventFilter(QObject *obj, QEvent *event);
    void bfs(int x, int y);
    void open(int x, int y);
    void sign(int x, int y);
    void dbopen(int x, int y);
    void bfs2(int x, int y);

private slots:




private:
    Ui::menu *ui;
    QGraphicsScene* scene;
    QGraphicsView* view;
};
#endif // MENU_H
