#include "menu.h"
#include "ui_menu.h"
#include "QTimer"
#include "QRandomGenerator"

#include <QGraphicsEffect>
#include <QPropertyAnimation>

#include<QGraphicsWidget>
#include<QGraphicsProxyWidget>
#include<QLabel>
#include <QGroupBox>
#include <QLineEdit>
#include <QFormLayout>

#include<QEvent>
#include <QGraphicsSceneMouseEvent>
#include<QMessageBox>
#include<QQueue>
#include <QImage>

const int len =30;//单元格边长
const int block_size=16;//区块边长
bool isCtrl;
QColor color[9]={{0,162,232},{34,177,76},{255,80,27},{0,2,219},{136,0,21},{21,128,129}};
int ran(int a, int b) {
    return QRandomGenerator::global()->bounded(a, b + 1);
}
QPoint getLoc(QPointF tp){//实际坐标得到单元格坐标

    return {(int)std::floor(1.0*tp.x()/len),(int)std::floor(1.0*tp.y()/len)};
}
QPoint getBlock(QPointF tp){//实际坐标得到区块坐标
    QPoint temp=getLoc(tp);
    return {(int)std::floor(1.0*temp.x()/block_size),(int)std::floor(1.0*temp.y()/block_size)};
}
QPoint getBlock_byItem(QPoint tp){//单位格坐标得到区块坐标
    return {(int)std::floor(1.0*tp.x()/block_size),(int)std::floor(1.0*tp.y()/block_size)};
}
void fadeItem(QGraphicsItem *item,int duration,float start,float end){//透明度
    QGraphicsOpacityEffect* opa=new QGraphicsOpacityEffect();
    item->setGraphicsEffect(opa);

    item->setVisible(true);
    QPropertyAnimation* animation = new QPropertyAnimation(opa,"opacity");
    animation->setDuration(duration);
    animation->setStartValue(start);
    animation->setEndValue(end);
    QObject::connect(animation, &QPropertyAnimation::finished, [=]() {
        item->setGraphicsEffect(nullptr);
    });
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}
void menu::shakeWidget(int duration=50, int range=5, int shakeCount=5){
    QTimer::singleShot(0,this, [=]() {
        QPropertyAnimation* animation = new QPropertyAnimation(this, "pos");
        QPoint sp = this->pos();
        animation->setKeyValueAt(0.1,QPoint(sp.x() + range, sp.y()));
        animation->setKeyValueAt(0.2,QPoint(sp.x() - range, sp.y()));
        animation->setKeyValueAt(0.3,QPoint(sp.x(), sp.y() + range));
        animation->setKeyValueAt(0.4,QPoint(sp.x(), sp.y() - range));
        animation->setKeyValueAt(0.5, sp);
        animation->setLoopCount(shakeCount);
        animation->setEndValue(sp);
        animation->setDuration(duration);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    });
}
void shakeItem(QGraphicsItem *item) {
    QTimer *timer = new QTimer();
    int idx = 0;
    int sk = 5; // 震动的距离
    QPointF loc = item->pos();
    QVector<QPointF> offsets = {
        QPointF(0, 0),QPointF(-sk, 0),
        QPointF(sk, 0),QPointF(0, -sk),
        QPointF(0, sk)
    };
    QObject::connect(timer, &QTimer::timeout, [=]() mutable {
        if (idx <= 20) {
            item->setPos(loc + offsets[idx % 5]);
        } else {
            timer->stop();
            timer->deleteLater();
        }
        idx++;
    });

    timer->start(10);
}



class MineSweeperItem : public QGraphicsRectItem {
public:
    MineSweeperItem(int col, int row, qreal size, int number)
        : QGraphicsRectItem(0, 0, size, size), number(number) {
        setPos(col * len, row * len);
        setBrush(Qt::lightGray);
        setPen(QColor(0, 0, 0));
        setAcceptHoverEvents(true);
    }

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override {
        Q_UNUSED(option);
        Q_UNUSED(widget);


        if(number==-1){//雷标记
            QPainterPath path;
            path.addRoundedRect(rect(), 5, 5);// 创建路径，绘制具有圆角的矩形
            painter->setPen(QPen(QColor(255, 255, 255), 0));// 设置边框颜色和宽度
            painter->setBrush(QColor(255, 225, 225));// 设置填充颜色
            painter->drawPath(path);// 绘制路径

            QFont font;
            font.setPixelSize(18);
            font.setFamily("Microsoft YaHei UI");

            painter->setFont(font);
            QRectF textRect = QRectF(rect().center().x() - 9, rect().center().y() - 9, 20, 20);
            painter->drawText(textRect, Qt::AlignCenter, "🚩");
        }else if(number==-2){//死亡格子
            QPainterPath path;
            path.addRoundedRect(rect(), 5, 5);
            painter->setPen(QPen(QColor(255, 255, 255), 1));
            painter->setBrush(QColor(180, 180, 180));
            painter->drawPath(path);
            QFont font;
            font.setPixelSize(20);
            font.setFamily("Microsoft YaHei UI");
            painter->setFont(font);
            QRectF textRect = QRectF(rect().center().x() - len/2, rect().center().y() - len/2, len, len);
            painter->drawText(textRect, Qt::AlignCenter, "💣");
        }else if (number >10) {// 绘制图片
            QPainterPath path;
            path.addRoundedRect(rect(), 5, 5);
            painter->setPen(QPen(QColor(255, 255, 255), 1));
            painter->setBrush(QColor(245, 245, 245));
            painter->drawPath(path);
            QPixmap image(":/icon ("+QString::number(number-10)+ ").png");
            painter->drawPixmap(rect().topLeft()+QPointF(8,8), image.scaledToWidth(rect().width()-16));
        }else{//数字格子
            QPainterPath path;
            path.addRoundedRect(rect(), 5, 5);
            painter->setPen(QPen(QColor(255, 255, 255), 1));
            if(!number) painter->setBrush(QColor(245, 245, 245));
            else painter->setBrush(QColor(235, 235, 235));

            painter->drawPath(path);

            if(!number) return;
            QFont font;
            font.setPixelSize(22);
            font.setFamily("Microsoft YaHei UI");
            font.setBold(true);

            painter->setFont(font);
            painter->setPen(color[number-1]);// 设置文字颜色
            QRectF textRect = QRectF(rect().center().x() - 10, rect().center().y() - 9, 20, 20);
            painter->drawText(textRect, Qt::AlignCenter, QString::number(number));
        }
    }

private:
    int number;  // 保存数字
};

struct Block{
    Block(int _x,int _y,int _mine_num){
        x=_x,y=_y;
        mine_num=std::min(_mine_num,block_size*block_size/3);
    }

    int x,y,mine_num;//区块位置,区块雷数
    int mines[block_size][block_size];
    QVector<QGraphicsItem*> items;

    void generate_block(){
        QVector<QPoint> v;
        for(int i=0;i<block_size;i++){
            for(int j=0;j<block_size;j++){
                mines[i][j]=0;
            }
        }
        while(v.size()<mine_num){
            QPoint tp={ran(0,block_size-1),ran(0,block_size-1)};
            if(v.contains(tp)) continue;
            v.append(tp);
        }
        for(auto i:v){
            mines[i.x()][i.y()]=1;
        }
    }
    int get_mine(int realx,int realy){
        int xx=(realx%block_size+block_size)%block_size;
        int yy=(realy%block_size+block_size)%block_size;

        return mines[xx][yy];
    }
};

int score=0,nowscore=0;
QHash<QPoint,int> block_id;//区块是否生成
QHash<QPoint,int> item_id;//单元格是否生成
QHash<QPoint,int> sign_id;//标记是否生成
QHash<QPoint,int> dead_id;//死亡格子
QVector<Block> blocks;
QVector<QGraphicsItem*> items;
QVector<QGraphicsItem*> signs;
QVector<QGraphicsItem*> deads;

int get_mine(int x0,int y0){//获取(x,y)雷的情况
    QPoint tp=getBlock_byItem({x0,y0});
    int xx=tp.x(),yy=tp.y();
    if(block_id.contains({xx,yy}))//若已经生成该区块
        return blocks[block_id[{xx,yy}]].get_mine(x0,y0);
    //否则创建区块
    int mine_num=45+sqrt(xx*xx+yy*yy)/2;
    if(ran(0,100)<8) mine_num-=15;//随机雷数增加/减少
    if(ran(0,100)<8) mine_num+=15;
    Block temp(xx,yy,mine_num);
    temp.generate_block();
    block_id[{xx,yy}]=blocks.size();
    blocks.append(temp);
    return temp.get_mine(x0,y0);
}
int get_num(int x0,int y0){//获取(x,y)周围的数字和
    int res=0;
    for(int i=-1;i<=1;i++){
        for(int j=-1;j<=1;j++){
            if (!i && !j) continue;
            int xx=x0+i,yy=y0+j;
            res+=get_mine(xx,yy);
        }
    }
    return res;
}
bool is_signed(int x,int y){//判断(x,y)是否被标记为雷
    if(!sign_id.contains({x,y})) return 0;
    if(signs[sign_id[{x,y}]]->isVisible()) return 1;
    return 0;
}
QImage image(1140/2, 870/2, QImage::Format_ARGB32);
bool first_pic=1;
MineSweeperItem * generate_item(int x,int y,int number){
    if(number) image.setPixel(1140/4+x,870/4+y,qRgb(120,120,120));
    else image.setPixel(1140/4+x,870/4+y,qRgb(150,150,150));
    score+=number;
    MineSweeperItem *temp=new MineSweeperItem(x,y,len,number);
    item_id[{x,y}]=items.size();
    items.append(temp);
    temp->setVisible(false);
    return temp;
}
MineSweeperItem * generate_dead(int x,int y){
    image.setPixel(1140/4+x,870/4+y,qRgb(0,0,0));
    MineSweeperItem *temp=new MineSweeperItem(x,y,len,-2);
    dead_id[{x,y}]=deads.size();
    deads.append(temp);
    temp->setVisible(false);
    return temp;
}
MineSweeperItem * generate_sign(int x,int y){
    MineSweeperItem *temp=new MineSweeperItem(x,y,len,-1);
    sign_id[{x,y}]=signs.size();
    signs.append(temp);
    temp->setVisible(true);
    return temp;
}

int pic_num=37;

MineSweeperItem * generate_pic(int x,int y){
    score+=50;
    for(int i=0;i<=2;i++){
        for(int j=0;j<=2;j++){
            image.setPixel(1140/4+x+i,870/4+y+j,qRgb(91,230,21));
        }
    }
    MineSweeperItem *temp=new MineSweeperItem(x,y,3*len,10+ran(1,pic_num));
    temp->setVisible(true);
    return temp;
}


void menu::bfs(int x,int y){
    //生成小建筑
    bool wg=(ran(0,1)==1);
    if(first_pic) wg=1;
    bool ig=0;

    QQueue<QPoint> q;
    QVector<QPoint> v;
    q.push_back({x,y});
    while(q.size()){
        QPoint t=q.front();
        q.pop_front();

        for(int i=-1;i<=1;i++){
            for(int j=-1;j<=1;j++){
                if(!i && !j) continue;
                int x0=t.x()+i,y0=t.y()+j;
                if(!item_id.contains({x0,y0}) && !get_mine(x0,y0) && !is_signed(x0,y0)){
                    int mine_num=get_num(x0,y0);
                    if(mine_num==0)q.push_back({x0,y0});
                    ui->graphicsView->scene()->addItem(generate_item(x0,y0,mine_num));
                    v.push_back({x0,y0});
                }
            }
        }
    }
    int idx=0;
    QTimer *timer=new QTimer();
    connect(timer,&QTimer::timeout,this,[this,timer,idx,v,wg,ig]() mutable{
        if(idx<v.size()){
            int id=item_id[v[idx]];
            fadeItem(items[id],100,0,1);
            if(wg && !ig && !get_num(v[idx].x(),v[idx].y())){
                int surround_num=0;
                for(int i=0;i<=2;i++){
                    for(int j=0;j<=2;j++){
                        int x0=v[idx].x()+i,y0=v[idx].y()+j;
                        surround_num+=get_num(x0,y0);
                    }
                }
                if(!surround_num){
                    first_pic=0;
                    ig=1;
                    ui->graphicsView->scene()->addItem(generate_pic(v[idx].x(),v[idx].y()));
                }

            }
        }else{
            timer->stop();
            timer->deleteLater();
        }
        idx++;
    });
    timer->start(5);
}
void menu::bfs2(int x,int y){
    QQueue<QPoint> q;
    QVector<QPoint> v_dead,v_alive;
    ui->graphicsView->scene()->addItem(generate_dead(x,y));
    v_dead.push_back({x,y});
    q.push_back({x,y});
    score-=20;
    while(q.size()){
        QPoint t=q.front();
        q.pop_front();
        for(int i=-1;i<=1;i++){
            for(int j=-1;j<=1;j++){
                if(!i && !j) continue;
                int x0=t.x()+i,y0=t.y()+j;
                if(get_mine(x0,y0) && !dead_id.contains({x0,y0})){//该格为雷且为被重复计算
                    ui->graphicsView->scene()->addItem(generate_dead(x0,y0));
                    if(sign_id.contains({x0,y0})){
                        signs[sign_id[{x0,y0}]]->setVisible(false);
                    }
                    score-=20;
                    v_dead.append({x0,y0});
                    q.push_back({x0,y0});
                }else if(!get_mine(x0,y0) && !item_id.contains({x0,y0})){//该格非雷且为被重复计算
                    if(!item_id.contains({x0,y0})){//未开格子
                        ui->graphicsView->scene()->addItem(generate_item(x0,y0,get_num(x0,y0)));
                        if(sign_id.contains({x0,y0})){
                            signs[sign_id[{x0,y0}]]->setVisible(false);
                        }
                        score-=5*get_num(x0,y0);
                        v_alive.append({x0,y0});
                    }
                }
            }
        }
    }

    shakeWidget(100,8);
    int idx=0;
    QTimer *timer=new QTimer();
    connect(timer,&QTimer::timeout,this,[timer,idx,v_dead,v_alive]() mutable{
        if(idx<v_dead.size()){
            int id=dead_id[v_dead[idx]];
            fadeItem(deads[id],100,0,1);
            shakeItem(deads[id]);
        }else if(idx<v_alive.size()+v_dead.size()){
            timer->setInterval(20);
            int id=item_id[v_alive[idx-v_dead.size()]];
            fadeItem(items[id],100,0,1);

        }else{
            timer->stop();
            timer->deleteLater();
        }
        idx++;
    });
    timer->start(50);
}
void menu::open(int x,int y){//左键单击
    if(item_id.contains({x,y}) || is_signed(x,y) || dead_id.contains({x,y}))//未被点开
        return;
    if(!get_mine(x,y)){//该位置不是雷
        int mine_num=get_num(x,y);
        ui->graphicsView->scene()->addItem(generate_item(x,y,mine_num));
        fadeItem(items[item_id[{x,y}]],100,0,1);
        if(mine_num==0){
            bfs(x,y);
        }
    }else{
        bfs2(x,y);
    }
}
void menu::sign(int x,int y){//右键标记
    if(item_id.contains({x,y}) || dead_id.contains({x,y}))//未被点开
        return;
    bool state=is_signed(x,y);
    if(sign_id.contains({x,y})){//已经生成过
        signs[sign_id[{x,y}]]->setVisible(!state);
    }else{
        ui->graphicsView->scene()->addItem(generate_sign(x,y));
    }
}

void menu::dbopen(int x,int y){//左右同时按下
    //若周围被标记的格子总数=当前数字
    if(item_id.contains({x,y})){//已经被打开
        int num=get_num(x,y),sign_num=0;
        for(int i=-1;i<=1;i++){
            for(int j=-1;j<=1;j++){
                if(!i && !j) continue;
                if(is_signed(x+i,y+j) || dead_id.contains({x+i,y+j}))
                    sign_num++;
            }
        }
        if(num==sign_num){
            for(int i=-1;i<=1;i++){
                for(int j=-1;j<=1;j++){
                    if(!i && !j) continue;
                    if(!is_signed(x+i,y+j) && !dead_id.contains({x+i,y+j}))
                        open(x+i,y+j);
                }
            }
        }
    }
}
QTimer * moveTimer[4];
menu::menu(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::menu)
{
    ui->setupUi(this);


    QRectF rect(-100000,-100000,200000,200000);
    QGraphicsScene* scene=new QGraphicsScene(rect,this);
    ui->graphicsView->setScene(scene);

    ui->graphicsView->viewport()->installEventFilter(this);
    this->grabKeyboard();
    this->installEventFilter(this);

    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QMessageBox::information(this,"游戏提示","<扫雷无限模式>\n\n"
                                               "-左键打开格子\n"
                                               "-右键标记地雷\n"
                                               "-左右键打开格子周围地雷\n"
                                               "-按住Ctrl可以进行拖拽和缩放地图\n"
                                               "-按下Tab显示小地图");

    image.fill(qRgba(255, 255, 255, 160));
    QGraphicsOpacityEffect* opa=new QGraphicsOpacityEffect();
    opa->setOpacity(0.7);
    ui->map->setGraphicsEffect(opa);
    ui->map->setVisible(false);
    QTimer* timer=new QTimer();
    connect(timer,&QTimer::timeout,this,[=](){
        if(nowscore!=score){
            int dx=(score-nowscore)/abs(score-nowscore);
            nowscore+=dx;
            ui->scoreLabel->setText("Score: "+ QString::number(nowscore));
        }
    });
    timer->start(20);

    QTimer* update=new QTimer();
    connect(update,&QTimer::timeout,this,[=](){
        ui->map->setPixmap(QPixmap::fromImage(image.scaled(1170,870)));
    });
    update->start(1000);


    double step=5;
    QPointF ts[]={{0,-step},{0,step},{-step,0},{step,0}};

    for(int i=0;i<4;i++){
        moveTimer[i]=new QTimer();
        connect(moveTimer[i],&QTimer::timeout,this,[=](){
            QPointF pos_g = ui->graphicsView->mapToScene(ui->graphicsView->viewport()->rect().center());
            ui->graphicsView->centerOn(pos_g+ts[i]);
        });
    }

    for(int i=1;i<=15;i++){
        for(int j=1;j<=15;j++){
            if(get_num(i,j)==0 && !get_mine(i,j)){
                open(i,j);
                ui->graphicsView->centerOn(i*len,j*len);
                return;
            }
        }
    }
}

bool menu::eventFilter(QObject *obj, QEvent *event) {
    if(obj==this){//ctrl操作
        if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Control) {
                // 更新 Ctrl 键的状态
                isCtrl = (event->type() == QEvent::KeyPress);
                if(isCtrl) ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
                else ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
            }
        }
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            int inter=10;
            if (keyEvent->key() == Qt::Key_W) {
                if(!moveTimer[0]->isActive()) moveTimer[0]->start(inter);
            } else if (keyEvent->key() == Qt::Key_S) {
                if(!moveTimer[1]->isActive())moveTimer[1]->start(inter);
            } else if (keyEvent->key() == Qt::Key_A) {
                if(!moveTimer[2]->isActive())moveTimer[2]->start(inter);
            } else if (keyEvent->key() == Qt::Key_D) {
                if(!moveTimer[3]->isActive())moveTimer[3]->start(inter);
            } else if(keyEvent->key() == Qt::Key_Tab){
                ui->map->setVisible(true);
            }
        }
        if (event->type() == QEvent::KeyRelease) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_W) {
                moveTimer[0]->stop();
            } else if (keyEvent->key() == Qt::Key_S) {
                moveTimer[1]->stop();
            } else if (keyEvent->key() == Qt::Key_A) {
                moveTimer[2]->stop();
            } else if (keyEvent->key() == Qt::Key_D) {
                moveTimer[3]->stop();
            } else if(keyEvent->key() == Qt::Key_Tab){
                ui->map->setVisible(false);
            }
        }
    }

    if (obj == ui->graphicsView->viewport()) {//点击操作
        if (event->type() == QEvent::MouseButtonPress && !isCtrl) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            QPointF scenePos = ui->graphicsView->mapToScene(mouseEvent->pos());
            QPoint t=getLoc(scenePos);
            if (mouseEvent->buttons() == (Qt::LeftButton | Qt::RightButton)){
                dbopen(t.x(),t.y());
            }else if (mouseEvent->button() == Qt::LeftButton) {
                open(t.x(),t.y());
            }else if (mouseEvent->button() == Qt::RightButton) {
                sign(t.x(),t.y());
            }
        }
        if (event->type() == QEvent::MouseButtonDblClick && !isCtrl){
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            QPointF scenePos = ui->graphicsView->mapToScene(mouseEvent->pos());
            QPoint t=getLoc(scenePos);
            if (mouseEvent->button() == Qt::LeftButton){
                dbopen(t.x(),t.y());
            }
        }

        if (event->type() == QEvent::Wheel) {//滚轮操作
            if(!isCtrl) return true;
            QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
            qreal scaleFactor = 1.2;
            if (wheelEvent->angleDelta().y() > 0) {
                ui->graphicsView->scale(scaleFactor, scaleFactor);
            } else {
                ui->graphicsView->scale(1.0 / scaleFactor, 1.0 / scaleFactor);
            }

            qreal minScale = 0.6;
            qreal maxScale = 1.5;
            qreal currentScale = ui->graphicsView->transform().m11();
            if (currentScale < minScale) {
                ui->graphicsView->scale(minScale / currentScale, minScale / currentScale);
            } else if (currentScale > maxScale) {
                ui->graphicsView->scale(maxScale / currentScale, maxScale / currentScale);
            }

            return true;
        }
    }

    return QObject::eventFilter(obj, event);
}

menu::~menu()
{
    delete ui;
}



