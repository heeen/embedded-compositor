#ifndef EMBEDDEDSHELLEXTENSION_H
#define EMBEDDEDSHELLEXTENSION_H

#include <QtWaylandCompositor/QWaylandShellSurfaceTemplate>
#include <QtWaylandCompositor/QWaylandShellTemplate>

#include "qwayland-server-embedded-shell.h"
#include <QWaylandResource>
#include <QtWaylandCompositor/QWaylandCompositor>
#include <QtWaylandCompositor/QWaylandQuickExtension>

#include <QtWaylandCompositor/QWaylandQuickShellIntegration>
#include <QtWaylandCompositor/QWaylandQuickShellSurfaceItem>

class EmbeddedShellSurface;
class EmbeddedShellSurfaceView;

class EmbeddedShellExtension
    : public QWaylandShellTemplate<EmbeddedShellExtension>,
      public QtWaylandServer::embedded_shell {
  Q_OBJECT
public:
  explicit EmbeddedShellExtension(QWaylandCompositor *compositor);
  explicit EmbeddedShellExtension();
  void embedded_shell_surface_create(Resource *resource,
                                     struct ::wl_resource *surface, uint32_t id,
                                     uint32_t anchor) override;

  void initialize() override;
signals:
  void surfaceAdded(EmbeddedShellSurface *surface);
};

class EmbeddedShellSurface
    : public QWaylandShellSurfaceTemplate<EmbeddedShellSurface>,
      public QtWaylandServer::embedded_shell_surface {
  Q_OBJECT
public:
  EmbeddedShellSurface(EmbeddedShellExtension *ext, QWaylandSurface *surface,
                       const QWaylandResource &resource,
                       embedded_shell_anchor_border anchor);
  QWaylandQuickShellIntegration *
  createIntegration(QWaylandQuickShellSurfaceItem *item) override;
  QWaylandSurface *surface() const { return m_surface; }
  embedded_shell_anchor_border getAnchor() { return m_anchor; }
  Q_PROPERTY(uint anchor READ getAnchor NOTIFY anchorChanged)
  void setAnchor(embedded_shell_anchor_border newAnchor);
  Q_INVOKABLE void sendConfigure(const QSize size);

signals:
  void anchorChanged(embedded_shell_anchor_border anchor);
  void createView(EmbeddedShellSurfaceView *view);

private:
  QWaylandSurface *m_surface;
  embedded_shell_anchor_border m_anchor =
      embedded_shell_anchor_border::EMBEDDED_SHELL_ANCHOR_BORDER_UNDEFINED;

  // embedded_shell_surface interface
protected:
  void embedded_shell_surface_set_anchor(Resource *resource,
                                         uint32_t anchor) override;
  void embedded_shell_surface_view_create(Resource *resource,
                                          wl_resource *shell_surface,
                                          const QString &label,
                                          uint32_t id) override;
};

class EmbeddedShellSurfaceView : public QObject,
                                 public QtWaylandServer::surface_view {
  Q_OBJECT
  Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged)
public:
  EmbeddedShellSurfaceView(const QString &label, wl_client *client, int id,
                           int version)
      : QtWaylandServer::surface_view(client, id, version), m_label(label) {}
  const QString &label() const { return m_label; }
  void setLabel(const QString &newLabel);

public slots:
  void select() {
    surface_view::send_selected();
  }
signals:
  void labelChanged();

protected:
  void surface_view_set_label(Resource *resource, const QString &text) override;

private:
  QString m_label;
};

class QuickEmbeddedShellIntegration : public QWaylandQuickShellIntegration {
  Q_OBJECT
public:
  QuickEmbeddedShellIntegration(QWaylandQuickShellSurfaceItem *item);
  ~QuickEmbeddedShellIntegration() override;

  Q_PROPERTY(uint anchor READ getAnchor)

  uint getAnchor() { return m_shellSurface->getAnchor(); }
  void sendConfigure(const QSize size);

private slots:
  void handleEmbeddedShellSurfaceDestroyed();

private:
  QWaylandQuickShellSurfaceItem *m_item = nullptr;
  EmbeddedShellSurface *m_shellSurface = nullptr;
};

Q_COMPOSITOR_DECLARE_QUICK_EXTENSION_CLASS(EmbeddedShellExtension)

#endif // EMBEDDEDSHELLEXTENSION_H
