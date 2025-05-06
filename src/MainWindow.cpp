#include "../include/MainWindow.h"
#include <QFileDialog>
#include <QColorDialog>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>
#include <QSpinBox>
#include <QMenu>
#include <QMenuBar>
#include <QActionGroup>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent){
    m_canvas = new CanvasWidget(this);
    m_canvas->update();
    setCentralWidget(m_canvas);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    connect(m_canvas, &CanvasWidget::shapeSelected, this, &MainWindow::updateStatusBar);
    connect(m_canvas, &CanvasWidget::fileModified, [this](bool modified){setWindowModified(modified);});
    
    setWindowTitle("Paint App[*]");
    resize(800, 600);
}

void MainWindow::closeEvent(QCloseEvent* event){
    if(maybeSave()){
        event->accept();
    }
    else{
        event->ignore();
    }
}

void MainWindow::newFile(){
    if (maybeSave()) {
        m_canvas->clearCanvas();
        m_currentFile.clear();
        setWindowTitle("Paint App");
        setWindowModified(false);
    }
}

void MainWindow::open(){
    if(maybeSave()){
        QString fileName = QFileDialog::getOpenFileName(this, "Open file", "", "Paint Files (*.paint)");
        if(!fileName.isEmpty()){
            if(m_canvas->loadFromFile(fileName)){
                m_currentFile = fileName;
                setWindowTitle(QFileInfo(fileName).fileName() + " - Paint App");
            }
            else{
                QMessageBox::warning(this, "Warning", "Failed to open file");
            }
        }
    }
}

bool MainWindow::save(){
    if(m_currentFile.isEmpty()){
        return saveAs();
    }
    else{
        return m_canvas->saveToFile(m_currentFile);
    }
}

bool MainWindow::saveAs(){
    QString fileName = QFileDialog::getSaveFileName(this,
        "Save File", "", "Paint Files (*.paint)");
    if (!fileName.isEmpty()) {
        if (!fileName.endsWith(".paint")) {
            fileName += ".paint";
        }
        if (saveFile(fileName)) {
            m_currentFile = fileName;
            setWindowTitle(QFileInfo(fileName).fileName() + " - Paint App");
            return true;
        }
    }
    return false;
}

void MainWindow::about()
{
    QMessageBox::about(this, "About",
        "<h2>Paint App</h2>"
        "<p>Version 1.0</p>"
        "<p>Simple app to paint geometric figures.</p>");
}

void MainWindow::selectTool(QAction* action){
    qDebug() << "action selected" << (action == nullptr);
    if(action){
        m_canvas->setCurrentShapeType(action->data().toString());
        qDebug() << action->data().toString();
        updateStatusBar("Selected: " + action->text());
    }
}

void MainWindow::selectColor(){
    QColor color = QColorDialog::getColor(m_canvas->penColor(), this, "Choose color");
    if(color.isValid()){
        m_canvas->setPenColor(color);
    }
}

void MainWindow::selectFillColor()
{
    QColor color = QColorDialog::getColor(m_canvas->fillColor(), this, "Choose fill color");
    if (color.isValid()) {
        m_canvas->setFillColor(color);
    }
}

void MainWindow::setPenWidth(int width){
    m_canvas->setPenWidth(width);
}

void MainWindow::showShapeProperties(){
    QMessageBox::information(this, "Propertiers", "The properties of the selected figure will be here");
}

void MainWindow::updateStatusBar(const QString& message){
    statusBar()->showMessage(message);
}

void MainWindow::createActions(){
    m_newAct = new QAction("New file", this);
    m_newAct->setShortcut(QKeySequence::New);
    connect(m_newAct, &QAction::triggered, this, &MainWindow::newFile);

    m_openAct = new QAction("Open...", this);
    m_openAct->setShortcut(QKeySequence::Open);
    connect(m_openAct, &QAction::triggered, this, &MainWindow::open);

    m_saveAct = new QAction("Save...", this);
    m_saveAct->setShortcut(QKeySequence::Save);
    connect(m_saveAct, &QAction::triggered, this, &MainWindow::save);

    m_saveAsAct = new QAction("Save As...", this);
    m_saveAsAct->setShortcut(QKeySequence::SaveAs);
    connect(m_saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);

    m_exitAct = new QAction("Exit", this);
    m_exitAct->setShortcut(QKeySequence::Quit);
    connect(m_exitAct, &QAction::triggered, this, &MainWindow::close);

    m_toolActionGroup = new QActionGroup(this);

    m_lineAct = new QAction("Line", this);
    m_lineAct->setData("Line");
    m_lineAct->setCheckable(true);
    m_toolActionGroup->addAction(m_lineAct);
    
    m_freehandAct = new QAction("Freehand", this);
    m_freehandAct->setData("Freehand");
    m_freehandAct->setCheckable(true);
    m_toolActionGroup->addAction(m_freehandAct);
    
    m_rectAct = new QAction("Rectangle", this);
    m_rectAct->setData("Rectangle");
    m_rectAct->setCheckable(true);
    m_toolActionGroup->addAction(m_rectAct);
    
    m_circleAct = new QAction("Ellipse", this);
    m_circleAct->setData("Ellipse");
    m_circleAct->setCheckable(true);
    m_toolActionGroup->addAction(m_circleAct);
    
    m_polygonAct = new QAction("Polygon", this);
    m_polygonAct->setData("Polygon");
    m_polygonAct->setCheckable(true);
    m_toolActionGroup->addAction(m_polygonAct);
    
    m_regularPolygonAct = new QAction("Regular polygon", this);
    m_regularPolygonAct->setData("RegularPolygon");
    m_regularPolygonAct->setCheckable(true);
    m_toolActionGroup->addAction(m_regularPolygonAct);
    
    connect(m_toolActionGroup, &QActionGroup::triggered, this, &MainWindow::selectTool);
    
    m_colorAct = new QAction("Line color...", this);
    connect(m_colorAct, &QAction::triggered, this, &MainWindow::selectColor);
    
    m_fillColorAct = new QAction("Fill color...", this);
    connect(m_fillColorAct, &QAction::triggered, this, &MainWindow::selectFillColor);
    
    m_deleteAct = new QAction("Delete", this);
    m_deleteAct->setShortcut(QKeySequence::Delete);
    connect(m_deleteAct, &QAction::triggered, m_canvas, &CanvasWidget::deleteSelectedShape);
    
    m_propertiesAct = new QAction("Properties...", this);
    connect(m_propertiesAct, &QAction::triggered, this, &MainWindow::showShapeProperties);
    
    m_bringToFrontAct = new QAction("Bring to front", this);
    connect(m_bringToFrontAct, &QAction::triggered, m_canvas, &CanvasWidget::bringToFront);
    
    m_sendToBackAct = new QAction("Bring to back", this);
    connect(m_sendToBackAct, &QAction::triggered, m_canvas, &CanvasWidget::sendToBack);
    
    m_aboutAct = new QAction("About", this);
    connect(m_aboutAct, &QAction::triggered, this, &MainWindow::about);

    m_lineAct->setToolTip("Draw straight lines");
    m_freehandAct->setToolTip("Draw freehand lines");
    m_rectAct->setToolTip("Draw rectangles");
    m_circleAct->setToolTip("Draw ellipses");
    m_polygonAct->setToolTip("Draw polygons");
    m_regularPolygonAct->setToolTip("Draw regular polygons");
    m_colorAct->setToolTip("Set line color");
    m_fillColorAct->setToolTip("Set fill color");


}

void MainWindow::createMenus(){
    m_fileMenu = menuBar()->addMenu("File");
    m_fileMenu->addAction(m_newAct);
    m_fileMenu->addAction(m_openAct);
    m_fileMenu->addAction(m_saveAct);
    m_fileMenu->addAction(m_saveAsAct);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_exitAct);
    
    m_editMenu = menuBar()->addMenu("Edit");
    m_editMenu->addAction(m_deleteAct);
    m_editMenu->addAction(m_propertiesAct);
    m_editMenu->addSeparator();
    m_editMenu->addAction(m_bringToFrontAct);
    m_editMenu->addAction(m_sendToBackAct);
    
    m_viewMenu = menuBar()->addMenu("View");
    
    m_helpMenu = menuBar()->addMenu("Help");
    m_helpMenu->addAction(m_aboutAct);
}

void MainWindow::createToolBars()
{
    m_drawingToolBar = addToolBar("Tools");
    m_drawingToolBar->addAction(m_lineAct);
    m_drawingToolBar->addAction(m_freehandAct);
    m_drawingToolBar->addAction(m_rectAct);
    m_drawingToolBar->addAction(m_circleAct);
    m_drawingToolBar->addAction(m_polygonAct);
    m_drawingToolBar->addAction(m_regularPolygonAct);
    m_drawingToolBar->addSeparator();
    m_drawingToolBar->addAction(m_colorAct);
    m_drawingToolBar->addAction(m_fillColorAct);
    
    QSpinBox *widthSpinBox = new QSpinBox();
    widthSpinBox->setRange(1, 20);
    widthSpinBox->setValue(m_canvas->penWidth());
    widthSpinBox->setSuffix(" px");
    widthSpinBox->setToolTip("Line width");
    
    connect(widthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::setPenWidth);
    
    m_drawingToolBar->addWidget(widthSpinBox);
    
    m_editToolBar = addToolBar("Editing");
    m_editToolBar->addAction(m_deleteAct);
    m_editToolBar->addAction(m_propertiesAct);
    m_editToolBar->addSeparator();
    m_editToolBar->addAction(m_bringToFrontAct);
    m_editToolBar->addAction(m_sendToBackAct);
}

void MainWindow::createStatusBar(){
    statusBar()->showMessage("Готово");
}

bool MainWindow::maybeSave(){
    if (!isWindowModified()) {
        return true;
    }
    
    QMessageBox::StandardButton ret;
    ret = QMessageBox::warning(this, "Paint App",
        "Document was modified.\nWant to save changes?",
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    
    if (ret == QMessageBox::Save) {
        return save();
    } else if (ret == QMessageBox::Cancel) {
        return false;
    }
    return true;
}

bool MainWindow::saveFile(const QString &fileName){
    return m_canvas->saveToFile(fileName);
}

bool MainWindow::loadFile(const QString &fileName){
    return m_canvas->loadFromFile(fileName);
} 