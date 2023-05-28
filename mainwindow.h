#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QContextMenuEvent>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTextBrowser>
#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QList>

#define INCREMENT 1.5       //温度升高/降低1度 y轴移动的像素点增量
#define PAINT_RADIUS 3      //曲线描点的大小
#define TEXT_OFFSET_X 12    //温度文本X偏移
#define TEXT_OFFSET_Y 12    //温度文本Y偏移

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void CreateLayout();
    void setLayout();

    void Request(QString cityCode);
    void parseJson(QByteArray &ByteArray);
    //空气质量label颜色
    void transColorwithAqi(QLabel *lbl, int aqi);
    //天气状况图标切换
    void transTypeIcon(QLabel *lbl, QString TypeName);

    //读取json文件
    void LoadJsonFile();
    //获取城市编码
    QString GetCityCode(QString cityName);

    //绘制高温曲线图
    void painterHigh();
    //绘制低温曲线图
    void painterLow();

protected:
    //重写事件过滤器函数
    virtual bool eventFilter(QObject *watched, QEvent *event);

    virtual void contextMenuEvent(QContextMenuEvent *event);

    virtual void mousePressEvent(QMouseEvent *event);

    virtual void mouseMoveEvent(QMouseEvent *event);

private slots:
    void slotReplyFinished(QNetworkReply *reply);

    void on_btnSearch_clicked();

private:
    Ui::MainWindow *ui;

    QMenu *m_pMenu;
    QAction *m_pAction;

    QPoint m_Offset;  //鼠标偏移量

private:
    QNetworkAccessManager *m_pNetworkAccessManager;
    QNetworkRequest *m_pNetworkRequest;
    QVariantMap m_map;

    QMap<QString, QString> m_CityCodeMap;

    //高低温
    int m_high0 = 0;
    int m_high1 = 0;
    int m_high2 = 0;
    int m_high3 = 0;
    int m_high4 = 0;
    int m_high5 = 0;
    int m_low0 = 0;
    int m_low1 = 0;
    int m_low2 = 0;
    int m_low3 = 0;
    int m_low4 = 0;
    int m_low5 = 0;
};
#endif // MAINWINDOW_H
