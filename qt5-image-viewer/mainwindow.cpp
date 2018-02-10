#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QStandardPaths>

#include <QImageReader>
#include <QImageWriter>

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_graphicsScene = new QGraphicsScene();
    m_graphicsScene->setItemIndexMethod(QGraphicsScene::NoIndex);
    QImage bground(50, 50, QImage::Format_RGB888);
    for (int y = 0; y < 25; y++)
    {
        for (int x = 0; x < 25; x++)
        {
            bground.setPixel(x, y, qRgb(0xCA, 0xCA, 0xCA));
            bground.setPixel(x + 25, y, qRgb(0xFF, 0xFF, 0xFF));
            bground.setPixel(x, y + 25, qRgb(0xFF, 0xFF, 0xFF));
            bground.setPixel(x + 25, y + 25, qRgb(0xCA, 0xCA, 0xCA));
        }
    }
    m_graphicsScene->setBackgroundBrush(QPixmap::fromImage(bground));

    ui->m_graphicsView->setScene(m_graphicsScene);

    ui->statusBar->showMessage("ready", 5000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onPressLoadImage()
{
    QString qStrFilePath = QFileDialog::getOpenFileName(this,
        tr("Open Image"),
        QStandardPaths::writableLocation(QStandardPaths::CacheLocation),
        tr("Image Files (*.png *.jpg *.bmp)"));

    if (qStrFilePath.isEmpty())
        return;

    QImageReader reader(qStrFilePath);
    if (!reader.canRead())
    {
        QMessageBox msgBox;
        msgBox.setText("Cannot read file");
        msgBox.exec();
        return;
    }
    if (!m_graphicsScene->sceneRect().isEmpty())
    {
        m_graphicsScene->clear();
    }
    QImage qimg = reader.read();
    m_graphicsScene->setSceneRect(qimg.rect());
    m_graphicsScene->addPixmap(QPixmap::fromImage(qimg));

    ui->m_graphicsView->viewFit();

    ui->statusBar->showMessage("image loaded", 3000);
}

void MainWindow::onPressSaveImage()
{
    if (m_graphicsScene->sceneRect().isEmpty())
        return;

    m_graphicsScene->clearSelection();
    QImage img(m_graphicsScene->sceneRect().size().toSize(), QImage::Format_RGB888);
    QPainter painter(&img);
    m_graphicsScene->render(&painter);

    QString qStrFilePath = QFileDialog::getSaveFileName(this,
            tr("Save Image"),
            QStandardPaths::writableLocation(QStandardPaths::CacheLocation).replace("cache", "newfile.jpg"),
            tr("JPG file (*.jpg);;PNG file (*.png);;BMP file (*.bmp)"));

    if (qStrFilePath.isEmpty())
        return;

    QImageWriter writer(qStrFilePath);
    if(!writer.canWrite())
    {
        QMessageBox msgBox;
        msgBox.setText("Cannot write file");
        msgBox.exec();
        return;
    }
    writer.write(img);

    ui->statusBar->showMessage("image saved", 3000);
}

void MainWindow::onPressFitWindow()
{
    ui->m_graphicsView->viewFit();
}
