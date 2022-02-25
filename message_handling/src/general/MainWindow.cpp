
#include <QDir>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QIcon>
#include <QAction>
#include <QDateTime>
#include <QTimer>

#include <algorithm>
#include <iterator>
#include <utility>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    const QString appConfigsPath = QApplication::applicationDirPath() + "/configs";
    QDir().mkpath(appConfigsPath);

    const QString appLogsPath = QApplication::applicationDirPath() + "/logs";
    QDir().mkpath(appLogsPath);

    createGroups(appConfigsPath, appLogsPath);
    create1Group(appConfigsPath, appLogsPath, "1Group");
}

void MainWindow::createGroups(const QString &appConfigsPath, const QString &appLogDirPath)
{
    const std::vector<QString> names =
    { {"Group1"},
      {"Group2"},
      {"Group3"} };

    std::generate_n(std::back_inserter(pprGroups_), 3,
    [&appConfigsPath, &appLogDirPath, &names_features, index = 0u, this]() mutable -> Group*
    {
        const QString groupName = names.at(index);
        index++;

        return createSMTHGroup(appConfigsPath, appLogDirPath, groupName);
    });

    for (Group* group : getActiveGroups())
    {
        connect(this, &MainWindow::internal_GetSMTH,
                group->getGroupBaseCommon(), &tech::GroupBaseCommon::on_internal_GetSMTH);
    }
}

auto MainWindow::createSMTHGroup(const QString &appConfigsPath, const QString &appLogDirPath, const QString& groupName) -> Group*
{
    const QString GroupConfig = Group::groupConfigFullPath(appConfigsPath, groupName);
    const QString NetConfig   = Group::groupNetConfigFullPath(appConfigsPath, groupName);
    const QString LogDirPath  = Group::groupLogsDirectory(appLogDirPath, groupName);

    const auto factoryGroup = [GroupConfig, NetConfig, LogDirPath]() -> Group*
    {
        return Group::create(GroupConfig, NetConfig, LogDirPath, utils::heavy::IThreadPlaceable::NO_PARENT);
    };

    return utils::heavy::create<Group>(this, factoryGroup);
}

void MainWindow::createNetLog()
{
    thread_netLog_ = new QThread(this);
    netLog_        = new ProtocolsLogger(nullptr);
    netLog_->moveToThread(thread_netLog_);
    thread_netLog_->start();

    connect(armProxy_, &ARMProxy::netLogMessage, netLog_, &ProtocolsLogger::onNetLogMessage);

    for (GroupPPRDozor* group : getActiveGroups())
        connect(group, &GroupPPRDozor::netLogMessage, netLog_, &ProtocolsLogger::onNetLogMessage);

    if (parGroup_)
        connect(parGroup_, &GroupPARDozor::netLogMessage, netLog_, &ProtocolsLogger::onNetLogMessage);
}

void MainWindow::createSystemTrayIcon()
{
    QMenu* systemTrayMenu = new QMenu(this);    // за удаление отвечает объект-родитель (this).

    int _pos_ = 0;
    std::vector<int> positionsToDisable;

#ifdef IS_SMTH
    systemTrayMenu->addAction(QString::fromLocal8Bit("Запустить!"));
    const int runTestPos = _pos_++;
    positionsToDisable.push_back(runTestPos);

    systemTrayMenu->addAction(QString::fromLocal8Bit("Остановить тест!"));
    const int stopTestPos = _pos_++;
    systemTrayMenu->actions().at(stopTestPos)->setEnabled(false);
    positionsToDisable.push_back(stopTestPos);

    systemTrayMenu->addSeparator();
    _pos_++;

    for (Group* group : getActiveGroups())
    {
        connectRutTestBttn(group, systemTrayMenu, runTestPos, stopTestPos);
        connectStopTestBttn(group, systemTrayMenu, runTestPos, stopTestPos);
    }
#else
    systemTrayMenu->addAction(QString::fromLocal8Bit("Показать"));
    const int showDispatcherPos = _pos_++;
    positionsToDisable.push_back(showDispatcherPos);
    connectShowDispatcherBttn(parGroup_, systemTrayMenu, showDispatcherPos);
#endif

    systemTrayMenu->addAction(QString::fromLocal8Bit("Выход"));
    const int exitPos = _pos_++;
    positionsToDisable.push_back(exitPos);

    connectExitBttn(systemTrayMenu, exitPos, positionsToDisable);

    systemTrayIcon_ = new QSystemTrayIcon(QIcon(__ICON_PATH), this);
    systemTrayIcon_->setToolTip(__APPLICATION_NAME);
    systemTrayIcon_->setContextMenu(systemTrayMenu);
    systemTrayIcon_->setVisible(true);                       // нужно явно указывать видимость иконки.
    systemTrayIcon_->show();
}

void MainWindow::connectRutTestBttn(Group* group, QMenu *systemTrayMenu, int runTestPos, int stopTestPos)
{
    connect(this, &MainWindow::runTest, group, &Group::runTest);

    QAction* runTestAction  = systemTrayMenu->actions().at(runTestPos);
    QAction* stopTestAction = systemTrayMenu->actions().at(stopTestPos);

    connect(runTestAction, &QAction::triggered, this,
    [this, runTestAction, stopTestAction]()
    {
        runTestAction->setEnabled(false);
        stopTestAction->setEnabled(true);
        this->systemTrayIcon_->showMessage(__APPLICATION_NAME, QString::fromLocal8Bit("Запущен тест!"), QSystemTrayIcon::Critical, 3000);

        emit this->runTest();
    });
}

void MainWindow::connectStopTestBttn(Group* group, QMenu *systemTrayMenu, int runTestPos, int stopTestPos)
{
    connect(this, &MainWindow::stopTest, group, &Group::stopTest);

    QAction* runTestAction  = systemTrayMenu->actions().at(runTestPos);
    QAction* stopTestAction = systemTrayMenu->actions().at(stopTestPos);

    connect(stopTestAction, &QAction::triggered, this,
    [this, runTestAction, stopTestAction]()
    {
        runTestAction->setEnabled(true);
        stopTestAction->setEnabled(false);
        this->systemTrayIcon_->showMessage(__APPLICATION_NAME, QString::fromLocal8Bit("Остановлен тест!"), QSystemTrayIcon::Critical, 3000);

        emit this->stopTest();
    });
}

void MainWindow::connectExitBttn(QMenu *systemTrayMenu, int exitPos, std::vector<int> positionsToDisable)
{
    connect(this, &MainWindow::shutDown, QCoreApplication::instance(), &QCoreApplication::quit);

    QAction* shutDownAction = systemTrayMenu->actions().at(exitPos);

    connect(shutDownAction, &QAction::triggered, this,
    [this, positionsToDisable, systemTrayMenu]()
    {
        for (int pos: positionsToDisable)
            systemTrayMenu->actions().at(pos)->setEnabled(false);    // чтобы не спамили кнопки.

        this->systemTrayIcon_->showMessage(__APPLICATION_NAME, QString::fromLocal8Bit("Выполняется выход..."));

        emit this->shutDown();
    });
}

void MainWindow::connectShowDispatcherBttn(Group* group, QMenu *systemTrayMenu, int showDispatcherPos)
{
    p_widget_.reset( new ui::Widget(nullptr) );
    ui::Widget* Dispatcher = p_widget_.get();

    QAction* showDispatcherAction         = systemTrayMenu->actions().at(showDispatcherPos);

    connect(showDispatcherAction, &QAction::triggered, this,
    [Dispatcher]()
    {
        if (not Dispatcher->isVisible())
            Dispatcher->show();
    });

    connect(group, &Group::newQueue, this,
    [Dispatcher](uint32_t groupId, const std::vector<uint32_t> &queue) -> void
    {
        Dispatcher->onNewQueue(queue);
    });

    connect(group, &Group::Mode, this,
    [Dispatcher](uint32_t groupId, uint32_t Id, bool isOn) -> void
    {
        Dispatcher->onSetMode(Id, isOn);
    });

    connect(group, &Group::Active, this,
    [Dispatcher](uint32_t groupId, uint32_t Id, bool isActive) -> void
    {
        Dispatcher->onSetActive(Id, isActive);
    });
}

MainWindow::~MainWindow()
{
    delete ui;

    destroyA();

    for (Group* group : Groups_)
        destroy(group);
}

void MainWindow::destroyPPRDozor(Group* p_group)
{
    utils::heavy::destroy(p_group);
}

void MainWindow::destroyNetLog()
{
    if (thread_netLog_)
    {
        thread_netLog_->quit();
        thread_netLog_->wait();
    }

    delete netLog_;
}

std::vector<Group*> MainWindow::getActiveGroups() const
{
    std::vector<Group*> active;

    std::copy_if(Groups_.cbegin(), Groups_.cend(), std::back_inserter(active),
    [](Group* group) -> bool
    {
        return group != nullptr;
    });

    return active;
}

void MainWindow::onGetTopologyPack()
{
    const auto onAwait = [this]() -> void
    {
        isWaitingTopologies_ = false;

        auto p_activeTopologies = std::make_shared<std::vector<Group_struct>>(activeTopologies_.size());
		
        std::transform(activeTopologies_.cbegin(), activeTopologies_.cend(), p_activeTopologies->begin(),
        [](const std::pair<const uint32_t, Group_struct> &topology) -> Group_struct
        {
            return topology.second;
        });

        emit topologyPack(p_activeTopologies);
    };

    if (not isWaitingTopologies_)
    {
        const int awaitTime = 1000;

        isWaitingTopologies_ = true;
        emit internal_GetTopologyPack();

        QTimer::singleShot(awaitTime, this, onAwait);
    }
}
