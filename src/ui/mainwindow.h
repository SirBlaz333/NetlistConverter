#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>

#include <src/app/app_controller.h>
#include <src/app/app_internal_storage.h>

#include <src/app/app_header.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void convertToSpice();
    void updateLibrePCB();
    void subcircuitCheckBoxStateChanged(int state);
    void nextNetlist();
    void previousNetlist();
    void lastNetlist();
    void openDirectoryDialog();
    void saveSpice();
    void saveSpiceAs();
    void closeSpice();
    void openLibre();
    void refreshLibre();
    void saveLibre();
    void saveLibreAs();
    void closeLibre();
    void openUserManual();
    void openLibreDocumentation();
    void loadExample();
    void simulate();

private:
    AppController appController;
    AppInternalStorage* storage;
    AppHeader header;
    Ui::MainWindow *ui;
    ConversionParams lastParams;
    ConversionParams getConversionParams();
    void saveSubcircuitIfNeeded(QString subcircuit, ConversionParams params);
    void saveAndUpdateState();
    void changeState(AppState node);
    void saveSpiceNetlist(bool forcedFileDialog);
    void saveLibreNetlist(bool forcedFileDialog);
};
#endif // MAINWINDOW_H
