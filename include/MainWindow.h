#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QActionGroup>
#include "CanvasWidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void about();
    
    void selectTool(QAction* action);
    void selectColor();
    void selectFillColor();
    void setPenWidth(int width);
    
    void showShapeProperties();
    void updateStatusBar(const QString& message);

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    
    bool maybeSave();
    bool saveFile(const QString& fileName);
    bool loadFile(const QString& fileName);
    
    CanvasWidget* m_canvas;
    QString m_currentFile;
    
    QMenu* m_fileMenu;
    QMenu* m_editMenu;
    QMenu* m_viewMenu;
    QMenu* m_helpMenu;
    
    QToolBar* m_drawingToolBar;
    QToolBar* m_editToolBar;
    
    QAction* m_newAct;
    QAction* m_openAct;
    QAction* m_saveAct;
    QAction* m_saveAsAct;
    QAction* m_exitAct;
    
    QAction* m_lineAct;
    QAction* m_circleAct;
    QAction* m_rectAct;
    QAction* m_freehandAct;
    QAction* m_polygonAct;
    QAction* m_regularPolygonAct;
    
    QAction* m_colorAct;
    QAction* m_fillColorAct;
    QAction* m_penWidthAct;
    
    QAction* m_deleteAct;
    QAction* m_propertiesAct;
    QAction* m_bringToFrontAct;
    QAction* m_sendToBackAct;
    
    QAction* m_aboutAct;
    
    QActionGroup* m_toolActionGroup;
};

#endif