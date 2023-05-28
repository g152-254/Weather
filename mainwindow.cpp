#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPixmap>
#include <QNetworkAccessManager>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <iterator>
#include <QJsonParseError>
#include <QTimer>
#include <QPainter>

QString sPath;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    CreateLayout();
    setLayout();
    LoadJsonFile();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_pNetworkRequest;
    delete m_pNetworkAccessManager;
}

void MainWindow::CreateLayout()
{
    m_pMenu = new QMenu(this);
    m_pAction = new QAction;
    m_pNetworkAccessManager = new QNetworkAccessManager(this);
    m_pNetworkRequest = new QNetworkRequest;
    //安装事件过滤器
    ui->lblHighCurve->installEventFilter(this);
    ui->lblLowCurve->installEventFilter(this);

    connect(m_pAction, &QAction::triggered, this, [=] { qApp->exit(0); });

    connect(m_pNetworkAccessManager, &QNetworkAccessManager::finished, this, &MainWindow::slotReplyFinished);
    //北京  101010100
    Request(R"(北京)");
}

void MainWindow::setLayout()
{
    setWindowFlag(Qt::FramelessWindowHint);
    this->resize(QSize(800,450));
    //MainWidget->setFixedSize(this->width(),this->height());
    m_pAction->setText("退出");
    m_pAction->setIcon(QIcon(":/res/close.png"));
    m_pMenu->addAction(m_pAction);
    //this->setStyleSheet("QMainWindow {background-image:url(:/res/background.png)}");

}

void MainWindow::Request(QString cityName)
{
    QString cityCode = GetCityCode(cityName);
    QUrl url(R"(http://t.weather.itboy.net/api/weather/city/)" + cityCode);
    //m_pNetworkRequest->setUrl(url);
    m_pNetworkAccessManager->get(QNetworkRequest(url));
}

void MainWindow::parseJson(QByteArray &ByteArray)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(ByteArray, &err);
    QJsonObject rootObject  = doc.object();
    QJsonObject todayObject = rootObject.value("data").toObject();
    //qDebug() << rootObject.value("message").toString();

    /************************************************更新昨天的数据*******************************************/
    QJsonObject yesterdayObject = todayObject.value("yesterday").toObject();
    //高温低温
    QString high0 = yesterdayObject.value("high").toString().split(" ").at(1);
    QString low0 = yesterdayObject.value("low").toString().split(" ").at(1);
    QString s_high0 = high0;
    QString s_low0 = low0;
    m_high0 = s_high0.remove("℃").toInt();
    m_low0 = s_low0.remove("℃").toInt();
    //日期
    QString str = yesterdayObject.value("ymd").toString();
    QString sYesterdayDt = QDateTime::fromString(str,"yyyy-MM-dd").toString("MM/dd");
    ui->lblDate0->setText(sYesterdayDt);
    //空气质量
    int nquality0 = yesterdayObject.value("aqi").toInt();
    //qDebug() << "昨天的空气质量:" << nquality0;
    transColorwithAqi(ui->lblQuality0,nquality0);
    //日期
    QString week0 = yesterdayObject.value("week").toString();
    ui->lblWeek0->setText("昨天");
    //风力风向
    QString fx0 = yesterdayObject.value("fx").toString();
    QString fl0 = yesterdayObject.value("fl").toString();
    ui->lblFx0->setText(fx0);
    ui->lblFl0->setText(fl0);
    //天气状况
    QString type0 = yesterdayObject.value("type").toString();
    ui->lblType0->setText(type0);
    transTypeIcon(ui->lblTypeIcon0, type0);
    /************************************************更新当天的数据*******************************************/

    QJsonObject cityinfoObject = rootObject.value("cityInfo").toObject();
    //湿度
    QString shidu = todayObject.value("shidu").toString();
    ui->lblShiDu->setText(shidu);
    //pm2.5
    int  pm25 = todayObject.value("pm25").toInt();
    ui->lblPM25->setText(QString::number(pm25));
    //空气质量
    QString quality = todayObject.value("quality").toString();
    ui->lblQuality->setText(quality);
    //温度
    QString  wendu = todayObject.value("wendu").toString();
    ui->lblTemp->setText(wendu);
    //城市
    QString city = cityinfoObject.value("city").toString();
    ui->lblCity->setText(city);

    QJsonArray forecastArray =  todayObject.value("forecast").toArray();
    //高温低温
    QJsonObject todayObj = forecastArray[0].toObject();   //今天的obj
    QString high = todayObj.value("high").toString().split(" ").at(1);
    QString low = todayObj.value("low").toString().split(" ").at(1);

    QString s_high1 = high;
    QString s_low1 = low;
    m_high1 = s_high1.remove("℃").toInt();
    m_low1 = s_low1.remove("℃").toInt();
    ui->lblLowHigh->setText(low + "~" + high);
    //天气状况
    QString type1 = todayObj.value("type").toString();
    ui->lblType->setText(type1);
    ui->lblType1->setText(type1);
    transTypeIcon(ui->lblTypeIcon, type1);
    transTypeIcon(ui->lblTypeIcon1, type1);
    //温馨提示
    QString tip = todayObj.value("notice").toString();
    ui->lblGanMao->setText(tip);
    //风向、风力
    QString fx = todayObj.value("fx").toString();
    QString fl = todayObj.value("fl").toString();
    ui->lblWindFx->setText(fx);
    ui->lblWindFl->setText(fl);
    ui->lblFx1->setText(fx);
    ui->lblFl1->setText(fl);
    //更新时间、星期几
    QString strDt = todayObj.value("ymd").toString();
    QString strWeek = todayObj.value("week").toString();
    strWeek.replace("星期","周");
    ui->lblDate->setText(strDt + "        " + strWeek + "  ");
    QString strdt1 = strDt.split("-").at(1);
    QString strdt2 = strDt.split("-").at(2);
    QString strdt = strdt1 + "/" + strdt2;
    ui->lblDate1->setText(strdt);
    ui->lblWeek1->setText("今天");
    //空气质量
    int nquality1 = todayObj.value("aqi").toInt();
    //qDebug() << "今天的空气质量:" << nquality1;
    transColorwithAqi(ui->lblQuality1,nquality1);



    /************************************************更新明天的数据*******************************************/
    QJsonObject mObj = forecastArray[1].toObject();   //明天的obj
    //高温低温
    QString high2 = mObj.value("high").toString().split(" ").at(1);
    QString low2 = mObj.value("low").toString().split(" ").at(1);
    QString s_high2 = high2;
    QString s_low2 = low2;
    m_high2 = s_high2.remove("℃").toInt();
    m_low2 = s_low2.remove("℃").toInt();
    //日期
    QString strm = mObj.value("ymd").toString();
    QString smDt = QDateTime::fromString(strm,"yyyy-MM-dd").toString("MM/dd");
    ui->lblDate2->setText(smDt);
    //空气质量
    int nquality2 = mObj.value("aqi").toInt();
    //qDebug() << "明天的空气质量:" << nquality2;
    transColorwithAqi(ui->lblQuality2,nquality2);
    //日期
    QString week2 = mObj.value("week").toString();
    ui->lblWeek2->setText("明天");
    //风力风向
    QString fx2 = mObj.value("fx").toString();
    QString fl2 = mObj.value("fl").toString();
    ui->lblFx2->setText(fx2);
    ui->lblFl2->setText(fl2);
    //天气状况
    QString type2 = mObj.value("type").toString();
    ui->lblType2->setText(type2);
    transTypeIcon(ui->lblTypeIcon2, type2);

    /************************************************更新后天的数据*******************************************/
    QJsonObject mmObj = forecastArray[2].toObject();   //后天的obj
    //高温低温
    QString high3 = mmObj.value("high").toString().split(" ").at(1);
    QString low3 = mmObj.value("low").toString().split(" ").at(1);
    QString s_high3 = high3;
    QString s_low3 = low3;
    m_high3 = s_high3.remove("℃").toInt();
    m_low3 = s_low3.remove("℃").toInt();
    //日期
    QString strm3 = mmObj.value("ymd").toString();
    QString smDt3 = QDateTime::fromString(strm3,"yyyy-MM-dd").toString("MM/dd");
    ui->lblDate3->setText(smDt3);
    //空气质量
    int nquality3 = mmObj.value("aqi").toInt();
    //qDebug() << "后天的空气质量:" << nquality3;
    transColorwithAqi(ui->lblQuality3,nquality3);
    //日期
    QString week3 = mmObj.value("week").toString();
    ui->lblWeek3->setText(week3);
    //风力风向
    QString fx3 = mmObj.value("fx").toString();
    QString fl3 = mmObj.value("fl").toString();
    ui->lblFx3->setText(fx3);
    ui->lblFl3->setText(fl3);
    //天气状况
    QString type3 = mmObj.value("type").toString();
    ui->lblType3->setText(type3);
    transTypeIcon(ui->lblTypeIcon3, type3);

    /************************************************更新大后天的数据*******************************************/
    QJsonObject mmmObj = forecastArray[3].toObject();   //大后天的obj
    //高温低温
    QString high4 = mmmObj.value("high").toString().split(" ").at(1);
    QString low4 = mmmObj.value("low").toString().split(" ").at(1);
    QString s_high4 = high4;
    QString s_low4 = low4;
    m_high4 = s_high4.remove("℃").toInt();
    m_low4 = s_low4.remove("℃").toInt();
    //日期
    QString strm4 = mmmObj.value("ymd").toString();
    QString smDt4 = QDateTime::fromString(strm4,"yyyy-MM-dd").toString("MM/dd");
    ui->lblDate4->setText(smDt4);
    //空气质量
    int nquality4 = mmmObj.value("aqi").toInt();
    //qDebug() << "后天的空气质量:" << nquality4;
    transColorwithAqi(ui->lblQuality4,nquality4);
    //日期
    QString week4 = mmmObj.value("week").toString();
    ui->lblWeek4->setText(week4);
    //风力风向
    QString fx4 = mmmObj.value("fx").toString();
    QString fl4 = mmmObj.value("fl").toString();
    ui->lblFx4->setText(fx4);
    ui->lblFl4->setText(fl4);
    //天气状况
    QString type4 = mmmObj.value("type").toString();
    ui->lblType4->setText(type4);
    transTypeIcon(ui->lblTypeIcon4, type4);

    /************************************************更新大大后天的数据*******************************************/
    QJsonObject mmmmObj = forecastArray[4].toObject();   //大大后天的obj
    //高温低温
    QString high5 = mmmmObj.value("high").toString().split(" ").at(1);
    QString low5 = mmmmObj.value("low").toString().split(" ").at(1);
    QString s_high5 = high5;
    QString s_low5 = low5;
    m_high5 = s_high5.remove("℃").toInt();
    m_low5 = s_low5.remove("℃").toInt();
    //日期
    QString strm5 = mmmmObj.value("ymd").toString();
    QString smDt5 = QDateTime::fromString(strm5,"yyyy-MM-dd").toString("MM/dd");
    ui->lblDate5->setText(smDt5);
    //空气质量
    int nquality5 = mmmmObj.value("aqi").toInt();
   // qDebug() << "后天的空气质量:" << nquality5;
    transColorwithAqi(ui->lblQuality5,nquality5);
    //日期
    QString week5 = mmmmObj.value("week").toString();
    ui->lblWeek5->setText(week5);
    //风力风向
    QString fx5 = mmmmObj.value("fx").toString();
    QString fl5 = mmmmObj.value("fl").toString();
    ui->lblFx5->setText(fx5);
    ui->lblFl5->setText(fl5);
    //天气状况
    QString type5 = mmmmObj.value("type").toString();
    ui->lblType5->setText(type5);
    transTypeIcon(ui->lblTypeIcon5, type5);

    ui->lblHighCurve->update();
    ui->lblLowCurve->update();

}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    m_pMenu->exec(QCursor::pos());

    event->accept();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    event->accept();
    m_Offset = event->globalPos() - this->pos();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() - m_Offset);
}

void MainWindow::slotReplyFinished(QNetworkReply *reply)
{
    int nStatu_Code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(reply->error() != QNetworkReply::NoError || nStatu_Code != 200)
    {
        qDebug() << "请求异常";
        qDebug() << "nStatu_Code：" << nStatu_Code;
    }
    else
    {
        QByteArray ba = reply->readAll();
        //qDebug() << ba.data();
        parseJson(ba);
    }
    reply->deleteLater();
}

void MainWindow::transColorwithAqi(QLabel *lbl, int aqi)
{
    if (aqi < 0) {
        lbl->setText("未知");
        lbl->setStyleSheet("background-color:rgb(0,255,0); color:rgb(255,255,255);");
    } else if (aqi >= 0 && aqi <= 50) {
        lbl->setText("优");
        lbl->setStyleSheet("background-color:rgb(0,255,0); color:rgb(255,255,255);");
    } else if (aqi > 50 && aqi <= 100) {
        lbl->setText("良");
        lbl->setStyleSheet("background-color:rgb(255,215,0); color:rgb(255,255,255)");
    } else if (aqi > 100 && aqi <= 150) {
        lbl->setText("轻度污染");
        lbl->setStyleSheet("background-color:rgb(255,128,0); color:rgb(255,255,255)");
    } else if (aqi > 150 && aqi <= 200) {
        lbl->setText("中度污染");
        lbl->setStyleSheet("background-color:rgb(255,0,0); color:rgb(255,255,255)");
    } else if (aqi > 200 && aqi <= 300) {
        lbl->setText("重度污染");
        lbl->setStyleSheet("background-color:rgb(138,43,226); color:rgb(255,255,255)");
    } else if (aqi > 300) {
        lbl->setText("严重污染");
        lbl->setStyleSheet("background-color:rgb(116,0,0); color:rgb(255,255,255)");
    }
}

void MainWindow::transTypeIcon(QLabel *lbl, QString TypeName)
{
    if(TypeName == "晴" || TypeName == "晴转多云"){
        lbl->setPixmap(QPixmap(":/res/type/Qing.png").scaled(lbl->size()));
    }
    else if(TypeName == "暴雪"){
        lbl->setPixmap(QPixmap(":/res/type/BaoXue.png").scaled(lbl->size()));
    }
    else if(TypeName == "暴雨"){
        lbl->setPixmap(QPixmap(":/res/type/BaoYu.png").scaled(lbl->size()));
    }
    else if(TypeName == "暴雨到大暴雨"){
        lbl->setPixmap(QPixmap(":/res/type/BaoYuDaoDaBaoYu.png").scaled(lbl->size()));
    }
    else if(TypeName == "大暴雨"){
        lbl->setPixmap(QPixmap(":/res/type/DaBaoYu.png").scaled(lbl->size()));
    }
    else if(TypeName == "大暴雨到特大暴雨"){
        lbl->setPixmap(QPixmap(":/res/type/DaBaoYuDaoTeDaBaoYu.png").scaled(lbl->size()));
    }
    else if(TypeName == "大到暴雪"){
        lbl->setPixmap(QPixmap(":/res/type/DaDaoBaoYu.png").scaled(lbl->size()));
    }
    else if(TypeName == "大到暴雨"){
        lbl->setPixmap(QPixmap(":/res/type/BaoXue.png").scaled(lbl->size()));
    }
    else if(TypeName == "大雪"){
        lbl->setPixmap(QPixmap(":/res/type/DaXue.png").scaled(lbl->size()));
    }
    else if(TypeName == "大雨"){
        lbl->setPixmap(QPixmap(":/res/type/DaYu.png").scaled(lbl->size()));
    }
    else if(TypeName == "冻雨"){
        lbl->setPixmap(QPixmap(":/res/type/DongYu.png").scaled(lbl->size()));
    }
    else if(TypeName == "多云"){
        lbl->setPixmap(QPixmap(":/res/type/DuoYun.png").scaled(lbl->size()));
    }
    else if(TypeName == "浮尘"){
        lbl->setPixmap(QPixmap(":/res/type/FuChen.png").scaled(lbl->size()));
    }
    else if(TypeName == "雷阵雨"){
        lbl->setPixmap(QPixmap(":/res/type/LeiZhenYu.png").scaled(lbl->size()));
    }
    else if(TypeName == "雷阵雨伴有冰雹"){
        lbl->setPixmap(QPixmap(":/res/type/LeiZhenYuBanYouBingBao.png").scaled(lbl->size()));
    }
    else if(TypeName == "霾"){
        lbl->setPixmap(QPixmap(":/res/type/Mai.png").scaled(lbl->size()));
    }
    else if(TypeName == "强沙尘暴"){
        lbl->setPixmap(QPixmap(":/res/type/QiangShaChenBao.png").scaled(lbl->size()));
    }
    else if(TypeName == "沙尘暴"){
        lbl->setPixmap(QPixmap(":/res/type/ShaChenBao.png").scaled(lbl->size()));
    }
    else if(TypeName == "特大暴雨"){
        lbl->setPixmap(QPixmap(":/res/type/TeDaBaoYu.png").scaled(lbl->size()));
    }
    else if(TypeName == "雾"){
        lbl->setPixmap(QPixmap(":/res/type/Wu.png").scaled(lbl->size()));
    }
    else if(TypeName == "小到中雪"){
        lbl->setPixmap(QPixmap(":/res/type/XiaoDaoZhongXue.png").scaled(lbl->size()));
    }
    else if(TypeName == "小到中雨"){
        lbl->setPixmap(QPixmap(":/res/type/XiaoDaoZhongYu.png").scaled(lbl->size()));
    }
    else if(TypeName == "雾"){
        lbl->setPixmap(QPixmap(":/res/type/Wu.png").scaled(lbl->size()));
    }
    else if(TypeName == "小雪"){
        lbl->setPixmap(QPixmap(":/res/type/XiaoXue.png").scaled(lbl->size()));
    }
    else if(TypeName == "小雨"){
        lbl->setPixmap(QPixmap(":/res/type/XiaoYu.png").scaled(lbl->size()));
    }
    else if(TypeName == "雪"){
        lbl->setPixmap(QPixmap(":/res/type/Xue.png").scaled(lbl->size()));
    }
    else if(TypeName == "扬沙"){
        lbl->setPixmap(QPixmap(":/res/type/YangSha.png").scaled(lbl->size()));
    }
    else if(TypeName == "阴"){
        lbl->setPixmap(QPixmap(":/res/type/Yin.png").scaled(lbl->size()));
    }
    else if(TypeName == "雨"){
        lbl->setPixmap(QPixmap(":/res/type/Yu.png").scaled(lbl->size()));
    }
    else if(TypeName == "雨夹雪"){
        lbl->setPixmap(QPixmap(":/res/type/YuJiaXue.png").scaled(lbl->size()));
    }
    else if(TypeName == "阵雨"){
        lbl->setPixmap(QPixmap(":/res/type/ZhenYu.png").scaled(lbl->size()));
    }
    else if(TypeName == "阵雪"){
        lbl->setPixmap(QPixmap(":/res/type/ZhenXue.png").scaled(lbl->size()));
    }
    else if(TypeName == "中到大雪"){
        lbl->setPixmap(QPixmap(":/res/type/ZhongDaoDaXue.png").scaled(lbl->size()));
    }
    else if(TypeName == "中到大雨"){
        lbl->setPixmap(QPixmap(":/res/type/ZhongDaoDaYu.png").scaled(lbl->size()));
    }
    else if(TypeName == "中雪"){
        lbl->setPixmap(QPixmap(":/res/type/ZhongXue.png").scaled(lbl->size()));
    }
    else if(TypeName == "中雨"){
        lbl->setPixmap(QPixmap(":/res/type/ZhongYu.png").scaled(lbl->size()));
    }
    else{
        lbl->setPixmap(QPixmap(":/res/type/undefined.png").scaled(lbl->size()));
    }
}

void MainWindow::LoadJsonFile()
{
    QFile jsonFile(sPath + "/weather_city.json");
    if(!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "json文件打开失败";
        return ;
    }
    QByteArray baJson = jsonFile.readAll();
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(baJson,&err);
    if(err.error != QJsonParseError::NoError)
    {
        qDebug() << "json文件打开失败";
        return ;
    }
    QJsonArray CodeArray = doc.array();
    for(int i = 0; i < CodeArray.size(); ++i)
    {
        QJsonObject object = CodeArray[i].toObject();
        QString sName = object.value("city_name").toString();
        QString sCode = object.value("city_code").toString();
        if(!sCode.isEmpty())
        {
            m_CityCodeMap.insert(sName, sCode);
        }
    }
}

QString MainWindow::GetCityCode(QString cityName)
{
    if(cityName.contains("市"))
        cityName.remove("市");
    if(m_CityCodeMap.isEmpty())
    {
        LoadJsonFile();
    }
    QMap<QString, QString>::iterator it = m_CityCodeMap.find(cityName);
    if(it == m_CityCodeMap.end())
        it = m_CityCodeMap.find(cityName + "市");
    if(it == m_CityCodeMap.end())
        it = m_CityCodeMap.find(cityName + "县");
    if(it != m_CityCodeMap.end())
    {
        //qDebug() << it.value();
        return it.value();
    }
    return "";
}

void MainWindow::painterHigh()
{
    QPainter painter(ui->lblHighCurve);
    //抗锯齿
    painter.setRenderHint(QPainter::Antialiasing,true);
    QPen pen = painter.pen();
    pen.setWidth(1);
    pen.setColor(QColor(255,170,0));
    painter.setPen(pen);
    painter.setBrush(QColor(255,170,0));  //设置画刷（内部填充）
    painter.save();

    //计算x，y的坐标
    QPoint point[6] = {};
    //高温和
    int highTempSum = 0;
    //六天高温的平均值
    int Avg = 0;
    //中心点
    float center = ui->lblHighCurve->height() / 2;

    highTempSum  = m_high0 + m_high1 + m_high2 + m_high3 + m_high4 + m_high5;

    Avg = highTempSum / 6;
    int offset0 = 0;
    offset0 = (m_high0 - Avg) * INCREMENT;

    point[0].setX(ui->lblWeek0->pos().x() +ui->lblWeek0->width() / 2);
    point[0].setY(center - offset0);
    //绘制点
    painter.drawEllipse(point[0], PAINT_RADIUS, PAINT_RADIUS);
    //绘制文字
    painter.drawText(QPoint(point[0].x() - TEXT_OFFSET_X, point[0].y() - TEXT_OFFSET_Y), QString::number(m_high0) + "°");

    int offset1 = 0;
    offset1 = (m_high1 - Avg) * INCREMENT;
    point[1].setX(ui->lblWeek1->pos().x() +ui->lblWeek1->width() / 2);
    point[1].setY(center - offset1);
    //绘制点
    painter.drawEllipse(point[1], PAINT_RADIUS, PAINT_RADIUS);
    //绘制文字
    painter.drawText(QPoint(point[1].x() - TEXT_OFFSET_X, point[1].y() - TEXT_OFFSET_Y), QString::number(m_high1) + "°");

    int offset2 = 0;
    offset2 = (m_high2 - Avg) * INCREMENT;
    point[2].setX(ui->lblWeek2->pos().x() +ui->lblWeek2->width() / 2);
    point[2].setY(center - offset2);
    //绘制点
    painter.drawEllipse(point[2], PAINT_RADIUS, PAINT_RADIUS);
    //绘制文字
    painter.drawText(QPoint(point[2].x() - TEXT_OFFSET_X, point[2].y() - TEXT_OFFSET_Y), QString::number(m_high2) + "°");

    int offset3 = 0;
    offset3 = (m_high3 - Avg) * INCREMENT;
    point[3].setX(ui->lblWeek3->pos().x() +ui->lblWeek3->width() / 2);
    point[3].setY(center - offset3);
    //绘制点
    painter.drawEllipse(point[3], PAINT_RADIUS, PAINT_RADIUS);
    //绘制文字
    painter.drawText(QPoint(point[3].x() - TEXT_OFFSET_X, point[3].y() - TEXT_OFFSET_Y), QString::number(m_high3) + "°");

    int offset4 = 0;
    offset4 = (m_high4 - Avg) * INCREMENT;
    point[4].setX(ui->lblWeek4->pos().x() +ui->lblWeek4->width() / 2);
    point[4].setY(center - offset4);
    //绘制点
    painter.drawEllipse(point[4], PAINT_RADIUS, PAINT_RADIUS);
    //绘制文字
    painter.drawText(QPoint(point[4].x() - TEXT_OFFSET_X, point[4].y() - TEXT_OFFSET_Y), QString::number(m_high4) + "°");

    int offset5 = 0;
    offset5 = (m_high5 - Avg) * INCREMENT;
    point[5].setX(ui->lblWeek5->pos().x() +ui->lblWeek5->width() / 2);
    point[5].setY(center - offset5);
    //绘制点
    painter.drawEllipse(point[5], PAINT_RADIUS, PAINT_RADIUS);
    //绘制文字
    painter.drawText(QPoint(point[5].x() - TEXT_OFFSET_X, point[5].y() - TEXT_OFFSET_Y), QString::number(m_high5) + "°");

    //只需要5段
    for (int i = 0; i < 5; ++i) {
        //绘制曲线,昨天为虚线,其余的为实线
        if (i == 0) {
            pen.setStyle(Qt::DotLine);
            painter.setPen(pen);
        }else{
            pen.setStyle(Qt::SolidLine);
            painter.setPen(pen);
        }
        //绘制线
        painter.drawLine(point[i].x(), point[i].y(), point[i + 1].x(), point[i + 1].y());
    }
    //恢复
    painter.restore();
}

void MainWindow::painterLow()
{
    QPainter painter(ui->lblLowCurve);
    //抗锯齿
    painter.setRenderHint(QPainter::Antialiasing,true);
    QPen pen = painter.pen();
    pen.setWidth(1);
    pen.setColor(QColor(0,255,255));
    painter.setPen(pen);
    painter.setBrush(QColor(0,255,255));  //设置画刷（内部填充）
    painter.save();

    //计算x，y的坐标
    QPoint point[6] = {};
    //高温和
    int lowTempSum = 0;
    //六天高温的平均值
    int Avg = 0;
    //中心点
    float center = ui->lblLowCurve->height() / 2;

    lowTempSum  = m_low0 + m_low1 + m_low2 + m_low3 + m_low4 + m_low5;

    Avg = lowTempSum / 6;
    int offset0 = 0;
    offset0 = (m_low0 - Avg) * INCREMENT;

    point[0].setX(ui->lblWeek0->pos().x() +ui->lblWeek0->width() / 2);
    point[0].setY(center - offset0);
    //绘制点
    painter.drawEllipse(point[0], PAINT_RADIUS, PAINT_RADIUS);
    //绘制文字
    painter.drawText(QPoint(point[0].x() - TEXT_OFFSET_X, point[0].y() - TEXT_OFFSET_Y), QString::number(m_low0) + "°");

    int offset1 = 0;
    offset1 = (m_low1 - Avg) * INCREMENT;
    point[1].setX(ui->lblWeek1->pos().x() +ui->lblWeek1->width() / 2);
    point[1].setY(center - offset1);
    //绘制点
    painter.drawEllipse(point[1], PAINT_RADIUS, PAINT_RADIUS);
    //绘制文字
    painter.drawText(QPoint(point[1].x() - TEXT_OFFSET_X, point[1].y() - TEXT_OFFSET_Y), QString::number(m_low1) + "°");

    int offset2 = 0;
    offset2 = (m_low2 - Avg) * INCREMENT;
    point[2].setX(ui->lblWeek2->pos().x() +ui->lblWeek2->width() / 2);
    point[2].setY(center - offset2);
    //绘制点
    painter.drawEllipse(point[2], PAINT_RADIUS, PAINT_RADIUS);
    //绘制文字
    painter.drawText(QPoint(point[2].x() - TEXT_OFFSET_X, point[2].y() - TEXT_OFFSET_Y), QString::number(m_low2) + "°");

    int offset3 = 0;
    offset3 = (m_low3 - Avg) * INCREMENT;
    point[3].setX(ui->lblWeek3->pos().x() +ui->lblWeek3->width() / 2);
    point[3].setY(center - offset3);
    //绘制点
    painter.drawEllipse(point[3], PAINT_RADIUS, PAINT_RADIUS);
    //绘制文字
    painter.drawText(QPoint(point[3].x() - TEXT_OFFSET_X, point[3].y() - TEXT_OFFSET_Y), QString::number(m_low3) + "°");

    int offset4 = 0;
    offset4 = (m_low4 - Avg) * INCREMENT;
    point[4].setX(ui->lblWeek4->pos().x() +ui->lblWeek4->width() / 2);
    point[4].setY(center - offset4);
    //绘制点
    painter.drawEllipse(point[4], PAINT_RADIUS, PAINT_RADIUS);
    //绘制文字
    painter.drawText(QPoint(point[4].x() - TEXT_OFFSET_X, point[4].y() - TEXT_OFFSET_Y), QString::number(m_low4) + "°");

    int offset5 = 0;
    offset5 = (m_low5 - Avg) * INCREMENT;
    point[5].setX(ui->lblWeek5->pos().x() +ui->lblWeek5->width() / 2);
    point[5].setY(center - offset5);
    //绘制点
    painter.drawEllipse(point[5], PAINT_RADIUS, PAINT_RADIUS);
    //绘制文字
    painter.drawText(QPoint(point[5].x() - TEXT_OFFSET_X, point[5].y() - TEXT_OFFSET_Y), QString::number(m_low5) + "°");

    //只需要5段
    for (int i = 0; i < 5; ++i) {
        //绘制曲线,昨天为虚线,其余的为实线
        if (i == 0) {
            pen.setStyle(Qt::DotLine);
            painter.setPen(pen);
        }else{
            pen.setStyle(Qt::SolidLine);
            painter.setPen(pen);
        }
        //绘制线
        painter.drawLine(point[i].x(), point[i].y(), point[i + 1].x(), point[i + 1].y());
    }
    //恢复
    painter.restore();
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->lblHighCurve && event->type() == QEvent::Paint)
    {
        painterHigh();
    }
    if(watched == ui->lblLowCurve && event->type() == QEvent::Paint)
    {
        painterLow();
    }

    return QWidget::eventFilter(watched, event);
}

void MainWindow::on_btnSearch_clicked()
{
    QString citName = ui->leCity->text();
    Request(citName);
}
