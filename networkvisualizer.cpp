
NetworkVisualizer::NetworkVisualizer(QWidget* parent)
    : QGraphicsView(parent), m_scene(new QGraphicsScene(this)) {
    setScene(m_scene);
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::RubberBandDrag);
    setAcceptDrops(true); // 启用拖拽功能
}
void NetworkVisualizer::updateConnections() {
    qDebug() << "Updating connections";
    for (const ConnectionLine& conn : m_connections) {
        if (!conn.fromGroup || !conn.toGroup || !conn.line) continue;

        QPointF p1 = conn.fromGroup->mapToScene(
            conn.fromGroup->boundingRect().center().x(),
            conn.fromGroup->boundingRect().bottom()
            );
        QPointF p2 = conn.toGroup->mapToScene(
            conn.toGroup->boundingRect().center().x(),
            conn.toGroup->boundingRect().top()
            );

        conn.line->setLine(QLineF(p1, p2));
    }
}
void NetworkVisualizer::createConnection(MovableLayerGroup* from, MovableLayerGroup* to) {
    QPointF p1 = from->sceneBoundingRect().center();
    p1.setY(from->sceneBoundingRect().bottom());
    QPointF p2 = to->sceneBoundingRect().center();
    p2.setY(to->sceneBoundingRect().top());

    QGraphicsLineItem* line = m_scene->addLine(QLineF(p1, p2), QPen(Qt::black));
    m_connections.append({line, from, to});
}


MovableLayerGroup* NetworkVisualizer::createDetailedLayer(
    const NeuralLayer& layer,

    int yPos
    ) {
    const QString layerName = QString("%1").arg(layer.layerType);
    const QString activation= QString("%1").arg(layer.activationFunction);
    const int width = 160;
    const int height = 130;
    const int x = 100;
    const ColorTheme& theme = ColorThemeManager::currentTheme();
    //QGraphicsItemGroup* group = new QGraphicsItemGroup();
    MovableLayerGroup* group = new MovableLayerGroup();
    connect(group, &MovableLayerGroup::positionChanged, this, &NetworkVisualizer::updateConnections);


    // 背景框
    QGraphicsRectItem* bg = new QGraphicsRectItem(0, 0, width, height);
    bg->setBrush(theme.neuronFill);//QColor(240, 240, 240)
    bg->setPen(QPen(theme.text));//QPen(Qt::blue)
    group->addToGroup(bg);

    // 层标签
    QGraphicsTextItem* title = new QGraphicsTextItem(layerName);
    title->setPos(40, 5);
    group->addToGroup(title);

    // 图形
    QGraphicsRectItem* w = nullptr;
    QGraphicsRectItem* b = nullptr;
    QGraphicsEllipseItem* plus = nullptr;
    QGraphicsRectItem* act = nullptr;

    if (layerName == "Hidden"||layerName == "Dense") {
        w = new QGraphicsRectItem(0, 0, 30, 30);
        w->setBrush(theme.weightBoxFill);//Qt::cyan
        w->setPos(10, 30);
        group->addToGroup(w);
        QGraphicsTextItem* wLabel = new QGraphicsTextItem("W", w);
        wLabel->setPos(8, 5);

        b = new QGraphicsRectItem(0, 0, 30, 30);
        b->setBrush(theme.weightBoxFill);//cyan
        b->setPos(width - 40, 30);
        group->addToGroup(b);
        QGraphicsTextItem* bLabel = new QGraphicsTextItem("b", b);
        bLabel->setPos(8, 5);
        plus = new QGraphicsEllipseItem(0, 0, 20, 20);
        plus->setBrush(theme.neuronFill);//Qt::white
        plus->setPos(width / 2 - 10, 60);
        group->addToGroup(plus);
        QGraphicsTextItem* plusLabel = new QGraphicsTextItem("+", plus);
        plusLabel->setPos(2, 0);
    }
    if (layerName == "Dropout") {
        //Dropout 层框
        QGraphicsRectItem* act = new QGraphicsRectItem(0, 0, 100, 26);
        act->setBrush(theme.activationBoxFill);
        act->setPos(30, 90);
        group->addToGroup(act);

        // 标签文本
        QString dropoutText = QString("rate: %1").arg(layer.dropoutRate, 0, 'f', 4);//（4位小数）
        QGraphicsTextItem* actLabel = new QGraphicsTextItem(dropoutText, act);
        actLabel->setPos(10, 5);

        // 动态计算文本宽度调整矩形框大小
        QFontMetrics metrics(actLabel->font());
        int textWidth = metrics.horizontalAdvance(dropoutText);  // 获取文本像素宽度
        act->setRect(0, 0, textWidth + 20, 26);

    }

    if (layerName== "LSTM" || layerName == "RNN" || layerName == "GRU"){
        QGraphicsRectItem* act = new QGraphicsRectItem(0, 0, 100, 26);
        act->setBrush(theme.activationBoxFill);
        act->setPos(30, 90);
        group->addToGroup(act);

        // 标签文本
        QString dropoutText = QString("units: %1").arg(layer.units);
        QGraphicsTextItem* actLabel = new QGraphicsTextItem(dropoutText, act);
        actLabel->setPos(10, 5);

        // 动态计算文本宽度调整矩形框大小
        QFontMetrics metrics(actLabel->font());
        int textWidth = metrics.horizontalAdvance(dropoutText);  // 获取文本像素宽度
        act->setRect(0, 0, textWidth + 20, 26);
    }

    if (layerName == "Convolutional"){

        QGraphicsRectItem* filtersRect = new QGraphicsRectItem(0, 0, 100, 26);
        filtersRect->setBrush(theme.activationBoxFill);
        filtersRect->setPos(30, 60);
        group->addToGroup(filtersRect);

        QGraphicsRectItem* kernelRect = new QGraphicsRectItem(0, 0, 100, 26);
        kernelRect->setBrush(theme.activationBoxFill);
        kernelRect->setPos(30, 90);  // 放在filters下方
        group->addToGroup(kernelRect);
        // 标签文本
        QString filtersText = QString("filters: %1").arg(layer.filters);
        QGraphicsTextItem* filtersLabel = new QGraphicsTextItem(filtersText, filtersRect);
        filtersLabel->setPos(10, 5);


        QString kernelText = QString("kernel: %1").arg(layer.kernelSize);
        QGraphicsTextItem* kernelLabel = new QGraphicsTextItem(kernelText, kernelRect);
        kernelLabel->setPos(10, 5);

        // 动态计算文本宽度调整矩形框大小
        QFontMetrics metrics(filtersLabel->font());
        int filtersWidth = metrics.horizontalAdvance(filtersText) + 20;
        filtersRect->setRect(0, 0, filtersWidth, 26);


        int kernelWidth = metrics.horizontalAdvance(kernelText) + 20;
        kernelRect->setRect(0, 0, kernelWidth, 26);
    }

    if (layerName== "MaxPooling" || layerName == "AveragePooling")  {
        //pooling层框
        QGraphicsRectItem* act = new QGraphicsRectItem(0, 0, 100, 26);
        act->setBrush(theme.activationBoxFill);
        act->setPos(30, 90);
        group->addToGroup(act);

        // 标签文本
        QString dropoutText = QString("poolingSize: %1").arg(layer.poolingSize);
        QGraphicsTextItem* actLabel = new QGraphicsTextItem(dropoutText, act);
        actLabel->setPos(10, 5);

        // 动态计算文本宽度调整矩形框大小
        QFontMetrics metrics(actLabel->font());
        int textWidth = metrics.horizontalAdvance(dropoutText);  // 获取文本像素宽度
        act->setRect(0, 0, textWidth + 20, 26);

    }

    if (!activation.trimmed().isEmpty()) {
        act = new QGraphicsRectItem(0, 0, 100, 26);
        act->setBrush(theme.activationBoxFill);//QColor(180, 220, 255)
        act->setPos(30, 90);
        group->addToGroup(act);
        QGraphicsTextItem* actLabel = new QGraphicsTextItem(activation, act);
        actLabel->setPos(10, 5);
    }

    if (w) w->setZValue(1);
    if (b) b->setZValue(1);
    if (plus) plus->setZValue(1);
    if (act) act->setZValue(1);


    // 连线（局部坐标系内）
    if (w && b && plus) {
        QPointF wCenter = w->pos() + QPointF(w->rect().width() / 2, w->rect().height() / 2);
        QPointF bCenter = b->pos() + QPointF(b->rect().width() / 2, b->rect().height() / 2);
        QPointF plusCenter = plus->pos() + QPointF(plus->rect().width() / 2, plus->rect().height() / 2);

        QPen pen(theme.connectionHighWeight);//Qt::black
        pen.setWidth(2);
        QGraphicsLineItem* lineW = new QGraphicsLineItem(QLineF(wCenter, plusCenter));
        lineW->setPen(pen);
        lineW->setZValue(0);
        group->addToGroup(lineW);

        QGraphicsLineItem* lineB = new QGraphicsLineItem(QLineF(bCenter, plusCenter));
        lineB->setPen(pen);
        lineB->setZValue(0);
        group->addToGroup(lineB);
    }

    if (plus && act) {
        QPointF plusCenter = plus->pos() + QPointF(plus->rect().width() / 2, plus->rect().height() / 2);
        QPointF actTopCenter = act->pos() + QPointF(act->rect().width() / 2, 0);

        QPen pen(theme.connectionHighWeight);//Qt::black
        pen.setWidth(2);
        QGraphicsLineItem* lineToAct = new QGraphicsLineItem(QLineF(plusCenter, actTopCenter));
        lineToAct->setPen(pen);
        lineToAct->setZValue(0);
        group->addToGroup(lineToAct);
    }




    group->setPos(x, yPos);  // 整体移动
    m_scene->addItem(group);
    //group->setZValue(0); // 在上层显示

    return group;
}
