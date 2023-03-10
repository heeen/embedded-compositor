// SPDX-License-Identifier: GPL-3.0-only

#ifndef DBUSINTERFACE_H
#define DBUSINTERFACE_H

#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusContext>
#include <QLoggingCategory>
#include <QObject>
#include <QQmlParserStatus>

Q_DECLARE_LOGGING_CATEGORY(compositorDBus)

bool InitDbusConnection(QString serviceName);

class DBusInterface : public QObject,
                      public QQmlParserStatus,
                      protected QDBusContext {
  Q_OBJECT
  Q_INTERFACES(QQmlParserStatus)

  Q_PROPERTY(QString interfaceName READ interfaceName CONSTANT)
  Q_PROPERTY(QString objectPath READ objectPath CONSTANT)
  Q_PROPERTY(bool valid READ valid NOTIFY validChanged)

public:
  ~DBusInterface() override = default;

  QDBusConnection busConnection() const;
  QString interfaceName() const;
  QString objectPath() const;

  bool valid() const;
  Q_SIGNAL void validChanged(bool valid);

protected:
  DBusInterface(const QString &objectPath, QObject *parent = nullptr);

  // QQmlParserStatus
  void classBegin() override;
  void componentComplete() override;

private Q_SLOTS:
  void onPropertyChanged();

private:
  QString m_objectPath;
  bool m_valid = false;
};

class QAbstractListModel;

struct TaskSwitcherEntry {
  int id;
  int pid;
  QString label;
};

Q_DECLARE_METATYPE(TaskSwitcherEntry)
Q_DECLARE_METATYPE(QList<TaskSwitcherEntry>)

QDBusArgument &operator<<(QDBusArgument &argument,
                          const TaskSwitcherEntry &entry);

const QDBusArgument &operator>>(const QDBusArgument &argument,
                                TaskSwitcherEntry &entry);

class TaskSwitcherInterface : public DBusInterface {
  Q_OBJECT

public:
  TaskSwitcherInterface(QObject *parent = nullptr);
  ~TaskSwitcherInterface() override = default;

  Q_PROPERTY(int CurrentView READ CurrentView WRITE setCurrentView NOTIFY
                 CurrentViewChanged)
  Q_PROPERTY(QList<TaskSwitcherEntry> Views READ Views NOTIFY ViewsChanged)
  Q_PROPERTY(QAbstractListModel *viewModel READ viewModel WRITE setViewModel
                 NOTIFY viewModelChanged)

  // DBus
  void Open();
  void Close();

  int CurrentView() const;
  void setCurrentView(int newCurrentView);

  const QList<TaskSwitcherEntry> &Views() const;

  QAbstractListModel *viewModel() const;
  void setViewModel(QAbstractListModel *newViewModel);

public slots:

Q_SIGNALS:
  void openRequested();
  void closeRequested();
  void CurrentViewChanged(int CurrentView);
  void ViewsChanged(const QList<TaskSwitcherEntry> &Views);

  void viewModelChanged(QAbstractListModel *viewModel);

private:
  int m_CurrentView = 1337;
  QList<TaskSwitcherEntry> m_Views = {{42, 123, "woot"}};
  QAbstractListModel *m_viewModel = nullptr;

  void publishViews();
};

class GlobalOverlayInterface : public DBusInterface {
  Q_OBJECT

public:
  GlobalOverlayInterface(QObject *parent = nullptr);
  ~GlobalOverlayInterface() override = default;

  // DBus
  void Show(const QString &message);
  void Hide();

Q_SIGNALS:
  void showRequested(const QString &message);
  void hideRequested();
};

class CompositorScreenInterface : public DBusInterface {
  Q_OBJECT

  Q_PROPERTY(QString orientation READ orientation WRITE setOrientation NOTIFY
                 orientationChanged)

public:
  CompositorScreenInterface(QObject *parent = nullptr);
  ~CompositorScreenInterface() override = default;

  QString orientation() const;
  void setOrientation(const QString &orientation);

Q_SIGNALS:
  void orientationChanged(const QString &orientation);

private:
  QString m_orientation;
};

#endif // DBUSINTERFACE_H
